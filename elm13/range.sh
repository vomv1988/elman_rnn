CTR1=1
CTR2=1

LIM1=$1
LIM2=$2

SUM=0

FILE="w_1_g_M2_elm5_sample_100.out"
LINES=`cat $FILE | grep "\.1\." | wc -l`

while test $CTR2 -le $LINES ; do
	while test $CTR1 -le $CTR2 ; do
		LAST=`echo "$CTR1 * 31" | bc`
		cat $FILE | head -n $LAST
		CTR1=`expr $CTR1 + 1`
	done > zseq2/zseq$CTR2.dat
	echo zseq$CTR2.dat
	CTR2=`expr $CTR2 + 1`
	echo 000
done
