#!/bin/bash

CTR=1

{
	printf "set key off\n"
	printf "plot "
	while test $CTR -lt 19 ; do
		printf "\"generation_%09d_srn_%09d.dat\" with lines lc rgb \"#000000\", " "$1" "$CTR"
		CTR=`expr $CTR + 1`
	done
	printf "\"generation_%09d_srn_%09d.dat\" with lines lc rgb \"#000000\", " "$1" "$CTR"
	printf "\"ts2.dat\" using 1 with lines lc rgb \"#0000FF\" lw 2, "
	printf "\"ts2.dat\" using 2 with lines lc rgb \"#FF0000\" lw 2,"
	printf "\"generation_%09d_srn_%09d.dat\" with lines lc rgb \"#00FF00\" lw 2\n" "$1" "0"
} | gnuplot -p
