#include "curl_function.h"
#include <jsoncpp/json/json.h>
#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <ticker> <api_key> <period>" << std::endl;
        return 1;
    }

    std::string ticker = argv[1];
    std::string api_key = argv[2];
    int period = std::stoi(argv[3]);

    std::vector<HistoricalData> history = get_historical_data(ticker, api_key, period);
    if (history.empty()) {
        std::cerr << "No historical data retrieved." << std::endl;
        return 1;
    }

    Json::Value root;
    Json::Value values(Json::arrayValue);
    for (const auto& data : history) {
        Json::Value entry;
        entry["close"] = data.close;
        entry["high"] = data.high;
        entry["low"] = data.low;
        entry["volume"] = data.volume;
        values.append(entry);
    }
    root["values"] = values;

    std::ofstream out("data.json");
    if (!out.is_open()) {
        std::cerr << "Failed to open data.json for writing." << std::endl;
        return 1;
    }
    out << root;
    out.close();

    std::cout << "Historical data saved to data.json" << std::endl;
    return 0;
}
