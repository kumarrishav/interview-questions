#!/bin/bash

# Write a script that, given a group name, finds and prints
# every user that belongs to that group

grep -E "^$1:" /etc/group | cut -d: -f4 | tr ',' '\n'
