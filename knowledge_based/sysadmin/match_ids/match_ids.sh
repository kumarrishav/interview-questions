#!/bin/bash

grep -E "^$1([^:]*:){6}[^:]*$" /etc/passwd | cut -d: -f3 | sort -n
