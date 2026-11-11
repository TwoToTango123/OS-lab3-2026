#!/bin/bash

SIZES="200 500 1000 2000"
THREADS="1 2 4 8 12 16 24 32"
RUNS=3

for n in $SIZES; do
    echo "n = $n"
    printf "%8s %12s %10s %12s\n" "threads" "time_ms" "speedup" "efficiency"
    t1=""
    for t in $THREADS; do
        best=""
        for r in $(seq $RUNS); do
            ms=$(./build/lab3 -t "$t" -n "$n" | grep "time:" | awk '{print $2}')
            if [ -z "$best" ] || [ "$(echo "$ms < $best" | bc)" -eq 1 ]; then
                best=$ms
            fi
        done
        if [ -z "$t1" ]; then
            t1=$best
        fi
        s=$(echo "scale=3; $t1 / $best" | bc)
        e=$(echo "scale=3; $s / $t" | bc)
        printf "%8s %12s %10s %12s\n" "$t" "$best" "$s" "$e"
    done
    echo
done
