#include <sys/ioctl.h>

// Type number
#define KBASE_IOCTL_TYPE 0x80

/* Enable KBase tracepoints for CSF builds */
#define BASE_TLSTREAM_ENABLE_CSF_TRACEPOINTS (1 << 2)

/* Enable additional CSF Firmware side tracepoints */
#define BASE_TLSTREAM_ENABLE_CSFFW_TRACEPOINTS (1 << 3)

typedef __u8 base_kcpu_queue_id; /* We support up to 256 active KCPU queues */

/**
 * struct kbase_ioctl_version_check - Check version compatibility between
 * kernel and userspace
 *
 * @major: Major version number
 * @minor: Minor version number
 */
struct kbase_ioctl_version_check {
  __u16 major;
  __u16 minor;
};

/**
 * struct kbase_ioctl_set_flags - Set kernel context creation flags
 *
 * @create_flags: Flags - see base_context_create_flags
 */
struct kbase_ioctl_set_flags {
  __u32 create_flags;
};


/**
 * struct kbase_ioctl_job_submit - Submit jobs/atoms to the kernel
 *
 * @addr: Memory address of an array of struct base_jd_atom_v2 or v3
 * @nr_atoms: Number of entries in the array
 * @stride: sizeof(struct base_jd_atom_v2) or sizeof(struct base_jd_atom)
 */
struct kbase_ioctl_job_submit {
  __u64 addr;
  __u32 nr_atoms;
  __u32 stride;
};


/**
 * struct kbase_ioctl_get_gpuprops - Read GPU properties from the kernel
 *
 * @buffer: Pointer to the buffer to store properties into
 * @size: Size of the buffer
 * @flags: Flags - must be zero for now
 *
 * The ioctl will return the number of bytes stored into @buffer or an error
 * on failure (e.g. @size is too small). If @size is specified as 0 then no
 * data will be written but the return value will be the number of bytes needed
 * for all the properties.
 *
 * @flags may be used in the future to request a different format for the
 * buffer. With @flags == 0 the following format is used.
 *
 * The buffer will be filled with pairs of values, a __u32 key identifying the
 * property followed by the value. The size of the value is identified using
 * the bottom bits of the key. The value then immediately followed the key and
 * is tightly packed (there is no padding). All keys and values are
 * little-endian.
 *
 * 00 = __u8
 * 01 = __u16
 * 10 = __u32
 * 11 = __u64
 */
struct kbase_ioctl_get_gpuprops {
  __u64 buffer;
  __u32 size;
  __u32 flags;
};


/**
 * union kbase_ioctl_mem_alloc - Allocate memory on the GPU
 * @in: Input parameters
 * @in.va_pages: The number of pages of virtual address space to reserve
 * @in.commit_pages: The number of physical pages to allocate
 * @in.extension: The number of extra pages to allocate on each GPU fault which grows the region
 * @in.flags: Flags
 * @out: Output parameters
 * @out.flags: Flags
 * @out.gpu_va: The GPU virtual address which is allocated
 */
union kbase_ioctl_mem_alloc {
  struct {
    __u64 va_pages;
    __u64 commit_pages;
    __u64 extension;
    __u64 flags;
  } in; 
  struct {
    __u64 flags;
    __u64 gpu_va;
  } out;
};


/**
 * struct kbase_ioctl_mem_query - Query properties of a GPU memory region
 * @in: Input parameters
 * @in.gpu_addr: A GPU address contained within the region
 * @in.query: The type of query
 * @out: Output parameters
 * @out.value: The result of the query
 *
 * Use a %KBASE_MEM_QUERY_xxx flag as input for @query.
 */
union kbase_ioctl_mem_query {
  struct {
    __u64 gpu_addr;
    __u64 query;
  } in;
  struct {
    __u64 value;
  } out;
};


/**
 * struct kbase_ioctl_mem_free - Free a memory region
 * @gpu_addr: Handle to the region to free
 */
struct kbase_ioctl_mem_free {
  __u64 gpu_addr;
};


/**
 * struct kbase_ioctl_hwcnt_reader_setup - Setup HWC dumper/reader
 * @buffer_count: requested number of dumping buffers
 * @fe_bm:        counters selection bitmask (Front end)
 * @shader_bm:    counters selection bitmask (Shader)
 * @tiler_bm:     counters selection bitmask (Tiler)
 * @mmu_l2_bm:    counters selection bitmask (MMU_L2)
 *  
 * A fd is returned from the ioctl if successful, or a negative value on error
 */
struct kbase_ioctl_hwcnt_reader_setup {
  __u32 buffer_count;
  __u32 fe_bm;
  __u32 shader_bm;
  __u32 tiler_bm;
  __u32 mmu_l2_bm;
};  


/**
 * struct kbase_ioctl_disjoint_query - Query the disjoint counter
 * @counter:   A counter of disjoint events in the kernel
 */
struct kbase_ioctl_disjoint_query {
  __u32 counter;
};


/**
 * struct kbase_ioctl_get_ddk_version - Query the kernel version
 * @version_buffer: Buffer to receive the kernel version string
 * @size: Size of the buffer
 * @padding: Padding
 *
 * The ioctl will return the number of bytes written into version_buffer
 * (which includes a NULL byte) or a negative error code
 *
 * The ioctl request code has to be _IOW because the data in ioctl struct is
 * being copied to the kernel, even though the kernel then writes out the
 * version info to the buffer specified in the ioctl.
 */
struct kbase_ioctl_get_ddk_version {
  __u64 version_buffer;
  __u32 size;
  __u32 padding;
};


/**
 * struct kbase_ioctl_mem_jit_init_10_2 - Initialize the just-in-time memory
 *                                        allocator (between kernel driver
 *                                        version 10.2--11.4)
 * @va_pages: Number of VA pages to reserve for JIT
 *
 * Note that depending on the VA size of the application and GPU, the value
 * specified in @va_pages may be ignored.
 *
 * New code should use KBASE_IOCTL_MEM_JIT_INIT instead, this is kept for
 * backwards compatibility.
 */
struct kbase_ioctl_mem_jit_init_10_2 {
  __u64 va_pages;
};


