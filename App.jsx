import React, { useState } from 'react';
import axios from 'axios';
import './App.css';

function App() {
  const [ticker, setTicker] = useState('');
  const [stockData, setStockData] = useState(null);
  const [error, setError] = useState('');
  const [loading, setLoading] = useState(false);

  const fetchStock = async () => {
    if (!ticker) {
      setError('Please enter a ticker symbol.');
      return;
    }

    setLoading(true);
    try {
      const res = await axios.get(`http://localhost:8080/api/stock?ticker=${ticker}`);
      setStockData(res.data);
      setError('');
    } catch (err) {
      setError('Failed to fetch stock data.');
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="container">
      <h1>Stock Trading Dashboard</h1>
      <div className="input-group">
        <input
          type="text"
          value={ticker}
          onChange={(e) => setTicker(e.target.value.toUpperCase())}
          placeholder="Enter ticker symbol (e.g., AAPL)"
        />
        <button onClick={fetchStock} disabled={loading}>
          {loading ? 'Loading...' : 'Get Stock Data'}
        </button>
      </div>
      {error && <p className="error">{error}</p>}
      {loading && <p className="loading">Loading...</p>}
      {stockData && (
        <div className="dashboard-grid">
          {/* Stock Overview */}
          <div className="card overview">
            <h2>{stockData.name} ({stockData.symbol})</h2>
            <p><strong>Exchange:</strong> {stockData.exchange}</p>
            <p><strong>Currency:</strong> {stockData.currency}</p>
            <p><strong>Price:</strong> ${stockData.price.toFixed(2)}</p>
            <p><strong>Change:</strong> {stockData.change.toFixed(2)}%</p>
          </div>

          {/* Price Chart */}
          <div className="card chart">
            <h3>Price Overview</h3>
            <div className="chart-container">
              ```chartjs
              {
                "type": "bar",
                "data": {
                  "labels": ["Open", "High", "Low", "Close"],
                  "datasets": [{
                    "label": "Price ($)",
                    "data": [
                      ${stockData.open.toFixed(2)},
                      ${stockData.high.toFixed(2)},
                      ${stockData.low.toFixed(2)},
                      ${stockData.close.toFixed(2)}
                    ],
                    "backgroundColor": [
                      "rgba(54, 162, 235, 0.7)",
                      "rgba(75, 192, 192, 0.7)",
                      "rgba(255, 99, 132, 0.7)",
                      "rgba(153, 102, 255, 0.7)"
                    ],
                    "borderColor": [
                      "rgba(54, 162, 235, 1)",
                      "rgba(75, 192, 192, 1)",
                      "rgba(255, 99, 132, 1)",
                      "rgba(153, 102, 255, 1)"
                    ],
                    "borderWidth": 1
                  }]
                },
                "options": {
                  "scales": {
                    "y": {
                      "beginAtZero": false,
                      "title": {
                        "display": true,
                        "text": "Price ($)"
                      }
                    }
                  },
                  "plugins": {
                    "legend": {
                      "display": false
                    }
                  }
                }
              }
              ```
            </div>
          </div>

          {/* Trading Signals */}
          <div className="card signals">
            <h3>Trading Signals</h3>
            <p><strong>Sentiment:</strong> {stockData.sentiment === 1 ? 'Positive' : stockData.sentiment === -1 ? 'Negative' : 'Neutral'}</p>
            <p><strong>Random Forest Prediction:</strong> {stockData.rf_pred ? 'Buy' : 'No Buy'}</p>
            <p><strong>C++ Trading Signal:</strong> {stockData.cpp_signal ? 'Buy' : 'No Buy'}</p>
            <p><strong>Final Decision (Logistic Regression):</strong> <span className={stockData.final_buy ? 'buy' : 'no-buy'}>{stockData.final_buy ? 'BUY' : 'NO BUY'}</span></p>
          </div>

          {/* Analysis */}
          <div className="card analysis">
            <h3>Analysis</h3>
            <p><strong>High Volume:</strong> {stockData.analysis.high_volume.toFixed(2)}</p>
            <p><strong>Low Volume:</strong> {stockData.analysis.low_volume.toFixed(2)}</p>
            <p><strong>Stop Loss:</strong> ${stockData.analysis.stop_loss.toFixed(2)}</p>
            <p><strong>Take Profit:</strong> ${stockData.analysis.take_profit.toFixed(2)}</p>
            <p><strong>Buy Signal (C++):</strong> {stockData.analysis.buy_signal ? 'Yes' : 'No'}</p>
          </div>
        </div>
      )}
    </div>
  );
}

export default App;
