# Code Walkthrough: Interrupt Handlers

## Source walkthrough map
### `intrpt`
- Files: `examples/intrpt.c`
- Primary concept: top-half IRQ callback
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.

### `bottomhalf`
- Files: `examples/bottomhalf.c`
- Primary concept: split top-half and workqueue bottom-half
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.

### `bh_threaded`
- Files: `examples/bh_threaded.c`
- Primary concept: request_threaded_irq flow
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.
