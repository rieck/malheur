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

#ifndef CLASSIFY_H
#define CLASSIFY_H

#include "farray.h"

/**
 * Assignment structure. 
 */
typedef struct {
    unsigned int *label;        /* Predicted labels */
    unsigned int *proto;        /* Nearest prototypes */
    double *dist;               /* Distance to prototypes */
    unsigned long len;          /* Length of assign arrays */
} assign_t;

/* Functions */
assign_t *classify_apply(farray_t *, farray_t *);
farray_t *classify_get_rejected(assign_t *, farray_t *f);
assign_t *assign_create(farray_t *);
void assign_destroy(assign_t *);

#endif                          /* CLASSIFY_H */
