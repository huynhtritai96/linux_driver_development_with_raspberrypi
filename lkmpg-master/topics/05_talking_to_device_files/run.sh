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

log "Running topic package for Talking To Device Files"

do_ioctl() {
    make_module ioctl
    sudo insmod "$EXAMPLES_DIR/ioctl.ko"
    major=$(awk '$2=="ioctltest"{print $1}' /proc/devices | head -n1)
    if [ -z "$major" ]; then
        echo "Failed to find ioctltest major"
        sudo rmmod ioctl
        return 1
    fi
    sudo mknod /tmp/ioctltest c "$major" 0 || true
    sudo dd if=/tmp/ioctltest bs=1 count=4 status=none | od -An -tx1 || true
    sudo rm -f /tmp/ioctltest
    sudo rmmod ioctl
}

do_chardev2() {
    make_module chardev2
    gcc -O2 -Wall -Wextra "$EXAMPLES_DIR/other/userspace_ioctl.c" -o "$EXAMPLES_DIR/other/userspace_ioctl"
    sudo insmod "$EXAMPLES_DIR/chardev2.ko"
    sudo "$EXAMPLES_DIR/other/userspace_ioctl"
    sudo rmmod chardev2
}

do_userspace_ioctl() {
    make_module chardev2
    gcc -O2 -Wall -Wextra "$EXAMPLES_DIR/other/userspace_ioctl.c" -o "$EXAMPLES_DIR/other/userspace_ioctl"
    sudo insmod "$EXAMPLES_DIR/chardev2.ko"
    sudo "$EXAMPLES_DIR/other/userspace_ioctl"
    sudo rmmod chardev2
}

run_example "ioctl" do_ioctl
run_example "chardev2" do_chardev2
run_example "userspace_ioctl" do_userspace_ioctl

log "Topic run complete."
