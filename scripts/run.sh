#!/usr/bin/env bash
set -euo pipefail

NUM_RAYS="${1:-10}"
RUN_GRAPHICS=1 RUN_ARGS="${NUM_RAYS}" cmake --build build --target run
