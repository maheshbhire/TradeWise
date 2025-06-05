#!/bin/bash
# Specify the name of the output executable for main.cpp
MAIN_EXECUTABLE="output/trade"
# Specify the name of the output executable for web_server.cpp
SERVER_EXECUTABLE="output/web_server"

# Create output directory if it doesn't exist
mkdir -p output

# Compile main.cpp
g++ -o "$MAIN_EXECUTABLE" main.cpp curl_functions.cpp functions.cpp -lcurl -ljsoncpp

# Check if main.cpp compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation of main.cpp successful."
else
    echo "Compilation of main.cpp failed."
    exit 1
fi

# Compile web_server.cpp
g++ -o "$SERVER_EXECUTABLE" web_server.cpp curl_functions.cpp functions.cpp -lcurl -ljsoncpp

# Check if web_server.cpp compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation of web_server.cpp successful. Running the web server..."
    ./"$SERVER_EXECUTABLE"
else
    echo "Compilation of web_server.cpp failed."
    exit 1
fi