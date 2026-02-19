#!/bin/bash
set -e

cd /home/ctl/projects/SimpleDataServer

echo "Creating lib directory..."
mkdir -p lib/nlohmann lib/uWebSockets

echo "Downloading nlohmann/json..."
curl -L -o lib/nlohmann/json.hpp https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp

echo "Cloning uWebSockets..."
git clone --depth 1 https://github.com/uNetworking/uWebSockets.git lib/uWebSockets

echo "Initializing uSockets submodule..."
cd lib/uWebSockets
git submodule update --init --depth 1 uSockets

echo "Done!"
