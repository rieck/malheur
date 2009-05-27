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

#ifndef FARRAY_H
#define FARRAY_H

#include "zlib.h"
#include "uthash.h"
#include "fvec.h"

/**
 * Array of feature vectors.
 */
typedef struct {
    fvec_t **x;             /**< Array of feature vectors (1 x n) */
    int *y;                 /**< Array of labels (1 x n)*/
    unsigned long len;      /**< Length of array (n) */
    unsigned long mem;      /**< Allocated memory */
    char **labels;          /**< Malware labels (null terminated) */    
} farray_t;

/**
 * Helper for hash table of labels
 */
typedef struct {
    char *name;             /**< Label name */
    int index;               /**< Label number */
    UT_hash_handle hh;      /**< Uthash handle */
} label_t;

/* Feature array functions */
farray_t *farray_extract_dir(char *);
void farray_destroy(farray_t *);
void farray_print(farray_t *);
void farray_save(farray_t *, gzFile *);
farray_t *farray_load(gzFile *);

#endif                          /* FARRAY_H */
