#! /bin/bash

echo "make all"
if [ ! -d "bin" ]; then
    mkdir bin
fi
g++ src/server.cpp -o bin/server
g++ src/client.cpp -o bin/client
g++ src/http/http_server.cpp -o bin/http_server