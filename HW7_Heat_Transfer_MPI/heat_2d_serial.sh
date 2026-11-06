#!/bin/bash
./heat_2d_serial 8192 100 | grep -E "Execution Time|Checksum"
