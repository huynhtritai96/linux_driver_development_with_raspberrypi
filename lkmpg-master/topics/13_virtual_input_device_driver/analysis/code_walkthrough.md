# Code Walkthrough: Virtual Input Device Driver

## Source walkthrough map
### `vinput`
- Files: `examples/vinput.c`, `examples/vinput.h`
- Primary concept: registering virtual input framework and class attributes
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.

### `vkbd`
- Files: `examples/vkbd.c`, `examples/vinput.h`, `examples/vinput.c`
- Primary concept: cross-module symbol dependency in out-of-tree modules
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.
