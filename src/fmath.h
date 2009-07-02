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

#ifndef FMATH_H
#define FMATH_H

#include "fvect.h"
#include "farray.h"

/** Normalization types for feature vectors */
typedef enum {
    NORM_L1, NORM_L2
} norm_t;

/** Embedding types for feature vectors */
typedef enum {
    EMBED_BIN, EMBED_CNT
} embed_t;

/* Functions */
void fvect_mul(fvect_t *, double);
void fved_div(fvect_t *, double);
fvect_t *fvect_adds(fvect_t *, fvect_t *, double);
fvect_t *fvect_add(fvect_t *, fvect_t *); 
fvect_t *fvect_sub(fvect_t *, fvect_t *);
fvect_t *farray_sums(farray_t *, double *);
fvect_t *farray_sum(farray_t *);
fvect_t *farray_mean(farray_t *);
double fvect_dot(fvect_t *, fvect_t *);
double fvect_norm1(fvect_t *);
double fvect_norm2(fvect_t *);
void fvect_normalize(fvect_t *, norm_t);
void fvect_sparsify(fvect_t *);
void farray_dot(farray_t *, farray_t *, double *);
void farray_normalize(farray_t *f, norm_t n);

#endif                          /* FMATH_H */
