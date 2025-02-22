                                      
                                     
   __   _ __    __ _   __ _   ___  _ __ 
   \ \ | '_ \  / _` | / _` | / _ \| '__|
   /_/ | |_) || (_| || (_| ||  __/| |   
       | .__/  \__,_| \__, | \___||_|   
       | |             __/ |            
       |_|            |___/             

Pager is a single-header memory manager supporting both static and dynamic
memory handling written in pure ANSI-C.

General

    The handler consists of a memory bank representing the available memory pool
    and the page map containing the states for all pages in the memory bank.


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

    To create a single-page block can be created using just the "11" state.

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
