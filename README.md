Tradewise
A comprehensive stock trading application that combines real-time stock data, sentiment analysis, machine learning predictions, and trading logic to provide actionable buy/sell signals. The backend is built with C++ and Python, using the Twelve Data API, while the frontend is a React-based web interface.
Features

Real-Time Data: Fetches stock prices, quotes, and historical data from Twelve Data API.
Volume Climax Indicator (VCI): Identifies trading opportunities based on volume spikes.
Sentiment Analysis: Analyzes mock tweet sentiment using VADER.
Machine Learning:
Random Forest predicts buy/sell based on stock data.
Logistic Regression combines sentiment, Random Forest, and C++ signals for final decisions.


React UI: Displays stock details, sentiment, and trading signals.
Trade Management: Includes stop-loss, take-profit, and partial exits.

Project Structure
stock-dashboard/
├── backend/
│   ├── main.cpp              # Console-based app
│   ├── web_server.cpp        # API server
│   ├── curl_function.h       # API call declarations
│   ├── curl_functions.cpp    # API call implementations
│   ├── functions.h           # Trading logic declarations
│   ├── functions.cpp         # Trading logic implementations
│   ├── config.json           # API key
│   ├── compile-and-run.sh    # Compilation script
│   ├── sentiment.py          # Sentiment analysis
│   ├── train_rf.py           # Train Random Forest
│   ├── predict_rf.py         # Predict with Random Forest
│   ├── train_lr.py           # Train Logistic Regression
│   ├── predict_lr.py         # Predict with Logistic Regression
├── frontend/
│   ├── App.jsx           # React component
│   ├── App.css           # Styles
│   ├── index.js          # React entry
│   ├── public/
│       ├── index.html        # HTML template
│       ├── package.json     # Dependencies
├── README.md

Prerequisites

Backend:
C++ compiler (g++)
libcurl4-openssl-dev: sudo apt-get install libcurl4-dev
libjsoncpp-dev: sudo apt-get install libjsoncpp-dev
Python 3: sudo apt-get install python3`
Python libraries: pip install pandas numpy scikit-learn vaderSentiment joblib
Twelve Data API key (sign up at Twelve Data)


Frontend:
Node.js and npmsudo apt-get install npm



Installation

Clone the Repository:

   git clone https://github.com/your_username/stock.git
   cd stock-dashboard


Set Up Backend:
Create config.json in backend/:{
  "api_key":  "your_twelve_data_api_key_here"
}


Install dependencies:



     sudo apt-get install install libcurl4-dev libjsoncpp-dev
     pip install pandas numpy scikit-learn vaderSentiment joblib


Set Up Frontend:

   - Navigate to `frontend`:
     cd frontend
     npm install

Usage

Train Models:

   cd backend
   python3 train_rf.py
   python3 train_lr.py


Run Backend:

   cd backend
   ./compile-and-run.sh


Run Frontend:

   cd frontend
   npm start


Access:
Open http://localhost:3000 in your browser.
Enter a ticker (e.g., AAPL) to view stock data, sentiment, and trading signals.



Authors

Mahesh Hire (GitHub)

Notes

Sentiment analysis uses mock data; replace with Twitter API for real tweets.
ML models use simulated data; integrate with real historical data for accuracy.

