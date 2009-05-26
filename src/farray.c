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
#include "fvec.h"
#include "fio.h"
#include "util.h"

/* External variables */
extern int verbose;


/**
 * Extracts an array of feature vectors from a directory. The function 
 * loads and converts files in the given directory. It does not process
 * subdirectories recursively.
 * @param dir directory containing file.
 */
farray_t *farray_create_dir(char *dir)
{
    DIR *d;
    long i = 0;
    struct dirent *dp;

    farray_t *a = calloc(1, sizeof(farray_t));
    if (!a) {
        error("Could not allocate array of feature vectors");
        return NULL;
    }

    /* Allocate elements of array */
    a->len = fio_count_files(dir);    
    a->x = malloc(a->len * sizeof(fvec_t *));
    a->y = malloc(a->len * sizeof(float));
    if (!a->x || !a->y) {
        farray_destroy(a);
        error("Could not allocate elements of array");
        return NULL;
    }
    
    /* Open directory */    
    d = opendir(dir);
    if (!d) {
        error("Could not open directory '%s'", dir);
        return NULL;
    }
    
    /* Loop over directory entries */
    while ((dp = readdir(d)) != NULL) {
        if (dp->d_type != DT_REG)
            continue;
    
        /* Load file contents */
        char *x = fio_load_file(dir, dp->d_name);
        a->x[i] = fvec_create(x, strlen(x));
        free(x);    
        
        /* Extract label from name */
        /* XXX */
        
        if (verbose > 1)
            prog_bar(0, a->len, i);
            
        i++;
    }   
    
    if (verbose > 1) {
        prog_bar(0, a->len, i);
        printf("\n");
    }

    closedir(d);
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
