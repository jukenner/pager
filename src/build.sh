#!/bin/sh

set -e

if command -v python3 &>/dev/null; then
    PYTHON=python3
else
    PYTHON=python
fi

$PYTHON pack.py --macro PG --pub pager.h --priv pager_memory.c
