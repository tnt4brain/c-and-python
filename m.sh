#!/bin/bash
# Copyright 2021 Sergey Pechenko
#
# Helper script to assist in writing and debugging minimal glue code for Python
# 
gcc test_python.c `python3.6m-config --cflags` `python3.6m-config --ldflags` && ./a.out 2>screen.dmp
