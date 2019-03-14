#!/bin/bash

if [ -a /proc/cpuinfo ]; then
    CPU_CORES_COUNT=$(expr $(grep 'processor' /proc/cpuinfo | wc -l) + 1)
fi

export CPU_CORES_COUNT
echo "CPU_CORES_COUNT=$CPU_CORES_COUNT"
