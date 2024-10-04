#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <fstream>  
#include <vector>
#include <jsoncpp/json/json.h>

typedef websocketpp::client<websocketpp::config::asio_client> client;

// Base class for Trading Strategy
class TradingStrategy {
public:
    virtual ~TradingStrategy() = default;
    virtual std::string analyse(double price) = 0; // Analyzes market data and returns "BUY", "SELL", or "HOLD"
};

// Moving Average Strategy
class MovingAverageStrategy : public TradingStrategy {
private:
    std::vector<double> prices;
    int window_size;

public:
    MovingAverageStrategy(int window) : window_size(window) {}

    std::string analyse(double price) override {
        prices.push_back(price);
        if (prices.size() < window_size) return "HOLD";

        double sum = 0;
        for (int i = prices.size() - window_size; i < prices.size(); ++i) {
            sum += prices[i];
        }
        double avg = sum / window_size;

        if (price > avg) return "BUY";
        else if (price < avg) return "SELL";
        else return "HOLD";
    }
};

// Momentum Strategy
class MomentumStrategy : public TradingStrategy {
public:
    std::string analyse(double price) override {
        static double last_price = 0;
        if (last_price == 0) {
            last_price = price;
            return "HOLD";
        }

        if (price > last_price) {
            last_price = price;
            return "BUY";
        } else if (price < last_price) {
            last_price = price;
            return "SELL";
        }
        return "HOLD";
    }
};

// Trading Bot with realistic balance and stock holdings
class TradingBot {
private:
    TradingStrategy* strategy1;
    TradingStrategy* strategy2;
    double balance;
    int stocks_held;

public:
    TradingBot(double initial_balance) 
        : strategy1(new MovingAverageStrategy(10)), 
          strategy2(new MomentumStrategy()), 
          balance(initial_balance), 
          stocks_held(0) {}

    void on_message(websocketpp::connection_hdl, client::message_ptr msg) {
        // Parse JSON data
        Json::CharReaderBuilder reader;
        Json::Value price_data;
        std::string errs;

        // Convert the payload (which is a string) to an input stream for parsing
        std::istringstream s(msg->get_payload());

        // Parse the JSON object
        if (!Json::parseFromStream(reader, s, &price_data, &errs)) {
            std::cerr << "Failed to parse JSON: " << errs << std::endl;
            return;
        }

        double stock_price;
        // Check if the 'price' field exists and extract it
        if (price_data.isMember("price") && price_data["price"].isNumeric()) {
            stock_price = price_data["price"].asDouble();
            std::cout << "Received stock price: " << stock_price << std::endl; 
        } else {
            std::cerr << "Invalid JSON format: 'price' field missing or not a number" << std::endl;
            return;
        }

        // Apply strategies to make decisions
        std::string action1 = strategy1->analyse(stock_price);
        // std::string action2 = strategy2->analyse(stock_price);

        execute_trade(action1, stock_price);
    }

    void execute_trade(const std::string &action, double price) {
        const double transaction_fee = 5.0;

        if (action == "BUY" && balance > price) {
            balance -= (price + transaction_fee);
            stocks_held++;
            log_trade("BUY", price);
            std::cout << "Bought 1 stock at price: " << price << ", Current Balance: " << balance 
                      << ", Stocks held: " << stocks_held << std::endl;

        } else if (action == "SELL" && stocks_held > 0) {
            balance += (price - transaction_fee);
            stocks_held--;
            log_trade("SELL", price);
            std::cout << "Sold 1 stock at price: " << price << ", Current Balance: " << balance 
                      << ", Stocks held: " << stocks_held << std::endl;

        } else {
            std::cout << "Action: " << action << " | No trade executed. Current Balance: " << balance 
                      << ", Stocks held: " << stocks_held << std::endl;
        }
    }

    void log_trade(const std::string &action, double price) {
        std::ofstream log_file("trade_log.txt", std::ios_base::app);
        log_file << std::fixed << std::setprecision(2)
                 << "Action: " << action << " | Price: " << price 
                 << " | Balance: " << balance << " | Stocks held: " << stocks_held << std::endl;
    }

    void run(const std::string &uri) {
        client bot_client;
        bot_client.init_asio();
        bot_client.set_message_handler([this](websocketpp::connection_hdl hdl, client::message_ptr msg) {
            this->on_message(hdl, msg);
        });

        websocketpp::lib::error_code ec;
        client::connection_ptr con = bot_client.get_connection(uri, ec);
        if (ec) {
            std::cerr << "Connection error: " << ec.message() << std::endl;
            return;
        }

        bot_client.connect(con);
        bot_client.run();  
    }


};


int main() {
    TradingBot bot(10000.0); // Starting with 10,000 rupees
    bot.run("ws://localhost:7999"); // Connect to the market simulator
    return 0;
}
