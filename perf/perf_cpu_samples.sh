#!/usr/bin/env bash

#source helper functions
__PERF_BASH_ENV="./perf_bash_env.sh"
source $__PERF_BASH_ENV || {
    echo "Missing $__PERF_BASH_ENV" >&2
    exit 1
}

PROC_CPU_SAMPLES=/mnt/qalogserver/data/perf/perf-tools/process_cpu_samples.py

proc_cpu_samples()
{
	echoinfo "$PROC_CPU_SAMPLES $1 -f mnvi.ko:mnvi_strategy -FPV -d10 -p0.01"
	$PROC_CPU_SAMPLES $1 -f mnvi.ko:mnvi_strategy -FPV -d10 -p0.01
}

usage()
{
	echo -e "Usage:"
	echo -e "\t$0 xxx.smpls.gz"
	echo "Example:"
	echo -e "\t$0 perf_manual_smpls/100.91.148.104_manual_17.smpls.gz"
}

main()
{
	if [ ! -e "$PROC_CPU_SAMPLES" ]; then
		echowarn "$PROC_CPU_SAMPLES does not exist!"
		usage
		exit 1
	fi
	proc_cpu_samples $1
}

### Main Entry ###
main "$@"
