#!/bin/bash

lscpu | grep "CPU MHz:" | cut -d ":" -f 2 | grep -m1 -Eo '[0-9]*\.?[0-9]+'
