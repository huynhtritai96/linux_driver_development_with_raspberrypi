# Bài 2.2: Initramfs & Switch_root

## Overview

This self-study package ties the lecture to the closest available LKMPG material without pretending the match is stronger than it is. Confidence for the mapping is **low**.

## Concepts you must understand

- Hiểu vai trò của initramfs trong boot process

## Lecture documents used

- `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/PDFs/Bai_2.2_Initramfs.pdf`
- `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/output/Bai_2.2_Initramfs__raw.md`

## LKMPG source mapping

- Matched topic areas: no_close_match
- Mapping note: LKMPG assumes a running Linux userspace and does not include initramfs assembly flows.
- Gap note: No close LKMPG example exists for initramfs init scripts, BusyBox rootfs assembly, or switch_root transitions.

- No close LKMPG code sample was selected.

## Source files in this lecture

- Lecture PDF: `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/PDFs/Bai_2.2_Initramfs.pdf`
- Parsed lecture Markdown: `/home/htritai/ldd/linux_driver_development_with_raspberrypi/HALA_Academy/System_Architect_ARM_Cortex_A/output/Bai_2.2_Initramfs__raw.md`
- Key figure captions:
  Hình 1: Luồng boot với Initramfs

## Example mapping

No close LKMPG example was selected for this lecture.

## How each example relates to the lecture

- No close LKMPG example exists for initramfs init scripts, BusyBox rootfs assembly, or switch_root transitions.

## Build steps

1. No example build is planned because LKMPG has no close match for this lecture.

## Run steps

1. `cd lectures/Bai_2.2_Initramfs/study_package`
1. `./run.sh`
1. Read the printed gap note and the analysis files; no code execution is attempted.

## Output

No runnable or buildable LKMPG example was selected for this lecture package.

## Output explanation

The package focuses on document analysis because LKMPG has no close code sample for this topic.


## Detailed code analysis

The selected examples emphasize init/exit flow, control interfaces, object lifetime, and the narrowest useful kernel API surface. For this lecture, pay attention to how the LKMPG sample either directly models the lecture concept or deliberately stops short of it. That boundary is educational: it tells you where generic kernel-module mechanics end and subsystem-specific engineering begins.

## Kernel / system behavior analysis

- Module init and cleanup are only the visible edges of the lifecycle; the important behavior lives in registration, callback context, and the external interface each example exposes.
- Kernel-space code runs with stricter failure modes than userspace. A short sample can still carry locking, lifetime, or context assumptions that matter in production code.
- Where the package uses expected-output-only files, the blocker is a real environment or coverage limitation, not an omitted execution attempt.

## Common pitfalls

- Treating module-level examples as full subsystem implementations when the lecture is actually about a larger boot, graphics, or media stack.
- Ignoring privilege boundaries: module load/unload, /proc writes, and sysfs writes often require root.
- Assuming successful module load means the example fully exercised the lecture concept; several packages are intentionally partial mappings.
- Forgetting to inspect the `outputs/` directory after `run.sh`; that is where execution blockers and expected-output notes are recorded.
- Missing the manual-review note. This lecture intentionally documents gaps where LKMPG does not provide a direct code sample.

## Suggested next steps

- Re-run `run.sh` after adjusting the target environment or hardware prerequisites.
- Compare the LKMPG example with the lecture commands and diagrams already embedded in the course Markdown.
- Use the analysis files in `analysis/` as a checklist for code reading and debugging practice.
- Add a board- or subsystem-specific reference implementation outside LKMPG if you need a truly direct lab for this lecture.
