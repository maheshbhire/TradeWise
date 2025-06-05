from sklearn.ensemble import RandomForestClassifier
import pandas as pd
import joblib

# Simulated historical data (replace with real data from curl_functions)
data = {
    'open': [300, 302, 298, 305, 310, 308, 306, 312, 315, 320],
    'high': [310, 308, 304, 312, 315, 313, 310, 318, 320, 325],
    'low': [295, 297, 294, 300, 305, 303, 302, 308, 310, 315],
    'close': [305, 304, 300, 308, 312, 310, 307, 315, 318, 322],
    'volume': [100000, 120000, 110000, 130000, 140000, 125000, 115000, 135000, 145000, 150000],
    'should_buy': [1, 0, 0, 1, 1, 1, 0, 1, 1, 1]
}
df = pd.DataFrame(data)

X = df[['open', 'high', 'low', 'close', 'volume']]
y = df['should_buy']

model = RandomForestClassifier(n_estimators=100, random_state=42)
model.fit(X, y)
joblib.dump(model, 'rf_model.pkl')