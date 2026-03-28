# Code Walkthrough: Hello World

## Source walkthrough map
### `hello-1`
- Files: `examples/hello-1.c`
- Primary concept: module lifecycle basics
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.

### `hello-2`
- Files: `examples/hello-2.c`
- Primary concept: preferred init/exit registration
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.

### `hello-3`
- Files: `examples/hello-3.c`
- Primary concept: init/exit section annotations
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.

### `hello-4`
- Files: `examples/hello-4.c`
- Primary concept: module metadata and licensing
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.

### `hello-5`
- Files: `examples/hello-5.c`
- Primary concept: module_param interfaces
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.

### `startstop`
- Files: `examples/start.c`, `examples/stop.c`
- Primary concept: modular compilation units
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.
