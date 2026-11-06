#!/bin/bash
./heat_2d_serial 4096 100 | grep -E "Execution Time|Checksum"
