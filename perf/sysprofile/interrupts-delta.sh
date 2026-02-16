#!/bin/bash

TMP1=$(mktemp)
TMP2=$(mktemp)

cat /proc/interrupts > "$TMP1"
sleep 10
cat /proc/interrupts > "$TMP2"

paste "$TMP1" "$TMP2" | awk '
{
    printf "%-35s", $1
    for (i=2; i<=NF/2; i++) {
        delta = $(i + NF/2) - $i
        printf "%10d", delta
    }
    print ""
}
'

rm -f "$TMP1" "$TMP2"