/**
 * struct kbase_ioctl_mem_jit_init_11_5 - Initialize the just-in-time memory
 *                                        allocator (between kernel driver
 *                                        version 11.5--11.19)
 * @va_pages: Number of VA pages to reserve for JIT
 * @max_allocations: Maximum number of concurrent allocations
 * @trim_level: Level of JIT allocation trimming to perform on free (0 - 100%)
 * @group_id: Group ID to be used for physical allocations
 * @padding: Currently unused, must be zero
 *
 * Note that depending on the VA size of the application and GPU, the value
 * specified in @va_pages may be ignored.
 *
 * New code should use KBASE_IOCTL_MEM_JIT_INIT instead, this is kept for
 * backwards compatibility.
 */
struct kbase_ioctl_mem_jit_init_11_5 {
  __u64 va_pages;
  __u8 max_allocations;
  __u8 trim_level;
  __u8 group_id;
  __u8 padding[5];
};


/**
 * struct kbase_ioctl_mem_jit_init - Initialize the just-in-time memory
 *                                   allocator
 * @va_pages: Number of GPU virtual address pages to reserve for just-in-time
 *            memory allocations
 * @max_allocations: Maximum number of concurrent allocations
 * @trim_level: Level of JIT allocation trimming to perform on free (0 - 100%)
 * @group_id: Group ID to be used for physical allocations
 * @padding: Currently unused, must be zero
 * @phys_pages: Maximum number of physical pages to allocate just-in-time
 *
 * Note that depending on the VA size of the application and GPU, the value
 * specified in @va_pages may be ignored.
 */
struct kbase_ioctl_mem_jit_init {
  __u64 va_pages;
  __u8 max_allocations;
  __u8 trim_level;
  __u8 group_id;
  __u8 padding[5];
  __u64 phys_pages;
};


/**
 * struct kbase_ioctl_mem_sync - Perform cache maintenance on memory
 *
 * @handle: GPU memory handle (GPU VA)
 * @user_addr: The address where it is mapped in user space
 * @size: The number of bytes to synchronise
 * @type: The direction to synchronise: 0 is sync to memory (clean),
 * 1 is sync from memory (invalidate). Use the BASE_SYNCSET_OP_xxx constants.
 * @padding: Padding to round up to a multiple of 8 bytes, must be zero
 */
struct kbase_ioctl_mem_sync {
  __u64 handle;
  __u64 user_addr;
  __u64 size;
  __u8 type;
  __u8 padding[7];
};


/**
 * union kbase_ioctl_mem_find_cpu_offset - Find the offset of a CPU pointer
 *
 * @in: Input parameters
 * @in.gpu_addr: The GPU address of the memory region
 * @in.cpu_addr: The CPU address to locate
 * @in.size: A size in bytes to validate is contained within the region
 * @out: Output parameters
 * @out.offset: The offset from the start of the memory region to @cpu_addr
 */
union kbase_ioctl_mem_find_cpu_offset {
  struct {
    __u64 gpu_addr;
    __u64 cpu_addr;
    __u64 size;
  } in;
  struct {
    __u64 offset;
  } out;
};


/**
 * struct kbase_ioctl_get_context_id - Get the kernel context ID
 *
 * @id: The kernel context ID
 */
struct kbase_ioctl_get_context_id {
  __u32 id;
};


/**
 * struct kbase_ioctl_tlstream_acquire - Acquire a tlstream fd
 *
 * @flags: Flags
 *
 * The ioctl returns a file descriptor when successful
 */
struct kbase_ioctl_tlstream_acquire {
  __u32 flags;
};


/**
 * struct kbase_ioctl_mem_commit - Change the amount of memory backing a region
 *
 * @gpu_addr: The memory region to modify
 * @pages:    The number of physical pages that should be present
 *
 * The ioctl may return on the following error codes or 0 for success:
 *   -ENOMEM: Out of memory
 *   -EINVAL: Invalid arguments
 */
struct kbase_ioctl_mem_commit {
  __u64 gpu_addr;
  __u64 pages;
};


/**
 * union kbase_ioctl_mem_alias - Create an alias of memory regions
 * @in: Input parameters
 * @in.flags: Flags, see BASE_MEM_xxx
 * @in.stride: Bytes between start of each memory region
 * @in.nents: The number of regions to pack together into the alias
 * @in.aliasing_info: Pointer to an array of struct base_mem_aliasing_info
 * @out: Output parameters
 * @out.flags: Flags, see BASE_MEM_xxx
 * @out.gpu_va: Address of the new alias
 * @out.va_pages: Size of the new alias
 */
union kbase_ioctl_mem_alias {
  struct {
    __u64 flags;
    __u64 stride;
    __u64 nents;
    __u64 aliasing_info;
  } in;
  struct {
    __u64 flags;
    __u64 gpu_va;
    __u64 va_pages;
  } out;
};


/**
 * union kbase_ioctl_mem_import - Import memory for use by the GPU
 * @in: Input parameters
 * @in.flags: Flags, see BASE_MEM_xxx
 * @in.phandle: Handle to the external memory
 * @in.type: Type of external memory, see base_mem_import_type
 * @in.padding: Amount of extra VA pages to append to the imported buffer
 * @out: Output parameters
 * @out.flags: Flags, see BASE_MEM_xxx
 * @out.gpu_va: Address of the new alias
 * @out.va_pages: Size of the new alias
 */
union kbase_ioctl_mem_import {
  struct {
    __u64 flags;
    __u64 phandle;
    __u32 type;
    __u32 padding;
  } in;
  struct {
    __u64 flags;
    __u64 gpu_va;
    __u64 va_pages;
  } out;
};


/**
 * struct kbase_ioctl_mem_flags_change - Change the flags for a memory region
 * @gpu_va: The GPU region to modify
 * @flags: The new flags to set
 * @mask: Mask of the flags to modify
 */
struct kbase_ioctl_mem_flags_change {
  __u64 gpu_va;
  __u64 flags;
  __u64 mask;
};


/**
 * struct kbase_ioctl_stream_create - Create a synchronisation stream
 * @name: A name to identify this stream. Must be NULL-terminated.
 *
 * Note that this is also called a "timeline", but is named stream to avoid
 * confusion with other uses of the word.
 *
 * Unused bytes in @name (after the first NULL byte) must be also be NULL bytes.
 *
 * The ioctl returns a file descriptor.
 */
