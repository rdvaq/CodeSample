        if (num_bytes_l > 0) {
            partial_page = TRUE;
            populated_blocks++;
        }

        // data array that stores the number of bytes to write 
        int data[(MAX_DATA_BLOCKS + 2)] = {0}; // increased by 2 to accomodate max file size
        // array that stores the block num of newly allocated blocks 
        int new_blocks[MAX_DATA_BLOCKS] = {0};

        int free_space = 0;
        int iterator = 0;
        int write_left = 0;

        if (partial_page == TRUE) {
            free_space = BLOCK_SIZE - num_bytes_l;

            if (count <= free_space){ // the current partial page is enough 
                data[iterator] = count;
                iterator++;
                data[iterator] = 0; // sentinel 
            } else { // needs more blocks after the partial page
                data[iterator] = free_space;
                iterator++;

                int write_left = count - free_space; // data left after filling partial page
            }

        } else { // no partial page 
            int write_left = count; 
            int iterator = 0;
        }
        
        while (write_left >= BLOCK_SIZE) {
        data[iterator] = BLOCK_SIZE;
        write_left = write_left - BLOCK_SIZE;
        iterator++;
        }

        data[iterator] = write_left; // takes care of the case where write left is a multiple of BS
        iterator++;
        data[iterator] = 0; // put in sentinel value 