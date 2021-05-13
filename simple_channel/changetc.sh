#!/bin/bash
function usage() { 
echo "$0 usage: 
-d delay
-v variable dalay
-s reorder
-l lost
-r rate[kbit/Mbit]
-c show current settings
-h help" 
exit 0;
}
delay="0"
variable="0"
reorder="0"
loss="0"
rate="0"

while getopts "d:v:s:l:r:ch" opt 
do
case $opt in 
d) 
	delay="$OPTARG"
	echo "found -d param $delay"
	;;	
v) 
	variable="$OPTARG"
	echo "found -d param $variable"
	;;
s) 
	reorder="$OPTARG"
	echo "found -d param $reorder"
	;;
l) 
	loss="$OPTARG";
	echo "found -d param $loss"
	;;
r) 
	rate="$OPTARG";
	echo "found -d param $rate"
	;;
c) 
	echo "show"
	exit 0 
	;;
h | *) 
	usage
esac
done

com="ip netns exec fServer tc qdisc change dev virt01 root netem"
if [ $rate != "0" ]
	then 
		com=$com" rate $rate "
	fi
if [ $delay != "0" ] 
	then 
		if [ $variable != "0" ]
			then
				com=$com" delay "$delay"ms "$variable"ms "
			else
				com=$com" delay "$delay"ms " 
		fi
	fi	 
if [ $loss -ne "0" ]
	then 
		com=$com" loss "$loss"% "	
	fi	 
if [ $reorder != "0" ]
	then 
		com=$com" reorder "$reorder"% "
	fi	 
$com
