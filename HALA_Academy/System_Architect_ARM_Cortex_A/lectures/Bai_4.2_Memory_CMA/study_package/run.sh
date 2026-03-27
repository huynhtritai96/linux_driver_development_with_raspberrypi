#!/usr/bin/env bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
printf '[study-package] %s\n' 'No close LKMPG example exists for CMA regions, DMA-BUF heaps, or allocator profiling.'
mkdir -p "$script_dir/outputs"
exit 0
