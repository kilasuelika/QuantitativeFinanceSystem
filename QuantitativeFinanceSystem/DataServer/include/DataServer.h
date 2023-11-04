#pragma once

#include <string>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/client.hpp>
#include "DBManager.h"

class DataServer {
public:
    using SocketServer = websocketpp::server<websocketpp::config::asio>;
    using SocketClient = websocketpp::client<websocketpp::config::asio>;

    bool Run(const std::string& configfile);
    // 对于前端，用来注册以主动推送数据。
    void RegisterListener(websocketpp::connection_hdl hdl);

    void on_open(websocketpp::connection_hdl hdl);
    void on_close(websocketpp::connection_hdl hdl);
    void on_message(websocketpp::connection_hdl hdl, SocketServer::message_ptr msg);

private:
    int _port;
    SocketServer _s;
    std::vector<websocketpp::connection_hdl> _listeners;

    DBManager _db;

};