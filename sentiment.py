import sys
import pandas as pd
from vaderSentiment.vaderSentiment import SentimentIntensityAnalyzer
import requests
import json
import logging

# Set up logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

def get_tweets(ticker, api_key):
    """
    Fetch tweets related to the ticker from a hypothetical Twitter/X API.
    Replace with actual Twitter/X API endpoint and authentication.
    """
    try:
        # Placeholder URL for Twitter/X API (replace with actual endpoint)
        url = f"https://api.twitter.com/2/tweets/search/recent?query={ticker}%20stock"
        headers = {"Authorization": f"Bearer {api_key}"}
        response = requests.get(url, headers=headers)
        response.raise_for_status()
        tweets_data = response.json().get('data', [])
        return [tweet['text'] for tweet in tweets_data]
    except Exception as e:
        logging.error(f"Failed to fetch tweets: {e}")
        return []  # Return empty list on failure

def analyze_sentiment(ticker, twitter_api_key=None):
    """
    Analyze sentiment for the given ticker using VADER.
    """
    try:
        analyzer = SentimentIntensityAnalyzer()
        tweets = get_tweets(ticker, twitter_api_key) if twitter_api_key else [
            f"{ticker} stock is soaring today!",
            f"Not sure about {ticker}, might drop soon.",
            f"{ticker} is a solid buy right now!"
        ]  # Fallback to mock data if no API key

        if not tweets:
            logging.warning(f"No tweets found for {ticker}. Returning neutral sentiment.")
            return 0

        total_score = 0
        for text in tweets:
            score = analyzer.polarity_scores(text)
            total_score += score['compound']
        
        average = total_score / len(tweets)
        sentiment = 1 if average >= 0.2 else -1 if average <= -0.2 else 0
        logging.info(f"Sentiment for {ticker}: {sentiment} (Average score: {average})")
        return sentiment
    except Exception as e:
        logging.error(f"Sentiment analysis failed for {ticker}: {e}")
        return 0

if __name__ == "__main__":
    if len(sys.argv) != 2:
        logging.error("Usage: python3 sentiment.py <ticker>")
        print(0)  # Neutral sentiment on error
        sys.exit(1)
    
    ticker = sys.argv[1].upper()
    # Replace with your Twitter/X API key or set to None for mock data
    twitter_api_key = None  # Example: "your_twitter_api_key_here"
    sentiment = analyze_sentiment(ticker, twitter_api_key)
    print(sentiment)
