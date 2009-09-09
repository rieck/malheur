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

#ifndef CLASS_H
#define CLASS_H

#include "farray.h"

/**
 * Classification structure. 
 */
typedef struct {
    unsigned int *label;        /* Predictied labels */
    unsigned int *proto;        /* Nearest prototypes */
    double *dist;               /* Distance to prototypes */
    unsigned long len;          /* Length of assign arrays */
} class_t;

/* Functions */
void class_destroy(class_t *c);
class_t *class_predict(farray_t *fa, proto_t *p);

#endif                          /* CLASS_H */
