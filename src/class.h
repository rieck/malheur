/*
 * MALHEUR - Automatic Analysis of Malware Behavior
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
 * Assignment structure. Assignments are used to either associate 
 * feature vectors to prototypes or to assign labels to feature 
 * vectors. 
 */
typedef struct {
    unsigned int *label;        /**< Predicted labels */
    unsigned int *proto;        /**< Nearest prototypes */
    double *dist;               /**< Distance to prototypes */
    unsigned long len;          /**< Length of assign arrays */
} assign_t;

/* Functions */
assign_t *class_assign(farray_t *, farray_t *);
farray_t *class_get_rejected(assign_t *, farray_t *f);
assign_t *assign_create(farray_t *);
void assign_destroy(assign_t *);

#endif                          /* CLASSIFY_H */
