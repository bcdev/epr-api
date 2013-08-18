#include <stdio.h>
#include <stdlib.h>


#define ARG_ERROR          1
#define ELEMSIZE_ERROR     2
#define FILESIZE_ERROR     3
#define SETPOS_ERROR       4
#define WRITE_ERROR        5
#define READ_ERROR         6
#define OPEN_ERROR         7


void printUsage();
void error(int errno, const char* message);
int checkCorrectElemSize(int size);
int swapBytes(FILE *file, int filesize, int typesize);


/**
 * swpeo: "Swap Endian Order": Utility to swap the endian order of given files
 * Usage: swpeo <element-size> <file-1> <file-2> ...
 */
int main(int argc, char** argv) {
    const char* filePath;
    int fileSize;
    int modulo;
    int swapStatus = 0;
    FILE *file;
    int elemSize = 4;
    char message[1024];
    int i;

    if (argc < 2) {
        printUsage();
        return ARG_ERROR;
    }

    elemSize = atoi(argv[1]);
    if (checkCorrectElemSize(elemSize) == -1) {
        error(ELEMSIZE_ERROR, "element size must be 2, 4 or 8");
    }

    for (i = 2; i < argc; i++) {

        filePath = argv[i];


        file = fopen(filePath, "rb+");
        if (file == NULL) {
            sprintf(message, "failed to open file '%s'", filePath);
            error(OPEN_ERROR, message);
        }

        fileSize = fseek(file, 0, SEEK_END);
        fileSize = ftell(file);

        modulo = fileSize % elemSize;
        if (modulo != 0) {
            sprintf(message, "file '%s': file size (%d) modulo element size (%d) should be zero, but was %d", filePath, fileSize, elemSize, modulo);
            error(FILESIZE_ERROR, message);
        }

        swapStatus = swapBytes(file, fileSize, elemSize);
        if (swapStatus != 0) {
            sprintf(message, "file '%s': I/O error", filePath);
            error(swapStatus, message);
        }

        fclose(file);
    }

    return swapStatus;
}


void printUsage() {
    fprintf(stdout, "Usage: swpeo <element size> <file 1> <file 2> ...\n");
}

void error(int errno, const char* message) {
    fprintf(stderr, "Error: %s", message);
    exit(errno);
}

void warn(const char* message) {
    fprintf(stderr, "Warning: %s", message);
}


int checkCorrectElemSize(int elemSize) {
    if (elemSize == 2 || elemSize == 4 || elemSize == 8) {
        return 0;
    }
    return -1;
}

int swapBytes(FILE *file, int fileSize, int elemSize) {
    long i, numItems, swaps;
    char buffer[8], a;
    fpos_t pos = 0;
    int numSwaps = fileSize / elemSize;

    for (swaps = 0; swaps < numSwaps; swaps++) {

        if (fsetpos(file, &pos) != 0) {
            return SETPOS_ERROR;
        }
        numItems = fread(buffer, elemSize, 1, file);
        if (numItems != 1) {
            return READ_ERROR;
        }

        for (i = 0; i < elemSize / 2; i++) {
            a = buffer[i];
            buffer[i] = buffer[elemSize -1 -i];
            buffer[elemSize - 1 - i] = a;
        }

        if (fsetpos(file, &pos) != 0) {
            return SETPOS_ERROR;
        }
        numItems = fwrite(buffer, elemSize, 1, file);
        if (numItems != 1) {
            return WRITE_ERROR;
        }
        fflush(file);

        pos += elemSize;
    }
    return 0;
}
