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
#include "fvect.h"

/* Allocate memory in blocks of this size */
#define BLOCK_SIZE          (4096 / sizeof(farray_t))

/**
 * Entry for class hash table. 
 */
typedef struct {
    char name[64];               /**< Class name (key 1)*/
    unsigned int index;          /**< Class index (key 2)*/
    UT_hash_handle hn;           /**< Uthash handle 1 */
    UT_hash_handle hi;           /**< Uthash handle 2 */
} class_t;

/**
 * Array of feature vectors.
 */
typedef struct {
    fvect_t **x;                 /**< Array of feature vectors */
    int *y;                     /**< Array of label indices */
    unsigned long len;          /**< Length of array */
    unsigned long mem;          /**< Allocated memory in bytes */
    
    class_t *class_name;        /**< Table of class names */
    class_t *class_index;       /**< Table of class indices */
    char *src;                  /**< Source of array, e.g. dir */
} farray_t;


/* Feature array functions */
farray_t *farray_create(char *);
void farray_add(farray_t *, fvect_t *, char *);
void farray_destroy(farray_t *);
void farray_print(farray_t *);
void farray_save(farray_t *, gzFile *);
farray_t *farray_load(gzFile *);
farray_t *farray_extract(char *path);
farray_t *farray_extract_dir(char *);
farray_t *farray_extract_archive(char *);

#endif                          /* FARRAY_H */
