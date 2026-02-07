#!/bin/bash
clang *.c -g -O0 -DPLTFRM_LINUX  -lX11 -lGL -o a.bin
