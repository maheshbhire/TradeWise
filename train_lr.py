from sklearn.linear_model import LogisticRegression
import pandas as pd
import joblib

# Simulated data (replace with real historical signals)
data = {
    'sentiment': [1, 0, -1, 1, 0, 1, -1, 0, 1, -1],
    'rf_pred': [1, 0, 0, 1, 1, 1, 0, 1, 1, 0],
    'cpp_signal': [1, 0, 0, 1, 1, 0, 1, 1, 0, 1],
    'should_buy': [1, 0, 0, 1, 1, 1, 0, 1, 1, 1]
}
df = pd.DataFrame(data)

X = df[['sentiment', 'rf_pred', 'cpp_signal']]
y = df['should_buy']

model = LogisticRegression(random_state=42)
model.fit(X, y)
joblib.dump(model, 'lr_model.pkl')
```

**Explanation**: Trains a Logistic Regression model on simulated signals (`sentiment`, `rf_pred`, `cpp_signal`) to predict `should_buy`. Saves the model to `lr_model.pkl`.

---

#### 13. `backend/predict_lr.py` (New)
<xaiArtifact artifact_id="59882e66-b275-40c0-a038-bc8e7afe1eeb" artifact_version_id="e1b89997-16da-4e92-b6a4-ab2c3d41bc3d" title="predict_lr.py" contentType="text/x-python">
import sys
import joblib
import json

model = joblib.load('lr_model.pkl')
features = json.loads(sys.argv[1])
pred = model.predict([[
    features['sentiment'],
    features['rf_pred'],
    features['cpp_signal']
]])
print(pred[0])  # 1=Buy, 0=No Buy