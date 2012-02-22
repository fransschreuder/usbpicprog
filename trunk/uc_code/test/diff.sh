#!/bin/bash

# this bash file will compare all the files in baseline with the output of the current runtest and allow on to create
# comments/explanations into a file in the directory baseline.diffs usefully when making changes that affect a number of devices
#


cd baseline
for i in *; do
	[[ -e ../baseline.diffs/$i.txt ]] && continue
	cmp -s $i <(../runtest $i) && continue
	diff $i <(../runtest $i)
	read -p "$i parallel diff? (Y/n) " f
echo "-$f-"
	[[ $f != "n" ]] && reset && diff $i <(../runtest $i) -y
echo "-$f-"
	read -p "reason? " f
	[[ $f == "" ]] && continue;
	while [[ $f != "" ]]; do
		echo "$f" >>../baseline.diffs/$i.txt
		read -p "reason? " f
		done
	done
