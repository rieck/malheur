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

/* Mask for assignments of prototypes */
#define PA_ASSIGN_MASK       0x7FFFFFFF
#define PA_PROTO_MASK        ~PA_ASSIGN_MASK

/**
 * Prototype structure. The 'protos' field holds the feature vectors
 * of the extracted prototypes. The field 'assign' contains the 
 * assignments of the original data points to the prototypes, where 
 * the most significant bit indicates the prototypes themselves. 
 */
typedef struct {
    farray_t *protos;          /* Array of prototype vectors */
    unsigned int *assign;      /* Assignments of prototypes */
    unsigned long alen;        /* Length of assign arrays */
    double avg_dist;           /* Average distance to prototypes */
} proto_t;

/* Function declarations */
proto_t *proto_extract(farray_t *);
void proto_destroy(proto_t *);
proto_t *proto_load(gzFile *);
proto_t *proto_load_file(char *);
void proto_save(proto_t *, gzFile *);
void proto_save_file(proto_t *, char *);
void proto_print(proto_t *);

#endif                          /* PROTO_H */
