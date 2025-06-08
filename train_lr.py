import pandas as pd
import numpy as np
from sklearn.linear_model import LogisticRegression
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score, classification_report
import joblib
import logging
import os

# Set up logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

def train_logistic_regression():
    """
    Train a Logistic Regression model using historical data from training_data.csv.
    """
    try:
        # Check if training data exists
        if not os.path.exists("training_data.csv"):
            logging.error("training_data.csv not found. Please run /api/generate_training_data endpoint first.")
            raise FileNotFoundError("training_data.csv not found")

        # Load data
        df = pd.read_csv("training_data.csv")
        logging.info(f"Loaded {len(df)} rows of training data")

        # Validate data
        required_columns = ['sentiment', 'rf_pred', 'cpp_signal', 'should_buy']
        for col in required_columns:
            if col not in df.columns:
                logging.error(f"Missing required column: {col}")
                raise ValueError(f"Missing required column: {col}")

        # Prepare features and target
        X = df[['sentiment', 'rf_pred', 'cpp_signal']]
        y = df['should_buy']

        # Handle missing or invalid data
        X = X.fillna(0)  # Replace NaN with 0
        y = y.fillna(0).astype(int)

        # Split data for training and evaluation
        X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)
        logging.info(f"Training set size: {len(X_train)}, Test set size: {len(X_test)}")

        # Train model
        model = LogisticRegression(random_state=42)
        model.fit(X_train, y_train)

        # Evaluate model
        y_pred = model.predict(X_test)
        accuracy = accuracy_score(y_test, y_pred)
        report = classification_report(y_test, y_pred, target_names=['No Buy', 'Buy'])
        logging.info(f"Model accuracy: {accuracy:.2f}")
        logging.info("Classification Report:\n" + report)

        # Save model
        joblib.dump(model, 'lr_model.pkl')
        logging.info("Logistic Regression model saved to lr_model.pkl")

        return model

    except Exception as e:
        logging.error(f"Failed to train Logistic Regression model: {e}")
        raise

if __name__ == "__main__":
    train_logistic_regression()
