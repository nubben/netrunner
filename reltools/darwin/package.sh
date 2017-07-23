#!/bin/bash
physicalCpuCount=$([[ $(uname) = 'Darwin' ]] &&
                       sysctl -n hw.physicalcpu_max ||
                       lscpu -p | egrep -v '^#' | sort -u -t, -k 2,4 | wc -l)
mkdir netrunner-$(date +%F)
cd netrunner-$(date +%F)
cp ../*.dylib .
cp ../../../*.ttf .
cd ../../../
make -j$physicalCpuCount
cd reltools/darwin/netrunner-$(date +%F)
cp ../../../netrunner .
install_name_tool -change /usr/local/lib/libGLEW.2.0.0.dylib libGLEW.2.0.0.dylib netrunner
install_name_tool -change /usr/local/lib/libfreetype.6.dylib libfreetype.6.dylib netrunner
install_name_tool -change /usr/local/lib/libharfbuzz.0.dylib libharfbuzz.0.dylib netrunner
otool -L netrunner
cd ..
zip -r -X netrunner-$(date +%F)-darwin.zip netrunner-$(date +%F)
rm -fr netrunner-$(date +%F)
# https://gitgud.io/snippets/29