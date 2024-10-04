# TBot
A basic trading bot implemented in C++.

## Features

1. Market Simulator: A market simulator is implemented that updates the stock price every second.
2. WebSocket Connection: The bot and the market simulator communicate using WebSocket instead of an API. This choice minimizes latency, which is important for real-time trading systems.
3. Trading Strategies: Two trading strategies are included in the bot: Moving Average and Momentum. Other strategies can be easily added due to the use of Object-Oriented Programming (OOP) principles.
4. Logbook: A logbook feature stores all previous trades made by the bot.
5. JSON Data Format: Data is sent through WebSocket in JSON format for easy sending and parsing.
6. Asynchronous I/O: The market simulator supports asynchronous input and output, making them non-blocking and improving performance.

## Design Choices

1. Choice of Language: C++ was chosen because it allows low-level programming, which helps reduce latency and optimize the bot for performance.
2. Library Usage: The project uses C++ libraries such as WebSocket++, JSONCPP, and Boost to implement the above features effectively.

## Installation

### Prerequisites
The following dependencies need to be installed (Linux only):
```bash
# Install Boost and JSON libraries
sudo apt install libboost-all-dev libjsoncpp-dev libwebsocketpp-dev

# Clone the repository
git clone https://github.com/sleepingeight/TBot.git
cd TBot

# Clone WebSocketPP library
git clone https://github.com/zaphoyd/websocketpp.git
```

### Compilation
```bash
# Compile Market Simulator
g++ -o market_simulator market_simulator.cpp -I./websocketpp -lboost_system -lpthread -ljsoncpp

# Compile Trading Bot
g++ -o trading_bot trading_bot.cpp -I./websocketpp -lboost_system -lpthread -ljsoncpp
```

## Important Notes
- **Platform Compatibility**: This code works only on Linux systems.
- **Port Configuration**: Before running, ensure to modify the port numbers to any free port numbers on your system:
  - Port number in `market_simulator.cpp` (line 88)
  - Port number in `trading_bot.cpp` (line 165)
