# Code Walkthrough: Device Tree

## Source walkthrough map
### `devicetree`
- Files: `examples/devicetree.c`
- Primary concept: of_match_table and DT property parsing
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.

### `dt-overlay`
- Files: `examples/dt-overlay.dts`
- Primary concept: runtime device-tree overlay mechanics
- Key flow: module init/register -> runtime callbacks or interactions -> module exit/unregister.
- Safety checks to inspect: copy_to_user/copy_from_user boundaries, error-path unwinding, and resource release ordering.
