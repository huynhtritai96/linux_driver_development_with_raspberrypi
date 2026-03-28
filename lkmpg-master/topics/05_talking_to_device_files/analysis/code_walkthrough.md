# Code Walkthrough: Talking To Device Files

## Source walkthrough map
### `ioctl`
- Files: `examples/ioctl.c`
- Primary concept: ioctl ABI surface and locking
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.

### `chardev2`
- Files: `examples/chardev2.c`, `examples/chardev.h`
- Primary concept: kernel/userspace ioctl contract
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.

### `userspace_ioctl`
- Files: `examples/other/userspace_ioctl.c`, `examples/chardev.h`
- Primary concept: userspace ioctl client flow
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.
