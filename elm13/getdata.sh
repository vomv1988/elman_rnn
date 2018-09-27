#!/bin/bash

# get_dataset.sh
# Sun Dec 28 17:33:06 CST 2014

# Descargar la WEEK-esima semana, a partir el 1 de enero del 2005,
# de tazas de intercambio de EURO a USDOLLAR

# <vomv1988@gmail.com>

# Ejemplo:
#	$ ./get_dataset.sh 1 M2
# esto descarga el precio del EURO en USDOLLARs desde
# Sun Jan  9 16:00:00 CST 2005
# hasta
# Fri Jan 14 16:00:00 CST 2005

WEEK="$1"
TIMEBASE="$2"

START=`date --date="2005-1-1 +$WEEK week +1 days 5 pm EST" +"%s"`
END=`date --date="2005-1-1 +$WEEK week +6 days 5 pm EST" +"%s"`

AUTH="Authorization: Bearer 0ce4d6a3f432ce948495e5973eec9629-8700dc7a981c5ff66a533ac4936d5286"
DOMN="https://api-fxpractice.oanda.com/v1/"
CMD="candles?instrument=EUR_USD&start=${START}&end=${END}&granularity=${TIMEBASE}"
DFORM="X-Accept-Datetime-Format: UNIX"

curl -s -H "$AUTH" -H "$DFORM" -X GET "${DOMN}${CMD}" | \
	grep 'closeAsk' | \
	sed 's/.* : //;s/,.*//' > \
	prices_trainset_week_${WEEK}_granularity_${TIMEBASE}.plot

exit 0
