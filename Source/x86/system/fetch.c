/*
 * Author   Nerijus Ramanauskas <nerijus.ramanauskas@mocosel.org>,
 * Date     11/11/2013,
 * Revision 11/11/2013,
 *
 * Copyright 2013 Nerijus Ramanauskas.
 */

#include <stdio.h>
#include "../plain.h"

void PLAIN_FETCH(const char* __restrict identifier, struct MOCOSEL_SEGMENT* __restrict segment) {
    MOCOSEL_ASSERT(identifier != NULL);
    MOCOSEL_ASSERT(segment != NULL);
    if(identifier == NULL || segment == NULL) {
        return;
    }
    FILE* file = fopen(identifier, "rb");
    if(file == NULL) {
        return (void)printf("System error: cannot access %s.\n", identifier);
    }
    if(fseek(file, 0, SEEK_END) != 0) {
        return (void)printf("System error: unsupported file system.\n");
    }
    long int length = ftell(file);
    if(length > 0) {
        segment->from = (MOCOSEL_BYTE*)MOCOSEL_RESIZE(NULL, (MOCOSEL_WORD_DOUBLE)length, 0);
        segment->to = segment->from + length;
        if(segment->from == NULL) {
            printf("System error: cannot allocate %ld bytes of memory.\n", length);
        } else {
            fseek(file, 0, SEEK_SET);
            if(fread(segment->from, length, 1, file) != 1) {
                printf("System error: failed reading file %s.\n", identifier);
            }
        }
    }
    fclose(file);
}