struct kbase_ioctl_stream_create {
  char name[32];
};


/**
 * struct kbase_ioctl_fence_validate - Validate a fd refers to a fence
 * @fd: The file descriptor to validate
 */
struct kbase_ioctl_fence_validate {
  int fd;
};


/**
 * struct kbase_ioctl_mem_profile_add - Provide profiling information to kernel
 * @buffer: Pointer to the information
 * @len: Length
 * @padding: Padding
 *
 * The data provided is accessible through a debugfs file
 */
struct kbase_ioctl_mem_profile_add {
  __u64 buffer;
  __u32 len;
  __u32 padding;
};


/**
 * struct kbase_ioctl_sticky_resource_map - Permanently map an external resource
 * @count: Number of resources
 * @address: Array of __u64 GPU addresses of the external resources to map
 */
struct kbase_ioctl_sticky_resource_map {
  __u64 count;
  __u64 address;
};


/**
 * struct kbase_ioctl_sticky_resource_unmap - Unmap a resource mapped which was
 *                                          previously permanently mapped
 * @count: Number of resources
 * @address: Array of __u64 GPU addresses of the external resources to unmap
 */
struct kbase_ioctl_sticky_resource_unmap {
  __u64 count;
  __u64 address;
};


/**
 * union kbase_ioctl_mem_find_gpu_start_and_offset - Find the start address of
 *                                                   the GPU memory region for
 *                                                   the given gpu address and
 *                                                   the offset of that address
 *                                                   into the region
 * @in: Input parameters
 * @in.gpu_addr: GPU virtual address
 * @in.size: Size in bytes within the region
 * @out: Output parameters
 * @out.start: Address of the beginning of the memory region enclosing @gpu_addr
 *             for the length of @offset bytes
 * @out.offset: The offset from the start of the memory region to @gpu_addr
 */
union kbase_ioctl_mem_find_gpu_start_and_offset {
  struct {
    __u64 gpu_addr;
    __u64 size;
  } in;
  struct {
    __u64 start;
    __u64 offset;
  } out;
};


/**
 * struct kbase_ioctl_hwcnt_values - Values to set dummy the dummy counters to.
 * @data:    Counter samples for the dummy model.
 * @size:    Size of the counter sample data.
 * @padding: Padding.
 */
struct kbase_ioctl_hwcnt_values {
  __u64 data;
  __u32 size;
  __u32 padding;
};


/**
 * union kbase_ioctl_cinstr_gwt_dump - Used to collect all GPU write fault
 *                                     addresses.
 * @in: Input parameters
 * @in.addr_buffer: Address of buffer to hold addresses of gpu modified areas.
 * @in.size_buffer: Address of buffer to hold size of modified areas (in pages)
 * @in.len: Number of addresses the buffers can hold.
 * @in.padding: padding
 * @out: Output parameters
 * @out.no_of_addr_collected: Number of addresses collected into addr_buffer.
 * @out.more_data_available: Status indicating if more addresses are available.
 * @out.padding: padding
 *
 * This structure is used when performing a call to dump GPU write fault
 * addresses.
 */
union kbase_ioctl_cinstr_gwt_dump {
  struct {
    __u64 addr_buffer;
    __u64 size_buffer;
    __u32 len;
    __u32 padding;

  } in;
  struct {
    __u32 no_of_addr_collected; 
    __u8 more_data_available;
    __u8 padding[27];
  } out;
};


/**
 * struct kbase_ioctl_cs_queue_register - Register a GPU command queue with the
 *                                        base back-end
 *
 * @buffer_gpu_addr: GPU address of the buffer backing the queue
 * @buffer_size: Size of the buffer in bytes
 * @priority: Priority of the queue within a group when run within a process
 * @padding: Currently unused, must be zero
 *
 * Note: There is an identical sub-section in kbase_ioctl_cs_queue_register_ex.
 *        Any change of this struct should also be mirrored to the latter.
 */
struct kbase_ioctl_cs_queue_register {
  __u64 buffer_gpu_addr;
  __u32 buffer_size;
  __u8 priority;
  __u8 padding[3];
};


/**
 * struct kbase_ioctl_cs_queue_kick - Kick the GPU command queue group scheduler
 *                                    to notify that a queue has been updated
 *
 * @buffer_gpu_addr: GPU address of the buffer backing the queue
 */
struct kbase_ioctl_cs_queue_kick {
  __u64 buffer_gpu_addr;
};


/**
 * struct kbase_ioctl_mem_exec_init - Initialise the EXEC_VA memory zone
 *
 * @va_pages: Number of VA pages to reserve for EXEC_VA
 */
struct kbase_ioctl_mem_exec_init {
  __u64 va_pages;
};


/**
 * union kbase_ioctl_cs_queue_bind - Bind a GPU command queue to a group
 *
 * @in:                 Input parameters
 * @in.buffer_gpu_addr: GPU address of the buffer backing the queue
 * @in.group_handle:    Handle of the group to which the queue should be bound
 * @in.csi_index:       Index of the CSF interface the queue should be bound to
 * @in.padding:         Currently unused, must be zero
 * @out:                Output parameters
 * @out.mmap_handle:    Handle to be used for creating the mapping of CS
 *                      input/output pages
 */
union kbase_ioctl_cs_queue_bind {
  struct {
    __u64 buffer_gpu_addr;
    __u8 group_handle;
    __u8 csi_index;
    __u8 padding[6];
  } in; 
  struct {
    __u64 mmap_handle;
  } out;
};


/**
 * struct kbase_ioctl_cs_queue_register_ex - Register a GPU command queue with the
 *                                           base back-end in extended format,
 *                                           involving trace buffer configuration
 *
 * @buffer_gpu_addr: GPU address of the buffer backing the queue
 * @buffer_size: Size of the buffer in bytes
 * @priority: Priority of the queue within a group when run within a process
 * @padding: Currently unused, must be zero
 * @ex_offset_var_addr: GPU address of the trace buffer write offset variable
 * @ex_buffer_base: Trace buffer GPU base address for the queue
 * @ex_buffer_size: Size of the trace buffer in bytes
 * @ex_event_size: Trace event write size, in log2 designation
 * @ex_event_state: Trace event states configuration
 * @ex_padding: Currently unused, must be zero
 *
 * Note: There is an identical sub-section at the start of this struct to that
 *        of @ref kbase_ioctl_cs_queue_register. Any change of this sub-section
 *        must also be mirrored to the latter. Following the said sub-section,
 *        the remaining fields forms the extension, marked with ex_*.
 */
