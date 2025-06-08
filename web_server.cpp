#include "crow_all.h"
#include "functions.h"
#include "curl_function.h"
#include <jsoncpp/json/json.h>
#include <fstream>
#include <string>
#include <stdexcept>
#include <iostream>

int main() {
    crow::SimpleApp app;

    // Existing endpoint for real-time stock data
    CROW_ROUTE(app, "/api/stock")
    ([](const crow::request& req) {
        auto ticker = req.url_params.get("ticker");
        if (!ticker) {
            return crow::response(400, "Ticker parameter is required");
        }

        Json::Value config;
        std::ifstream config_file("config.json", std::ifstream::binary);
        if (!config_file.is_open()) {
            return crow::response(500, "Failed to open config.json");
        }

        try {
            config_file >> config;
        } catch (const std::exception& e) {
            return crow::response(500, "Failed to parse config.json: " + std::string(e.what()));
        }
        config_file.close();

        if (!config.isMember("api_key")) {
            return crow::response(500, "API key missing in config.json");
        }
        std::string api_key = config["api_key"].asString();
        if (api_key.empty()) {
            return crow::response(500, "API key is empty in config.json");
        }

        float price;
        try {
            std::string price_str = get_price(ticker, api_key);
            if (price_str.empty()) {
                return crow::response(500, "Failed to fetch stock price");
            }
            price = std::stof(price_str);
        } catch (const std::exception& e) {
            return crow::response(500, "Invalid price data: " + std::string(e.what()));
        }

        Json::Value quote = get_stock_quote(ticker, api_key);
        if (quote.isNull()) {
            return crow::response(500, "Failed to fetch stock quote");
        }

        std::vector<HistoricalData> history = get_historical_data(ticker, api_key, 300);
        if (history.empty()) {
            return crow::response(500, "Failed to fetch historical data");
        }

        std::string name = quote["name"].asString();
        std::string exchange = quote["exchange"].asString();
        std::string currency = quote["currency"].asString();
        float open, high, low, close, volume, change;
        try {
            open = std::stof(quote["open"].asString());
            high = std::stof(quote["high"].asString());
            low = std::stof(quote["low"].asString());
            close = std::stof(quote["close"].asString());
            volume = std::stof(quote["volume"].asString());
            change = std::stof(quote["change"].asString());
        } catch (const std::exception& e) {
            return crow::response(500, "Invalid stock quote data: " + std::string(e.what()));
        }

        TradingState state;
        state.thresholdHigh = 2.5;
        state.thresholdMedium = 1.0;
        state.thresholdNormal = -0.5;
        update_trading_state(history, state, price, 4.0);

        std::string cmd = "python3 sentiment.py " + std::string(ticker);
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) {
            return crow::response(500, "Failed to execute sentiment analysis");
        }
        char buffer[128];
        std::string sentiment = "";
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            sentiment += buffer;
        }
        pclose(pipe);
        sentiment.erase(sentiment.find_last_not_of("\n\r") + 1);
        int sentiment_score;
        try {
            sentiment_score = sentiment.empty() ? 0 : std::stoi(sentiment);
        } catch (const std::exception& e) {
            return crow::response(500, "Invalid sentiment score: " + std::string(e.what()));
        }

        std::string rf_cmd = "python3 predict_rf.py '{\"open\":" + std::to_string(open) +
                             ",\"high\":" + std::to_string(high) +
                             ",\"low\":" + std::to_string(low) +
                             ",\"close\":" + std::to_string(close) +
                             ",\"volume\":" + std::to_string(volume) + "}'";
        pipe = popen(rf_cmd.c_str(), "r");
        if (!pipe) {
            return crow::response(500, "Failed to execute Random Forest prediction");
        }
        std::string rf_pred = "";
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            rf_pred += buffer;
        }
        pclose(pipe);
        rf_pred.erase(rf_pred.find_last_not_of("\n\r") + 1);
        int rf_signal;
        try {
            rf_signal = rf_pred.empty() ? 0 : std::stoi(rf_pred);
        } catch (const std::exception& e) {
            return crow::response(500, "Invalid Random Forest prediction: " + std::string(e.what()));
        }

        std::string lr_cmd = "python3 predict_lr.py '{\"sentiment\":" + std::to_string(sentiment_score) +
                             ",\"rf_pred\":" + std::to_string(rf_signal) +
                             ",\"cpp_signal\":" + std::to_string(state.lastSignalBuy) + "}'";
        pipe = popen(lr_cmd.c_str(), "r");
        if (!pipe) {
            return crow::response(500, "Failed to execute Logistic Regression prediction");
        }
        std::string lr_pred = "";
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            lr_pred += buffer;
        }
        pclose(pipe);
        lr_pred.erase(lr_pred.find_last_not_of("\n\r") + 1);
        bool final_buy;
        try {
            final_buy = lr_pred.empty() ? false : (std::stoi(lr_pred) == 1);
        } catch (const std::exception& e) {
            return crow::response(500, "Invalid Logistic Regression prediction: " + std::string(e.what()));
        }

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

    // New endpoint to generate training data for Logistic Regression
    CROW_ROUTE(app, "/api/generate_training_data")
    ([](const crow::request& req) {
        auto ticker = req.url_params.get("ticker");
        if (!ticker) {
            return crow::response(400, "Ticker parameter is required");
        }

        Json::Value config;
        std::ifstream config_file("config.json", std::ifstream::binary);
        if (!config_file.is_open()) {
            return crow::response(500, "Failed to open config.json");
        }

        try {
            config_file >> config;
        } catch (const std::exception& e) {
            return crow::response(500, "Failed to parse config.json: " + std::string(e.what()));
        }
        config_file.close();

        if (!config.isMember("api_key")) {
            return crow::response(500, "API key missing in config.json");
        }
        std::string api_key = config["api_key"].asString();
        if (api_key.empty()) {
            return crow::response(500, "API key is empty in config.json");
        }

        // Fetch historical data for 365 days
        std::vector<HistoricalData> history = get_historical_data(ticker, api_key, 365);
        if (history.empty()) {
            return crow::response(500, "Failed to fetch historical data");
        }

        // Open CSV file to store training data
        std::ofstream csv_file("training_data.csv");
        if (!csv_file.is_open()) {
            return crow::response(500, "Failed to open training_data.csv for writing");
        }

        // Write CSV header
        csv_file << "sentiment,rf_pred,cpp_signal,should_buy\n";

        // Process each historical data point
        for (size_t i = 0; i < history.size() - 1; i++) { // -1 to avoid last day (no next day for should_buy)
            // Use a subset of historical data up to the current day
            std::vector<HistoricalData> sub_history(history.begin(), history.begin() + i + 1);
            if (sub_history.size() < 2) continue; // Need at least 2 days for trading logic

            // Current day's data
            float open = history[i].open;
            float high = history[i].high;
            float low = history[i].low;
            float close = history[i].close;
            float volume = history[i].volume;
            float price = close; // Use closing price as current price

            // Run trading logic
            TradingState state;
            state.thresholdHigh = 2.5;
            state.thresholdMedium = 1.0;
            state.thresholdNormal = -0.5;
            update_trading_state(sub_history, state, price, 4.0);

            // Sentiment analysis (simplified; in reality, you'd need historical sentiment)
            std::string cmd = "python3 sentiment.py " + std::string(ticker);
            FILE* pipe = popen(cmd.c_str(), "r");
            if (!pipe) {
                csv_file.close();
                return crow::response(500, "Failed to execute sentiment analysis");
            }
            char buffer[128];
            std::string sentiment = "";
            while (fgets(buffer, sizeof buffer, pipe) != NULL) {
                sentiment += buffer;
            }
            pclose(pipe);
            sentiment.erase(sentiment.find_last_not_of("\n\r") + 1);
            int sentiment_score;
            try {
                sentiment_score = sentiment.empty() ? 0 : std::stoi(sentiment);
            } catch (const std::exception& e) {
                csv_file.close();
                return crow::response(500, "Invalid sentiment score: " + std::string(e.what()));
            }

            // Random Forest prediction
            std::string rf_cmd = "python3 predict_rf.py '{\"open\":" + std::to_string(open) +
                                 ",\"high\":" + std::to_string(high) +
                                 ",\"low\":" + std::to_string(low) +
                                 ",\"close\":" + std::to_string(close) +
                                 ",\"volume\":" + std::to_string(volume) + "}'";
            pipe = popen(rf_cmd.c_str(), "r");
            if (!pipe) {
                csv_file.close();
                return crow::response(500, "Failed to execute Random Forest prediction");
            }
            std::string rf_pred = "";
            while (fgets(buffer, sizeof buffer, pipe) != NULL) {
                rf_pred += buffer;
            }
            pclose(pipe);
            rf_pred.erase(rf_pred.find_last_not_of("\n\r") + 1);
            int rf_signal;
            try {
                rf_signal = rf_pred.empty() ? 0 : std::stoi(rf_pred);
            } catch (const std::exception& e) {
                csv_file.close();
                return crow::response(500, "Invalid Random Forest prediction: " + std::string(e.what()));
            }

            // C++ signal from trading logic
            int cpp_signal = state.lastSignalBuy ? 1 : 0;

            // Determine should_buy: 1 if next day's close is higher, 0 otherwise
            float next_close = history[i + 1].close;
            int should_buy = (next_close > close) ? 1 : 0;

            // Write to CSV
            csv_file << sentiment_score << "," << rf_signal << "," << cpp_signal << "," << should_buy << "\n";
        }

        csv_file.close();
        return crow::response(200, "Training data generated and saved to training_data.csv");
    });

    std::cout << "📡 Server running at http://localhost:8080/api/stock?ticker=AAPL" << std::endl;
    std::cout << "📊 Training data endpoint: http://localhost:8080/api/generate_training_data?ticker=AAPL" << std::endl;
    app.port(8080).multithreaded().run();
}
