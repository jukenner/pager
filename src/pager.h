#ifndef _PAGER_MEMORY_H
#define _PAGER_MEMORY_H

#define PG_INTERN       static
#define PG_API          extern

#define PG_IGNORE(x)    (void)(x)

#define PG_CEILING(x,y) (((x) + (y) - 1) / (y))

/*
 * Redefine NULL as it's not builtin-type for whatever reason. :(
 */
#ifndef NULL
#define NULL ((void *)0)
#endif

typedef void *(*pg_func)(void *, int);

#define PG_PAGE_SIZE            512
#define PG_PAGE_MIN             8

enum pg_memory_mode {
        PG_UNDEF,
        PG_FIXED,
        PG_DYNAMIC
};

struct pg_functions {
        void *(alloc)(int);
        void *(realloc)(void *, int);
        void *(free)(void *);
};

enum pg_page_state {
        PG_UNUSED       = 1,    /* 00 */
        PG_START        = 2,    /* 01 */
        PG_CONTINUE     = 3,    /* 10 */
        PG_END          = 4     /* 11 */
};

struct pg_handler {
        /*
         * The mode of the handler.
         *
         *  FIXED: Only use the provided memory
         *  DYNAMIC: Use the given functions to allocate more memory
         */
        enum pg_memory_mode     mode;

        /*
         * The memory bank containing all requestable memory.
         * It's divided into pages each N-bytes long. Each page has a
         * state stored in the page-map.
         */
        void                    *mem_bank;
        int                     mem_bank_size;

        /*
         * The number of available pages and the number of pages currently in
         * use.
         */
        int                     page_number;
        int                     used_number;

        /*
         * The page map indicating the state of every page in the memory bank.
         * Each page is represented by 2 bits to indicate its state.
         *
         *  PG_UNUSED:   Unused 
         *  PG_START:    Used, Start of a memory block
         *  PG_CONTINUE: Used, Continuation of a memory block
         *  PG_END:      Used, End of a memory block (Can be used by itself)
         */
        unsigned char           *page_map;
        int                     page_map_size;

        /*
         * A set of functions used for handling dynamic memory.
         */
        struct pg_functions     functions;
};

#ifdef PG_STANDARD_LIB

#include <stdlib.h>

/*
 * Initialize the memory handler using the default configuration. This will
 * mark the handler as dynamic using the standard clib handler functions
 * (malloc, free, realloc).
 *
 * @hdl: Pointer to the memory handler
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PG_API int pg_init_default(struct pg_handler *hdl);

#endif /* _PG_STANDARD_LIB */

/*
 * Initialize the memory handler configured as fixed with the provided
 * buffer. No additional memory can be provided, beyond the buffer.
 *
 * @hdl: Pointer to the memory handler
 * @buf: Pointer to the memory buffer to use
 * @buf_sz: The size of the provided buffer in bytes
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PG_API int pg_init_fixed(struct pg_handler *hdl, void *buf, int buf_sz);

/*
 * Initialize the memory manager using a set of custom functions for allocating,
 * reallocating and freeing the memory buffer. 
 *
 * @hdl: Pointer to the memory manager
 * @fnc: A set of custom memory functions
 *
 * Returns: 0 on success or -1 if an error occurred
 */
PG_API int pg_init_custom(struct pg_handler *hdl, struct pg_functions fnc);

/*
 * Close and destroy the memory manager and free the contained memory if the
 * manager is configured as dynamic.
 *
 * @hdl: Pointer to the memory manager
 */
PG_API void pg_close(struct pg_handler *hdl);


PG_API void *pg_alloc(int size);

PG_API void *pg_realloc(void *ptr, int size);

PG_API void pg_free(void *ptr);

PG_API void pg_copy(void *dst, void *src, int size);

PG_API void pg_move(void *dst, void *src, int size);

PG_API void pg_set(void *ptr, unsigned char b, int size);

#endif /* _PAGER_MEMORY_H */
