import sys
import joblib
import json

model = joblib.load('rf_model.pkl')
features = json.loads(sys.argv[1])
pred = model.predict([[
    features['open'],
    features['high'],
    features['low'],
    features['close'],
    features['volume']
]])
print(pred[0])  # 1=Buy, 0=No Buy