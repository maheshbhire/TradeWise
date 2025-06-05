#include "functions.h"
#include <iostream>
#include <bits/stdc++.h>
#include <cmath>

using namespace std;

void ProfitLoss(float open, float close, string name) {
    float percentage_change = ((close - open) / open) * 100;

    if (percentage_change > 0) {
        cout << "\nThe " + name + " stocks is in profit of " << percentage_change << "%\n";
    } else if (percentage_change < 0) {
        cout << "\nThe " + name + " stocks is in loss of " << percentage_change << "%\n";
    } else {
        cout << "\nThe " + name + " stocks is in a neutral state of " << percentage_change << "%\n";
    }
}

void wantTransact(string ans, int amount, float price) {
    cout << "Enter the amount of stock for buying/selling: ";
    cin >> amount;
    cout << "\nWant to know the cost of buying or selling stock? Enter Y for yes and N for no\n";
    cout << "Your Response: ";
    cin >> ans;
    transform(ans.begin(), ans.end(), ans.begin(), ::toupper);
    if (ans == "Y") {
        cout << "\nThe stock cost: " << amount * price << "\n";
    } else if (ans == "N") {
        cout << "\nOKAY\n\n";
    } else {
        cout << "\nInvalid Choice\n";
    }
}

void print_data(string name, string exchange, string currency, float open, float high, float low, float close, float volume, float change, float price) {
    cout << "\nGenerating Data....\n\n";
    cout <<
        "name: " << name << "\n" <<
        "exchange: " << exchange << "\n" <<
        "currency: " << currency << "\n" <<
        "Open price: " << open << "\n" <<
        "High price: " << high << "\n" <<
        "Low price: " << low << "\n" <<
        "Close price: " << close << "\n" <<
        "Volume: " << volume << "\n" <<
        "Change: " << change << "\n" <<
        "Current Price: " << price << "\n";
}

float calculate_mean(const vector<float>& data) {
    if (data.empty()) return 0.0;
    float sum = 0.0;
    for (float val : data) {
        sum += val;
    }
    return sum / data.size();
}

float calculate_stddev(const vector<float>& data, float mean) {
    if (data.empty()) return 0.0;
    float sum_of_squares = 0.0;
    for (float val : data) {
        float diff = val - mean;
        sum_of_squares += diff * diff;
    }
    return sqrt(sum_of_squares / data.size());
}

void update_trading_state(const vector<HistoricalData>& historical_data, TradingState& state, float current_price, float thresholdExtraHigh) {
    vector<float> volumes;
    float latest_high = 0.0, latest_low = 0.0;
    for (const auto& data : historical_data) {
        volumes.push_back(data.volume);
        latest_high = data.high;
        latest_low = data.low;
    }

    float mean = calculate_mean(volumes);
    float std = calculate_stddev(volumes, mean);

    float latest_volume = volumes[0];
    float stdbar = (latest_volume - mean) / std;

    bool conditionExtraHigh = stdbar > thresholdExtraHigh;
    bool conditionHigh = stdbar <= thresholdExtraHigh && stdbar > state.thresholdHigh;
    bool conditionMedium = stdbar <= state.thresholdHigh && stdbar > state.thresholdMedium;
    bool conditionNormal = stdbar <= state.thresholdMedium && stdbar > state.thresholdNormal;
    bool conditionLow = stdbar <= state.thresholdNormal;

    if (conditionExtraHigh) {
        state.low = latest_low;
        state.high = latest_high;
        state.stopLoss = state.low * 0.99;
        float sl_distance = state.high - state.stopLoss;
        state.takeProfit = state.high + sl_distance * 2;
        cout << "\nExtra High Volume Detected!\n";
        cout << "LOW: " << state.low << "\n";
        cout << "HIGH: " << state.high << "\n";
        cout << "Stop Loss: " << state.stopLoss << "\n";
        cout << "Take Profit: " << state.takeProfit << "\n";
    }
    if (conditionHigh) {
        cout << "\nHigh Volume Detected (stdbar: " << stdbar << ")\n";
    } else if (conditionMedium) {
        cout << "\nMedium Volume Detected (stdbar: " << stdbar << ")\n";
    } else if (conditionNormal) {
        cout << "\nNormal Volume Detected (stdbar: " << stdbar << ")\n";
    } else if (conditionLow) {
        cout << "\nLow Volume Detected (stdbar: " << stdbar << ")\n";
    }
}

void execute_trading_strategy(TradingState& state, float current_price, string& ans, int& amount) {
    bool buyCross = current_price > state.high;
    bool buySignal = buyCross && !state.lastSignalBuy;

    bool buyRule = false;
    if (buySignal) {
        state.buySignalHigh = state.high;
        state.buySignalTriggered = false;
        cout << "\nPotential Buy Signal: Price crossed above HIGH (" << state.high << ")\n";
    }
    if (state.buySignalHigh > 0 && current_price > state.buySignalHigh && !state.buySignalTriggered) {
        buyRule = true;
        state.buySignalTriggered = true;
        cout << "\nBUY Rule Triggered: Price confirmed above " << state.buySignalHigh << "\n";
    }

    bool sellCross = current_price < state.stopLoss;
    bool sellSignal = sellCross && state.lastSignalBuy;

    if (buyRule && !state.inPosition) {
        cout << "Entering Long Position at " << current_price << "\n";
        state.entryPrice = current_price;
        state.inPosition = true;
        state.lastSignalBuy = true;
        wantTransact(ans, amount, current_price);
    }

    if (state.inPosition && current_price >= state.stopLoss * 2) {
        cout << "Partial Exit: Selling 50% of position at " << current_price << "\n";
        wantTransact(ans, amount, current_price);
    }

    if (sellSignal && state.inPosition) {
        cout << "Stop Loss Hit: Exiting position at " << current_price << "\n";
        state.inPosition = false;
        state.lastSignalBuy = false;
        state.buySignalHigh = 0.0;
        state.buySignalTriggered = false;
        wantTransact(ans, amount, current_price);
    }
}

void Sendsignal(float price, float high, float low, string ans, int amount) {
    if (price <= low) {
        cout << "Sending Buying signal\n\n";
        wantTransact(ans, amount, price);
    } else if (price >= high) {
        cout << "Sending Selling signal\n\n";
        wantTransact(ans, amount, price);
    } else {
        cout << "Not suitable for buying and selling\nHappy Trading!!!\n\n";
    }
}