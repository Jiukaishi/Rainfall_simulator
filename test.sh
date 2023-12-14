#!/bin/bash

count=0

count=$((count+1))
echo "Running Program $count"
./rainfall_pt_v2 1 20 0.5 16 ./hw5/sample_16x16.in > "output$count.txt"

count=$((count+1))
echo "Running Program $count"
./rainfall_pt_v2 2 20 0.5 16 ./hw5/sample_16x16.in > "output$count.txt"

count=$((count+1))
echo "Running Program $count"
./rainfall_pt_v2 4 20 0.5 16 ./hw5/sample_16x16.in > "output$count.txt"

count=$((count+1))
echo "Running Program $count"
./rainfall_pt_v2 8 20 0.5 16 ./hw5/sample_16x16.in > "output$count.txt"

echo "All test finished."
