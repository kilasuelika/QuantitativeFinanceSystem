#pragma once

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/client.hpp>
#include <boost/json.hpp>
#include <spdlog/spdlog.h>

export module DataServer;


import std.core;

import DBManager;
import DataBridge;
import DataMantainer;
import Util;

export {
    class DataServer {
    public:
        using SocketServer = websocketpp::server<websocketpp::config::asio>;
        using SocketClient = websocketpp::client<websocketpp::config::asio>;

        bool Run(const std::string& configfile);
        void Wait();
        // 对于前端，用来注册以主动推送数据。
        void RegisterListener(websocketpp::connection_hdl hdl);

        void on_open(websocketpp::connection_hdl hdl);
        void on_close(websocketpp::connection_hdl hdl);
        void on_message(websocketpp::connection_hdl hdl,
            SocketServer::message_ptr msg);

    private:
        void InitDataBridge();
        void InitDBManager();
        void InitDataMantainer();
        void InitWebSocketServer();

        int _port;
        SocketServer _s;
        std::future<void> _sf;
        std::vector<websocketpp::connection_hdl> _listeners;

        DBManager _db;
        DataBridge _bridge;
        DataMantainer _mantainer;

        boost::json::value _config;
    };
}

void DataServer::Wait() {
    if (_sf.valid()) {
        _sf.wait();
    }
    spdlog::warn("Websocket server stopped or not started.");
}
void DataServer::InitWebSocketServer() {
    spdlog::info("Init WebSocket Server");

    _s.set_error_channels(websocketpp::log::elevel::all);
    _s.set_access_channels(websocketpp::log::alevel::all ^ websocketpp::log::alevel::frame_payload);
    _s.init_asio();

    _s.set_open_handler(bind(&DataServer::on_open, this, std::placeholders::_1));
    _s.set_close_handler(bind(&DataServer::on_close, this, std::placeholders::_1));
    _s.set_message_handler(bind(&DataServer::on_message, this, std::placeholders::_1, std::placeholders::_2));

    auto port = _config.as_object()["port"].as_int64();
    spdlog::info("Listen on port {}", port);
    _s.listen(port);
    _s.start_accept();

    _sf = std::async(std::launch::async, [&] {_s.run(); });
}

bool DataServer::Run(const std::string& configfile) {
    try {
        _config = load_json(configfile);
        InitDataBridge();
        InitDBManager();
        InitDataMantainer();
        InitWebSocketServer();
    }
    catch (...) {
        return false;
    }
}
void DataServer::RegisterListener(websocketpp::connection_hdl hdl) {}
void DataServer::on_open(websocketpp::connection_hdl hdl) {}
void DataServer::on_close(websocketpp::connection_hdl hdl) {}
void DataServer::on_message(websocketpp::connection_hdl hdl,
    SocketServer::message_ptr msg) {
}

void DataServer::InitDataBridge() {
    spdlog::info("Init DataBridge");
    _bridge.Init(_config);

}
void DataServer::InitDBManager() {
    spdlog::info("Init DBManager");
    _db.Init();
}
void DataServer::InitDataMantainer() {
    spdlog::info("Init DataMaintainer");
    _mantainer.Init(_config.as_object(), &_db, &_bridge);
}