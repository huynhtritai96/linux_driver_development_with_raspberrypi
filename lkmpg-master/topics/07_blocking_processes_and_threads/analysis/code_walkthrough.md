# Code Walkthrough: Blocking Processes and threads

## Source walkthrough map
### `sleep`
- Files: `examples/sleep.c`
- Primary concept: blocking semantics and wakeups
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.

### `cat_nonblock`
- Files: `examples/other/cat_nonblock.c`
- Primary concept: non-blocking userspace behavior
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.

### `completions`
- Files: `examples/completions.c`
- Primary concept: one-shot synchronization points
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.
