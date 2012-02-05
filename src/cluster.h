/*
 * MALHEUR - Automatic Analysis of Malware Behavior
 * Copyright (c) 2009-2012 Konrad Rieck (konrad@mlsec.org)
 * University of Goettingen, Berlin Institute of Technology 
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

#include "farray.h"
#include "proto.h"

/**
 * Clustering structure. The structure holds a clustering in form of 
 * indices to clusters. Moreover, the number and the run of the clustering
 * are saved for incremental analysis.
 */
typedef struct {
    unsigned int *cluster;      /**< Assignments of clustering */
    unsigned long len;          /**< Length of assignments */
    unsigned long num;          /**< Number of clusters */
    unsigned int run;           /**< Run number of clustering */
} cluster_t;

/* Functions */
cluster_t *cluster_linkage(farray_t *, int);
void cluster_destroy(cluster_t *);
void cluster_extrapolate(cluster_t *c, assign_t *a);
void cluster_trim(cluster_t *c);

farray_t *cluster_get_prototypes(cluster_t *, assign_t *, farray_t *);
farray_t *cluster_get_rejected(cluster_t *, farray_t *);
char *cluster_get_name(cluster_t *c, int i);

#endif                          /* CLUSTER_H */
