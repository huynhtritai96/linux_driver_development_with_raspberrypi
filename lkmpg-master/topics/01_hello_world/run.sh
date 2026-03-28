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

log "Running topic package for Hello World"

do_hello_1() {
    make_module "hello-1"
    sudo insmod "$EXAMPLES_DIR/hello-1.ko"
    sudo rmmod hello-1
}

do_hello_2() {
    make_module "hello-2"
    sudo insmod "$EXAMPLES_DIR/hello-2.ko"
    sudo rmmod hello-2
}

do_hello_3() {
    make_module "hello-3"
    sudo insmod "$EXAMPLES_DIR/hello-3.ko"
    sudo rmmod hello-3
}

do_hello_4() {
    make_module "hello-4"
    sudo insmod "$EXAMPLES_DIR/hello-4.ko"
    sudo rmmod hello-4
}

do_hello_5() {
    make_module "hello-5"
    sudo insmod "$EXAMPLES_DIR/hello-5.ko"
    sudo rmmod hello-5
}

do_startstop() {
    make_module startstop
    sudo insmod "$EXAMPLES_DIR/startstop.ko"
    sudo rmmod startstop
}

run_example "hello-1" do_hello_1
run_example "hello-2" do_hello_2
run_example "hello-3" do_hello_3
run_example "hello-4" do_hello_4
run_example "hello-5" do_hello_5
run_example "startstop" do_startstop

log "Topic run complete."
