#!/bin/bash

find . -name '*.log' | while read fname; do echo mv $fname ${fname/.log/.LOG}; done | bash -x
