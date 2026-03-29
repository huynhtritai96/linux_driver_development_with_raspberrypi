```
========================================================================================================
LINUX KERNEL MEMORY ALLOCATION — SENIOR MENTAL MODEL (ONE COMPLETE TEXT DIAGRAM)
========================================================================================================

GOAL OF THIS MODULE
────────────────────────────────────────────────────────────────────────────────────────────────────────
This module is not a device driver interface example.
It is a kernel-memory allocation demo that compares FOUR allocation mechanisms:
    1) kmalloc      -> small, physically contiguous kernel memory
    2) kzalloc      -> kmalloc + zero initialization
    3) vmalloc      -> virtually contiguous kernel memory for larger regions
    4) slab cache   -> efficient repeated allocation of fixed-size objects

So the real learning goal is:
    "When kernel code needs memory, which allocator should I choose, why, and what cleanup order follows?"

========================================================================================================
BIG PICTURE — WHAT THE KERNEL IS TRYING TO PROVIDE
========================================================================================================

KERNEL CODE NEEDS MEMORY
    │
    ├── sometimes needs a small raw byte buffer
    ├── sometimes needs zeroed memory
    ├── sometimes needs a large region that may not be physically contiguous
    └── sometimes repeatedly creates the same object type many times

Linux does NOT use a single allocator path for all cases.
Instead it gives specialized allocation mechanisms, because different workloads need different properties:
    +----------------+----------------------+----------------------+-------------------------------+
    | allocator      | virtual contiguous?  | physical contiguous? | best use case                 |
    +----------------+----------------------+----------------------+-------------------------------+
    | kmalloc        | yes                  | yes                  | small/medium buffers          |
    | kzalloc        | yes                  | yes                  | zeroed small/medium buffers   |
    | vmalloc        | yes                  | no                   | larger buffers                |
    | slab cache     | yes                  | usually yes backing  | many repeated same objects    |
    +----------------+----------------------+----------------------+-------------------------------+

========================================================================================================
MODULE INIT FLOW
========================================================================================================

insmod memory_alloc_example.ko
        │
        ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ my_init()                                                                                            │
│   1. kmalloc_ptr = kmalloc(ALLOC_SIZE_SMALL, GFP_KERNEL)                                             │
│   2. kzalloc_ptr = kzalloc(ALLOC_SIZE_SMALL, GFP_KERNEL)                                             │
│   3. vmalloc_ptr = vmalloc(ALLOC_SIZE_LARGE)                                                         │
│   4. my_cache = kmem_cache_create("my_cache", sizeof(struct my_object), ...)                         │
│   5. obj1 = kmem_cache_alloc(my_cache, GFP_KERNEL)                                                   │
│   6. initialize obj1 fields                                                                          │
│                                                                                                      │
│   If any step fails: rollback in reverse order using goto labels                                     │
└──────────────────────────────────────────────────────────────────────────────────────────────────────┘

This is a classic kernel resource-acquisition pattern:
    acquire resources in dependency order
    free them in reverse order on failure

========================================================================================================
1) KMALLOC LAYER
========================================================================================================

CODE
────
    kmalloc_ptr = kmalloc(ALLOC_SIZE_SMALL, GFP_KERNEL);

ALLOC_SIZE_SMALL = 1024 bytes

MENTAL MODEL
────────────
kmalloc asks the kernel for a block of memory that is:
    - virtually contiguous
    - physically contiguous
    - suitable for normal kernel access
    - relatively efficient for small and medium allocations

DIAGRAM
───────
physical memory pages
    [page A........................................]
            └──────── 1024-byte region returned ───┘

kernel virtual address space
    kmalloc_ptr  ─────────────────────────────────► that same physically contiguous region

Why physically contiguous matters:
    Some kernel operations or hardware interactions need memory that exists as one real physical run.
    Example categories:
        - DMA-related constraints (sometimes, though dedicated DMA APIs are preferred)
        - low-level buffer assumptions
        - fast direct memory access patterns

Why kmalloc is usually preferred first:
    It is the normal, fast, simple allocator for ordinary kernel memory.

Why GFP_KERNEL:
    This means: "normal kernel allocation in process context, sleeping allowed"

So the allocator may block/sleep to satisfy the request.
This is the standard choice unless you are in atomic/interrupt context.

OUTPUT
──────
    kmalloc_ptr -> valid 1 KB kernel buffer
    pr_info shows virtual address
    pr_info also prints virt_to_phys(kmalloc_ptr)

Important note on virt_to_phys:
    It only makes sense for memory types where that translation is valid for your intended use.
    In this demo it is used illustratively to show that kmalloc memory maps to real physical memory.

WHEN TO CHOOSE kmalloc
──────────────────────
Use when you need:
    - a normal kernel buffer
    - small or moderate size
    - contiguous physical backing
    - simple lifecycle

========================================================================================================
2) KZALLOC LAYER
========================================================================================================

CODE
────
    kzalloc_ptr = kzalloc(ALLOC_SIZE_SMALL, GFP_KERNEL);

MENTAL MODEL
────────────
kzalloc is:
    kmalloc + memset(..., 0, size)

So it has the same allocation characteristics as kmalloc, but the returned memory is zero-filled.

DIAGRAM
───────
allocation:
    kzalloc_ptr ─────────────────────────────────► [ 00 00 00 00 00 00 00 00 ... ]

Why this matters:
    A lot of kernel objects must start in a known clean state.
    Zero-initialized memory prevents accidental use of uninitialized fields.

Common use cases:
    - structs with many fields
    - string/buffer work that expects a zero terminator
    - safer initialization for new state objects

Tradeoff:
    Slightly more work than kmalloc because it clears the memory,
    but often worth it for correctness and simplicity.

OUTPUT
──────
    kzalloc_ptr -> valid zeroed 1 KB buffer

WHEN TO CHOOSE kzalloc
──────────────────────
Use when you need:
    - the same properties as kmalloc
    - but want initial contents to be zero automatically

Senior habit:
    for new structs, kzalloc is often cleaner than kmalloc + manual memset

========================================================================================================
3) VMALLOC LAYER
========================================================================================================

CODE
────
    vmalloc_ptr = vmalloc(ALLOC_SIZE_LARGE);

ALLOC_SIZE_LARGE = 1 MB

MENTAL MODEL
────────────
vmalloc returns memory that is:
    - virtually contiguous
    - NOT necessarily physically contiguous

That means the kernel gives you one clean virtual range, but underneath it may stitch together many unrelated physical pages.

DIAGRAM
───────
kernel virtual address space
    vmalloc_ptr
        │
        ▼
    [virtual page V1][virtual page V2][virtual page V3][virtual page V4] ... appears contiguous

maps to physical memory like:
    V1 -> physical page P10
    V2 -> physical page P403
    V3 -> physical page P77
    V4 -> physical page P912

So to kernel code:
    vmalloc_ptr[0 ... 1MB-1] looks like one continuous array

But physically:
    the pages may be scattered all over RAM

Why vmalloc exists:
    Large physically contiguous allocations become harder as memory gets fragmented.
    vmalloc avoids that constraint by only requiring virtual contiguity.

Tradeoffs:
    - slower than kmalloc
    - page-table setup overhead
    - worse locality/TLB behavior
    - not suitable for places that require physically contiguous memory

Why useful:
    It makes larger allocations feasible even when a same-sized kmalloc would fail.

OUTPUT
──────
    vmalloc_ptr -> valid 1 MB virtually contiguous buffer

WHEN TO CHOOSE vmalloc
──────────────────────
Use when you need:
    - a larger region
    - normal CPU access is enough
    - physical contiguity is not required

Senior rule:
    prefer kmalloc first for modest allocations;
    use vmalloc when size/fragmentation makes kmalloc unsuitable

========================================================================================================
4) SLAB CACHE LAYER
========================================================================================================

CODE
────
    struct my_object {
        int id;
        char name[32];
    };

    my_cache = kmem_cache_create("my_cache",
                                 sizeof(struct my_object),
                                 0,
                                 SLAB_HWCACHE_ALIGN | SLAB_NO_MERGE,
                                 NULL);

    obj1 = kmem_cache_alloc(my_cache, GFP_KERNEL);

MENTAL MODEL
────────────
The slab allocator is for repeated allocation of many objects of the same size/type.
Instead of saying: "give me arbitrary bytes"
you say: "create a cache optimized for objects shaped like struct my_object"

Then the kernel can manage object reuse efficiently.

DIAGRAM
───────
create cache:
    my_cache
       │
       ├── metadata describing object size = sizeof(struct my_object)
       ├── alignment policy
       └── slabs containing multiple object slots

conceptual slab contents:
    slab #1
      ├── slot 0 -> obj1
      ├── slot 1 -> free
      ├── slot 2 -> free
      └── slot 3 -> free

Then:
    obj1 = kmem_cache_alloc(my_cache, GFP_KERNEL);

returns one object from that cache:
    obj1 ─────────────────────────────────────────► struct my_object instance

You initialize:
    obj1->id = 1;
    strcpy(obj1->name, "slab_object");

Why slab exists:
    Repeated allocations of the same object type are common in the kernel.
    General-purpose alloc/free each time is less efficient than object caches.

Benefits:
    - faster repeated allocation/deallocation
    - better cache locality
    - reduced fragmentation for fixed-size objects
    - constructor hooks possible in advanced usage

Meaning of flags:
    SLAB_HWCACHE_ALIGN
        -> align objects to improve CPU cache behavior

    SLAB_NO_MERGE
        -> keep this cache distinct; do not merge with compatible caches

OUTPUT
──────
    my_cache -> object cache definition
    obj1     -> one allocated my_object from that cache

WHEN TO CHOOSE SLAB CACHE
─────────────────────────
Use when you need:
    - many allocations of the same struct type
    - repeated object lifecycle
    - performance and reuse matter

Examples in real kernel design:
    - inode-like objects
    - request descriptors
    - protocol/session objects
    - driver-internal repeated structs

========================================================================================================
INIT FAILURE ROLLBACK — WHY THE goto ORDER MATTERS
========================================================================================================

RESOURCE ACQUISITION ORDER
──────────────────────────
    1. kmalloc_ptr
    2. kzalloc_ptr
    3. vmalloc_ptr
    4. my_cache
    5. obj1

If later acquisition fails, earlier ones must be released.

FAILURE PATH DIAGRAM
────────────────────
if obj1 allocation fails:
    goto label_kmem_cache_free
        │
        ▼
    kmem_cache_destroy(my_cache)
        │
        ▼
    vfree(vmalloc_ptr)
        │
        ▼
    kfree(kzalloc_ptr)
        │
        ▼
    kfree(kmalloc_ptr)
        │
        ▼
    return -ENOMEM

if cache creation fails:
    goto label_vmalloc_ptr_free
        │
        ▼
    vfree(vmalloc_ptr)
        │
        ▼
    kfree(kzalloc_ptr)
        │
        ▼
    kfree(kmalloc_ptr)

if vmalloc fails:
    goto label_kzalloc_ptr_free
        │
        ▼
    kfree(kzalloc_ptr)
        │
        ▼
    kfree(kmalloc_ptr)

if kzalloc fails:
    goto label_kmalloc_ptr_free
        │
        ▼
    kfree(kmalloc_ptr)

Why reverse order:
    Because each later step depends on the system state created by earlier steps.
    Reverse-order unwind is the standard kernel lifetime discipline.

========================================================================================================
MODULE EXIT FLOW
========================================================================================================

rmmod memory_alloc_example
        │
        ▼
┌──────────────────────────────────────────────────────────────────────────────────────────────────────┐
│ my_exit()                                                                                            │
│                                                                                                      │
│   kfree(kmalloc_ptr);                                                                                │
│   kfree(kzalloc_ptr);                                                                                │
│   vfree(vmalloc_ptr);                                                                                │
│   kmem_cache_free(my_cache, obj1);                                                                   │
│   kmem_cache_destroy(my_cache);                                                                      │
│                                                                                                      │
│   pr_info("Memory freed\n");                                                                         │
└──────────────────────────────────────────────────────────────────────────────────────────────────────┘

SENIOR LIFETIME INTERPRETATION
──────────────────────────────
You are destroying:
    - raw buffers
    - then slab object
    - then the slab cache itself

Conceptually for slab:
    object must be returned before cache is destroyed

So the dependency is:
    obj1 depends on my_cache
Therefore:
    kmem_cache_free(my_cache, obj1)
must happen before:
    kmem_cache_destroy(my_cache)

========================================================================================================
WHAT EACH POINTER REALLY REPRESENTS
========================================================================================================

kmalloc_ptr
    -> one general-purpose contiguous kernel byte buffer
    -> best thought of as "ordinary small kernel memory"

kzalloc_ptr
    -> same as above, but guaranteed zero-initialized
    -> best thought of as "safe clean small kernel memory"

vmalloc_ptr
    -> one large virtually contiguous byte region
    -> best thought of as "CPU-usable large region without physical contiguity guarantee"

my_cache
    -> not data itself
    -> it is a cache manager / allocator definition for struct my_object

obj1
    -> one actual object instance allocated from my_cache

========================================================================================================
ADDRESS / CONTIGUITY VIEW
========================================================================================================

KMALLOC / KZALLOC
─────────────────
virtual:
    [A A A A A A A A]
physical:
    [P P P P P P P P]
contiguous in both views

VMALLOC
───────
virtual:
    [A A A A A A A A]
physical:
    [P1][P9][P42][P7][P103]...
contiguous virtually, scattered physically

SLAB OBJECT
───────────
obj1 is one object slot inside a slab-managed region:
    cache -> slab pages -> object slots -> obj1

========================================================================================================
OUTPUTS OF THIS MODULE
========================================================================================================

During load:
    - kernel logs show kmalloc virtual address and physical translation
    - kernel logs show kzalloc virtual address
    - kernel logs show vmalloc virtual address
    - kernel logs show slab object address

Functional outputs:
    kmalloc_ptr usable
    kzalloc_ptr usable and zeroed
    vmalloc_ptr usable for large virtual range
    obj1 usable as typed struct

During unload:
    - all memory released
    - "Memory freed" log printed

========================================================================================================
HOW A SENIOR DEVELOPER CHOOSES BETWEEN THEM
========================================================================================================

QUESTION 1: "Do I just need a normal small kernel buffer?"
        -> kmalloc

QUESTION 2: "Do I want the same, but safely zero-initialized?"
        -> kzalloc

QUESTION 3: "Do I need a larger region and do not care about physical contiguity?"
        -> vmalloc

QUESTION 4: "Will I allocate/free lots of identical structs repeatedly?"
        -> kmem_cache_create + kmem_cache_alloc

========================================================================================================
IMPORTANT DESIGN DISTINCTION
========================================================================================================

These allocators solve DIFFERENT PROBLEMS:

kmalloc/kzalloc: solve "give me bytes"
vmalloc: solves "give me a larger CPU-visible virtually contiguous region"
slab cache: solves "give me many efficient objects of one shape"

So the module is really teaching allocation by categories:
    raw bytes
    raw zeroed bytes
    large mapped bytes
    repeated typed objects

========================================================================================================
FINAL SENIOR TAKEAWAY
========================================================================================================

This code is best understood as a memory-allocation decision tree:
    Need kernel memory
        │
        ├── small / ordinary / contiguous?         -> kmalloc
        ├── same but zeroed?                       -> kzalloc
        ├── larger / only virtual contiguity?      -> vmalloc
        └── repeated same-sized typed objects?     -> slab cache

And the lifetime rule remains:
    acquire forward
    free in reverse

That is the core engineering model behind this example.

========================================================================================================
```