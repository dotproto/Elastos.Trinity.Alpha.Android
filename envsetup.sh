#!/bin/bash

SCRIPT=$(readlink -f "${BASH_SOURCE[0]}")
SCRIPTPATH=$(dirname ${SCRIPT})
export PATH="${SCRIPTPATH}/depot_tools:$PATH"
