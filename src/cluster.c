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

/**
 * @defgroup cluster Clustering functionality
 * The module contains functions for grouping feature vectors using
 * linkage clustering.
 * @author Konrad Rieck (rieck@cs.tu-berlin.de)
 * @{
 */

#include "config.h"
#include "common.h"
#include "util.h"
#include "cluster.h"

/* External variables */
extern int verbose;

/**
 * Clusters a set of feature vectors using complete-linkage clustering
 * @param a Array of feature vectors
 * @return clustering
 */
clustering_t *cluster_linkage(farray_t *fa) 
{
    assert(fa);
    
    


    return NULL;
} 

/** @} */
