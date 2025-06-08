#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <jsoncpp/json/json.h>
#include <vector>
#include "curl_function.h"

using namespace std;

// Struct to hold the state of trading strategy
struct TradingState {
    float low = 0.0;
    float high = 0.0;
    float stopLoss = 0.0;
    float takeProfit = 0.0;
    bool lastSignalBuy = false;
    float buySignalHigh = 0.0;
    bool buySignalTriggered = false;
    float entryPrice = 0.0;
    bool inPosition = false;
    float thresholdHigh = 2.5;
    float thresholdMedium = 1.0;
    float thresholdNormal = -0.5;
};

// Calculates profit or loss percentage and prints the result
void ProfitLoss(float open, float close, string name);

// Handles user input for stock transactions and calculates cost if requested
void wantTransact(string ans, int amount, float price);

// Prints stock data to the console
void print_data(string name, string exchange, string currency, float open, float high, float low, float close, float volume, float change, float price);

// Sends a buy or sell signal based on price levels
void Sendsignal(float price, float high, float low, string ans, int amount);

// Calculates the mean of a vector of floats
float calculate_mean(const vector<float>& data);

// Calculates the standard deviation of a vector of floats given the mean
float calculate_stddev(const vector<float>& data, float mean);

// Updates the trading state based on historical data and current price
void update_trading_state(const vector<HistoricalData>& historical_data, TradingState& state, float current_price, float thresholdExtraHigh);

// Executes the trading strategy and updates the state
void execute_trading_strategy(TradingState& state, float current_price, string& ans, int& amount);

#endif // FUNCTIONS_H
