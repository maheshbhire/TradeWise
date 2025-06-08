#include "curl_function.h"
#include <iostream>
#include <curl/curl.h>
#include <string>
#include <vector>

// Callback function for curl to write response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

string get_price(const string& ticker_symbol, const string& api) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize cURL." << endl;
        return "";
    }

    string url = "https://api.twelvedata.com/price?symbol=" + ticker_symbol + "&apikey=" + api;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    string jsonResponse;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &jsonResponse);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        cerr << "cURL request failed: " << curl_easy_strerror(res) << endl;
        curl_easy_cleanup(curl);
        return "";
    }

    curl_easy_cleanup(curl);

    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();
    Json::Value root;
    string errors;
    if (!reader->parse(jsonResponse.c_str(), jsonResponse.c_str() + jsonResponse.size(), &root, &errors)) {
        cerr << "Failed to parse JSON: " << errors << endl;
        delete reader;
        return "";
    }
    delete reader;

    if (!root.isMember("price")) {
        cerr << "Price field missing in API response." << endl;
        return "";
    }

    string price = root["price"].asString();
    return price;
}

Json::Value get_stock_quote(const string& ticker_symbol, const string& api) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize cURL." << endl;
        return Json::Value();
    }

    string url = "https://api.twelvedata.com/quote?symbol=" + ticker_symbol + "&apikey=" + api;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    string jsonResponse;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &jsonResponse);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        cerr << "cURL request failed: " << curl_easy_strerror(res) << endl;
        curl_easy_cleanup(curl);
        return Json::Value();
    }

    curl_easy_cleanup(curl);

    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();
    Json::Value root;
    string errors;
    if (!reader->parse(jsonResponse.c_str(), jsonResponse.c_str() + jsonResponse.size(), &root, &errors)) {
        cerr << "Failed to parse JSON: " << errors << endl;
        delete reader;
        return Json::Value();
    }
    delete reader;

    // Validate required fields
    const char* required_fields[] = {"name", "exchange", "currency", "open", "high", "low", "close", "volume", "change"};
    for (const auto& field : required_fields) {
        if (!root.isMember(field)) {
            cerr << "Missing required field in stock quote: " << field << endl;
            return Json::Value();
        }
    }

    return root;
}

vector<HistoricalData> get_historical_data(const string& ticker_symbol, const string& api, int period) {
    CURL* curl = curl_easy_init();
    vector<HistoricalData> historical_data;

    if (!curl) {
        cerr << "Failed to initialize cURL." << endl;
        return historical_data;
    }

    string url = "https://api.twelvedata.com/time_series?symbol=" + ticker_symbol +
                 "&interval=1day&outputsize=" + to_string(period) + "&apikey=" + api;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    string jsonResponse;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &jsonResponse);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        cerr << "cURL request failed: " << curl_easy_strerror(res) << endl;
        curl_easy_cleanup(curl);
        return historical_data;
    }

    curl_easy_cleanup(curl);

    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();
    Json::Value root;
    string errors;
    if (!reader->parse(jsonResponse.c_str(), jsonResponse.c_str() + jsonResponse.size(), &root, &errors)) {
        cerr << "Failed to parse JSON: " << errors << endl;
        delete reader;
        return historical_data;
    }
    delete reader;

    if (!root["values"].isArray()) {
        cerr << "No historical data found." << endl;
        return historical_data;
    }

    for (const auto& data : root["values"]) {
        // Validate required fields
        if (!data.isMember("open") || !data.isMember("close") || !data.isMember("high") || 
            !data.isMember("low") || !data.isMember("volume")) {
            cerr << "Missing required fields in historical data entry." << endl;
            continue;
        }

        HistoricalData entry;
        entry.open = stof(data["open"].asString());
        entry.close = stof(data["close"].asString());
        entry.high = stof(data["high"].asString());
        entry.low = stof(data["low"].asString());
        entry.volume = stof(data["volume"].asString());
        historical_data.push_back(entry);
    }

    return historical_data;
}
