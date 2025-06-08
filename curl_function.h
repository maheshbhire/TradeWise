#ifndef CURL_FUNCTION_H
#define CURL_FUNCTION_H

#include <string>
#include <jsoncpp/json/json.h>
#include <vector>

using namespace std;

// Struct to hold historical stock data
struct HistoricalData {
    float open;   // Added open price
    float close;
    float high;
    float low;
    float volume;
};

// Fetches the current price for a given ticker symbol
string get_price(const string& ticker_symbol, const string& api);

// Fetches detailed stock quote information
Json::Value get_stock_quote(const string& ticker_symbol, const string& api);

// Fetches historical data for a given period
vector<HistoricalData> get_historical_data(const string& ticker_symbol, const string& api, int period);

#endif // CURL_FUNCTION_H
