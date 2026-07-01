#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"
g++ -std=c++17 -O2 -I include -pthread main.cpp DiagnosticEngine.cpp -o hantavirus_server
echo "Built ./hantavirus_server — run it with: ./hantavirus_server"
