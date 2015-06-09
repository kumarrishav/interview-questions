#!/bin/bash

# You are in a directory that has numerous log files scattered all over the place, including
# sub-directories.
# The log files names are not uniform: some end with .log, others with .lOg, others with .LOG,
# and other variations.
# Write a script that finds all of these files and standardizes the names such that all of them
# end with .log

find . -iname '*.log' | while read fname; do newname=`echo $fname | sed 's/\.log/.log/i'`; if [ $fname != $newname ]; then echo mv $fname $newname; fi; done | bash -x
