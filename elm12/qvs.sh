CTR=0

while test $CTR -le 99; do
	ZEROS=`cat qvs2 | grep "0.$CTR " | grep "0$" | wc -l`
	ONES=`cat qvs2 | grep "0.$CTR " | grep "1$" | wc -l`
#	RATE=`echo "scale=4;$ONES / ($ONES + $ZEROS)" | bc`
	RATE=`echo "scale=4;$ONES + $ZEROS" | bc`
	echo "0.$CTR $RATE"
	CTR=`expr $CTR + 1`
done
