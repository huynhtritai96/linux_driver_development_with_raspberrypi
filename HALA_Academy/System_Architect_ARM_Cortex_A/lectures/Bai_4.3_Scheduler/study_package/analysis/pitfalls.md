# Pitfalls

- Watch for kernel-version guards in the LKMPG sources; several examples carry compatibility branches for modern kernels.
- Distinguish between buildability and faithful demonstration. A module may compile cleanly and still require hardware, DT nodes, or interactive setup to exercise the interesting path.
- Keep kernel-space vs userspace responsibilities explicit when reasoning about logs, blocking behavior, and permissions.
