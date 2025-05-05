#!/bin/bash

# Benchmark script for BFS implementations
# Tests performance with different thread counts

DATASET="data/facebook_combined.txt"
SOURCE=0
DIRECTED=1
RUNS=3

echo "==== BFS Performance Benchmark ===="
echo "Dataset: $DATASET"
echo "Source vertex: $SOURCE"
echo "Run count: $RUNS"
echo "=================================="

# Run sequential first to get baseline
echo -e "\nRunning Sequential BFS for baseline..."
./build/bfs_program "$DATASET" "$SOURCE" "$DIRECTED" "$RUNS" 1 | grep -E "BFS Performance|Average|Speedup" > sequential_results.txt
cat sequential_results.txt

# Extract sequential time for speedup calculation
SEQ_TIME=$(grep "Average Sequential BFS time:" sequential_results.txt | awk '{print $5}')
echo -e "\nBaseline sequential time: $SEQ_TIME ms"

# Test with different thread counts
echo -e "\n==== OpenMP Thread Scaling ===="
echo "Threads | Time (ms) | Speedup"
echo "----------------------------"

# Get max threads available on system
MAX_THREADS=$(nproc)
echo "Maximum available threads: $MAX_THREADS"

# Test with different thread counts
for THREADS in 1 2 4 8 $(nproc)
do
    # Skip duplicates (if nproc is 4, don't repeat the test)
    if [[ "$THREADS" -gt "$MAX_THREADS" ]] || 
       [[ "$THREADS" -eq 8 && "$MAX_THREADS" -lt 8 ]] ||
       [[ "$THREADS" -eq 4 && "$MAX_THREADS" -eq 4 && "$THREADS" -ne $(nproc) ]]; then
        continue
    fi
    
    echo -e "\nRunning OpenMP BFS with $THREADS threads..."
    ./build/bfs_program "$DATASET" "$SOURCE" "$DIRECTED" "$RUNS" "$THREADS" | grep -E "OpenMP BFS time:|Speedup" > openmp_${THREADS}_results.txt
    
    # Extract results
    OMP_TIME=$(grep "Average OpenMP BFS time:" openmp_${THREADS}_results.txt | awk '{print $5}')
    SPEEDUP=$(grep "Average speedup:" openmp_${THREADS}_results.txt | awk '{print $3}')
    
    # If speedup isn't available, calculate it
    if [ -z "$SPEEDUP" ]; then
        SPEEDUP=$(echo "scale=2; $SEQ_TIME / $OMP_TIME" | bc)
    fi
    
    echo "$THREADS | $OMP_TIME | ${SPEEDUP}x"
done

echo -e "\nComplete results are in *_results.txt files"