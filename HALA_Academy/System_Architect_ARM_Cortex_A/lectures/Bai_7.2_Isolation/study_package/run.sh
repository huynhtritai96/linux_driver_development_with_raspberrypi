#!/usr/bin/env bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
printf '[study-package] %s\n' 'No close LKMPG example exists for cpu isolation, housekeeping cores, or nohz_full tuning.'
mkdir -p "$script_dir/outputs"
exit 0
