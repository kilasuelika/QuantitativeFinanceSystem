#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/client.hpp>

#include <functional>
//#include <fmt/format.h>
#include <format>
typedef websocketpp::server<websocketpp::config::asio> server;
typedef websocketpp::client<websocketpp::config::asio> client;

class Server
{
public:
    Server()
    {
        // Set logging settings
        s_.set_error_channels(websocketpp::log::elevel::all);
        s_.set_access_channels(websocketpp::log::alevel::all ^ websocketpp::log::alevel::frame_payload);

        // Initialize Asio
        s_.init_asio();

        //指定回调函数
        s_.set_open_handler(bind(&Server::on_open, this, std::placeholders::_1));
        s_.set_close_handler(bind(&Server::on_close, this, std::placeholders::_1));
        s_.set_message_handler(bind(&Server::on_message, this, std::placeholders::_1, std::placeholders::_2));
    }

    //在open时存储连接，便于后续主动发送消息
    void on_open(websocketpp::connection_hdl hdl)
    {
        std::cout << std::format("New connection\n");
        con = hdl;
    }

    void on_close(websocketpp::connection_hdl hdl)
    {
        // Handle a connection closing
    }

    void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg)
    {
        //读取内容
        std::print(std::cout, "[Server]: new message from client. Content: {}\n", msg->get_payload());
    }
    void run()
    {
        // Listen on port 9002
        s_.listen(9002);

        // Queues a connection accept operation
        s_.start_accept();

        // Start the Asio io_service run loop
        s_.run();
    }
    void send_message(const std::string& message)
    {
        websocketpp::lib::error_code ec;
        s_.send(con, message, websocketpp::frame::opcode::text, ec);

        if (ec)
        {
            std::cout << "Send Failed because: " << ec.message() << std::endl;
        }
        else
        {
            std::cout << "Send Success" << std::endl;
        }
    }

private:
    server s_;
    websocketpp::connection_hdl con;
};




class Client
{
public:


    Client()
    {
        // Set logging to be pretty verbose (everything except message payloads)
        c_.set_access_channels(websocketpp::log::alevel::all);
        c_.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize ASIO
        c_.init_asio();

        // Register our message handler
        c_.set_message_handler([this](websocketpp::connection_hdl hdl, client::message_ptr msg) {
            on_message(hdl, msg);
            });
    }

    void on_message(websocketpp::connection_hdl hdl, client::message_ptr msg)
    {
        std::print(std::cout, "[Client] New message from server: {}\n", msg->get_payload());
    }

    void run(std::string const& uri)
    {
        websocketpp::lib::error_code ec;

        con = c_.get_connection(uri, ec);

        if (ec)
        {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return;
        }

        c_.connect(con);
        c_.run();
    }

    void send_message(const std::string& message)
    {
        websocketpp::lib::error_code ec;
        c_.send(con, message, websocketpp::frame::opcode::text, ec);

        if (ec)
        {
            std::cout << "Send Failed because: " << ec.message() << std::endl;
        }
        else
        {
            std::cout << "Send Success" << std::endl;
        }
    }
private:
    client c_;
    client::connection_ptr con;
};

int main()
{
    Server s;
    auto server = std::async(std::launch::async, [&]() {s.run(); });

    //先睡眠一下，等待服务端启动
    std::this_thread::sleep_for(std::chrono::seconds(1));
    Client c;
    auto client = std::async(std::launch::async, [&]() {c.run("ws://127.0.0.1:9002"); });

    //从客户端发送消息
    std::this_thread::sleep_for(std::chrono::seconds(1));
    c.send_message("Hello, ");

    //从服务端发送消息
    std::this_thread::sleep_for(std::chrono::seconds(1));
    s.send_message("world!");

    server.get();
    client.get();
    return 0;
}