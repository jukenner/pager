#ifndef _MEMORY_H
#define _MEMORY_H

#define MEM_INTERN      static
#define MEM_API         extern


typedef void *(*mem_func)(void *, int);

enum mem_mode {
        MEM_FIXED,
        MEM_DYNAMIC
};

struct mem_handles {
        mem_func        alloc;
        mem_func        realloc;
        mem_func        free;
};

struct mem_allocator {
        enum mem_mode           mode;

        void                    *buffer;
        int                     buffer_size;

        struct mem_handles      handles;
};


/*
 * Initialize the memory allocator using the default configuration. This will
 * mark the allocator as dynamic using the standard clib allocator functions
 * (malloc, free, realloc).
 *
 * @mem: Pointer to the memory allocator
 * @prealloc: The number of bytes to preallocated during initialization (Use 0
 *            for default value)
 *
 * Returns: 0 on success or -1 if an error occurred
 */
MEM_API int mem_init_default(struct mem_allocator *mem, int prealloc);

/*
 * Initialize the memory allocator configured as fixed with the provided
 * buffer. No additional memory can be provided, beyond the provided buffer.
 *
 * @mem: Pointer to the memory allocator
 * @buf: Pointer to the memory buffer to use
 * @buf_sz: The size of the provided buffer in bytes
 *
 * Returns: 0 on success or -1 if an error occurred
 */
MEM_API int mem_init_fixed(struct mem_allocator *mem, void *buf, int buf_sz);

/*
 * Initialize the memory manager using a set of custom functions for allocating,
 * reallocating and freeing memory. 
 *
 * @mem: Pointer to the memory manager
 * @hdl: A set of custom memory functions
 * @prealloc: The number of bytes to preallocate during initialization(Use 0 for
 *            default value)
 *
 * Returns: 0 on success or -1 if an error occurred
 */
MEM_API int mem_init_custom(struct mem_allocator *mem, struct mem_handles hdl,
                int prealloc);

#endif /* _MEMORY_H */
