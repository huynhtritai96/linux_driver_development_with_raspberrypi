#!/usr/bin/env bash
set -u
set -o pipefail

# Auto-generated LKMPG topic runner
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/../../.." && pwd)"
EXAMPLES_DIR="$ROOT_DIR/examples"
OUTPUT_DIR="$SCRIPT_DIR/outputs"
STATUS_FILE="$OUTPUT_DIR/_run_status.tsv"

mkdir -p "$OUTPUT_DIR"
: > "$STATUS_FILE"

log() { printf "[%s] %s\n" "$(date +%H:%M:%S)" "$*"; }

make_module() {
    local module="$1"
    shift || true
    make -C "/lib/modules/$(uname -r)/build" M="$EXAMPLES_DIR" "$@" "${module}.ko"
}

run_example() {
    local name="$1"
    local fn="$2"
    local outfile="$OUTPUT_DIR/${name}.txt"
    local before=0
    before=$(sudo dmesg | wc -l 2>/dev/null || echo 0)
    {
        echo "# Example: $name"
        echo "# Timestamp: $(date -Iseconds)"
        echo "# Kernel: $(uname -r)"
        echo
        "$fn"
    } >"$outfile" 2>&1
    local rc=$?
    {
        echo
        echo "# Exit code: $rc"
        echo
        echo "# dmesg delta"
        sudo dmesg | tail -n +$((before + 1)) || true
    } >>"$outfile" 2>&1
    printf "%s\tactual\t%d\n" "$name" "$rc" >> "$STATUS_FILE"
    if [ "$rc" -eq 0 ]; then
        log "OK $name"
    else
        log "WARN $name failed with rc=$rc"
    fi
    return 0
}

write_expected() {
    local name="$1"
    local reason="$2"
    local excerpt="$3"
    local outfile="$OUTPUT_DIR/${name}__expected_output.txt"
    {
        echo "# Example: $name"
        echo "# Status: expected-output-only"
        echo "# Timestamp: $(date -Iseconds)"
        echo
        echo "Reason: $reason"
        echo
        echo "Expected behavior summary:"
        echo "$excerpt"
    } > "$outfile"
    printf "%s\texpected\t0\n" "$name" >> "$STATUS_FILE"
    log "EXPECTED $name ($reason)"
}

if ! command -v sudo >/dev/null 2>&1; then
    log "sudo is required for module operations."
fi

log "Running topic package for Blocking Processes and threads"

do_sleep() {
    make_module sleep
    sudo insmod "$EXAMPLES_DIR/sleep.ko"
    printf "napmsg" | sudo tee /proc/sleep >/dev/null
    cat /proc/sleep || true
    sudo rmmod sleep
}

do_cat_nonblock() {
    make_module sleep
    gcc -O2 -Wall -Wextra "$EXAMPLES_DIR/other/cat_nonblock.c" -o "$EXAMPLES_DIR/other/cat_nonblock"
    sudo insmod "$EXAMPLES_DIR/sleep.ko"
    ( exec 3</proc/sleep; sleep 2 ) &
    holder=$!
    sleep 0.2
    "$EXAMPLES_DIR/other/cat_nonblock" /proc/sleep || true
    wait "$holder" || true
    sudo rmmod sleep
}

do_completions() {
    make_module completions
    sudo insmod "$EXAMPLES_DIR/completions.ko"
    sudo rmmod completions
}

run_example "sleep" do_sleep
run_example "cat_nonblock" do_cat_nonblock
run_example "completions" do_completions

log "Topic run complete."
