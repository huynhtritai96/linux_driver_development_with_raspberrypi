#include <linux/module.h>
#include <linux/slab.h>    // for kmalloc, kzalloc
#include <linux/vmalloc.h> // for vmalloc
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MPCoding - LDD");
MODULE_DESCRIPTION("Memory Allocation Example");

static char *kmalloc_ptr; // kmalloc: physically contiguous, no zeroing
static char *kzalloc_ptr; // kzalloc: same as kmalloc but zeros memory
static char *vmalloc_ptr; // vmalloc: virtually contiguous, suitable for large blocks

#define ALLOC_SIZE_SMALL  1024              // small allocation
#define ALLOC_SIZE_LARGE  (1024 * 1024)     // 1 MB, large allocation, not physically contiguous

struct my_object {
    int id;
    char name[32];
};

static struct kmem_cache *my_cache;
static struct my_object  *obj1;

static int __init my_init(void) 
{
    // kmalloc: physically contiguous, no zeroing
    // GFP_KERNEL: Can sleep, most common flag for kernel allocations. 
    // It allows the allocator to block if memory is not immediately available, 
    // which is suitable for most kernel code that can afford to wait.
    // in slab.h file
    kmalloc_ptr = kmalloc(ALLOC_SIZE_SMALL, GFP_KERNEL); 
    if (!kmalloc_ptr)
    {
        pr_err("kmalloc failed\n");
        return -ENOMEM;
    }
    // virt_to_phys() converts a virtual address to a physical address, which is useful for debugging and understanding memory layout.
    pr_info("kmalloc allocated at %px (physical %llx)\n", kmalloc_ptr, virt_to_phys(kmalloc_ptr));


    // kzalloc: same as kmalloc but zeros memory
    kzalloc_ptr = kzalloc(ALLOC_SIZE_SMALL, GFP_KERNEL);
    if (!kzalloc_ptr)
    {
        pr_err("kzalloc failed\n");
        goto label_kmalloc_ptr_free;
    }
    pr_info("kzalloc allocated at %px\n", kzalloc_ptr);


    // vmalloc: virtually contiguous, suitable for large blocks
    vmalloc_ptr = vmalloc(ALLOC_SIZE_LARGE);
    if (!vmalloc_ptr)
    {
        pr_err("vmalloc failed\n");
        goto label_kzalloc_ptr_free;
    }
    pr_info("vmalloc allocated at %px\n", vmalloc_ptr);


    // slab allocation
    // kmem_cache_create() creates a cache for objects of a specific size, 
    // allowing for efficient allocation and deallocation of those objects.
    // Return: a pointer to the cache on success, NULL on failure.
    my_cache = kmem_cache_create("my_cache",                // name of the cache
                                 sizeof(struct my_object),  // size of each object in the cache
                                 0,                         // alignment (0 means default alignment)
                                 SLAB_HWCACHE_ALIGN | SLAB_NO_MERGE, 
                                                            // SLAB_HWCACHE_ALIGN: Align objects to cache lines for better performance. 
                                                            // SLAB_NO_MERGE: Prevent merging of this cache with others, sudo cat /proc/slabinfo | grep kmalloc-64
                                                            // ensuring it remains separate and optimized for its specific object size.
                                 NULL                       // constructor function, can be NULL if no initialization is needed
                                );

    if (!my_cache) goto label_vmalloc_ptr_free;

    obj1 = kmem_cache_alloc(my_cache, GFP_KERNEL); // Allocate an object from the cache/slab (Memory Page)
    // obj2 = kmem_cache_alloc(my_cache, GFP_KERNEL); // Dont need to allocate another Memory Page for another object, it will be allocated from the same Memory Page as obj1
    // No fragmentation, can reuse 
    if (!obj1)
    {
        goto label_kmem_cache_free;
    }
    // Initialize slab object
    obj1->id = 1;                       // Set the id field of the slab object to 1
    strcpy(obj1->name, "slab_object");  // Set the name field of the slab object to "slab_object"

    pr_info("Slab object allocated at %px\n", obj1);

    return 0;


label_kmem_cache_free:
    kmem_cache_destroy(my_cache);
    // No need to free obj1 separately as kmem_cache_destroy will free all objects in the cache

label_vmalloc_ptr_free:
    vfree(vmalloc_ptr);

label_kzalloc_ptr_free:
    kfree(kzalloc_ptr);

label_kmalloc_ptr_free:
    kfree(kmalloc_ptr);

    return -ENOMEM;
}

