#!/bin/bash

perl -n -e'(/[(,]([^0),]+)[),]/ && print $_, `awk -vq="'"'"'"'" '"'/$1/ { print "PIN_DELAY=" q \$2 q ";" }'"'"' fileA`) || print $_' fileB
