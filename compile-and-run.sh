#!/bin/bash
# Specify the names of the output executables
MAIN_EXECUTABLE="output/trade"
SERVER_EXECUTABLE="output/web_server"
FETCH_EXECUTABLE="output/fetch_data"

# Create output directory if it doesn't exist
mkdir -p output

# Compile main.cpp
g++ -o "$MAIN_EXECUTABLE" main.cpp curl_functions.cpp functions.cpp -lcurl -ljsoncpp
if [ $? -eq 0 ]; then
    echo "Compilation of main.cpp successful."
else
    echo "Compilation of main.cpp failed."
    exit 1
fi

# Compile web_server.cpp
g++ -o "$SERVER_EXECUTABLE" web_server.cpp curl_functions.cpp functions.cpp -lcurl -ljsoncpp
if [ $? -eq 0 ]; then
    echo "Compilation of web_server.cpp successful."
else
    echo "Compilation of web_server.cpp failed."
    exit 1
fi

# Compile fetch_data.cpp
g++ -o "$FETCH_EXECUTABLE" fetch_data.cpp curl_functions.cpp -lcurl -ljsoncpp
if [ $? -eq 0 ]; then
    echo "Compilation of fetch_data.cpp successful."
else
    echo "Compilation of fetch_data.cpp failed."
    exit 1
fi

# Run the web server
echo "Running the web server..."
./"$SERVER_EXECUTABLE"