struct kbase_ioctl_cs_queue_register_ex {
  __u64 buffer_gpu_addr;
  __u32 buffer_size;
  __u8 priority;
  __u8 padding[3];
  __u64 ex_offset_var_addr;
  __u64 ex_buffer_base;
  __u32 ex_buffer_size;
  __u8 ex_event_size;
  __u8 ex_event_state;
  __u8 ex_padding[2];
};


/**
 * struct kbase_ioctl_cs_queue_terminate - Terminate a GPU command queue
 *
 * @buffer_gpu_addr: GPU address of the buffer backing the queue
 */
struct kbase_ioctl_cs_queue_terminate {
  __u64 buffer_gpu_addr;
};


/**
 * union kbase_ioctl_cs_queue_group_create_1_6 - Create a GPU command queue
 *                                               group
 * @in:               Input parameters
 * @in.tiler_mask:    Mask of tiler endpoints the group is allowed to use.
 * @in.fragment_mask: Mask of fragment endpoints the group is allowed to use.
 * @in.compute_mask:  Mask of compute endpoints the group is allowed to use.
 * @in.cs_min:        Minimum number of CSs required.
 * @in.priority:      Queue group's priority within a process.
 * @in.tiler_max:     Maximum number of tiler endpoints the group is allowed
 *                    to use.
 * @in.fragment_max:  Maximum number of fragment endpoints the group is
 *                    allowed to use.
 * @in.compute_max:   Maximum number of compute endpoints the group is allowed
 *                    to use.
 * @in.padding:       Currently unused, must be zero
 * @out:              Output parameters
 * @out.group_handle: Handle of a newly created queue group.
 * @out.padding:      Currently unused, must be zero
 * @out.group_uid:    UID of the queue group available to base.
 */
union kbase_ioctl_cs_queue_group_create_1_6 {
  struct {
    __u64 tiler_mask;
    __u64 fragment_mask;
    __u64 compute_mask;
    __u8 cs_min;
    __u8 priority;
    __u8 tiler_max;
    __u8 fragment_max;
    __u8 compute_max;
    __u8 padding[3];

  } in;
  struct {
    __u8 group_handle;
    __u8 padding[3];
    __u32 group_uid;
  } out;
};


/**
 * struct kbase_ioctl_cs_queue_group_term - Terminate a GPU command queue group
 *
 * @group_handle: Handle of the queue group to be terminated
 * @padding: Padding to round up to a multiple of 8 bytes, must be zero
 */
struct kbase_ioctl_cs_queue_group_term {
  __u8 group_handle;
  __u8 padding[7];
};


/**
 * struct kbase_ioctl_kcpu_queue_new - Create a KCPU command queue
 *
 * @id: ID of the new command queue returned by the kernel
 * @padding: Padding to round up to a multiple of 8 bytes, must be zero
 */
struct kbase_ioctl_kcpu_queue_new {
  base_kcpu_queue_id id;
  __u8 padding[7];
};


/**
 * struct kbase_ioctl_kcpu_queue_delete - Destroy a KCPU command queue
 *
 * @id: ID of the command queue to be destroyed
 * @padding: Padding to round up to a multiple of 8 bytes, must be zero
 */
struct kbase_ioctl_kcpu_queue_delete {
  base_kcpu_queue_id id;
  __u8 padding[7];
};


/**
 * struct kbase_ioctl_kcpu_queue_enqueue - Enqueue commands into the KCPU queue
 *
 * @addr: Memory address of an array of struct base_kcpu_queue_command
 * @nr_commands: Number of commands in the array
 * @id: kcpu queue identifier, returned by KBASE_IOCTL_KCPU_QUEUE_CREATE ioctl
 * @padding: Padding to round up to a multiple of 8 bytes, must be zero
 */
struct kbase_ioctl_kcpu_queue_enqueue {
  __u64 addr;
  __u32 nr_commands;
  base_kcpu_queue_id id;
  __u8 padding[3];
};


/**
 * union kbase_ioctl_cs_tiler_heap_init - Initialize chunked tiler memory heap
 * @in:                Input parameters
 * @in.chunk_size:     Size of each chunk.
 * @in.initial_chunks: Initial number of chunks that heap will be created with.
 * @in.max_chunks:     Maximum number of chunks that the heap is allowed to use.
 * @in.target_in_flight: Number of render-passes that the driver should attempt to
 *                     keep in flight for which allocation of new chunks is
 *                     allowed.
 * @in.group_id:       Group ID to be used for physical allocations.
 * @in.padding:        Padding
 * @in.buf_desc_va:    Buffer descriptor GPU VA for tiler heap reclaims.
 * @out:               Output parameters
 * @out.gpu_heap_va:   GPU VA (virtual address) of Heap context that was set up
 *                     for the heap.
 * @out.first_chunk_va: GPU VA of the first chunk allocated for the heap,
 *                     actually points to the header of heap chunk and not to
 *                     the low address of free memory in the chunk.
 */
union kbase_ioctl_cs_tiler_heap_init {
  struct {
    __u32 chunk_size;
    __u32 initial_chunks;
    __u32 max_chunks;
    __u16 target_in_flight;
    __u8 group_id;
    __u8 padding;
    __u64 buf_desc_va;
  } in;
  struct {
    __u64 gpu_heap_va;
    __u64 first_chunk_va;
  } out;
};


/**
 * union kbase_ioctl_cs_tiler_heap_init_1_13 - Initialize chunked tiler memory heap,
 *                                             earlier version upto 1.13
 * @in:                Input parameters
 * @in.chunk_size:     Size of each chunk.
 * @in.initial_chunks: Initial number of chunks that heap will be created with.
 * @in.max_chunks:     Maximum number of chunks that the heap is allowed to use.
 * @in.target_in_flight: Number of render-passes that the driver should attempt to
 *                     keep in flight for which allocation of new chunks is
 *                     allowed.
 * @in.group_id:       Group ID to be used for physical allocations.
 * @in.padding:        Padding
 * @out:               Output parameters
 * @out.gpu_heap_va:   GPU VA (virtual address) of Heap context that was set up
 *                     for the heap.
 * @out.first_chunk_va: GPU VA of the first chunk allocated for the heap,
 *                     actually points to the header of heap chunk and not to
 *                     the low address of free memory in the chunk.
 */
