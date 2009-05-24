/*
 * MALHEUR - Automatic Malware Analysis on Steroids
 * Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
 * Berlin Institute of Technology (TU Berlin).
 * --
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.  This program is distributed without any
 * warranty. See the GNU General Public License for more details. 
 * --
 */

#include "config.h"
#include "common.h"
#include "farray.h"
#include "util.h"

/**
 * Extracts an array of feature vectors from a directory. The function 
 * loads and converts files in the given directory. It does not process
 * further subdirectories recursively.
 * @param dir directory containing file.
 */
farray_t *farray_extract_dir(char *dir)
{
    assert(dir);
    
    farray_t *a = malloc(sizeof(farray_t));
    if (!a) {
        error("Could not allocate array of feature vectors");
        return NULL;
    }
    
    return a;
}

/**
 * Destroys an array of feature vectors
 * @param a array of feature vectors
 */
void farray_destroy(farray_t *a)
{
    int i;    
    if (!a)
        return;
    
    /* Free vectors */
    if (a->x) {
        for (i = 0; i < a->len; i++)
            fvec_destroy(a->x[i]);
        free(a->x);
    }

    /* Free labels */
    if (a->y)
        free(a->y);
    
    /* Free classes */
    if (a->labels) {
        for (i = 0; a->labels[i]; i++)
            free(a->labels[i]);
        free(a->labels);
    }    
    
    free(a);
}
