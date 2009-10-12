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

#ifndef CLUSTER_H
#define CLUSTER_H

#include "uthash.h"
#include "farray.h"

/* Clustering structure */
typedef struct {
    unsigned int *cluster;  /* Assignments of clustering */
    unsigned long len;      /* Length of assignments */
    unsigned long num;      /* Number of clusters */
} cluster_t;

/* Functions */
cluster_t *cluster_linkage(farray_t *);
void cluster_destroy(cluster_t *);
void cluster_extrapolate(cluster_t *c, farray_t *pr, farray_t *fa);
void cluster_trim(cluster_t *c);

#endif                          /* CLUSTER_H */
