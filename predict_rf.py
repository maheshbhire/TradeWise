import sys
import joblib
import json
import logging

# Set up logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

def preprocess_features(features):
    """
    Preprocess input features to match training data.
    """
    try:
        # Calculate additional features
        features["price_change"] = (features["close"] - features["open"]) / features["open"]
        features["volatility"] = (features["high"] - features["low"]) / features["close"]
        # Assume volume change is 0 for single data point
        features["volume_change"] = 0.0
        return [
            features["open"],
            features["high"],
            features["low"],
            features["close"],
            features["volume"],
            features["price_change"],
            features["volatility"],
            features["volume_change"]
        ]
    except Exception as e:
        logging.error(f"Feature preprocessing failed: {e}")
        return None

if __name__ == "__main__":
    if len(sys.argv) != 2:
        logging.error("Usage: python3 predict_rf.py '<json_data>'")
        print(0)  # Default to No Buy on error
        sys.exit(1)
    
    try:
        model = joblib.load("rf_model.pkl")
        scaler = joblib.load("scaler.pkl")
        features = json.loads(sys.argv[1])
        
        processed_features = preprocess_features(features)
        if processed_features is None:
            print(0)
            sys.exit(1)
        
        # Scale features
        scaled_features = scaler.transform([processed_features])
        
        # Predict
        pred = model.predict(scaled_features)
        logging.info(f"Random Forest prediction: {pred[0]}")
        print(pred[0])  # 1=Buy, 0=No Buy
    except Exception as e:
        logging.error(f"Prediction failed: {e}")
        print(0)
        sys.exit(1)
