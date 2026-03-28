# Code Walkthrough: GPIO

## Source walkthrough map
### `led`
- Files: `examples/led.c`
- Primary concept: GPIO output via char device writes
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.

### `dht11`
- Files: `examples/dht11.c`
- Primary concept: timing-sensitive sensor protocol in kernel space
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.
