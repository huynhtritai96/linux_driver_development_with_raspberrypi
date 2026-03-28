# Code Walkthrough: Replacing Print Macros

## Source walkthrough map
### `print_string`
- Files: `examples/print_string.c`
- Primary concept: tty write path from kernel
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.

### `kbleds`
- Files: `examples/kbleds.c`
- Primary concept: timer callbacks + tty ioctl integration
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.
