#!/bin/sh
git describe --long --tags --always | awk -F '-' '{print $1 "." $2}'
