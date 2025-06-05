#ifndef CURL_FUNCTION_H
#define CURL_FUNCTION_H

#include <string>
#include <jsoncpp/json/json.h>
#include <vector>

using namespace std;

struct HistoricalData {
    float close;
    float high;
    float low;
    float volume;
};

string get_price(const string& ticker_symbol, const string& api);
Json::Value get_stock_quote(const string& ticker_symbol, const string& api);
vector<HistoricalData> get_historical_data(const string& ticker_symbol, const string& api, int period);

#endif // CURL_FUNCTION_H