static void __exit my_exit(void)
{
    kfree(kmalloc_ptr);
    kfree(kzalloc_ptr);
    vfree(vmalloc_ptr);
    kmem_cache_free(my_cache, obj1);    // Free the allocated slab object back to the cache. This does not free the memory page itself, but marks the object as available for future allocations.
    kmem_cache_destroy(my_cache);       // Destroy the cache, which will free all memory pages associated with it. This should be done after all objects have been freed back to the cache.

    pr_info("Memory freed\n");
}

module_init(my_init);
module_exit(my_exit);

/*

Terminal 1:
$ make
make -C /lib/modules/6.12.47+rpt-rpi-v8/build M=/home/htritai/ldd/linux_driver_development_with_raspberrypi/09_memory_alloc modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/09_memory_alloc/memory_alloc.o
  MODPOST /home/htritai/ldd/linux_driver_development_with_raspberrypi/09_memory_alloc/Module.symvers
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/09_memory_alloc/memory_alloc.mod.o
  CC [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/09_memory_alloc/.module-common.o
  LD [M]  /home/htritai/ldd/linux_driver_development_with_raspberrypi/09_memory_alloc/memory_alloc.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-v8'

htritai@Rasp26:~/ldd/linux_driver_development_with_raspberrypi/09_memory_alloc $ sudo insmod memory_alloc.ko
htritai@Rasp26:~/ldd/linux_driver_development_with_raspberrypi/09_memory_alloc $ sudo rmmod memory_alloc 

 $ sudo cat /proc/meminfo 
MemTotal:        3887992 kB <----------- Total physical memory available
MemFree:         1402644 kB <----------- Free memory available for allocation
MemAvailable:    2132764 kB
Buffers:           14948 kB
Cached:           705912 kB
SwapCached:        37048 kB
Active:          1476080 kB
Inactive:         783676 kB
Active(anon):    1188284 kB
Inactive(anon):   363952 kB
Active(file):     287796 kB
Inactive(file):   419724 kB
Unevictable:        9568 kB
Mlocked:               0 kB
SwapTotal:        524284 kB
SwapFree:         382204 kB
Zswap:                 0 kB
Zswapped:              0 kB
Dirty:               460 kB
Writeback:             0 kB
AnonPages:       1518188 kB
Mapped:           283284 kB
Shmem:             13344 kB
KReclaimable:      92584 kB
Slab:             128212 kB <----------- Memory used by kernel data structures (slab allocator)
SReclaimable:      92584 kB
SUnreclaim:        35628 kB
KernelStack:        6320 kB
PageTables:        11644 kB
SecPageTables:         0 kB
NFS_Unstable:          0 kB
Bounce:                0 kB
WritebackTmp:          0 kB
CommitLimit:     2468280 kB
Committed_AS:    2461480 kB
VmallocTotal:   261087232 kB
VmallocUsed:       26996 kB
VmallocChunk:          0 kB
Percpu:              768 kB
CmaTotal:         524288 kB
CmaFree:          231648 kB


 $ sudo cat /proc/slabinfo <--- find the memory allocated for our slab object "my_cache", free, available for allocation, and total memory allocated for the slab cache "my_cache"
slabinfo - version: 2.1
# name            <active_objs> <num_objs> <objsize> <objperslab> <pagesperslab> : tunables <limit> <batchcount> <sharedfactor> : slabdata <active_slabs> <num_slabs> <sharedavail>
fuse_request          56     56    144   28    1 : tunables    0    0    0 : slabdata      2      2      0
fuse_inode            18     18    896   18    4 : tunables    0    0    0 : slabdata      1      1      0
kcopyd_job             0      0   3240   10    8 : tunables    0    0    0 : slabdata      0      0      0
ip6-frags              0      0    184   22    1 : tunables    0    0    0 : slabdata      0      0      0
UDPv6                120    120   1344   24    8 : tunables    0    0    0 : slabdata      5      5      0
tw_sock_TCPv6        352    496    248   16    1 : tunables    0    0    0 : slabdata     31     31      0
request_sock_TCPv6      0      0    312   26    2 : tunables    0    0    0 : slabdata      0      0      0
TCPv6                210    299   2496   13    8 : tunables    0    0    0 : slabdata     23     23      0
ext4_groupinfo_4k    242    242    184   22    1 : tunables    0    0    0 : slabdata     11     11      0
fscrypt_inode_info      0      0    120   34    1 : tunables    0    0    0 : slabdata      0      0      0
bio-120              192    192    128   32    1 : tunables    0    0    0 : slabdata      6      6      0
io_kiocb             432    576    256   16    1 : tunables    0    0    0 : slabdata     36     36      0
bfq_io_cq              0      0   1360   24    8 : tunables    0    0    0 : slabdata      0      0      0
bio-248               16     16    256   16    1 : tunables    0    0    0 : slabdata      1      1      0
mqueue_inode_cache     17     17    960   17    4 : tunables    0    0    0 : slabdata      1      1      0
f2fs_extent_node       0      0     72   56    1 : tunables    0    0    0 : slabdata      0      0      0
f2fs_discard_entry      0      0     88   46    1 : tunables    0    0    0 : slabdata      0      0      0
f2fs_free_nid          0      0     24  170    1 : tunables    0    0    0 : slabdata      0      0      0
f2fs_inode_cache       0      0   1072   30    8 : tunables    0    0    0 : slabdata      0      0      0
nfs4_xattr_cache_cache      0      0   2128   15    8 : tunables    0    0    0 : slabdata      0      0      0
nfs_direct_cache       0      0    216   18    1 : tunables    0    0    0 : slabdata      0      0      0
nfs_read_data         36     36    896   18    4 : tunables    0    0    0 : slabdata      2      2      0
nfs_inode_cache        0      0   1064   30    8 : tunables    0    0    0 : slabdata      0      0      0
fat_inode_cache       42     42    776   21    4 : tunables    0    0    0 : slabdata      2      2      0
fat_cache            204    204     40  102    1 : tunables    0    0    0 : slabdata      2      2      0
jbd2_journal_head    340    340    120   34    1 : tunables    0    0    0 : slabdata     10     10      0
jbd2_revoke_table_s    256    256     16  256    1 : tunables    0    0    0 : slabdata      1      1      0
ext4_inode_cache   53705  53760   1160   28    8 : tunables    0    0    0 : slabdata   1920   1920      0
ext4_allocation_context    104    104    152   26    1 : tunables    0    0    0 : slabdata      4      4      0
ext4_prealloc_space    144    144    112   36    1 : tunables    0    0    0 : slabdata      4      4      0
ext4_system_zone     102    102     40  102    1 : tunables    0    0    0 : slabdata      1      1      0
bio_post_read_ctx    170    170     48   85    1 : tunables    0    0    0 : slabdata      2      2      0
pending_reservation    512    512     32  128    1 : tunables    0    0    0 : slabdata      4      4      0
extent_status      10194  14280     40  102    1 : tunables    0    0    0 : slabdata    140    140      0
mb_cache_entry       803    803     56   73    1 : tunables    0    0    0 : slabdata     11     11      0
kioctx                 0      0    576   28    4 : tunables    0    0    0 : slabdata      0      0      0
dnotify_struct         0      0     32  128    1 : tunables    0    0    0 : slabdata      0      0      0
kvm_vcpu               0      0   7040    4    8 : tunables    0    0    0 : slabdata      0      0      0
rpc_inode_cache       23     23    704   23    4 : tunables    0    0    0 : slabdata      1      1      0
ip4-frags              0      0    200   20    1 : tunables    0    0    0 : slabdata      0      0      0
MPTCP                  0      0   1984   16    8 : tunables    0    0    0 : slabdata      0      0      0
request_sock_subflow_v4      0      0    384   21    2 : tunables    0    0    0 : slabdata      0      0      0
xfrm_dst               0      0    320   25    2 : tunables    0    0    0 : slabdata      0      0      0
xfrm_state             0      0    832   19    4 : tunables    0    0    0 : slabdata      0      0      0
ip_fib_trie          255    255     48   85    1 : tunables    0    0    0 : slabdata      3      3      0
ip_fib_alias         219    219     56   73    1 : tunables    0    0    0 : slabdata      3      3      0
tw_sock_TCP          400    544    248   16    1 : tunables    0    0    0 : slabdata     34     34      0
request_sock_TCP      78     78    312   26    2 : tunables    0    0    0 : slabdata      3      3      0
TCP                  130    252   2304   14    8 : tunables    0    0    0 : slabdata     18     18      0
bio-240               32     32    256   16    1 : tunables    0    0    0 : slabdata      2      2      0
ep_head             1536   1536     16  256    1 : tunables    0    0    0 : slabdata      6      6      0
request_queue         32     32    968   16    4 : tunables    0    0    0 : slabdata      2      2      0
bio-184              567    672    192   21    1 : tunables    0    0    0 : slabdata     32     32      0
biovec-128            64     64   2048   16    8 : tunables    0    0    0 : slabdata      4      4      0
user_namespace         0      0    608   26    4 : tunables    0    0    0 : slabdata      0      0      0
uid_cache            128    128    128   32    1 : tunables    0    0    0 : slabdata      4      4      0
iommu_iova_magazine    112    112   1024   16    4 : tunables    0    0    0 : slabdata      7      7      0
dmaengine-unmap-2    741    768     64   64    1 : tunables    0    0    0 : slabdata     12     12      0
sock_inode_cache     710    760    832   19    4 : tunables    0    0    0 : slabdata     40     40      0
skbuff_small_head    462    506    704   23    4 : tunables    0    0    0 : slabdata     22     22      0
skbuff_head_cache    833    864    256   16    1 : tunables    0    0    0 : slabdata     54     54      0
tracefs_inode_cache    144    144    656   24    4 : tunables    0    0    0 : slabdata      6      6      0
configfs_dir_cache    184    184     88   46    1 : tunables    0    0    0 : slabdata      4      4      0
file_lease_cache       0      0    160   25    1 : tunables    0    0    0 : slabdata      0      0      0
fsnotify_mark_connector   1700   1700     24  170    1 : tunables    0    0    0 : slabdata     10     10      0
buffer_head         7503  22932    104   39    1 : tunables    0    0    0 : slabdata    588    588      0
task_delay_info        0      0    144   28    1 : tunables    0    0    0 : slabdata      0      0      0
taskstats             72     72    432   18    2 : tunables    0    0    0 : slabdata      4      4      0
proc_dir_entry      1050   1050    192   21    1 : tunables    0    0    0 : slabdata     50     50      0
proc_inode_cache    2962   3312    696   23    4 : tunables    0    0    0 : slabdata    144    144      0
seq_file             136    136    120   34    1 : tunables    0    0    0 : slabdata      4      4      0
sigqueue            1606   2091     80   51    1 : tunables    0    0    0 : slabdata     41     41      0
bdev_cache            63     63   1536   21    8 : tunables    0    0    0 : slabdata      3      3      0
shmem_inode_cache   1477   1554    752   21    4 : tunables    0    0    0 : slabdata     74     74      0
kernfs_iattrs_cache    561    561     80   51    1 : tunables    0    0    0 : slabdata     11     11      0
kernfs_node_cache  23238  23370    136   30    1 : tunables    0    0    0 : slabdata    779    779      0
mnt_cache            696    777    384   21    2 : tunables    0    0    0 : slabdata     37     37      0
filp                3400   3654    192   21    1 : tunables    0    0    0 : slabdata    174    174      0
inode_cache         3472   4186    624   26    4 : tunables    0    0    0 : slabdata    161    161      0
dentry             65258  68964    192   21    1 : tunables    0    0    0 : slabdata   3284   3284      0
names_cache          132    208   4096    8    8 : tunables    0    0    0 : slabdata     26     26      0
net_namespace         21     21   4480    7    8 : tunables    0    0    0 : slabdata      3      3      0
uts_namespace         54     54    432   18    2 : tunables    0    0    0 : slabdata      3      3      0
nsproxy              224    224     72   56    1 : tunables    0    0    0 : slabdata      4      4      0
vma_lock           11775  14382     40  102    1 : tunables    0    0    0 : slabdata    141    141      0
vm_area_struct     11732  13400    160   25    1 : tunables    0    0    0 : slabdata    536    536      0
files_cache          184    184    704   23    4 : tunables    0    0    0 : slabdata      8      8      0
signal_cache         307    448   1152   28    8 : tunables    0    0    0 : slabdata     16     16      0
sighand_cache        285    330   2112   15    8 : tunables    0    0    0 : slabdata     22     22      0
task_struct          412    435   8448    3    8 : tunables    0    0    0 : slabdata    145    145      0
anon_vma            4134   4758    104   39    1 : tunables    0    0    0 : slabdata    122    122      0
shared_policy_node      0      0     48   85    1 : tunables    0    0    0 : slabdata      0      0      0
numa_policy         1152   1152     32  128    1 : tunables    0    0    0 : slabdata      9      9      0
perf_event             0      0   1312   24    8 : tunables    0    0    0 : slabdata      0      0      0
trace_event_file    2352   2352     96   42    1 : tunables    0    0    0 : slabdata     56     56      0
ftrace_event_field   6862   6862     56   73    1 : tunables    0    0    0 : slabdata     94     94      0
pool_workqueue       416    496    512   16    2 : tunables    0    0    0 : slabdata     31     31      0
maple_node          1891   2848    256   16    1 : tunables    0    0    0 : slabdata    178    178      0
radix_tree_node    15515  15596    584   28    4 : tunables    0    0    0 : slabdata    557    557      0
task_group           100    100    640   25    4 : tunables    0    0    0 : slabdata      4      4      0
mm_struct            165    182   1216   26    8 : tunables    0    0    0 : slabdata      7      7      0
vmap_area           2280   6496     72   56    1 : tunables    0    0    0 : slabdata    116    116      0
kmalloc-cg-8k          0      0   8192    4    8 : tunables    0    0    0 : slabdata      0      0      0
kmalloc-cg-4k        115    184   4096    8    8 : tunables    0    0    0 : slabdata     23     23      0
kmalloc-cg-2k        138    176   2048   16    8 : tunables    0    0    0 : slabdata     11     11      0
kmalloc-cg-1k        532    640   1024   16    4 : tunables    0    0    0 : slabdata     40     40      0
kmalloc-cg-512       225    320    512   16    2 : tunables    0    0    0 : slabdata     20     20      0
kmalloc-cg-256        96     96    256   16    1 : tunables    0    0    0 : slabdata      6      6      0
kmalloc-cg-64       8379  10048     64   64    1 : tunables    0    0    0 : slabdata    157    157      0
kmalloc-cg-192       762    840    192   21    1 : tunables    0    0    0 : slabdata     40     40      0
kmalloc-cg-128      1313   1728    128   32    1 : tunables    0    0    0 : slabdata     54     54      0
dma-kmalloc-8k         0      0   8192    4    8 : tunables    0    0    0 : slabdata      0      0      0
dma-kmalloc-4k         0      0   4096    8    8 : tunables    0    0    0 : slabdata      0      0      0
dma-kmalloc-2k         0      0   2048   16    8 : tunables    0    0    0 : slabdata      0      0      0
dma-kmalloc-1k         0      0   1024   16    4 : tunables    0    0    0 : slabdata      0      0      0
dma-kmalloc-512        0      0    512   16    2 : tunables    0    0    0 : slabdata      0      0      0
dma-kmalloc-256        0      0    256   16    1 : tunables    0    0    0 : slabdata      0      0      0
dma-kmalloc-64         0      0     64   64    1 : tunables    0    0    0 : slabdata      0      0      0
dma-kmalloc-192        0      0    192   21    1 : tunables    0    0    0 : slabdata      0      0      0
dma-kmalloc-128        0      0    128   32    1 : tunables    0    0    0 : slabdata      0      0      0
kmalloc-rcl-8k         0      0   8192    4    8 : tunables    0    0    0 : slabdata      0      0      0
kmalloc-rcl-4k         0      0   4096    8    8 : tunables    0    0    0 : slabdata      0      0      0
kmalloc-rcl-2k         0      0   2048   16    8 : tunables    0    0    0 : slabdata      0      0      0
kmalloc-rcl-1k         0      0   1024   16    4 : tunables    0    0    0 : slabdata      0      0      0
kmalloc-rcl-512        0      0    512   16    2 : tunables    0    0    0 : slabdata      0      0      0
kmalloc-rcl-256       96     96    256   16    1 : tunables    0    0    0 : slabdata      6      6      0
kmalloc-rcl-64       896    896     64   64    1 : tunables    0    0    0 : slabdata     14     14      0
kmalloc-rcl-192       63     63    192   21    1 : tunables    0    0    0 : slabdata      3      3      0
kmalloc-rcl-128      928    960    128   32    1 : tunables    0    0    0 : slabdata     30     30      0
kmalloc-8k            68     68   8192    4    8 : tunables    0    0    0 : slabdata     17     17      0
kmalloc-4k           366    400   4096    8    8 : tunables    0    0    0 : slabdata     50     50      0
kmalloc-2k           684    768   2048   16    8 : tunables    0    0    0 : slabdata     48     48      0
kmalloc-1k          1023   1056   1024   16    4 : tunables    0    0    0 : slabdata     66     66      0
kmalloc-512         1616   1760    512   16    2 : tunables    0    0    0 : slabdata    110    110      0
kmalloc-256         1240   1280    256   16    1 : tunables    0    0    0 : slabdata     80     80      0
kmalloc-64         21652  22080     64   64    1 : tunables    0    0    0 : slabdata    345    345      0
kmalloc-192         1771   1911    192   21    1 : tunables    0    0    0 : slabdata     91     91      0
kmalloc-128         5788   6400    128   32    1 : tunables    0    0    0 : slabdata    200    200      0
kmem_cache_node      460    512     64   64    1 : tunables    0    0    0 : slabdata      8      8      0
kmem_cache           161    176    256   16    1 : tunables    0    0    0 : slabdata     11     11      0


$ sudo cat /proc/slabinfo | grep kmalloc-64
dma-kmalloc-64         0      0     64   64    1 : tunables    0    0    0 : slabdata      0      0      0
kmalloc-64         21605  22080     64   64    1 : tunables    0    0    0 : slabdata    345    345      0

$ sudo insmod memory_alloc.ko

$ sudo cat /proc/slabinfo | grep kmalloc-64
dma-kmalloc-64         0      0     64   64    1 : tunables    0    0    0 : slabdata      0      0      0
kmalloc-64         21621  22080     64   64    1 : tunables    0    0    0 : slabdata    345    345      0 
NOTE <-- some change in the number of active objects and total objects allocated for kmalloc-64 slab cache, which indicates that some memory has been allocated from this slab cache for our kmalloc allocation in the kernel module.

$ sudo cat /proc/slabinfo | grep my_cache
my_cache              64     64     64   64    1 : tunables    0    0    0 : slabdata      1      1      0

Terminal 2:
 $ dmesg -W
[42306.198097] kmalloc allocated at ffffff808513b000 (physical 8513b000)
[42306.198132] kzalloc allocated at ffffff808513a800
[42306.198319] vmalloc allocated at ffffffc0839d5000 <------ Different Virtual Address Range (vmalloc area)
[42306.198458] Slab object allocated at ffffff80420c3000 <------ Same Virtual Address Range as kmalloc (kmalloc area)

[42402.821518] Memory freed
*/