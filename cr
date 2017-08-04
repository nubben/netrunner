#!/bin/bash
physicalCpuCount=$([[ $(uname) = 'Darwin' ]] &&
                       sysctl -n hw.physicalcpu_max ||
                       lscpu -p | egrep -v '^#' | sort -u -t, -k 2,4 | wc -l)
make -j$physicalCpuCount && ./netrunner http://motherfuckingwebsite.com/ -log debug
