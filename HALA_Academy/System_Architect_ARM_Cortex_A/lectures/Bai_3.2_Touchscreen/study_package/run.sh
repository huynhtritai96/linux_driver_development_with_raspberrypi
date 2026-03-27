#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
EXAMPLES_DIR="$SCRIPT_DIR/examples"
OUTPUTS_DIR="$SCRIPT_DIR/outputs"
mkdir -p "$OUTPUTS_DIR"

log() {
    printf '[study-package] %s\n' "$*"
}

write_expected() {
    local output_path="$1"
    local example_name="$2"
    local reason="$3"
    local note="$4"
    {
        printf 'Example: %s
' "$example_name"
        printf 'Status: expected_output_only
'
        printf 'Reason: %s
' "$reason"
        printf '
Expected behavior:
%s
' "$note"
    } >"$output_path"
}

build_example() {
    local example_dir="$1"
    local build_log="$2"
    (
        cd "$example_dir"
        make clean >/dev/null 2>&1 || true
        make
    ) >"$build_log" 2>&1
}

capture_dmesg_delta() {
    local before_lines="$1"
    sudo dmesg | tail -n +"$((before_lines + 1))" || true
}

run_module_example() {
    local example_name="$1"
    local example_dir="$2"
    local ko_file="$3"
    local module_name="$4"
    local mode="$5"
    local runtime_cmds="$6"
    local expected_note="$7"

    local build_log cmd_log before_lines output_path expected_path
    build_log=$(mktemp)
    cmd_log=$(mktemp)
    output_path="$OUTPUTS_DIR/${example_name}.txt"
    expected_path="$OUTPUTS_DIR/${example_name}__expected_output.txt"
    rm -f "$output_path" "$expected_path"

    if [ ! -e /lib/modules/$(uname -r)/build ]; then
        write_expected "$expected_path" "$example_name" "Missing kernel headers at /lib/modules/$(uname -r)/build." "$expected_note"
        rm -f "$build_log" "$cmd_log"
        return 0
    fi

    if ! build_example "$example_dir" "$build_log"; then
        {
            echo "Example: $example_name"
            echo "Status: expected_output_only"
            echo "Reason: build failed."
            echo
            echo "Build log:"
            cat "$build_log"
            echo
            echo "Expected behavior:"
            echo "$expected_note"
        } >"$expected_path"
        rm -f "$build_log" "$cmd_log"
        return 0
    fi

    if [ "$mode" != "autorun" ]; then
        {
            echo "Example: $example_name"
            echo "Status: expected_output_only"
            echo "Reason: execution intentionally skipped because this example is not on the auto-run allowlist for the study package."
            echo
            echo "Build log:"
            cat "$build_log"
            echo
            echo "Expected behavior:"
            echo "$expected_note"
        } >"$expected_path"
        rm -f "$build_log" "$cmd_log"
        return 0
    fi

    if ! sudo -n true >/dev/null 2>&1; then
        write_expected "$expected_path" "$example_name" "sudo -n true failed; module insertion is unavailable." "$expected_note"
        rm -f "$build_log" "$cmd_log"
        return 0
    fi

    before_lines=$(sudo dmesg | wc -l || echo 0)

    if ! {
        echo "$ sudo insmod $example_dir/$ko_file"
        sudo insmod "$example_dir/$ko_file"
        if [ -n "$runtime_cmds" ]; then
            echo "$ bash -lc $runtime_cmds"
            bash -lc "set -euo pipefail; $runtime_cmds"
        fi
        echo "$ sudo rmmod $module_name"
        sudo rmmod "$module_name"
    } >"$cmd_log" 2>&1; then
        sudo rmmod "$module_name" >/dev/null 2>&1 || true
        {
            echo "Example: $example_name"
            echo "Status: expected_output_only"
            echo "Reason: runtime execution failed or the environment blocked the example."
            echo
            echo "Build log:"
            cat "$build_log"
            echo
            echo "Command log:"
            cat "$cmd_log"
            echo
            echo "Expected behavior:"
            echo "$expected_note"
        } >"$expected_path"
        rm -f "$build_log" "$cmd_log"
        return 0
    fi

    {
        echo "Example: $example_name"
        echo "Status: executed"
        echo
        echo "Build log:"
        cat "$build_log"
        echo
        echo "Command log:"
        cat "$cmd_log"
        echo
        echo "Kernel log delta:"
        capture_dmesg_delta "$before_lines"
    } >"$output_path"

    rm -f "$build_log" "$cmd_log"
}

log "Generating outputs for the selected examples"
run_module_example "vinput" "$EXAMPLES_DIR/vinput" "vinput.ko" "vinput" "build_only" '' 'Creates the vinput character-device framework and waits for a provider such as vkbd.'
run_module_example "devicetree" "$EXAMPLES_DIR/devicetree" "devicetree.ko" "devicetree" "build_only" '' 'Module load succeeds, but probe logs appear only when a matching DT node is present.'
log "Finished"
