# Code Walkthrough: System Calls

## Source walkthrough map
### `syscall-steal`
- Files: `examples/syscall-steal.c`
- Primary concept: syscall table hooking risks
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.
