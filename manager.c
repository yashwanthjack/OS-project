#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int VM_SIZE = 256;
const int PAGE_SIZE = 256;
const int TLB_SIZE = 16;
const int MM_SIZE = 256;

#define MAX_LINE_LENGTH 1024

int main(int argc, char* argv[]) {
    FILE *fd;

    if (argc < 2) {
        printf("NOT ENOUGH ARGUMENTS\nEXITING\n");
        return 0;
    }
    
    fd = fopen("address.txt", "r");
    if (fd == NULL) {
        printf("ERROR OPENING FILE\nFILE FAILED TO OPEN\n");
        return 0;
    }
    
    char value[MAX_LINE_LENGTH];
    long long page_no, offset, totalhits = 0, faults = 0, pages = 0;
    
    int qp = 0; // to maintain the queue position
    int physicalad = 0, frame, logicalad;
    
    int tlb[TLB_SIZE][2];
    int pagetable[PAGE_SIZE];

    memset(tlb, -1, sizeof(tlb));
    memset(pagetable, -1, sizeof(pagetable));

    while (fgets(value, sizeof(value), fd) != NULL) {
        pages++;
        logicalad = atoi(value);
        
        // Get page number and offset from logical address
        page_no = (logicalad >> 8) & 0xFF;  // Masking for page number
        offset = logicalad & 0xFF;          // Masking for offset
        
        int hit = 0; // 1 if found in TLB
        
        // CHECK IN TLB
        for (int i = 0; i < TLB_SIZE; i++) {
            if (tlb[i][0] == page_no) {
                hit = 1;
                totalhits++;
                frame = tlb[i][1];
                break;
            }
        }

        // If present in TLB
        if (hit) {
            printf("TLB HIT\n");
        } else {
            // Search in page table
            int f = 0;
            for (int i = 0; i < PAGE_SIZE; i++) {
                if (pagetable[i] == page_no) {
                    frame = i;
                    f = 1;
                    break;
                }
                if (pagetable[i] == -1) {
                    pagetable[i] = page_no;
                    frame = i;
                    f = 1;
                    faults++;
                    break;
                }
            }
            // If page not found, we should replace it (not shown in your code)
            if (!f) {
                // Implement page replacement logic if needed
                faults++;
                frame = 0; // Example: replace with frame 0 (this logic should be improved)
            }
            
            // Replace in TLB using FIFO
            tlb[qp][0] = page_no;
            tlb[qp][1] = frame;
            qp = (qp + 1) % TLB_SIZE; // Wrap around correctly
        }

        physicalad = frame * PAGE_SIZE + offset;
        printf("VIRTUAL ADDRESS = %d\tPHYSICAL ADDRESS = %d\n", logicalad, physicalad);
    }

    fclose(fd);  // Close the file
    
    double hitrate = (double)totalhits / pages * 100;
    double faultrate = (double)faults / pages * 100;
    
    printf("\nTLB HIT RATE = %.2f %%\n", hitrate);
    printf("TLB MISS RATE = %.2f %%\n", (100 - hitrate));
    printf("PAGE TABLE HIT RATE = %.2f %%\n", (1 - (faultrate / 100)) * 100);
    printf("PAGE TABLE MISS RATE = %.2f %%\n", faultrate);

    return 0;
}
