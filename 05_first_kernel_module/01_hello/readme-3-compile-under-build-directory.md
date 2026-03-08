Because with **external kernel modules**, `M=` is not just an “output directory”. It is treated as the **module directory** by Kbuild.

That is the core reason.

## Your command

```bash
make -C /lib/modules/$(uname -r)/build M=/path/to/build src=/path/to/src modules
```

You expected:

* source files in `src`
* generated files in `build`

But Kbuild interprets:

* `M=/path/to/build` as: **this is the external module tree**
* therefore it looks in that directory for the module description and object relationships

So when your Makefile says:

```make
obj-m += hello.o
```

Kbuild thinks:

* module object: `hello.o`
* module directory: `/path/to/build`
* source it should be able to derive: `/path/to/build/hello.c` or an equivalent explicit Kbuild mapping

Since `hello.c` is not there, it cannot form the rule.

That is why you got:

```text
No rule to make target '/.../build/hello.o'
```

---

# The senior-level explanation

## 1. In normal GNU Make thinking, build dir and source dir are separate concepts

In many user-space projects, this is normal:

```bash
src/   -> contains .c
build/ -> contains .o
```

and the build system explicitly knows rules like:

```make
build/hello.o: src/hello.c
```

So Make understands the dependency graph.

But **Kbuild is not a generic out-of-source build system in the same way**.

---

## 2. In Kbuild, `M=` means “external module directory”

When you run:

```bash
make -C $(KDIR) M=$(PWD) modules
```

the kernel build system says:

> I am building an external module located at `$(PWD)`.

That directory is expected to contain the Kbuild metadata for the module, typically:

* `Makefile` or `Kbuild`
* source files referenced by Kbuild
* intermediate outputs

So `M=` is effectively:

* module definition directory
* source discovery base
* object output location

all at once for the simple external-module flow.

---

## 3. Why `obj-m += hello.o` is not enough for split source/output

This line:

```make
obj-m += hello.o
```

is only a **declaration of module objects**.

It does **not** define an explicit GNU Make rule like:

```make
/build/hello.o: /src/hello.c
```

It simply tells Kbuild:

> There is a module named `hello`, built from `hello.o`.

Then Kbuild applies its own conventions to locate the source.

For the common external module case, that convention is effectively:

* current module tree = `M`
* source is expected relative to that module tree

So if:

```bash
M=/.../build
```

then Kbuild tries to resolve `hello` under `build/`.

---

## 4. Why `src=` did not save it

You used:

```bash
src=/path/to/source
```

This often creates confusion because people think it fully separates source and object trees like CMake or Autotools.

But in external module builds, `src=` is not a magic full out-of-tree mapping for arbitrary cases.

Kbuild still builds around the module directory represented by `M=` / `obj=`.
If the Kbuild file is simplistic:

```make
obj-m += hello.o
```

there is no detailed rule graph telling Kbuild:

* exactly where `hello.c` is
* how `/build/hello.o` depends on `/src/hello.c`
* how to treat the parent source tree as the authoritative source root

So the source/object split is incomplete.

---

# What Kbuild is effectively doing

Your command causes the kernel build system to descend into:

```text
obj=/.../build
```

Then it tries to build:

```text
/.../build/hello.o
```

But no usable rule exists in that module context to produce it from:

```text
/.../01_hello/hello.c
```

So Make reports:

```text
No rule to make target '.../build/hello.o'
```

This is not because `build/` is forbidden.

It is because **you told Kbuild that `build/` is the module tree, but your module source layout and Kbuild declarations do not match that statement**.

---

# Why building in the source directory works

This works:

```make
obj-m += hello.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
```

because now:

* `M=$(PWD)` points to the directory containing `Makefile`
* `hello.c` is also in that same directory
* Kbuild’s default assumptions are satisfied

So it can infer:

```text
hello.c -> hello.o -> hello.ko
```

without extra mapping.

---

# The real mental model

Do not think of this:

```bash
M=build
```

as:

> write output files into build

Think of it as:

> the external module lives in build

That is a much more accurate model.

If the external module “lives in build”, then Kbuild expects its world to be there:

* Kbuild file
* source relationships
* object outputs

Your setup only put outputs there, not the full module structure.

---

# Why Linux kernel examples usually avoid this for beginners

Kernel external module examples almost always use:

```make
make -C /lib/modules/$(uname -r)/build M=$(PWD) modules
```

with source and build artifacts in the same directory because:

* it matches Kbuild assumptions
* it is robust
* it avoids hidden source-resolution issues
* it is easier to debug

The kernel build system is optimized for **kernel-style trees**, not for arbitrary user-space-style source/output separation.

---

# Can you build under `build/` at all?

Yes, but there are only two practical ways.

## Option 1: Put the whole module tree in `build/`

Structure:

```text
01_hello/
└── build/
    ├── Makefile
    └── hello.c
```

Then:

```bash
cd build
make
```

This works because now `build/` is truly the module directory.

---

## Option 2: Use a more advanced Kbuild split layout

This requires a more explicit arrangement, usually involving:

* a dedicated `Kbuild`
* careful source path handling
* sometimes wrapper Makefiles
* sometimes separate invocation layers

This is possible, but it is no longer the simple canonical one-file external module example.

For learning, it adds complexity without much benefit.

---

# Why the error message mentions `build/hello.o`

Because from Kbuild’s point of view, that is the expected object target:

```text
obj = /.../build
target = hello.o
full path = /.../build/hello.o
```

And it cannot discover how to generate that target.

So the error is actually very accurate.

---

# Senior conclusion

You cannot build “under `build/`” with your current Makefile because your configuration mixes **two incompatible models**:

* **user-space model**: source in one place, objects in another
* **simple Kbuild external module model**: `M=` directory is the module’s working tree

Your line:

```make
obj-m += hello.o
```

only works cleanly when Kbuild can resolve `hello.c` relative to the module directory.
Once `M` points to `build/`, that assumption breaks unless you provide a more advanced Kbuild structure.

## Best practice

For kernel-module learning and most real driver bring-up:

```make
obj-m += hello.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
```

with:

```text
Makefile
hello.c
```

in the same directory.

That is the correct starting point.

If you want, I can next show you a **proper split source/build kernel module layout** that works, and explain exactly why each file is needed.
