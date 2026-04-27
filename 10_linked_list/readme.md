## Kernel Linked List
NOTE: This lecture is no longer available 
### Video :
[![Youtube Video](https://img.youtube.com/vi/FOJ0vXf2Qhw/0.jpg)](https://www.youtube.com/watch?v=FOJ0vXf2Qhw)

```
========================================================================================================
LINUX KERNEL LINKED LIST — SENIOR MENTAL MODEL (ONE COMPLETE TEXT DIAGRAM)
========================================================================================================

GOAL OF THIS MODULE
────────────────────────────────────────────────────────────────────────────────────────────────────────
This module is not about hardware access.
It is a kernel data-structure example showing how Linux kernel linked lists work with:
    1. embedded list metadata inside a user-defined struct
    2. compile-time list head initialization
    3. dynamic node allocation with kmalloc
    4. insertion at the tail
    5. traversal
    6. safe deletion and cleanup

So the real lesson is:
    "How does the Linux kernel represent and manage a doubly linked list without allocating a separate generic node wrapper type?"

========================================================================================================
CORE DESIGN IDEA
========================================================================================================

Instead of storing data inside a generic linked-list node like:
    node -> { data, next, prev }

the Linux kernel usually does this:
    your object -> { your fields..., struct list_head list; }

Meaning:
    the list metadata is EMBEDDED inside your real data structure

In this module:
    struct my_data {
        uint32_t id;
        char name[32];
        struct list_head list;
    };

So each application-level object is also a list node.

This is one of the most important kernel design patterns.

========================================================================================================
WHAT struct list_head REALLY IS
========================================================================================================

Kernel doubly linked list metadata:
    struct list_head {
        struct list_head *next;
        struct list_head *prev;
    };

Important:
    struct list_head does NOT know anything about:
        - id
        - name
        - struct my_data
        - your business logic

It only stores linkage.

So the kernel list system manages topology, while your container struct manages actual payload/data.

========================================================================================================
LIST SHAPE IN THIS MODULE
========================================================================================================

YOUR DATA OBJECT
────────────────
    struct my_data
        ├── id
        ├── name[32]
        └── list   (embedded kernel linked-list metadata)

LIST HEAD
─────────
    static LIST_HEAD(my_list);

Meaning:
    my_list itself is a struct list_head
    and acts as the SENTINEL / ANCHOR of the whole list

This is a circular doubly linked list design.

Empty list state:
    my_list.next = &my_list
    my_list.prev = &my_list

Diagram of empty list:
    
          ┌──────────────────────┐
          │      my_list         │
          │  next ───────────┐   │
          │  prev ─────────┐ │   │
          └────────────────┼─┼───┘
                           │ │
                           └─┴──> points back to itself

Why this design is useful:
    - no NULL checks for normal insertion/removal edges
    - head is a real list node object, not a special-case pointer variable
    - empty/non-empty operations become simpler and faster

========================================================================================================
COMPILE-TIME INITIALIZATION
========================================================================================================

CODE
────
    static LIST_HEAD(my_list);

MENTAL MODEL
────────────
This macro creates a list head and initializes it at compile time.

Equivalent conceptual state:
    my_list.next = &my_list;
    my_list.prev = &my_list;

Output:
    a valid empty circular doubly linked list exists before module init runs

Why this matters:
    you do NOT need to allocate the list head dynamically and you do NOT need a separate "is_initialized" phase for the head

========================================================================================================
MODULE INIT — RESOURCE CREATION PHASE
========================================================================================================

my_init()
    │
    ├── declare:
    │      struct my_data *tmp, *next;
    │      struct list_head *ptr;
    │
    ├── allocate node #1
    ├── fill node #1 fields
    ├── add node #1 to tail
    │
    ├── allocate node #2
    ├── fill node #2 fields
    ├── add node #2 to tail
    │
    ├── allocate node #3
    ├── fill node #3 fields
    ├── add node #3 to tail
    │
    ├── iterate list and print contents
    └── return success

If any allocation fails:
    goto mem_free
        └── safely iterate existing nodes, remove them, kfree them, abort init

========================================================================================================
STEP 1 — NODE ALLOCATION
========================================================================================================

CODE PATTERN
────────────
    tmp = kmalloc(sizeof(struct my_data), GFP_KERNEL);
    if (!tmp)
        goto mem_free;

Meaning:
    allocate one whole my_data object in kernel memory

Important:
    the kernel list does NOT allocate nodes for you
    you allocate your object, and because your object contains struct list_head, it can participate in the list

Diagram:
before allocation:
    my_list (empty)

after allocation, before insertion:
    tmp ─────────────► [ id=?, name=?, list.next=?, list.prev=? ]

At this moment:
    tmp exists in memory
    but it is NOT yet part of the list

Why this distinction matters:
    allocation creates an object
    insertion links the object into list topology
    these are two separate phases

========================================================================================================
STEP 2 — INITIALIZE PAYLOAD
========================================================================================================

CODE PATTERN
────────────
    tmp->id = 0;
    strcpy(tmp->name, "MP Coding");

Meaning:
    fill the business data fields before or around insertion

Current three payloads created:
    node 1 -> id=0, name="MP Coding"
    node 2 -> id=1, name="Hello World"
    node 3 -> id=2, name="Madhawa Polkotuwa"

At this moment:
    payload is ready
    list linkage still becomes meaningful only after insertion

========================================================================================================
STEP 3 — INSERT INTO LIST
========================================================================================================

CODE
────
    list_add_tail(&tmp->list, &my_list);

MENTAL MODEL
────────────
You are NOT adding `tmp` directly.
You are adding the embedded list node inside tmp:
    &tmp->list

to the list anchored by:
    &my_list

So the kernel only manipulates struct list_head links.

Conceptually:
    add this node just before the sentinel head,
    which means "at the logical end of the list"

Why tail insertion:
    preserves the order in which nodes were created

========================================================================================================
HOW THE CIRCULAR DOUBLY LINKED STRUCTURE LOOKS
========================================================================================================

After inserting first node:
    my_list <-> node0 <-> my_list

Detailed view:

    my_list.next ─────────────► node0.list
    my_list.prev ─────────────► node0.list

    node0.list.next ──────────► my_list
    node0.list.prev ──────────► my_list

After inserting second node at tail:
    my_list <-> node0 <-> node1 <-> my_list

After inserting third node at tail:
    my_list <-> node0 <-> node1 <-> node2 <-> my_list

Full conceptual view:

        ┌───────── sentinel / anchor ─────────┐
        │                                     │
        ▼                                     │
    [my_list] <-> [node0.list] <-> [node1.list] <-> [node2.list]
        ▲                                                    │
        └────────────────────────────────────────────────────┘

Payload attached to each node:
    node0.list belongs to struct my_data { id=0, name="MP Coding", ... }
    node1.list belongs to struct my_data { id=1, name="Hello World", ... }
    node2.list belongs to struct my_data { id=2, name="Madhawa Polkotuwa", ... }

Key point:
    the list walks through list_head objects,
    but each list_head lives inside a real struct my_data object

========================================================================================================
TRAVERSAL PHASE — list_for_each
========================================================================================================

CODE
────
    list_for_each(ptr, &my_list) {
        tmp = list_entry(ptr, struct my_data, list);
        pr_info(... tmp->id, tmp->name ...);
    }

MENTAL MODEL
────────────
The macro `list_for_each` iterates using a cursor of type:
    struct list_head *ptr

So it walks through linkage nodes, not your payload type directly.
Traversal sequence:
    ptr = my_list.next
    while (ptr != &my_list) {
        ...
        ptr = ptr->next;
    }

That means:
    iteration stops when it comes back to the head

Why ptr is struct list_head *:
    because the generic list API only knows list topology, not your struct type

========================================================================================================
CONVERTING FROM LINK NODE TO REAL OBJECT — list_entry
========================================================================================================

CODE
────
    tmp = list_entry(ptr, struct my_data, list);

MENTAL MODEL
────────────
You currently have:
    ptr -> points to tmp->list

But you actually want:
    tmp -> pointer to the surrounding struct my_data

So `list_entry` performs the classic kernel "container_of" style conversion:
    "Given pointer to member `list`, compute pointer to parent struct `my_data` that contains it."

Diagram:
    struct my_data
    ┌───────────────────────────────┐
    │ id                            │
    │ name[32]                      │
    │ list   <──── ptr points here  │
    └───────────────────────────────┘
      ▲
      │
      tmp returned by list_entry(...)

Why this is powerful:
    a single generic list implementation can work for any struct type as long as that struct embeds a list_head member

========================================================================================================
WHAT GETS PRINTED DURING ITERATION
========================================================================================================

For each element:
    tmp->id
    tmp->name

So iteration output is logically:
    Element id:0 name:MP Coding
    Element id:1 name:Hello World
    Element id:2 name:Madhawa Polkotuwa

Meaning:
    the list preserves insertion order because list_add_tail was used

========================================================================================================
FAILURE PATH — WHY mem_free EXISTS
========================================================================================================

Suppose allocation fails on node #2 or node #3.

Without cleanup:
    earlier successfully allocated nodes remain linked and allocated
    module init returns error
    kernel memory leaks

So the module uses:
    goto mem_free

This label performs cleanup of every node already inserted into my_list.

This is correct kernel lifetime discipline:
    once you begin building a dynamic structure,
    failure must unwind all successfully acquired resources

========================================================================================================
SAFE DELETION WHILE ITERATING — list_for_each_entry_safe
========================================================================================================

CODE
────
    list_for_each_entry_safe(tmp, next, &my_list, list) {
        list_del(&tmp->list);
        kfree(tmp);
    }

MENTAL MODEL
────────────
This macro iterates directly in terms of your container type:
    tmp  = current struct my_data *
    next = saved next struct my_data *

Why "safe" version is needed:
    deleting current node invalidates its linkage for continuing traversal
    so the next element must be saved before deletion

Conceptual safe loop:

    current = first element
    next    = current->next element

    while (current != end) {
        save next first
        unlink current
        free current
        current = next
    }

If you used a non-safe iteration macro while deleting:
    you could lose traversal state or dereference invalid memory

========================================================================================================
DELETION PHASE — list_del
========================================================================================================

CODE
────
    list_del(&tmp->list);

MENTAL MODEL
────────────
This unlinks the current node from the doubly linked topology.

Before:
    prev <-> current <-> next

After:
    prev <---------------> next

The node memory still exists after list_del.
Only the topology link is removed.

Then:
    kfree(tmp);

actually releases the memory.

Important distinction:
    list_del   = unlink from list
    kfree      = free storage

You normally need both when the node was dynamically allocated.

========================================================================================================
EXIT PATH — FULL STRUCTURE TEARDOWN
========================================================================================================

my_exit()
    │
    ├── iterate safely over all remaining elements
    ├── list_del each node
    ├── kfree each node
    └── print goodbye message

This is the same fundamental cleanup as mem_free,
but here it is the normal module-unload path rather than error unwind.

So the lifetime model is:

    successful init:
        build list
    successful exit:
        destroy whole list

    failed init:
        partially build list
        then destroy whatever was built so far

========================================================================================================
WHY LINKED LISTS IN KERNEL INSTEAD OF ARRAYS
========================================================================================================

ARRAY MODEL
───────────
    data stored contiguously
    random indexing is easy
    insert/delete in middle is expensive
    may require realloc/move/copy
    fixed or resize-heavy management

KERNEL LINKED LIST MODEL
────────────────────────
    nodes can live anywhere in memory
    insertion/removal is cheap once node pointer is known
    no need for contiguous allocation of the whole sequence
    traversal is linear
    random access by index is poor

Why kernel often likes linked lists:
    many kernel subsystems manage dynamic sets of objects where:
        - insertions/deletions are frequent
        - object lifetime is independent
        - contiguous bulk storage is not required

Examples in real kernel subsystems:
    - task lists
    - device lists
    - wait queues / internal queues
    - module-maintained object registries

========================================================================================================
DOUBLY LINKED + CIRCULAR — WHY THIS DESIGN
========================================================================================================

This Linux list API uses a circular doubly linked list.

Why doubly linked:
    each node knows both previous and next
    removal is efficient when you already have the node

Why circular:
    the head is a sentinel
    no special NULL terminator cases
    empty/non-empty checks and splicing operations become cleaner

So the head is not "data node zero";
it is an anchor object that simplifies list algorithms.

========================================================================================================
TYPE ROLES IN THIS MODULE
========================================================================================================

struct my_data *tmp : current my_data object being allocated or processed
struct my_data *next : temporary saved next object for safe deletion traversal
struct list_head *ptr : low-level generic cursor used by list_for_each
static LIST_HEAD(my_list) : sentinel list anchor, always exists for module lifetime

========================================================================================================
WHAT THE LIST REALLY OWNS VS WHAT THE MODULE OWNS
========================================================================================================

The list itself does NOT allocate or free memory automatically.

The module owns:
    - node allocation via kmalloc
    - node payload initialization
    - node insertion
    - node removal
    - node freeing

The kernel list API only provides:
    - metadata structure
    - linkage operations
    - iteration macros
    - container conversion helpers

This is a critical senior understanding:
    Linux kernel lists manage relationships between objects, not object lifetime by themselves.

========================================================================================================
FINAL SHAPE AFTER SUCCESSFUL INIT
========================================================================================================

Memory contains roughly:
    my_list (static sentinel head)

    nodeA: struct my_data
        id   = 0
        name = "MP Coding"
        list = {prev=?, next=?}

    nodeB: struct my_data
        id   = 1
        name = "Hello World"
        list = {prev=?, next=?}

    nodeC: struct my_data
        id   = 2
        name = "Madhawa Polkotuwa"
        list = {prev=?, next=?}

Topology:

    my_list
      ├── next -> nodeA.list
      └── prev -> nodeC.list

    nodeA.list
      ├── prev -> my_list
      └── next -> nodeB.list

    nodeB.list
      ├── prev -> nodeA.list
      └── next -> nodeC.list

    nodeC.list
      ├── prev -> nodeB.list
      └── next -> my_list

========================================================================================================
OUTPUTS OF THIS MODULE
========================================================================================================

On load:
    - three nodes allocated
    - three nodes linked into my_list
    - iteration prints all three payloads
    - "Added 3 elements to the list!" message printed

On failed init:
    - any previously inserted nodes are removed and freed
    - init aborts cleanly with -ENOMEM

On unload:
    - all nodes are safely removed and freed
    - "Goodbye Kernel" printed

========================================================================================================
KEY SENIOR TAKEAWAY
========================================================================================================

The Linux kernel linked-list model is:
    your object
        contains
    embedded struct list_head
        which lets generic kernel list code link objects together

So the true conceptual flow is:
    allocate real object
        ↓
    initialize payload
        ↓
    insert embedded list_head into sentinel-anchored circular doubly linked list
        ↓
    traverse via generic list_head cursors
        ↓
    recover parent object using list_entry
        ↓
    unlink with list_del
        ↓
    free object with kfree

That is the core engineering mental model behind kernel linked lists.

========================================================================================================
```