union kbase_ioctl_cs_tiler_heap_init_1_13 {
  struct {
    __u32 chunk_size;
    __u32 initial_chunks;
    __u32 max_chunks;
    __u16 target_in_flight;
    __u8 group_id;
    __u8 padding;
  } in;
  struct {
    __u64 gpu_heap_va;
    __u64 first_chunk_va;
  } out;
};


/**
 * struct kbase_ioctl_cs_tiler_heap_term - Terminate a chunked tiler heap
 *                                         instance
 *
 * @gpu_heap_va: GPU VA of Heap context that was set up for the heap.
 */
struct kbase_ioctl_cs_tiler_heap_term {
  __u64 gpu_heap_va;
};


/**
 * union kbase_ioctl_get_cpu_gpu_timeinfo - Request zero or more types of
 *                                          cpu/gpu time (counter values)
 * @in: Input parameters
 * @in.request_flags: Bit-flags indicating the requested types.
 * @in.paddings:      Unused, size alignment matching the out.
 * @out: Output parameters
 * @out.sec:           Integer field of the monotonic time, unit in seconds.
 * @out.nsec:          Fractional sec of the monotonic time, in nano-seconds.
 * @out.padding:       Unused, for __u64 alignment
 * @out.timestamp:     System wide timestamp (counter) value.
 * @out.cycle_counter: GPU cycle counter value.
 */
union kbase_ioctl_get_cpu_gpu_timeinfo {
  struct {
    __u32 request_flags;
    __u32 paddings[7];
  } in; 
  struct {
    __u64 sec;
    __u32 nsec;
    __u32 padding;
    __u64 timestamp;
    __u64 cycle_counter;
  } out;
};


/**
 * union kbase_ioctl_cs_get_glb_iface - Request the global control block
 *                                        of CSF interface capabilities
 *
 * @in:                    Input parameters
 * @in.max_group_num:      The maximum number of groups to be read. Can be 0, in
 *                         which case groups_ptr is unused.
 * @in.max_total_stream_num: The maximum number of CSs to be read. Can be 0, in
 *                         which case streams_ptr is unused.
 * @in.groups_ptr:         Pointer where to store all the group data (sequentially).
 * @in.streams_ptr:        Pointer where to store all the CS data (sequentially).
 * @out:                   Output parameters
 * @out.glb_version:       Global interface version.
 * @out.features:          Bit mask of features (e.g. whether certain types of job
 *                         can be suspended).
 * @out.group_num:         Number of CSGs supported.
 * @out.prfcnt_size:       Size of CSF performance counters, in bytes. Bits 31:16
 *                         hold the size of firmware performance counter data
 *                         and 15:0 hold the size of hardware performance counter
 *                         data.
 * @out.total_stream_num:  Total number of CSs, summed across all groups.
 * @out.instr_features:    Instrumentation features. Bits 7:4 hold the maximum
 *                         size of events. Bits 3:0 hold the offset update rate.
 *                         (csf >= 1.1.0)
 *
 */
union kbase_ioctl_cs_get_glb_iface {
  struct {
    __u32 max_group_num;
    __u32 max_total_stream_num;
    __u64 groups_ptr;
    __u64 streams_ptr;
  } in;
  struct {
    __u32 glb_version;
    __u32 features;
    __u32 group_num;
    __u32 prfcnt_size;
    __u32 total_stream_num;
    __u32 instr_features;
  } out;
};


/**
 * struct kbase_kinstr_jm_fd_out - Explains the compatibility information for
 * the `struct kbase_kinstr_jm_atom_state_change` structure returned from the
 * kernel
 *
 * @size:    The size of the `struct kbase_kinstr_jm_atom_state_change`
 * @version: Represents a breaking change in the
 *           `struct kbase_kinstr_jm_atom_state_change`
 * @padding: Explicit padding to get the structure up to 64bits. See
 * https://www.kernel.org/doc/Documentation/ioctl/botching-up-ioctls.rst
 *
 * The `struct kbase_kinstr_jm_atom_state_change` may have extra members at the
 * end of the structure that older user space might not understand. If the
 * `version` is the same, the structure is still compatible with newer kernels.
 * The `size` can be used to cast the opaque memory returned from the kernel.
 */
struct kbase_kinstr_jm_fd_out {
  __u16 size;
  __u8 version;
  __u8 padding[5];
};


/**
 * struct kbase_kinstr_jm_fd_in - Options when creating the file descriptor
 *
 * @count: Number of atom states that can be stored in the kernel circular
 *         buffer. Must be a power of two
 * @padding: Explicit padding to get the structure up to 64bits. See
 * https://www.kernel.org/doc/Documentation/ioctl/botching-up-ioctls.rst
 */
struct kbase_kinstr_jm_fd_in {
  __u16 count;
  __u8 padding[6];
};

union kbase_kinstr_jm_fd {
  struct kbase_kinstr_jm_fd_in in; 
  struct kbase_kinstr_jm_fd_out out;
};


struct kbase_ioctl_cs_cpu_queue_info {
  __u64 buffer;
  __u64 size;
};


/**
 * struct kbase_ioctl_context_priority_check - Check the max possible priority
 * @priority: Input priority & output priority
 */

struct kbase_ioctl_context_priority_check {
  __u8 priority;
};


/**
 * struct kbase_ioctl_set_limited_core_count - Set the limited core count.
 *
 * @max_core_count: Maximum core count
 */
struct kbase_ioctl_set_limited_core_count {
  __u8 max_core_count;
};


/**
 * struct kbase_ioctl_kinstr_prfcnt_enum_info - Enum Performance counter
 *                                              information
 * @info_item_size:  Performance counter item size in bytes.
 * @info_item_count: Performance counter item count in the info_list_ptr.
 * @info_list_ptr:   Performance counter item list pointer which points to a
 *                   list with info_item_count of items.
 *
 * On success: returns info_item_size and info_item_count if info_list_ptr is
 * NULL, returns performance counter information if info_list_ptr is not NULL.
 * On error: returns a negative error code.
 */
