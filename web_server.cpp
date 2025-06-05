#include "crow_all.h"
#include "functions.h"
#include "curl_function.h"
#include <jsoncpp/json/json.h>
#include <fstream>
#include <string>

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/api/stock")
    ([](const crow::request& req) {
        auto ticker = req.url_params.get("ticker");
        if (!ticker) return crow::response(400, "Ticker parameter is required");

        Json::Value config;
        std::ifstream config_file("config.json", std::ifstream::binary);
        if (!config_file.is_open()) return crow::response(500, "Failed to open config.json");

        config_file >> config;
        std::string api_key = config["api_key"].asString();

        // Step 1: Fetch stock data
        float price = std::stof(get_price(ticker, api_key));
        Json::Value quote = get_stock_quote(ticker, api_key);
        std::vector<HistoricalData> history = get_historical_data(ticker, api_key, 300);

        // Step 2: Extract required fields from quote
        std::string name = quote["name"].asString();
        std::string exchange = quote["exchange"].asString();
        std::string currency = quote["currency"].asString();
        float open = std::stof(quote["open"].asString());
        float high = std::stof(quote["high"].asString());
        float low = std::stof(quote["low"].asString());
        float close = std::stof(quote["close"].asString());
        float volume = std::stof(quote["volume"].asString());
        float change = std::stof(quote["change"].asString());

        // Step 3: Run trading logic
        TradingState state;
        state.thresholdHigh = 2.5;
        state.thresholdMedium = 1.0;
        state.thresholdNormal = -0.5;
        update_trading_state(history, state, price, 4.0);

        // Step 4: Sentiment analysis
        std::string cmd = "python3 sentiment.py " + std::string(ticker);
        FILE* pipe = popen(cmd.c_str(), "r");
        char buffer[128];
        std::string sentiment = "";
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            sentiment += buffer;
        }
        pclose(pipe);
        sentiment.erase(sentiment.find_last_not_of("\n\r") + 1);
        int sentiment_score = sentiment.empty() ? 0 : std::stoi(sentiment);

        // Step 5: Random Forest prediction
        std::string rf_cmd = "python3 predict_rf.py '{\"open\":" + std::to_string(open) +
                             ",\"high\":" + std::to_string(high) +
                             ",\"low\":" + std::to_string(low) +
                             ",\"close\":" + std::to_string(close) +
                             ",\"volume\":" + std::to_string(volume) + "}'";
        pipe = popen(rf_cmd.c_str(), "r");
        std::string rf_pred = "";
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            rf_pred += buffer;
        }
        pclose(pipe);
        rf_pred.erase(rf_pred.find_last_not_of("\n\r") + 1);
        int rf_signal = rf_pred.empty() ? 0 : std::stoi(rf_pred);

        // Step 6: Logistic Regression prediction
        std::string lr_cmd = "python3 predict_lr.py '{\"sentiment\":" + std::to_string(sentiment_score) +
                             ",\"rf_pred\":" + std::to_string(rf_signal) +
                             ",\"cpp_signal\":" + std::to_string(state.lastSignalBuy) + "}'";
        pipe = popen(lr_cmd.c_str(), "r");
        std::string lr_pred = "";
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            lr_pred += buffer;
        }
        pclose(pipe);
        lr_pred.erase(lr_pred.find_last_not_of("\n\r") + 1);
        bool final_buy = lr_pred.empty() ? false : (std::stoi(lr_pred) == 1);

        // Step 7: Prepare JSON response
        crow::json::wvalue result;
        result["symbol"] = ticker;
        result["name"] = name;
        result["exchange"] = exchange;
        result["currency"] = currency;
        result["price"] = price;
        result["open"] = open;
        result["high"] = high;
        result["low"] = low;
        result["close"] = close;
        result["volume"] = volume;
        result["change"] = change;
        result["sentiment"] = sentiment_score;
        result["rf_pred"] = rf_signal;
        result["cpp_signal"] = state.lastSignalBuy;
        result["final_buy"] = final_buy;
        result["analysis"]["high_volume"] = state.high;
        result["analysis"]["low_volume"] = state.low;
        result["analysis"]["stop_loss"] = state.stopLoss;
        result["analysis"]["take_profit"] = state.takeProfit;
        result["analysis"]["buy_signal"] = state.lastSignalBuy;

        return crow::response(result);
    });

    std::cout << "📡 Server running at http://localhost:8080/api/stock?ticker=AAPL" << std::endl;
    app.port(8080).multithreaded().run();
}