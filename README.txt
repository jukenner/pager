                                      
                                     
__   _ __    __ _   __ _   ___  _ __ 
\ \ | '_ \  / _` | / _` | / _ \| '__|
/_/ | |_) || (_| || (_| ||  __/| |   
    | .__/  \__,_| \__, | \___||_|   
    | |             __/ |            
    |_|            |___/             


Pager is a single-header memory manager supporting both static and dynamic
memory handling written in pure ANSI-C.

General Structure

    The handler consists of a memory bank representing the available memory pool
    which is divided into pages and the page map containing the states for all
    pages in the memory bank. Depending on the configuration of the handler the
    memory bank and page map will either be allocated dynamically or both be
    contained in the provided buffer. If the handler is configured as dynamic,
    both the bank and map will scale to meet the needs of the user. In case of a
    fixed configuration both the bank and map will not scale and the available
    memory will be limited to the provided buffer.


Memory Bank

    The memory bank is the central pool of all usable memory. It can either be
    provided by the user or be allocated dynamically using the configured
    functions. The memory is divided into pages with the size of each page set
    through the PG_PAGE_SIZE macro.

    The memory bank has the following structure:

        +------------------+
        |      Page 1      |
        +------------------+
        |      Page 2      |
        +------------------+
        |      Page 3      |
        +------------------+
        |        ...       |
        +------------------+
        |     Page N       |
        +------------------+
 

Page Map

    Each page has a 2-bit state in the page map indicating its availability and
    usage. Multiple pages can form a memory block by chaining them together.

    The state of a page can be one of the following 2-bit codes:

        00: Unused
        01: Used, Beginning of a memory block
        10: Used, Continuation of a memory block
        11: Used, End of a memory block (Can be used by itself)

    A single-page block can be created using just the "11" state.

    The page map has the following structure:

        0     2     4     6     8
        +-----+-----+-----+-----+
        | P-1 | P-2 | P-3 | P-4 |
        +-----+-----+-----+-----+
        | P-5 | P-6 | ... | P-N |
        +-----+-----+-----+-----+

    Some examples for memory blocks are:

                        +-----+-----+-----+
        1-page block:   | ... |  11 | ... |
                        +-----+-----+-----+

                        +-----+-----+-----+-----+
        2-page block:   | ... |  01 |  11 | ... |
                        +-----+-----+-----+-----+

                        +-----+-----+-----+-----+-----+-----+-----+
        N-page block:   | ... |  01 |  10 |  10 | ... |  11 | ... |
                        +-----+-----+-----+-----+-----+-----+-----+

Including the library

    To include the library in a project, simply dowload the pager.h file and
    include it. The header can be included as many times as necessary, but the
    macro PG_IMPLEMENTATION has to be defined once. In case the standard
    libraries need to be included, if for example the default allocator
    functions are needed, the macro PG_STANDARD_LIBRARY has to be defined
    once aswell.

    This is an example of how to include the library:

        ...
        #define PG_IMPLEMENTATION 
        #include "pager.h"
        ...

    If you want the default functionality use the following code:

        ...
        #define PG_IMPLEMENTATION
        #define PG_STANDARD_LIBRARY
        #include "pager.h"
        ...

API functions

    The following is an overview of all essential API functions provided by this
    library. For actual demonstrations of the different features see the
    examples in the folder labeled "demo".

    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    >> int pg_init_default(struct pg_handler *hdl)

    Description:

        Initialize the handler using the dynamic configuration with the the
        default allocation functions(malloc, realloc, free). The memory bank and
        page map will automatically be allocated. To use this function the macro
        PG_STANDARD_LIB has to be defined beforehand.

    Parameters:

        @hdl: Pointer to the memory handler struct

    Returns:

        The function return 0 on success or -1 if an error occurred.

    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    >> int pg_init_fixed(struct pg_handler *hdl, void *buf, int size)

    Description:

        Initialize the memory handler with a fixed memory buffer. Both the
        memory bank and the page map will be contained in the provided buffer.
        No additional memory will be allocated. Note that the provided memory
        buffer may only be freed after the memory handler has been shutdown!

    Parameters:

        @hdl: Pointer to the memory handler struct
        @buf: Pointer to the memory buffer
        @size: The size of the memory buffer in bytes

    Returns:

        The function return 0 on success or -1 if an error occurred.

    Example:

        ...
        struct pg_handler hdl;
        unsigned char buffer[512];
        pg_init_fixed(&hdl, buf, 512);
        ...

    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    >> int pg_init_custom(struct pg_handler *hdl, struct pg_functions fnc)

    Description:

        This function initializes the memory handler with a set of custom
        allocation functions. These will be used to allocate memory for both the
        memory bank and page map and scale both to meet the need of the user.
        The three functions that have to be set are:

            - void *alloc(int size)
            - void *realloc(void *ptr, int size)
            - void  free(void *ptr)

        Both alloc and realloc should either return a pointer for the requested
        memory or NULL in case of failure.

    Parameters:
        
        @hdl: Pointer to the memory handler struct
        @fnc: A set of custom allocation functions

    Returns:

        The function return 0 on success or -1 if an error occurred.

    Example(using custom wrapper for the default allocator functions):

        ...
        void *cstm_alloc(int size) {
            return malloc(size);
        }

        void *cstm_realloc(void *ptr, int size) {
            return realloc(ptr, size);
        }

        void free(void *ptr) {
            return free(ptr);
        }
        ...
        struct pg_handler hdl;
        struct pg_functions func;

        func.f_alloc   = &cstm_alloc;
        func.f_realloc = &cstm_realloc;
        func.f_free    = &ctsm_free;

        pg_init_custom(&hdl, func);
        ...
        
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    >> void *pg_shutdown(struct pg_handler *hdl)

    Description:

        Shutdown the memory manager and free all dynamically allocated memory.
        If the manager has been initialized with a provided buffer, the provided
        memory has to be freed manually by the user.

    Parameters:

        @hdl: Pointer to the memory handler struct

    Returns:

        The function will return a pointer to the memory that has to be freed by
        the user. If there is no more memory to free NULL will be returned.

    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    >> void *pg_alloc(struct pg_handler *hdl, int size)

    Description:

        This function will create a memory block in the memory bank and mark the
        corresponding pages as used. It then returns the pointer for this memory
        block. If there are no more free pages left and the handler is
        configured as dynamic, it will try to scale memory bank to meet the
        demands of the user. If the bank is full and the handler is configured
        as fixed, the function will just return NULL without affecting the
        memory.

    Parameters:

        @hdl: Pointer to the memory handler struct
        @size: The number of bytes to allocate from the memory bank

    Returns:
        
        The function will either return a pointer for the created memory block
        or NULL if there are no more free pages or the scaling failed.

    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    >> void *pg_realloc(struct pg_handler *hdl, void *ptr, int size)

    Description:

        Scale an existing memory block to a bigger size. The function will first
        try to attach the following pages to the memory block or if that is not
        possible allocate an entirely new memory block and copy over the content
        from the original pages. If there are no more free pages left and the
        handler is configured as dynamic, the memory bank will scale to meet
        demand. Otherwise if the handler is configured as fixed and no more
        unused pages are left, the function will return NULL and do nothing to
        the memory.

    Parameters:

        @hdl: Pointer to the memory handler struct
        @ptr: Pointer to the original memory block
        @size: The size in bytes to scale the memory block to

    Returns:

        The function will either return a pointer for the updated memory block
        or NULL if there are no more free pages or the scaling failed. 

    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    >> void pg_free(struct pg_handler *hdl, void *ptr);

    Description:

        Release all pages in the memory block and mark them as unused so they
        can be allocated again.

    Parameters:

        @hdl: Pointer to the memory handler struct
        @ptr: Pointer to the memory block to free

    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    >> void *pg_set(void *ptr, unsigned char val, int num)

    Description:

        Set the value of all bytes in a memory block.

    Parameters:

        @ptr: Pointer to a memory block
        @val: The code to write to every byte
        @num: The number of bytes to set in the memory block

    Returns:

        This function returns a pointer to the modified memory or NULL if an
        error occurred.

    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    >> void *pg_copy(void *dst, void *src, int num)

    Description:

        Copy over bytes from the source to the destination. This function starts
        at the provided pointers and copies byte by byte. So in case the source
        and destination areas overlap, the source can potentially be overwritten
        while currently copying from it. This can lead to loss of data. For this
        reason, if the areas overlap it is recommended to use pg_move() instead.

    Parameters:

        @dst: Pointer to the address to copy to
        @src: Pointer to the address to copy from
        @num: The number of bytes to copy from the source to the destination

    Returns:

        This function will return a pointer to the destination area or NULL
        if an error occurred.

    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    >> void *pg_move(void *dst, void *src, int num)

    Description:

        Copy over memory from the source area to the destination area. This
        function will take overlap into consideration. So even if the source
        area and destination area overlap, the function take care to copy over
        the memory before it is overwritten. This will ensure that no data is
        lost. This will however take longer than the standard pg_copy()
        function, so in case the areas don't overlap, it is recommended to use
        the copy function instead.

    Parameters:

        @dst: Pointer to the destination area to copy to
        @src: Pointer to the source area to copy from
        @num: The number of bytes to copy from the source to the destination
        
    Returns:
        
        This function will return a pointer to the destination area or NULL if
        an error occurred.

    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    >> void *pg_compare(void *ptr1, void *ptr2, int num);

    Description:

        Compare the first num bytes starting from ptr1 to the first num bytes
        starting from ptr2.

    Parameters:

        @ptr1: Pointer to the first memory area
        @ptr2: Pointer to the second memory area
        @num: The number of bytes to compare starting from the first and second
              pointer

    Returns:

        This function will return 0 if both memory areas are equal. Otherwise
        if the first byte that does not match is smaller in ptr1 than in ptr2 -1
        will be returned. Otherwise if the byte is smaller in ptr2 than in ptr1
        1 will be returned.
