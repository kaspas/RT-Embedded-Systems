#!/bin/sh

if [ "$#" -ne 2 ]
then
echo "You must provide 2 variables: "
echo "Execution time in seconds"
echo "Number of samples"
exit
fi

time=$1
samples=$2
time2=$time
samples2=$samples
sampleTime=`expr $time / $samples`
i=0
sleepTime=$sampleTime
if [ "$time" -ge 600 ]
then
samples=`expr $samples / 600`
sleepTime=600
fi
./mysleep $time2 $samples2 &
echo "==== Run starts now ======= `date` " >> datelog.log
while [ "$i" -lt "$samples" ]
do
sleep $sleepTime
echo "==== Run ends now ======= `date` " >> datelog.log
i=`expr $i + 1`
done
