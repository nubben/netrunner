#!/bin/bash
mkdir netrunner-$(date +%F)
cd netrunner-$(date +%F)
  cp ../../../*.ttf .
  curl https://gyroninja.net:1615/job/NetRunner/lastSuccessfulBuild/artifact/netrunner > netrunner
cd ..
tar zcf netrunner-$(date +%F)-linux.tar.gz netrunner-$(date +%F)
rm -fr netrunner-$(date +%F)
# https://gitgud.io/snippets/29
