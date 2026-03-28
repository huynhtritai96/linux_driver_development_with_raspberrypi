# Code Walkthrough: Synchronization

## Source walkthrough map
### `example_mutex`
- Files: `examples/example_mutex.c`
- Primary concept: sleepable lock semantics
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.

### `example_spinlock`
- Files: `examples/example_spinlock.c`
- Primary concept: atomic-context locking
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.

### `example_rwlock`
- Files: `examples/example_rwlock.c`
- Primary concept: read-mostly lock strategy
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.

### `example_atomic`
- Files: `examples/example_atomic.c`
- Primary concept: lock-free scalar updates
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.
