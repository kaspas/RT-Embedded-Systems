#!/bin/sh

if [ "$#" -ne 1 ]
then
echo "You must provide 1 variables: "
echo "Scan time in seconds"
exit
fi

time=$1

Date=$(date "+%Y-%m-%d-%H:%M:%S")

echo "==== Run starts now ======= `date` " > datelog.log

./wifi $time &> errorlog.log

echo "==== Run ends now ======= `date` " >> datelog.log
