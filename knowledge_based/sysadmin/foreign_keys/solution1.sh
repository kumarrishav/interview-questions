#!/bin/bash

perl -n -e'(/[(,]([^0),]+)[),]/ && print $_, `awk "/$1/ { printf("'"'"'"PIN_DELAY='"'"'"'"'"'%d'"'"'"'"'"';\\n", \$2) }'"'"'"" fileA`) || print $_' fileB
