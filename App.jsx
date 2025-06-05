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
        <div className="stock-output">
          <h2>{stockData.name} ({stockData.symbol})</h2>
          <p>Exchange: {stockData.exchange}</p>
          <p>Currency: {stockData.currency}</p>
          <p>Price: ${stockData.price.toFixed(2)}</p>
          <p>Open: {stockData.open.toFixed(2)}</p>
          <p>High: {stockData.high.toFixed(2)}</p>
          <p>Low: {stockData.low.toFixed(2)}</p>
          <p>Close: {stockData.close.toFixed(2)}</p>
          <p>Volume: {stockData.volume.toLocaleString()}</p>
          <p>Change: {stockData.change.toFixed(2)}%</p>
          <hr />
          <h3>Signals</h3>
          <p>Sentiment: {stockData.sentiment === 1 ? 'Positive' : stockData.sentiment === -1 ? 'Negative' : 'Neutral'}</p>
          <p>Random Forest Prediction: {stockData.rf_pred ? 'Buy' : 'No Buy'}</p>
          <p>C++ Trading Signal: {stockData.cpp_signal ? 'Buy' : 'No Buy'}</p>
          <p><strong>Final Decision (Logistic Regression): {stockData.final_buy ? 'BUY' : 'NO BUY'}</strong></p>
          <hr />
          <h3>Analysis</h3>
          <p>High Volume: {stockData.analysis.high_volume.toFixed(2)}</p>
          <p>Low Volume: {stockData.analysis.low_volume.toFixed(2)}</p>
          <p>Stop Loss: ${stockData.analysis.stop_loss.toFixed(2)}</p>
          <p>Take Profit: ${stockData.analysis.take_profit.toFixed(2)}</p>
          <p>Buy Signal (C++): {stockData.analysis.buy_signal ? 'Yes' : 'No'}</p>
        </div>
      )}
    </div>
  );
}

export default App;