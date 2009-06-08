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

#ifndef FVEC_H
#define FVEC_H

#include <zlib.h>
#include <stdint.h>

/** Data type for a feature */
typedef uint64_t feat_t;

/** Placeholder for non-initialized delimiters */
#define DELIM_NOT_INIT    42
  
/**
 * Sparse feature vector. The vector is stored as a sorted list 
 * of non-zero dimensions containing real numbers. The dimensions
 * are specified as regular indices or alternatively as 64bit
 * hash values. 
 */
typedef struct {
    feat_t *dim;            /**< List of dimensions */
    float *val;             /**< List of values */
    unsigned long len;      /**< Length of list */
    unsigned long total;    /**< Total features in sequence */
    unsigned long mem;      /**< Allocated memory in bytes */
    char *src;              /**< Source of features, e.g. file */
} fvect_t;  

/* Functions */
fvect_t *fvect_zero();
fvect_t *fvect_extract(char *, int l, char *);
void fvect_destroy(fvect_t *);
fvect_t *fvect_clone(fvect_t *);
void fvect_print(fvect_t *);
void fvect_save(fvect_t *, gzFile *);
fvect_t *fvect_load(gzFile *);
void fvect_reset_delim();
void fvect_realloc(fvect_t *);

#endif                          /* FVEC_H */
