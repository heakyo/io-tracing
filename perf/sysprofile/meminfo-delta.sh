#!/usr/bin/env bash
set -euo pipefail

# ===== 参数处理 =====
INTERVAL="${1:-10}"
if ! [[ "$INTERVAL" =~ ^[0-9]+([.][0-9]+)?$ ]]; then
  echo "Usage: $0 [interval_seconds]"
  echo "Example: $0        # default 10s"
  echo "Example: $0 30     # 30s"
  exit 1
fi

LOG_FILE="meminfo_delta_$(date +%Y%m%d_%H%M%S).log"

declare -A BEFORE AFTER
declare -a KEYS

read_meminfo() {
  local -n ref=$1
  while read -r key value unit; do
    key="${key%:}"
    # /proc/meminfo 主体字段均为整数(kB)
    ref["$key"]="$value"
  done < /proc/meminfo
}

write_header() {
  {
    echo "===== /proc/meminfo Delta Report ====="
    echo "StartTime : $START_TIME"
    echo "EndTime   : $END_TIME"
    echo "Interval  : ${INTERVAL}s"
    echo "LogFile   : $LOG_FILE"
    echo
    printf "%-24s %14s %14s %14s\n" "Metric" "Before(kB)" "After(kB)" "Delta(kB)"
    printf "%-24s %14s %14s %14s\n" "------------------------" "----------" "---------" "---------"
  } >> "$LOG_FILE"
}

collect_and_print_delta() {
  # 收集所有键（并集）
  local k
  for k in "${!BEFORE[@]}"; do KEYS+=("$k"); done
  for k in "${!AFTER[@]}"; do KEYS+=("$k"); done

  # 去重+排序
  mapfile -t KEYS < <(printf "%s\n" "${KEYS[@]}" | sort -u)

  for k in "${KEYS[@]}"; do
    local b="${BEFORE[$k]:-0}"
    local a="${AFTER[$k]:-0}"
    local d=$((a - b))
    printf "%-24s %14d %14d %+14d\n" "$k" "$b" "$a" "$d" >> "$LOG_FILE"
  done
}

append_conclusion() {
  local memavail_delta=$(( ${AFTER[MemAvailable]:-0} - ${BEFORE[MemAvailable]:-0} ))
  local memfree_delta=$(( ${AFTER[MemFree]:-0} - ${BEFORE[MemFree]:-0} ))
  local cached_delta=$(( ${AFTER[Cached]:-0} - ${BEFORE[Cached]:-0} ))
  local dirty_delta=$(( ${AFTER[Dirty]:-0} - ${BEFORE[Dirty]:-0} ))
  local swapfree_delta=$(( ${AFTER[SwapFree]:-0} - ${BEFORE[SwapFree]:-0} ))
  local slab_delta=$(( ${AFTER[Slab]:-0} - ${BEFORE[Slab]:-0} ))
  local sreclaimable_delta=$(( ${AFTER[SReclaimable]:-0} - ${BEFORE[SReclaimable]:-0} ))
  # 修正上面可能拼写问题（防御写法）
  local sunreclaim_delta=$(( ${AFTER[SUnreclaim]:-0} - ${BEFORE[SUnreclaim]:-0} ))

  # 阈值：10MB
  local THRESHOLD=10240

  {
    echo
    echo "===== Conclusion ====="
    echo "1) MemAvailable Delta: ${memavail_delta} kB"
    echo "2) MemFree Delta     : ${memfree_delta} kB"
    echo "3) Cached Delta      : ${cached_delta} kB"
    echo "4) Dirty Delta       : ${dirty_delta} kB"
    echo "5) SwapFree Delta    : ${swapfree_delta} kB"
    echo "6) Slab Delta        : ${slab_delta} kB (SReclaimable=${sreclaimable_delta}, SUnreclaim=${sunreclaim_delta})"
    echo
    echo "Potential causes:"
    
    if (( memavail_delta < -THRESHOLD && cached_delta > THRESHOLD )); then
      echo "- 可用内存下降且 Cached 增长：可能是文件缓存(page cache)增长，常见于读文件/IO扫描，未必是异常泄漏。"
    fi

    if (( dirty_delta > THRESHOLD )); then
      echo "- Dirty 明显上升：可能有大量写入尚未回刷磁盘（写缓存堆积）。"
    fi

    if (( swapfree_delta < -THRESHOLD )); then
      echo "- SwapFree 下降：系统可能发生内存压力并开始使用 swap。"
    fi

    if (( slab_delta > THRESHOLD || sunreclaim_delta > THRESHOLD )); then
      echo "- Slab/SUnreclaim 上升：可能是内核对象缓存增长，需关注内核态内存占用。"
    fi

    if (( memfree_delta < -THRESHOLD && memavail_delta > -THRESHOLD )); then
      echo "- MemFree 下降但 MemAvailable 变化不大：多为正常缓存行为，不一定是问题。"
    fi

    if (( memavail_delta > THRESHOLD )); then
      echo "- 可用内存上升：可能是负载结束或缓存被回收。"
    fi

    if (( memavail_delta > -THRESHOLD && memavail_delta < THRESHOLD && \
          swapfree_delta > -THRESHOLD && swapfree_delta < THRESHOLD && \
          slab_delta > -THRESHOLD && slab_delta < THRESHOLD )); then
      echo "- 整体变化较小：当前间隔内内存状态基本稳定。"
    fi

    echo
    echo "Recommendation:"
    echo "- 若怀疑泄漏，建议连续采样（如每10秒采样10分钟）并结合 top/pmap/slabtop/vmstat 联合分析。"
  } >> "$LOG_FILE"
}

START_TIME="$(date '+%F %T')"
read_meminfo BEFORE
sleep "$INTERVAL"
read_meminfo AFTER
END_TIME="$(date '+%F %T')"

write_header
collect_and_print_delta
append_conclusion

echo "Done. Report written to: $LOG_FILE"
