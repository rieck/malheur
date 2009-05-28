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

#include "fvec.h"

/** Normalization types for feature vectors */
typedef enum {
    NORM_BIN, NORM_L1, NORM_L2
} norm_t;

/* Functions */
void fvec_mul(fvec_t *, double);
void fved_div(fvec_t *, double);
fvec_t *fvec_adds(fvec_t *, fvec_t *, double);
fvec_t *fvec_add(fvec_t *, fvec_t *); 
fvec_t *fvec_sub(fvec_t *, fvec_t *);
double fvec_norm1(fvec_t *fv);
double fvec_norm2(fvec_t *fv);
void fvec_normalize(fvec_t *, norm_t);

#endif                          /* FMATH_H */
