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

/* Macro definitions (hacks) */
#define fvec_div(f,s)           fvec_mul(f,1.0/s)

/* Functions */
void fvec_norm(fvec_t *, norm_t);
void fvec_mul(fvec_t *, double);

#endif                          /* FMATH_H */
