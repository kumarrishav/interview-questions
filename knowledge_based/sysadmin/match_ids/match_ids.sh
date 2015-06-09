#!/bin/bash

# Write a script that, given a pattern, finds and prints every user
# that matches the pattern

grep -E "^$1([^:]*:){6}[^:]*$" /etc/passwd | cut -d: -f3 | sort -n
