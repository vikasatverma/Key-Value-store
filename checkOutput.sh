#!/bin/sh

cmake-build-debug/KVClient port 8181 request op

cmp op output && echo 'equal' || echo 'Not correct'

rm op
