#!/bin/bash

source ~/.bashrc

##############################################################################

while [ true ] ; do
	echo "Starting MIRA framework at `date`"
	mira $1 -p1234
	sleep 5
done
