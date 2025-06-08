import sys
import joblib
import json
import logging
import os

# Set up logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

if __name__ == "__main__":
    if len(sys.argv) != 2:
        logging.error("Usage: python3 predict_lr.py '<json_data>'")
        print(0)  # Default to No Buy on error
        sys.exit(1)

    try:
        # Load model
        if not os.path.exists("lr_model.pkl"):
            logging.error("Logistic Regression model file 'lr_model.pkl' not found")
            print(0)
            sys.exit(1)
        model = joblib.load("lr_model.pkl")

        # Parse input features
        features = json.loads(sys.argv[1])
        required_fields = ["sentiment", "rf_pred", "cpp_signal"]
        for field in required_fields:
            if field not in features:
                logging.error(f"Missing required field: {field}")
                print(0)
                sys.exit(1)

        # Predict
        pred = model.predict([[
            features["sentiment"],
            features["rf_pred"],
            features["cpp_signal"]
        ]])
        logging.info(f"Logistic Regression prediction: {pred[0]}")
        print(pred[0])  # 1=Buy, 0=No Buy
    except Exception as e:
        logging.error(f"Prediction failed: {e}")
        print(0)
        sys.exit(1)
