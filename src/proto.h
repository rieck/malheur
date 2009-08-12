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

#ifndef PROTO_H
#define PROTO_H

#include <stdint.h>
#include "farray.h"

/* Mask for assignments of prototypes */
#define PA_ASSIGN_MASK       0x7FFFFFFF
#define PA_PROTO_MASK        0x80000000

/**
 * Prototype structure. The 'protos' field holds the feature vectors
 * of the extracted prototypes. The field 'assign' contains the 
 * assignments of the original data points to the prototypes, where 
 * the most significant bit indicates the prototypes themselves. 
 */
typedef struct {
    farray_t *protos;          /* Array of prototype vectors */
    uint32_t *assign;          /* Assignments of prototypes */
    unsigned long len;         /* Length of assign arrays */
} proto_t;

/* Function declarations */
proto_t *proto_extract(farray_t *);
void proto_destroy(proto_t *);
proto_t *proto_load(gzFile *);
void proto_save(proto_t *, gzFile *);
void proto_print(proto_t *);

#endif                          /* PROTO_H */
