#!/bin/bash

# Benchmark script for BFS implementations with larger graphs
# Tests performance with different thread counts

# Choose which dataset to use
if [ "$1" == "web" ]; then
    DATASET="data/web-Google.txt"
    echo "Using web-Google dataset"
elif [ "$1" == "road" ]; then
    DATASET="data/roadNet-CA.txt"
    echo "Using roadNet-CA dataset"
else
    echo "Usage: $0 [web|road]"
    echo "  web: Use web-Google dataset"
    echo "  road: Use roadNet-CA dataset"
    exit 1
fi

SOURCE=0
DIRECTED=1
RUNS=3

echo "==== BFS Performance Benchmark ===="
echo "Dataset: $DATASET"
echo "Source vertex: $SOURCE"
echo "Run count: $RUNS"
echo "=================================="

# Create directory for results
mkdir -p results
RESULT_DIR="results/$(basename $DATASET .txt)"
mkdir -p $RESULT_DIR

# Run sequential first to get baseline
echo -e "\nRunning Sequential BFS for baseline..."
./build/bfs_program "$DATASET" "$SOURCE" "$DIRECTED" "$RUNS" 1 | tee $RESULT_DIR/sequential.log
# Extract sequential time for speedup calculation
SEQ_TIME=$(grep "Average Sequential BFS time:" $RESULT_DIR/sequential.log | awk '{print $5}')

if [ -z "$SEQ_TIME" ]; then
    # If average time isn't available (single run), get the execution time
    SEQ_TIME=$(grep "Execution time:" $RESULT_DIR/sequential.log | awk '{print $3}')
fi

echo -e "\nBaseline sequential time: $SEQ_TIME ms"

# Test with different thread counts
echo -e "\n==== OpenMP Thread Scaling ===="
echo "Threads | Time (ms) | Speedup | MTEPS" | tee $RESULT_DIR/scaling.txt
echo "----------------------------------------" | tee -a $RESULT_DIR/scaling.txt

# Get max threads available on system
MAX_THREADS=$(nproc)
echo "Maximum available threads: $MAX_THREADS"

# Test with different thread counts (powers of 2)
for THREADS in 1 2 4 8 16 $(nproc)
do
    # Skip duplicates or if threads > MAX_THREADS
    if [[ "$THREADS" -gt "$MAX_THREADS" ]] || 
       [[ "$THREADS" -eq 16 && "$MAX_THREADS" -lt 16 ]] ||
       [[ "$THREADS" -eq 8 && "$MAX_THREADS" -eq 8 && "$THREADS" -ne $(nproc) ]]; then
        continue
    fi
    
    echo -e "\nRunning OpenMP BFS with $THREADS threads..."
    ./build/bfs_program "$DATASET" "$SOURCE" "$DIRECTED" "$RUNS" "$THREADS" | tee $RESULT_DIR/openmp_${THREADS}.log
    
    # Extract results
    OMP_TIME=$(grep "Average OpenMP BFS time:" $RESULT_DIR/openmp_${THREADS}.log | awk '{print $5}')
    if [ -z "$OMP_TIME" ]; then
        # If average time isn't available (single run)
        OMP_TIME=$(grep "Execution time:" $RESULT_DIR/openmp_${THREADS}.log | grep "OpenMP" | awk '{print $3}')
    fi
    
    SPEEDUP=$(grep "Average speedup:" $RESULT_DIR/openmp_${THREADS}.log | awk '{print $3}')
    if [ -z "$SPEEDUP" ]; then
        # Calculate speedup manually
        SPEEDUP=$(echo "scale=2; $SEQ_TIME / $OMP_TIME" | bc)
    fi
    
    # Get MTEPS if available
    MTEPS=$(grep "MTEPS:" $RESULT_DIR/openmp_${THREADS}.log | grep "OpenMP" | awk '{print $3}')
    
    echo "$THREADS | $OMP_TIME | ${SPEEDUP}x | $MTEPS" | tee -a $RESULT_DIR/scaling.txt
done

echo -e "\nComplete results saved in $RESULT_DIR/"

# Generate summary
echo -e "\n==== Performance Summary ====" | tee $RESULT_DIR/summary.txt
echo "Graph: $(basename $DATASET .txt)" | tee -a $RESULT_DIR/summary.txt
echo "Vertices: $(grep "Vertices:" $RESULT_DIR/sequential.log | awk '{print $2}')" | tee -a $RESULT_DIR/summary.txt
echo "Edges: $(grep "Edges:" $RESULT_DIR/sequential.log | awk '{print $2}')" | tee -a $RESULT_DIR/summary.txt
echo "Sequential time: $SEQ_TIME ms" | tee -a $RESULT_DIR/summary.txt

# Fix the best parallel time extraction - skip header line and pick the fastest time
echo "Best parallel time: $(grep -v "Threads \|------" $RESULT_DIR/scaling.txt | sort -n -k 2 -t '|' | head -1 | awk -F '|' '{print $2}' | xargs) ms" | tee -a $RESULT_DIR/summary.txt

# Fix the max speedup and best thread count extraction - skip header line
echo "Max speedup: $(grep -v "Threads \|------" $RESULT_DIR/scaling.txt | sort -n -r -k 3 -t '|' | head -1 | awk -F '|' '{print $3}' | xargs)" | tee -a $RESULT_DIR/summary.txt
echo "Best thread count: $(grep -v "Threads \|------" $RESULT_DIR/scaling.txt | sort -n -r -k 3 -t '|' | head -1 | awk -F '|' '{print $1}' | xargs)" | tee -a $RESULT_DIR/summary.txt