#!/bin/bash

set -e

LOG=mpstat_cpu0.log
RUNTIME=$1

if [[ -z "$RUNTIME" || ! "$RUNTIME" =~ ^[0-9]+$ || "$RUNTIME" -le 0 ]]; then
    echo "Usage: $0 <runtime_seconds>   (must be an integer > 0)"
    exit 1
fi

stdbuf -oL -eL mpstat -P 0 1 \
| stdbuf -oL -eL awk '
  /^Linux/ {print; next}
  /^..:..:..[[:space:]]+CPU/ { if (hdr++) next; print; next }
  $1 ~ /^[0-9]/ {print}
' > "$LOG" &
PIPE_PID=$!

echo "mpstat pipeline pid = $PIPE_PID"

# trap 'command' signal
# When the script exits, the command below would be executed.
trap 'kill "$PIPE_PID" 2>/dev/null || true; wait "$PIPE_PID" 2>/dev/null || true' EXIT

echo "Command: taskset -c 0 iperf3 -c 10.124.59.13 -P 1 -t $RUNTIME"
taskset -c 0 iperf3 -c 10.124.59.13 -P 1 -t $RUNTIME
