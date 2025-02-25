#include "pager.h"

#include <stdlib.h>


#ifdef PG_STANDARD_LIB

/*
 * The default memory handling functions.
 */
PG_INTERN void *_pg_alloc(int size)
{
        return malloc(size);
}

PG_INTERN void *_pg_realloc(void *ptr, int size)
{
        return realloc(ptr, size);
}

PG_INTERN void _pg_free(void *ptr)
{
        free(ptr);
}

PG_API int pg_init_default(struct pg_handler *hdl)
{
        int i;

        hdl->mode = PG_DYNAMIC;

        hdl->functions.alloc   = &_pg_alloc;
        hdl->functions.realloc = &_pg_realloc;
        hdl->functions.free    = &_pg_free;

        hdl->page_number   = PG_PAGE_MIN;
        hdl->mem_bank_size = hdl->page_number * PG_PAGE_SIZE;
        hdl->used_number   = 0;
        hdl->page_map_size = PG_CEILING(hdl->page_number, 4); 

        /* Allocate memory for the main buffer */
        if(!(hdl->mem_bank = hdl->functions.alloc(hdl->mem_bank_size)))
                goto err_return;

        /* Allocate memory for the page map */
        if(!(hdl->page_map = hdl->functions.alloc(hdl->page_map_size)))
                goto err_free_buffer;

        /* Clear the memory for the page map */
        for(i = 0; i < hdl->page_map_size; i++)
                hdl->page_map[i] = 0;

        return 0;

err_free_buffer:
        hdl->functions.free(hdl->mem_bank);

err_return:
        return -1;
}

#endif /* PG_STANDARD_LIB */


PG_API int pg_init_fixed(struct pg_handler *hdl, void *buf, int size)
{
        int i;
        int n = size / (PG_PAGE_SIZE + 0.25);

        /*
         * Check if the provided buffer is big enough to fit at least one page.
         */
        if(n < 1)
                return -1;

        hdl->mode = PG_FIXED;

        hdl->provided_pointer = buf;
        hdl->provided_size = size;
                
        hdl->page_number = n;
        hdl->used_number = 0;

        hdl->page_map_size = PG_CEILING(n, 4);
        if(hdl->page_map_size < 1) hdl->page_map_size = 1;

        hdl->page_map = buf;
        hdl->mem_bank = (unsigned char *)buf + hdl->page_map_size;
        hdl->mem_bank_size = hdl->page_number * PG_PAGE_SIZE;

        /* Clear the memory for the page map */
        for(i = 0; i < hdl->page_map_size; i++)
                hdl->page_map[i] = 0;

        return 0;
}


PG_API int pg_init_custom(struct pg_handler *hdl, struct pg_functions fnc)
{
        int i;

        hdl->mode = PG_DYNAMIC;

        hdl->functions = fnc;

        hdl->page_number   = PG_PAGE_MIN;
        hdl->mem_bank_size = hdl->page_number * PG_PAGE_SIZE;
        hdl->used_number   = 0;
        hdl->page_map_size = PG_CEILING(hdl->page_number, 4); 

        /* Allocate memory for the main buffer */
        if(!(hdl->mem_bank = hdl->functions.alloc(hdl->mem_bank_size)))
                goto err_return;

        /* Allocate memory for the page map */
        if(!(hdl->page_map = hdl->functions.alloc(hdl->page_map_size)))
                goto err_free_buffer;

        /* Clear the memory for the page map */
        for(i = 0; i < hdl->page_map_size; i++)
                hdl->page_map[i] = 0;

        return 0;

err_free_buffer:
        hdl->functions.free(hdl->mem_bank);

err_return:
        return -1;
}


PG_API void *pg_shutdown(struct pg_handler *hdl)
{
        if(hdl->mode == PG_FIXED)
                return hdl->provided_pointer;

        hdl->functions.free(hdl->page_map);
        hdl->functions.free(hdl->mem_bank);
        return NULL;
}


PG_API void *pg_alloc(int size)
{
        
}
