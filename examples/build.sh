#!/bin/bash

ARG_COUNT=$#
PL0_SOURCE=$1
SHOW_IR=$(( 0 ))

if [ $ARG_COUNT == 2 ] && [ $1 == '-emit-llvm' ]
then
	SHOW_IR=$(( 1 ))
	PL0_SOURCE=$2

elif [ $ARG_COUNT != 1 ]
then
	echo "Usage:"
	echo './build.sh [-emit-llvm] <filename>.pl0'
	echo 'This will give a binary named <filename>'
	exit 1
fi

LL_EXT=".ll"
OBJ_EXT=".o"

NAME_LEN=${#PL0_SOURCE}
NAME_WO_EXT=${PL0_SOURCE:0:$(($NAME_LEN-4))}

LL_SOURCE="$NAME_WO_EXT$LL_EXT"
OBJ="$NAME_WO_EXT$OBJ_EXT"

.././pl0c $PL0_SOURCE
llc -filetype=obj $LL_SOURCE
clang -c io.c
clang $OBJ io.o -o $NAME_WO_EXT

if [ $SHOW_IR == 0 ]
then
	rm -f $LL_SOURCE
fi
rm -f $OBJ
rm -f io.o