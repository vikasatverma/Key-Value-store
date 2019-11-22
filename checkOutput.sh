#!/bin/sh

./KVClient request op

cmp op output && echo 'equal' || echo 'Not correct'

rm op
