#include <iostream>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

void on_message(client *c, websocketpp::connection_hdl hdl, const message_ptr& msg) {
    try {
        std::string body = msg->get_payload();
        std::cout << "received message: " << body << std::endl;
    } catch(websocketpp::exception &e) {
        std::cout << e.what() << "\n";
    }
}

static context_ptr on_tls_init() {
    // establishes a SSL connection
    context_ptr ctx = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

    try {
        ctx->set_options(boost::asio::ssl::context::default_workarounds |
                         boost::asio::ssl::context::no_sslv2 |
                         boost::asio::ssl::context::no_sslv3 |
                         boost::asio::ssl::context::single_dh_use);
    } catch (std::exception &e) {
        std::cout << e.what() << "\n";
    }
    return ctx;
}

void setupwebosocket() {
    // Create a client endpoint
    client c;

    std::string uri = "url";

    try {
        c.set_access_channels(websocketpp::log::alevel::none);
        c.clear_access_channels(websocketpp::log::alevel::none);

        // Initialize ASIO
        c.init_asio();
        c.set_tls_init_handler(bind(&on_tls_init));

        // Register our message handler
        c.set_message_handler(bind(&on_message,&c,::_1,::_2));

        websocketpp::lib::error_code ec;
        client::connection_ptr con = c.get_connection(uri, ec);
        if (ec) {
            std::cout << "Connection error: " << ec.message() << "\n";
            return;
        }

        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
        c.connect(con);

        // Start the ASIO io_service run loop
        // this will cause a single connection to be made to the server. c.run()
        // will exit when this connection is closed.
        c.run();
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << "\n";
    }
}

int main() {
    setupwebosocket();

    return 0;
}
