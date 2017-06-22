#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "Usage: sh $0 [java bytecode file]"	
	exit 1
fi

if [ ! -e "$1" ]; then
	echo "$1 does not exist in this directory"
	exit 1
fi	


java -jar JVM/jasmin.jar "$1"

name=$(find "./" -maxdepth 1 -mindepth 1 -name "*.class")
size=$((${#name}-8))
#echo "$name"
#echo "$size"
#echo ${name:2:size}
java "${name:2:size}"
rm -rf *.class

