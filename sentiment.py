import tweepy
from textblob import TextBlob
import logging

# Set up logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

# Twitter API v2 Bearer Token (replace with your own)
BEARER_TOKEN = "AAAAAAAAAAAAAAAAAAAAAEto2QEAAAAAiOf1NI9GeMpsIPyTs2W4q36vdV4%3DzwoEbFu5f7p6OSGImZaT5BZrftpTgwLViWdxPqdP9YNwg93PJq"

# Initialize the Twitter API client
client = tweepy.Client(bearer_token=BEARER_TOKEN)

# Function to clean tweet text (remove URLs, mentions, hashtags)
def clean_tweet(tweet):
    words = tweet.split()
    cleaned_words = [word for word in words if not (word.startswith('http') or word.startswith('@') or word.startswith('#'))]
    return ' '.join(cleaned_words)

# Function to perform sentiment analysis
def analyze_sentiment(tweet_text):
    cleaned_tweet = clean_tweet(tweet_text)
    if not cleaned_tweet.strip():  # Skip empty tweets after cleaning
        return None, None
    analysis = TextBlob(cleaned_tweet)
    polarity = analysis.sentiment.polarity  # Score between -1.0 and 1.0
    # Classify sentiment
    if polarity > 0:
        sentiment = "positive"
    elif polarity < 0:
        sentiment = "negative"
    else:
        sentiment = "neutral"
    return polarity, sentiment

# Function to fetch and analyze tweets
def fetch_and_analyze_tweets(query, max_results=10):
    try:
        # Fetch recent tweets (Twitter API v2)
        tweets = client.search_recent_tweets(query=query, max_results=max_results, tweet_fields=["created_at"])
        
        if not tweets.data:
            logging.info("No tweets found for the query.")
            return
        
        # Analyze each tweet
        for tweet in tweets.data:
            polarity, sentiment = analyze_sentiment(tweet.text)
            if polarity is None:  # Skip if tweet is empty after cleaning
                continue
            logging.info(f"Tweet: {tweet.text}")
            logging.info(f"Polarity: {polarity:.2f}, Sentiment: {sentiment}")
            logging.info("-" * 50)
            
    except Exception as e:
        logging.error(f"Error fetching/analyzing tweets: {e}")

if __name__ == "__main__":
    # Query to search for (e.g., "Tesla")
    search_query = "Tesla -is:retweet lang:en"  # Exclude retweets, English only
    fetch_and_analyze_tweets(search_query, max_results=10)
