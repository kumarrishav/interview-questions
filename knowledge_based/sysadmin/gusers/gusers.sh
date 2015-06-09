#!/bin/bash
grep -E "^$1:" /etc/group | cut -d: -f4 | tr ',' '\n'
