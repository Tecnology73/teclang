#!/bin/bash

in="./source/playground.lang"
out="./out/app.exe"
optLevel="1"
llOut="./out/tmp.ll"

for (( i=1; i<=$#; i += 2)); do
    j=$((i+1))
	if [[ "${!i}" = "-i" ]]
	then
		in="${!j}"
	elif [[ "${!i}" = "-o" ]]
	then
		out="${!j}"
	elif [[ "${!i}" = "-O" ]]
	then
		optLevel="${!j}"
	fi
done

./cmake-build-debug/Debug/teclang.exe -i $in -o $llOut -O $optLevel && clang $llOut -o $out
rm $llOut 2> /dev/null
