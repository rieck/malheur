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

/* Allocate memory in blocks of this size */
#define BLOCK_SIZE          (4096 / sizeof(farray_t))

/**
 * Entry for label hash table. 
 */
typedef struct {
    char *name;                  /**< Label name (key 1)*/
    int index;                   /**< Label index (key 2)*/
    UT_hash_handle hname;        /**< Uthash handle 1 */
    UT_hash_handle hindex;       /**< Uthash handle 2 */
} label_t;

/**
 * Array of feature vectors.
 */
typedef struct {
    fvec_t **x;                 /**< Array of feature vectors */
    int *y;                     /**< Array of label indices */
    unsigned long len;          /**< Length of array */
    unsigned long mem;          /**< Allocated memory in bytes */
    
    label_t *label_name;        /**< Table of label names */
    label_t *label_index;       /**< Table of label indices */
} farray_t;



/* Feature array functions */
farray_t *farray_create();
void fattay_add(farray_t *, fvec_t *, char *);
void farray_destroy(farray_t *);
void farray_print(farray_t *);
void farray_save(farray_t *, gzFile *);
farray_t *farray_load(gzFile *);

farray_t *farray_extract_dir(char *);

#endif                          /* FARRAY_H */
