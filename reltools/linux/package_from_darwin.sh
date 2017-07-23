#!/bin/bash
physicalCpuCount=$([[ $(uname) = 'Darwin' ]] &&
                       sysctl -n hw.physicalcpu_max ||
                       lscpu -p | egrep -v '^#' | sort -u -t, -k 2,4 | wc -l)
mkdir netrunner-$(date +%F)
cd netrunner-$(date +%F)
cp ../../../*.ttf .
curl https://gyroninja.net:1615/job/NetRunner/lastSuccessfulBuild/artifact/netrunner > netrunner
cd ..
zip -r -X netrunner-$(date +%F)-linux.zip netrunner-$(date +%F)
rm -fr netrunner-$(date +%F)
# https://gitgud.io/snippets/29