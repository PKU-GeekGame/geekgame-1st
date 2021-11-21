#!/bin/bash

python3 example.py > /dev/null 2>&1 &


while [[ true ]]; do
    sleep 10;
done
