#!/bin/bash

# You are in a directory that has numerous log files scattered all over the place, including
# sub-directories.
# The log files names are not uniform: some end with .log, others with .lOg, others with .LOG,
# and other variations.
# Write a script that finds all of these files and standardizes the names such that all of them
# end with .log

find . -iname '*.log' | while read fname; do newname=`echo $fname | sed 's/\.log/.log/i'`; if [ $fname != $newname ]; then echo mv $fname $newname; fi; done | bash -x

# Example execution result for the following test directory:
#
# filipe@filipe-Kubuntu:~/sandbox$ tree .
# .
# ├── alallala.lOg
# ├── sdjfhjd.LOG
# ├── sdjk.LOG
# ├── some-log
# │   ├── hahaha.LOG
# │   ├── hehehe.LOg
# │   └── hihihi.LoG
# ├── some-logs-to-you
# │   ├── xpto2.LOG
# │   └── xpto.loG
# └── xpto.log
#
# 2 directories, 9 files
#
# Script output (bash -x makes bash print each command before executing it):
#
# + mv ./sdjfhjd.LOG ./sdjfhjd.log
# + mv ./some-log/hahaha.LOG ./some-log/hahaha.log
# + mv ./some-log/hihihi.LoG ./some-log/hihihi.log
# + mv ./some-log/hehehe.LOg ./some-log/hehehe.log
# + mv ./sdjk.LOG ./sdjk.log
# + mv ./some-logs-to-you/xpto.loG ./some-logs-to-you/xpto.log
# + mv ./some-logs-to-you/xpto2.LOG ./some-logs-to-you/xpto2.log
# + mv ./alallala.lOg ./alallala.log
#
# After execution:
#
# filipe@filipe-Kubuntu:~/sandbox$ tree .
# .
# ├── alallala.log
# ├── sdjfhjd.log
# ├── sdjk.log
# ├── some-log
# │   ├── hahaha.log
# │   ├── hehehe.log
# │   └── hihihi.log
# ├── some-logs-to-you
# │   ├── xpto2.log
# │   └── xpto.log
# └── xpto.log
#
# 2 directories, 9 files
#
