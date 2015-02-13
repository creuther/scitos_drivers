#!/bin/bash

source ~/.bashrc

##############################################################################

while [ true ] ; do
	echo "Starting MIRA framework at `date`"
	mira $1 -p$2 -d$3
	sleep 5
done
