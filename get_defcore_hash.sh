#!/bin/sh
git submodule status | awk -F " " '{print $1}'
