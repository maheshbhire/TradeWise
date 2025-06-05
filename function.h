#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <jsoncpp/json/json.h>
#include <vector>
#include "curl_function.h"

using namespace std;

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

void ProfitLoss(float open, float close, string name);
void wantTransact(string ans, int amount, float price);
void print_data(string name, string exchange, string currency, float open, float high, float low, float close, float volume, float change, float price);
void Sendsignal(float price, float high, float low, string ans, int amount);

float calculate_mean(const vector<float>& data);
float calculate_stddev(const vector<float>& data, float mean);
void update_trading_state(const vector<HistoricalData>& historical_data, TradingState& state, float current_price, float thresholdExtraHigh);
void execute_trading_strategy(TradingState& state, float current_price, string& ans, int& amount);

#endif // FUNCTIONS_H