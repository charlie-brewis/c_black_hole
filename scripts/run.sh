#!/usr/bin/env bash
set -euo pipefail

RUN_GRAPHICS=1 cmake --build build --target run
