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
#include "proto.h"

/**
 * Clustering structure. 
 */
typedef struct {
    unsigned int *cluster;  /* Assignments of clustering */
    unsigned long len;      /* Length of assignments */
    unsigned long num;      /* Number of clusters */
    unsigned int run;       /* Runnumber of clustering */
} cluster_t;

/* Functions */
cluster_t *cluster_linkage(farray_t *, assign_t *, int);
void cluster_destroy(cluster_t *);
farray_t *cluster_prototypes(cluster_t *, assign_t *, farray_t *);
farray_t *cluster_rejected(cluster_t *, farray_t *);
char *cluster_get_name(cluster_t *c, int i);

#endif                          /* CLUSTER_H */
