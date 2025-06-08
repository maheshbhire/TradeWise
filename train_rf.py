import pandas as pd
import numpy as np
from sklearn.ensemble import RandomForestClassifier
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import train_test_split
import joblib
import json
import subprocess
import logging

# Set up logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

def fetch_historical_data(ticker, api_key, period=300):
    """
    Fetch historical data using the C++ curl_functions via a subprocess.
    Assumes curl_functions.cpp outputs JSON to a file.
    """
    try:
        # Run a C++ program that calls get_historical_data and saves to data.json
        cmd = f"./output/fetch_data {ticker} {api_key} {period}"
        subprocess.run(cmd, shell=True, check=True)
        
        with open("data.json", "r") as f:
            data = json.load(f)
        
        if not data or "values" not in data:
            logging.error(f"No historical data found for {ticker}")
            return None
        
        df = pd.DataFrame(data["values"])
        df["close"] = df["close"].astype(float)
        df["high"] = df["high"].astype(float)
        df["low"] = df["low"].astype(float)
        df["volume"] = df["volume"].astype(float)
        return df[["open", "high", "low", "close", "volume"]]
    except Exception as e:
        logging.error(f"Failed to fetch historical data: {e}")
        return None

def prepare_data(df):
    """
    Preprocess data and create target variable (1=Buy, 0=No Buy).
    Simple strategy: Buy if next day's close > current close.
    """
    try:
        df = df.copy()
        # Create features
        df["price_change"] = df["close"].pct_change()
        df["volatility"] = (df["high"] - df["low"]) / df["close"]
        df["volume_change"] = df["volume"].pct_change()
        
        # Create target: 1 if next day's close is higher, 0 otherwise
        df["should_buy"] = (df["close"].shift(-1) > df["close"]).astype(int)
        
        # Drop NaN rows
        df = df.dropna()
        
        features = ["open", "high", "low", "close", "volume", "price_change", "volatility", "volume_change"]
        X = df[features]
        y = df["should_buy"]
        
        return X, y
    except Exception as e:
        logging.error(f"Data preparation failed: {e}")
        return None, None

def train_model(X, y):
    """
    Train Random Forest model with feature scaling.
    """
    try:
        # Split data
        X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)
        
        # Scale features
        scaler = StandardScaler()
        X_train_scaled = scaler.fit_transform(X_train)
        X_test_scaled = scaler.transform(X_test)
        
        # Train model
        model = RandomForestClassifier(n_estimators=100, random_state=42)
        model.fit(X_train_scaled, y_train)
        
        # Evaluate model
        score = model.score(X_test_scaled, y_test)
        logging.info(f"Model accuracy on test set: {score:.2f}")
        
        # Save model and scaler
        joblib.dump(model, "rf_model.pkl")
        joblib.dump(scaler, "scaler.pkl")
        return model, scaler
    except Exception as e:
        logging.error(f"Model training failed: {e}")
        return None, None

if __name__ == "__main__":
    # Read API key from config.json
    try:
        with open("config.json", "r") as f:
            config = json.load(f)
        api_key = config["api_key"]
    except Exception as e:
        logging.error(f"Failed to read config.json: {e}")
        sys.exit(1)
    
    ticker = "AAPL"  # Default ticker; can be passed as argument
    if len(sys.argv) > 1:
        ticker = sys.argv[1].upper()
    
    # Fetch and prepare data
    df = fetch_historical_data(ticker, api_key)
    if df is None:
        sys.exit(1)
    
    X, y = prepare_data(df)
    if X is None or y is None:
        sys.exit(1)
    
    # Train and save model
    model, scaler = train_model(X, y)
    if model is None or scaler is None:
        sys.exit(1)
    
    logging.info(f"Random Forest model trained and saved for {ticker}")
