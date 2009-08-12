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

#include "farray.h"

typedef struct {
    farray_t *protos;       /* Array of prototype vectors */
    int *indices;           /* Indices of prototypes (length: protos->len)*/
    int *assign;            /* Assignments of prototypes (length: len)*/
    float *dist;            /* Distances to prototypes (length: len) */
    unsigned long len;      /* Length of assign and dist arrays */
} proto_t;

/* Function declarations */
proto_t *proto_extract(farray_t *);
void proto_destroy(proto_t *);
proto_t *proto_load(gzFile *);
void proto_save(proto_t *, gzFile *);
void proto_print(proto_t *);

#endif                          /* PROTO_H */
