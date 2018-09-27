#!/bin/bash

# Example
#	$ ./check31.sh w_1_g_M5.out

SXS=`cat $1 | grep "\.$2\." | sed 's/.*sxs //;s/,.*//' | grep '1' | wc -l`
ACT=`cat $1 | grep "\.$2\." | sed 's/.*act //;s/, .*//' | grep -- "^-1$" | wc -l`
PRED=`cat $1 | grep "\.$2\." | sed 's/.*pred //;s/, .*//' | grep -- "^-1$" | wc -l`
TOT=`cat $1 | grep "\.$2\." | wc -l`

P1=`echo "$ACT * 100 / $TOT" | bc`
P2=`echo "$PRED * 100 / $TOT" | bc`

echo "p1 = $P1, p2 = $P2, sxs = $SXS"

Q=`echo "($P1 * $P2 + (100 - $P1) * (100 - $P2))/100" | bc`

Z=`echo "scale=4;(100 * $SXS - $TOT * $Q)/sqrt($TOT * $Q * (100 - $Q))" | bc`

PERC=`echo "scale=4; $SXS / $TOT" | bc`

./prob_3 $TOT 10000 $P1 $P2

THEOMU=`./prob_3 $TOT 10000 $P1 $P2 | sed 's/.*mu //;s/,.*//'`
DIFFMU=`echo "$SXS - $THEOMU" | bc`

echo perc $PERC
echo "diffmu $DIFFMU"
echo Z $Z
