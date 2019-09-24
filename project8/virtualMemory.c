#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define Size 256
#define PageNum 256
#define FrameSize 256
#define FrameNum 256
#define TLBNum 16

typedef struct
{
    int systick;
    int pageNumber;
    int frameNumber;
}tlb;

typedef struct
{
    int systick;
    int frameNumber;
    char data[Size];
}pm;

pm physicalMemory[FrameNum];     // physical memory, each includes a systick , its frameNumber and 256 single-byte data
int pageTable[PageNum];          // page table, index is page number, value is frame number
tlb TLB[TLBNum];		 // TLB, each includes a systick, a valid page number and its corresbonding frame number
int time = 0;			 // systick

// auxiliary functions
void TLBReplace(int time, int pageNumber, int frameNumber);
int PageReplace(FILE *fp, int time, int pageNumber, int frameNumber);
bool getValue(int frameNumber, int offset, int *value, int time);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
	printf("Wrong arguments!\n");
	return 0;
    }

    FILE *address = fopen(argv[1], "r");
    int addr;
    int mask = 0x000000ff;
    int pageNumber;
    int offset;
    int frameNumber;
    int value;

    float pageFault = 0;
    float TLBHit = 0;
    int pageTableSize = 0;

    bool TLBHitFlag = false;
    bool pageFaultFlag = true;
    bool pageReplaceFlag = true;

    FILE *data = fopen("BACKING_STORE.bin", "rb");

    // initialize TLB, physical memory and page table
    for (int i = 0; i < FrameNum; ++i)
    {
	physicalMemory[i].systick = 0;
	physicalMemory[i].frameNumber = -1;
    }
    for (int i = 0; i < PageNum; ++i)
    {
	pageTable[i] = -1;
    }
    for (int i = 0; i < TLBNum; ++i)
    {
	TLB[i].systick = 0;
	TLB[i].pageNumber = -1;
	TLB[i].frameNumber = -1;
    }

    // read in one logical address a time and translate
    while(time < 1000)
    {
	fscanf(address, "%d", &addr);
	TLBHitFlag = false;
	pageFaultFlag = true;
	pageReplaceFlag = true;

	offset = addr & mask;
	pageNumber = (addr >> 8) & mask;
	time++;

	// TLB hit
	for (int i = 0; i < TLBNum; ++i)
	{
	    if (TLB[i].pageNumber == pageNumber)
	    {
		TLBHitFlag = true;
		frameNumber = TLB[i].frameNumber;
		TLBHit++;
		TLB[i].systick = time;
		break;
	    }
	}
	// TLB hit and no page replacement
	if (TLBHitFlag)
	{
	    pageFaultFlag = false;
	    pageReplaceFlag = getValue(frameNumber, offset, &value, time);
	}

	// TLB not hit, but no page fault
	if (!TLBHitFlag && pageTable[pageNumber] != -1)
	{
	    frameNumber = pageTable[pageNumber];
	    pageFaultFlag = false;
	    TLBReplace(time, pageNumber, frameNumber);
	    pageReplaceFlag = getValue(frameNumber, offset, &value, time);
	}
	// TLB not hit, and page fault
	if (!TLBHitFlag && pageFaultFlag)
	{
	    pageTable[pageNumber] = pageTableSize;
	    pageTableSize++;
	    frameNumber = pageTable[pageNumber];
	    TLBReplace(time, pageNumber, frameNumber);
	}

	// page replacement
	if (pageReplaceFlag)
	{
            int index = PageReplace(data, time, pageNumber, frameNumber);
            value = physicalMemory[index].data[offset];
            pageFault++;
        }

	printf("Virtual address:%d Physical address:%d Value:%d\n", addr, frameNumber * 256 + offset, value);
    }

    fclose(address);
    fclose(data);

    printf("TLB hit rate: %f%, page fault rate: %f%\n", TLBHit / 10, pageFault / 10);
    return 0;
}

bool getValue(int frameNumber, int offset, int *value, int time)
{
    for (int i = 0; i < FrameNum; ++i)
    {
        if (physicalMemory[i].frameNumber == frameNumber)
        {
            *value = physicalMemory[i].data[offset];
            physicalMemory[i].systick = time;
            return false;
	}
    }
    return true;
}

void TLBReplace(int time, int pageNumber, int frameNumber)
{
    int min = 99999;
    int index = 0;
    // LRU
    for (int i = 0; i < TLBNum; ++i)
	if (TLB[i].systick < min)
	{
	    min = TLB[i].systick;
	    index = i;
	}

    TLB[index].systick = time;
    TLB[index].pageNumber = pageNumber;
    TLB[index].frameNumber = frameNumber;
}

int PageReplace(FILE *fp, int time, int pageNumber, int frameNumber)
{
    int min = 99999;
    int index = 0;
    // LRU
    for (int i = 0; i < FrameNum; ++i)
        if (physicalMemory[i].systick < min)
        {
            min = physicalMemory[i].systick;
            index = i;
        }

    physicalMemory[index].systick = time;
    physicalMemory[index].frameNumber = frameNumber;
    fseek(fp, pageNumber * Size, SEEK_SET);
    fread(physicalMemory[index].data, sizeof(char), Size, fp);
    return index;
}
