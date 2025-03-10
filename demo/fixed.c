#include <stdio.h>

#define PG_IMPLEMENTATION
#include "../pager.h"

void print_page_map(struct pg_handler *hdl)
{
        int i;
        unsigned char page_byte;
        unsigned char state;

        char letter[4] = {'X', 'A', 'B', 'C'};

        for(i = 0; i < hdl->page_number; i++) {
                page_byte = *(hdl->page_map + (i / 4));

                if(i == 0)
                        printf("%02x: ", page_byte);
                else if(i % 4 == 0)
                        printf("\n%02x: ", page_byte);

                state = (page_byte >> ((3 - (i % 4)) * 2)) & 3;
                printf("%c ", letter[state]);
        }
}


int main(void)
{
        struct pg_handler hdl;

        #define BUFFER_SIZE 513
        unsigned char buffer[BUFFER_SIZE];

        if(pg_init_fixed(&hdl, buffer, BUFFER_SIZE) < 0) {
                printf("Failed to initialize PG\n");
                return -1;
        }
        

        printf("page_number:   %d\n", hdl.page_number);
        printf("mem_bank_size: %d\n", hdl.mem_bank_size);
        printf("page_map_size: %d\n", hdl.page_map_size);

        printf("Page Map:\n");
        print_page_map(&hdl);
        printf("\n");

        pg_shutdown(&hdl);

        return 0;
}
