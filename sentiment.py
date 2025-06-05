import sys
import pandas as pd
from vaderSentiment.vaderSentiment import SentimentIntensityAnalyzer

# Mock tweet data (replace with actual Twitter scraping)
def get_tweets(ticker):
    # Simulated tweets for demonstration
    return [
        f"{ticker} stock is soaring today!",
        f"Not sure about {ticker}, might drop soon.",
        f"{ticker} is a solid buy right now!"
    ]

analyzer = SentimentIntensityAnalyzer()
ticker = sys.argv[1]
tweets = get_tweets(ticker)
total_score = 0
for text in tweets:
    score = analyzer.polarity_scores(text)
    total_score += score['compound']
average = total_score / len(tweets) if tweets else 0
sentiment = 1 if average >= 0.2 else -1 if average <= -0.2 else 0
print(sentiment)