struct kbase_ioctl_kinstr_prfcnt_enum_info {
  __u32 info_item_size;
  __u32 info_item_count;
  __u64 info_list_ptr;
};


/**
 * struct kbase_ioctl_kinstr_prfcnt_setup - Setup HWC dumper/reader
 * @in: input parameters.
 * @in.request_item_count: Number of requests in the requests array.
 * @in.request_item_size:  Size in bytes of each request in the requests array.
 * @in.requests_ptr:       Pointer to the requests array.
 * @out: output parameters.
 * @out.prfcnt_metadata_item_size: Size of each item in the metadata array for
 *                                 each sample.
 * @out.prfcnt_mmap_size_bytes:    Size in bytes that user-space should mmap
 *                                 for reading performance counter samples.
 *
 * A fd is returned from the ioctl if successful, or a negative value on error.
 */
union kbase_ioctl_kinstr_prfcnt_setup {
  struct {
    __u32 request_item_count;
    __u32 request_item_size;
    __u64 requests_ptr;
  } in;
  struct {
    __u32 prfcnt_metadata_item_size;
    __u32 prfcnt_mmap_size_bytes;
  } out;
};


/**
 * union kbase_ioctl_cs_queue_group_create - Create a GPU command queue group
 * @in:               Input parameters
 * @in.tiler_mask:    Mask of tiler endpoints the group is allowed to use.
 * @in.fragment_mask: Mask of fragment endpoints the group is allowed to use.
 * @in.compute_mask:  Mask of compute endpoints the group is allowed to use.
 * @in.cs_min:        Minimum number of CSs required.
 * @in.priority:      Queue group's priority within a process.
 * @in.tiler_max:     Maximum number of tiler endpoints the group is allowed
 *                    to use.
 * @in.fragment_max:  Maximum number of fragment endpoints the group is
 *                    allowed to use.
 * @in.compute_max:   Maximum number of compute endpoints the group is allowed
 *                    to use.
 * @in.csi_handlers:  Flags to signal that the application intends to use CSI
 *                    exception handlers in some linear buffers to deal with
 *                    the given exception types.
 * @in.padding:       Currently unused, must be zero
 * @out:              Output parameters
 * @out.group_handle: Handle of a newly created queue group.
 * @out.padding:      Currently unused, must be zero
 * @out.group_uid:    UID of the queue group available to base.
 */
union kbase_ioctl_cs_queue_group_create {
  struct {
    __u64 tiler_mask;
    __u64 fragment_mask;
    __u64 compute_mask;
    __u8 cs_min;
    __u8 priority;
    __u8 tiler_max;
    __u8 fragment_max;
    __u8 compute_max;
    __u8 csi_handlers;
    __u8 padding[2];
    /**
     * @in.reserved: Reserved
     */
    __u64 reserved;
  } in;
  struct {
    __u8 group_handle;
    __u8 padding[3];
    __u32 group_uid;
  } out;
};


/**
 * union kbase_ioctl_mem_alloc_ex - Allocate memory on the GPU
 * @in: Input parameters
 * @in.va_pages: The number of pages of virtual address space to reserve
 * @in.commit_pages: The number of physical pages to allocate
 * @in.extension: The number of extra pages to allocate on each GPU fault which grows the region
 * @in.flags: Flags
 * @in.fixed_address: The GPU virtual address requested for the allocation,
 *                    if the allocation is using the BASE_MEM_FIXED flag.
 * @in.extra: Space for extra parameters that may be added in the future.
 * @out: Output parameters
 * @out.flags: Flags
 * @out.gpu_va: The GPU virtual address which is allocated
 */
union kbase_ioctl_mem_alloc_ex {
  struct {
    __u64 va_pages;
    __u64 commit_pages;
    __u64 extension;
    __u64 flags;
    __u64 fixed_address;
    __u64 extra[3];
  } in;
  struct {
    __u64 flags;
    __u64 gpu_va;
  } out;
};


/**
 * union kbase_ioctl_read_user_page - Read a register of USER page
 *
 * @in:               Input parameters.
 * @in.offset:        Register offset in USER page.
 * @in.padding:       Padding to round up to a multiple of 8 bytes, must be zero.
 * @out:              Output parameters.
 * @out.val_lo:       Value of 32bit register or the 1st half of 64bit register to be read.
 * @out.val_hi:       Value of the 2nd half of 64bit register to be read.
 */
union kbase_ioctl_read_user_page {
  struct {
    __u32 offset;
    __u32 padding;
  } in;
  struct {
    __u32 val_lo;
    __u32 val_hi;
  } out;
};


/**
 * struct kbase_ioctl_apc_request - GPU asynchronous power control (APC) request
 *
 * @dur_usec: Duration for GPU to stay awake.
 */
struct kbase_ioctl_apc_request {
  __u32 dur_usec;
};


/**
 * struct kbase_ioctl_buffer_liveness_update - Update the live ranges of buffers from previous frame
 *
 * @live_ranges_address: Array of live ranges
 * @live_ranges_count: Number of elements in the live ranges buffer
 * @buffer_va_address: Array of buffer base virtual addresses
 * @buffer_sizes_address: Array of buffer sizes
 * @buffer_count: Number of buffers
 * @padding: Unused
 */
struct kbase_ioctl_buffer_liveness_update {
  __u64 live_ranges_address;
  __u64 live_ranges_count;
  __u64 buffer_va_address;
  __u64 buffer_sizes_address;
  __u64 buffer_count;
};


/**
 * enum kbase_pixel_gpu_slc_liveness_mark_type - Determines the type of a live range mark
 *
 * @KBASE_PIXEL_GPU_LIVE_RANGE_BEGIN: Signifies that a mark is the start of a live range
 * @KBASE_PIXEL_GPU_LIVE_RANGE_END:   Signifies that a mark is the end of a live range
 *
 */
enum kbase_pixel_gpu_slc_liveness_mark_type {
  KBASE_PIXEL_GPU_LIVE_RANGE_BEGIN,
  KBASE_PIXEL_GPU_LIVE_RANGE_END,
};

