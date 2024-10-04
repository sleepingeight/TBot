#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <jsoncpp/json/json.h>
#include <cstdlib>  
#include <ctime>    
#include <boost/asio.hpp>  
#include <iostream>

typedef websocketpp::server<websocketpp::config::asio> server;
boost::asio::io_context io_context;

class MarketSimulator {
private:
    server m_server;
    websocketpp::connection_hdl m_hdl;
    boost::asio::steady_timer timer_;
    boost::asio::io_context& io_context_;  // Shared io_context for managing WebSocket and timer

public:
    MarketSimulator(boost::asio::io_context& io_context) 
        : timer_(io_context), io_context_(io_context) {
        std::srand(std::time(0));
    }

    void on_open(websocketpp::connection_hdl hdl) {
        m_hdl = hdl;
        send_prices();
    }

    void on_close(websocketpp::connection_hdl) {
        std::cout << "Connection closed" << std::endl;
    }



    // Function to send stock prices asynchronously
    void send_prices() {
        double stock_price = 50.0 + static_cast<double>(std::rand()) / (static_cast<double>(RAND_MAX / (150.0 - 50.0)));

        // Create JSON object for the price
        Json::Value price_data;
        price_data["price"] = stock_price;
        Json::StreamWriterBuilder writer;
        std::string message = Json::writeString(writer, price_data);

        // Send the message through WebSocket
        m_server.send(m_hdl, message, websocketpp::frame::opcode::text);
        std::cout << "Sent price: " << stock_price << std::endl;

        // Set the timer to expire after 1 second and then send the next price
        timer_.expires_after(std::chrono::seconds(1));
        timer_.async_wait([this](const boost::system::error_code& ec) {
            if (!ec) {
                send_prices();  
            } else {
                std::cerr << "Timer error: " << ec.message() << std::endl;
            }
        });
    }

    void run(uint16_t port) {
        m_server.init_asio(&io_context_);  // Use the same io_context for WebSocket and timer
        m_server.set_open_handler(websocketpp::lib::bind(&MarketSimulator::on_open, this, websocketpp::lib::placeholders::_1));
        m_server.set_close_handler(websocketpp::lib::bind(&MarketSimulator::on_close, this, websocketpp::lib::placeholders::_1));

        websocketpp::lib::error_code ec;
        m_server.listen(port, ec);
        if (ec) {
            std::cerr << "Listen failed: " << ec.message() << std::endl;
            return;
        }

        m_server.start_accept();
    }
};



void handle_signal(int signal) {
    std::cout << "\nClosing connection due to signal: " << signal << std::endl;

    io_context.stop(); 
}


int main() {
    MarketSimulator simulator(io_context);
    simulator.run(7999);  
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    signal(SIGTSTP, handle_signal); 

    io_context.run();  // Start the IO context event loop
}
