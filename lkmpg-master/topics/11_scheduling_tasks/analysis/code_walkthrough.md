# Code Walkthrough: Scheduling Tasks

## Source walkthrough map
### `example_tasklet`
- Files: `examples/example_tasklet.c`
- Primary concept: softirq-based deferred work
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.

### `sched`
- Files: `examples/sched.c`
- Primary concept: workqueue-based deferred execution
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.