/**
 * struct kbase_pixel_gpu_slc_liveness_mark - Live range marker
 *
 * @type: See @struct kbase_pixel_gpu_slc_liveness_mark_type
 * @index: Buffer index (within liveness update array) that this mark represents
 *
 */
struct kbase_pixel_gpu_slc_liveness_mark {
  __u32 type : 1;
  __u32 index : 31; 
};


/**
 * struct gpu_slc_liveness_update_info - Buffer info, and live ranges
 *
 * @buffer_va:         Array of buffer base virtual addresses
 * @buffer_sizes:      Array of buffer sizes
 * @live_ranges:       Array of &struct kbase_pixel_gpu_slc_liveness_mark denoting live ranges for
 *                     each buffer
 * @live_ranges_count: Number of elements in the live ranges buffer
 */
struct gpu_slc_liveness_update_info {
  __u64* buffer_va;
  __u64* buffer_sizes;
  struct kbase_pixel_gpu_slc_liveness_mark* live_ranges;
  __u64 live_ranges_count;
};


//#define KBASE_IOCTL_VERSION_CHECK \
//  _IOWR(KBASE_IOCTL_TYPE, 0, struct kbase_ioctl_version_check)
#define KBASE_IOCTL_SET_FLAGS \
  _IOW(KBASE_IOCTL_TYPE, 1, struct kbase_ioctl_set_flags)
#define KBASE_IOCTL_JOB_SUBMIT \
  _IOW(KBASE_IOCTL_TYPE, 2, struct kbase_ioctl_job_submit)
#define KBASE_IOCTL_GET_GPUPROPS \
  _IOW(KBASE_IOCTL_TYPE, 3, struct kbase_ioctl_get_gpuprops)
#define KBASE_IOCTL_POST_TERM \
  _IO(KBASE_IOCTL_TYPE, 4)
#define KBASE_IOCTL_MEM_ALLOC \
  _IOWR(KBASE_IOCTL_TYPE, 5, union kbase_ioctl_mem_alloc)
#define KBASE_IOCTL_MEM_QUERY \
  _IOWR(KBASE_IOCTL_TYPE, 6, union kbase_ioctl_mem_query)
#define KBASE_IOCTL_MEM_FREE \
  _IOW(KBASE_IOCTL_TYPE, 7, struct kbase_ioctl_mem_free)
#define KBASE_IOCTL_HWCNT_READER_SETUP \
  _IOW(KBASE_IOCTL_TYPE, 8, struct kbase_ioctl_hwcnt_reader_setup)
#define KBASE_IOCTL_DISJOINT_QUERY \
  _IOR(KBASE_IOCTL_TYPE, 12, struct kbase_ioctl_disjoint_query)
#define KBASE_IOCTL_GET_DDK_VERSION \
  _IOW(KBASE_IOCTL_TYPE, 13, struct kbase_ioctl_get_ddk_version)
#define KBASE_IOCTL_MEM_JIT_INIT_10_2 \
  _IOW(KBASE_IOCTL_TYPE, 14, struct kbase_ioctl_mem_jit_init_10_2)
#define KBASE_IOCTL_MEM_JIT_INIT_11_5 \
  _IOW(KBASE_IOCTL_TYPE, 14, struct kbase_ioctl_mem_jit_init_11_5)
#define KBASE_IOCTL_MEM_JIT_INIT \
  _IOW(KBASE_IOCTL_TYPE, 14, struct kbase_ioctl_mem_jit_init)
#define KBASE_IOCTL_MEM_SYNC \
  _IOW(KBASE_IOCTL_TYPE, 15, struct kbase_ioctl_mem_sync)
#define KBASE_IOCTL_MEM_FIND_CPU_OFFSET \
  _IOWR(KBASE_IOCTL_TYPE, 16, union kbase_ioctl_mem_find_cpu_offset)
#define KBASE_IOCTL_GET_CONTEXT_ID \
  _IOR(KBASE_IOCTL_TYPE, 17, struct kbase_ioctl_get_context_id)
#define KBASE_IOCTL_TLSTREAM_ACQUIRE \
  _IOW(KBASE_IOCTL_TYPE, 18, struct kbase_ioctl_tlstream_acquire)
#define KBASE_IOCTL_TLSTREAM_FLUSH \
  _IO(KBASE_IOCTL_TYPE, 19)
#define KBASE_IOCTL_MEM_COMMIT \
  _IOW(KBASE_IOCTL_TYPE, 20, struct kbase_ioctl_mem_commit)
#define KBASE_IOCTL_MEM_ALIAS \
  _IOWR(KBASE_IOCTL_TYPE, 21, union kbase_ioctl_mem_alias)
#define KBASE_IOCTL_MEM_IMPORT \
  _IOWR(KBASE_IOCTL_TYPE, 22, union kbase_ioctl_mem_import)
#define KBASE_IOCTL_MEM_FLAGS_CHANGE \
  _IOW(KBASE_IOCTL_TYPE, 23, struct kbase_ioctl_mem_flags_change)
#define KBASE_IOCTL_STREAM_CREATE \
  _IOW(KBASE_IOCTL_TYPE, 24, struct kbase_ioctl_stream_create)
#define KBASE_IOCTL_FENCE_VALIDATE \
  _IOW(KBASE_IOCTL_TYPE, 25, struct kbase_ioctl_fence_validate)
#define KBASE_IOCTL_MEM_PROFILE_ADD \
  _IOW(KBASE_IOCTL_TYPE, 27, struct kbase_ioctl_mem_profile_add)
#define KBASE_IOCTL_STICKY_RESOURCE_MAP \
  _IOW(KBASE_IOCTL_TYPE, 29, struct kbase_ioctl_sticky_resource_map)
#define KBASE_IOCTL_STICKY_RESOURCE_UNMAP \
  _IOW(KBASE_IOCTL_TYPE, 30, struct kbase_ioctl_sticky_resource_unmap)
#define KBASE_IOCTL_MEM_FIND_GPU_START_AND_OFFSET \
  _IOWR(KBASE_IOCTL_TYPE, 31, union kbase_ioctl_mem_find_gpu_start_and_offset)
