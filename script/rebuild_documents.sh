#/bin/bash

tmp=${PWD}
cd `dirname ${PWD}/$0`
cd ../data
ls | grep -v "documents" | awk '{print NR" "$0}' > documents
cd ${tmp}
