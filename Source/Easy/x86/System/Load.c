/*
 * Author   Nerijus Ramanauskas <nerijus.ramanauskas@mocosel.org>,
 * Date     11/11/2013,
 * Revision 12/02/2013,
 *
 * Copyright 2013 Nerijus Ramanauskas.
 */

#include "../Easy.h"

/* For use within the translation unit. */
MOCOSEL_WORD_DOUBLE EASY_FETCH(const char* MOCOSEL_RESTRICT identifier, struct MOCOSEL_SEGMENT* MOCOSEL_RESTRICT segment, struct EASY_SESSION* MOCOSEL_RESTRICT session) {
    MOCOSEL_ASSERT(identifier != NULL);
    MOCOSEL_ASSERT(segment != NULL);
    MOCOSEL_ASSERT(session != NULL);
    /* MOCOSEL_ERROR_SYSTEM. */
    if(identifier == NULL || segment == NULL || session == NULL) {
        return MOCOSEL_ERROR_SYSTEM;
    }
    FILE* file = fopen(identifier, "rb");
    /* MOCOSEL_ERROR_RUNTIME. */
    if(file == NULL) {
        return MOCOSEL_ERROR_RUNTIME;
    }
    MOCOSEL_WORD_DOUBLE error = 0;
    /* MOCOSEL_ERROR_RUNTIME. */
    if(fseek(file, 0, SEEK_END) != 0) {
        error = MOCOSEL_ERROR_RUNTIME;
    }
    long int length = ftell(file);
    if(length > 0) {
        segment->from = (MOCOSEL_BYTE*)MOCOSEL_RESIZE(NULL, (MOCOSEL_WORD_DOUBLE)length, 0);
        segment->to = segment->from + length;
        /* MOCOSEL_ERROR_SYSTEM. */
        if(segment->from == NULL) {
            error = MOCOSEL_ERROR_SYSTEM;
        } else {
            fseek(file, 0, SEEK_SET);
            /* MOCOSEL_ERROR_RUNTIME. */
            if(fread(segment->from, length, 1, file) != 1) {
                error = MOCOSEL_ERROR_RUNTIME;
            }
        }
    }
    fclose(file);
    return error;
}

MOCOSEL_WORD_DOUBLE EASY_LOAD(const char* MOCOSEL_RESTRICT identifier, struct MOCOSEL_OBJECT* MOCOSEL_RESTRICT object, struct EASY_SESSION* MOCOSEL_RESTRICT session) {
    MOCOSEL_ASSERT(identifier != NULL);
    MOCOSEL_ASSERT(object != NULL);
    MOCOSEL_ASSERT(session != NULL);
    /* MOCOSEL_ERROR_SYSTEM. */
    if(identifier == NULL || object == NULL || session == NULL) {
        return MOCOSEL_ERROR_SYSTEM;
    }
    MOCOSEL_WORD_DOUBLE error = EASY_FETCH(identifier, &object->segment.data, session);
    /* MOCOSEL_ERROR_RUNTIME. */
    if(error & MOCOSEL_ERROR_RUNTIME) {
        EASY_WRITE("Failed accessing %s.\n", session, identifier);
    /* MOCOSEL_ERROR_SYSTEM. */
    } else if(error & MOCOSEL_ERROR_SYSTEM)  {
        EASY_WRITE("System error %d occured while operating on %s.\n", session, error, identifier);
    } else if(error != 0) {
        EASY_WRITE("Error %d occured while operating on %s.\n", session, error, identifier);
    }
    struct MOCOSEL_SEGMENT segment = {object->segment.data.from, object->segment.data.to};
    if(error != 0 || segment.from == NULL) {
        return error;
    }
    error = MOCOSEL_RUN(NULL, MOCOSEL_SEGMENT_COMPILE, &session->manifest, object, &segment);
    /* MOCOSEL_ERROR_RUNTIME. */
    if(error & MOCOSEL_ERROR_RUNTIME) {
        EASY_WRITE("Runtime error %d occured while compiling %s.\n", session, error, identifier);
    /* MOCOSEL_ERROR_SYSTEM. */
    } else if(error & MOCOSEL_ERROR_SYSTEM) {
        EASY_WRITE("System error %d occured while compiling %s.\n", session, error, identifier);
        EASY_WRITE("Error %d occured while compiling %s.\n", session, error, identifier);
    }
    MOCOSEL_FREE(segment.from);
    return error;
}