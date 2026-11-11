#!/bin/bash

THREADS=${1:-4}
COUNT=${2:-3000}

./build/lab3 -t "$THREADS" -n "$COUNT" &
PID=$!
sleep 1

echo "----- ps -L -p $PID -----"
ps -L -o pid,lwp,nlwp,pcpu,stat,comm -p "$PID"

echo "----- /proc/$PID/status -----"
grep Threads /proc/"$PID"/status

echo "----- /proc/$PID/task -----"
ls /proc/"$PID"/task

wait "$PID"