#define KBASE_IOCTL_HWCNT_SET \
  _IOW(KBASE_IOCTL_TYPE, 32, struct kbase_ioctl_hwcnt_values)
#define KBASE_IOCTL_CINSTR_GWT_START \
  _IO(KBASE_IOCTL_TYPE, 33)
#define KBASE_IOCTL_CINSTR_GWT_STOP \
  _IO(KBASE_IOCTL_TYPE, 34)
#define KBASE_IOCTL_CINSTR_GWT_DUMP \
  _IOWR(KBASE_IOCTL_TYPE, 35, union kbase_ioctl_cinstr_gwt_dump)
#define KBASE_IOCTL_CS_QUEUE_REGISTER \
  _IOW(KBASE_IOCTL_TYPE, 36, struct kbase_ioctl_cs_queue_register)
#define KBASE_IOCTL_CS_QUEUE_KICK \
  _IOW(KBASE_IOCTL_TYPE, 37, struct kbase_ioctl_cs_queue_kick)
#define KBASE_IOCTL_MEM_EXEC_INIT \
  _IOW(KBASE_IOCTL_TYPE, 38, struct kbase_ioctl_mem_exec_init)
#define KBASE_IOCTL_CS_QUEUE_BIND \
  _IOWR(KBASE_IOCTL_TYPE, 39, union kbase_ioctl_cs_queue_bind)
#define KBASE_IOCTL_CS_QUEUE_REGISTER_EX \
  _IOW(KBASE_IOCTL_TYPE, 40, struct kbase_ioctl_cs_queue_register_ex)
#define KBASE_IOCTL_CS_QUEUE_TERMINATE \
  _IOW(KBASE_IOCTL_TYPE, 41, struct kbase_ioctl_cs_queue_terminate)
#define KBASE_IOCTL_CS_QUEUE_GROUP_TERMINATE \
  _IOW(KBASE_IOCTL_TYPE, 43, struct kbase_ioctl_cs_queue_group_term)
#define KBASE_IOCTL_CS_EVENT_SIGNAL \
  _IO(KBASE_IOCTL_TYPE, 44)
#define KBASE_IOCTL_KCPU_QUEUE_CREATE \
  _IOR(KBASE_IOCTL_TYPE, 45, struct kbase_ioctl_kcpu_queue_new)
#define KBASE_IOCTL_KCPU_QUEUE_DELETE \
  _IOW(KBASE_IOCTL_TYPE, 46, struct kbase_ioctl_kcpu_queue_delete)
#define KBASE_IOCTL_KCPU_QUEUE_ENQUEUE \
  _IOW(KBASE_IOCTL_TYPE, 47, struct kbase_ioctl_kcpu_queue_enqueue)
#define KBASE_IOCTL_CS_TILER_HEAP_INIT \
  _IOWR(KBASE_IOCTL_TYPE, 48, union kbase_ioctl_cs_tiler_heap_init)
#define KBASE_IOCTL_CS_TILER_HEAP_INIT_1_13 \
  _IOWR(KBASE_IOCTL_TYPE, 48, union kbase_ioctl_cs_tiler_heap_init_1_13)
#define KBASE_IOCTL_CS_TILER_HEAP_TERM \
  _IOW(KBASE_IOCTL_TYPE, 49, struct kbase_ioctl_cs_tiler_heap_term)
#define KBASE_IOCTL_GET_CPU_GPU_TIMEINFO \
  _IOWR(KBASE_IOCTL_TYPE, 50, union kbase_ioctl_get_cpu_gpu_timeinfo)
#define KBASE_IOCTL_CS_GET_GLB_IFACE \
  _IOWR(KBASE_IOCTL_TYPE, 51, union kbase_ioctl_cs_get_glb_iface)
#define KBASE_IOCTL_KINSTR_JM_FD \
  _IOWR(KBASE_IOCTL_TYPE, 51, union kbase_kinstr_jm_fd)
#define KBASE_IOCTL_VERSION_CHECK \
  _IOWR(KBASE_IOCTL_TYPE, 52, struct kbase_ioctl_version_check)
#define KBASE_IOCTL_VERSION_CHECK_RESERVED \
  _IOWR(KBASE_IOCTL_TYPE, 52, struct kbase_ioctl_version_check)
#define KBASE_IOCTL_CS_CPU_QUEUE_DUMP \
  _IOW(KBASE_IOCTL_TYPE, 53, struct kbase_ioctl_cs_cpu_queue_info)
#define KBASE_IOCTL_CONTEXT_PRIORITY_CHECK \
  _IOWR(KBASE_IOCTL_TYPE, 54, struct kbase_ioctl_context_priority_check)
#define KBASE_IOCTL_SET_LIMITED_CORE_COUNT \
  _IOW(KBASE_IOCTL_TYPE, 55, struct kbase_ioctl_set_limited_core_count)
#define KBASE_IOCTL_KINSTR_PRFCNT_ENUM_INFO \
  _IOWR(KBASE_IOCTL_TYPE, 56, struct kbase_ioctl_kinstr_prfcnt_enum_info)
#define KBASE_IOCTL_KINSTR_PRFCNT_SETUP \
  _IOWR(KBASE_IOCTL_TYPE, 57, union kbase_ioctl_kinstr_prfcnt_setup)
#define KBASE_IOCTL_CS_QUEUE_GROUP_CREATE \
  _IOWR(KBASE_IOCTL_TYPE, 58, union kbase_ioctl_cs_queue_group_create)
#define KBASE_IOCTL_MEM_ALLOC_EX \
  _IOWR(KBASE_IOCTL_TYPE, 59, union kbase_ioctl_mem_alloc_ex)
#define KBASE_IOCTL_READ_USER_PAGE \
  _IOWR(KBASE_IOCTL_TYPE, 60, union kbase_ioctl_read_user_page)

// Pixel specific ioctls
#define KBASE_IOCTL_APC_REQUEST \
  _IOW(KBASE_IOCTL_TYPE, 66, struct kbase_ioctl_apc_request)
#define KBASE_IOCTL_BUFFER_LIVENESS_UPDATE \
  _IOW(KBASE_IOCTL_TYPE, 67, struct kbase_ioctl_buffer_liveness_update)
