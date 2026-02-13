#!/usr/bin/bash

set -euo pipefail

OUTPUT_DIR="${1:-/tmp/perf_snapshot_$(date +%Y%m%d_%H%M%S)}"
mkdir -p "$OUTPUT_DIR"

log() {
	echo "[$(date '+%Y-%m-%d %H:%M:%S')] $*"
}

collect() {
	local name="$1"
	shift
	log "采集 $name ..."
	"$@" > "$OUTPUT_DIR/$name.txt" 2>&1 || echo"采集 $name 失败: $?" >> "$OUTPUT_DIR/errors.log"
}

log "开始采集，输出目录: $OUTPUT_DIR"

# 基础信息
collect "uname" uname -a
collect "uptime" uptime
collect "date" date '+%Y-%m-%d %H:%M:%S %Z'
collect "hostname" hostname -f
collect "nproc" nproc
