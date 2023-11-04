#include "../include/DataServer.h"

bool DataServer::Run(const std::string& configfile) {}
void DataServer::RegisterListener(websocketpp::connection_hdl hdl) {}
void DataServer::on_open(websocketpp::connection_hdl hdl) {}
void DataServer::on_close(websocketpp::connection_hdl hdl) {}
void DataServer::on_message(websocketpp::connection_hdl hdl,
    SocketServer::message_ptr msg) {
}