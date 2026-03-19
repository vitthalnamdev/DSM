#!/usr/bin/env bash
set -euo pipefail

rm -f *.o main

g++ -std=c++17 -I./headers \
    main.cpp \
    $(find src -name '*.cpp') \
    -luring -pthread \
    -o main