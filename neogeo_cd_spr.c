#include <stdio.h>
#include <stdint.h>

void print_bits(uint32_t value) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
        if (i % 4 == 0 && i != 0) {
            printf(" ");  // Add space every 4 bits for readability
        }
    }
    printf("\n");
}

uint32_t swap_bits(uint32_t value) {
    uint32_t swapped = 0;
    int src_bits[] =  {23,31,7,15,22,30,6, 14,21,29,5, 13,20,28,4, 12,19,27,3, 11,18,26,2, 10,17,25,1, 9,16,24,0, 8};
    int dest_bits[] = {31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

    for (int i = 0; i < 32; ++i) {
        swapped |= ((value >> src_bits[i]) & 1) << dest_bits[i];
    }

    return swapped;
}

int main(int argc, char *argv[]) {
/*
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input SPR file> <output_file>\n", argv[0]);
        return 1;
    }
*/
    const char *input_file1 = argv[1];
    const char *output_file = argv[2];

    FILE *file1 = fopen(input_file1, "rb");
    FILE *outFile = fopen(output_file, "wb");

    if (file1 == NULL || outFile == NULL) {
        perror("Error opening files");
        return 1;
    }

    uint32_t byte1[4];
    int16_t postype = 1;
    uint8_t waiting_i_clear = 0;
    uint16_t i=0;
    while (fread(byte1, 1, 4, file1) == 4) {
        // Merge the bytes into a 32-bit integer
        uint32_t mergedValue = 0;
        mergedValue |= (uint32_t)byte1[3] << 24;
        mergedValue |= (uint32_t)byte1[2] << 16;
        mergedValue |= (uint32_t)byte1[1] << 8;
        mergedValue |= (uint32_t)byte1[0];

        // Swap the bits
        uint32_t swappedValue = swap_bits(mergedValue);
        if (i==16) {
            fseek(outFile, -128, SEEK_CUR);
            postype=-postype;
            //printf("r!!!!\n");

        }

        if (i==32) {
            i = 0;
            postype=-postype;
        }

        //printf("i and postype: %u, %d\n", i,postype);
        if (postype == 1) {
            fseek(outFile, 4, SEEK_CUR);
            fwrite(&swappedValue, sizeof(uint32_t), 1, outFile);
            //printf("section2 Value: %x\n",swappedValue);
        } else if (postype == -1) {
            fwrite(&swappedValue, sizeof(uint32_t), 1, outFile);
            fseek(outFile, 4, SEEK_CUR);
            //printf("section1 Value: %x\n",swappedValue);
        }
        // printf("i is: %u\n", i);
        i++;
    }

    // Close the files
    fclose(file1);
    fclose(outFile);

    return 0;
}
