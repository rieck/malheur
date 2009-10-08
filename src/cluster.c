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
#include "fmath.h"

/* External variables */
extern int verbose;
extern config_t cfg;

/* Simple macro distance matrix */
#define D(x,y)      dist[x * fa->len + y]

/**
 * Linkage clustering algorithm by Mutargh. The algorithm has a 
 * worst-case run-time of O(n^3) but usually runs in O(n^2). Note
 * that in the generic case linkage clustering has a worst-case 
 * time complexity of O(n^2 log n).
 * @param fa Feature vectors
 * @param c Clustering structure
 * @param dm Minimum distance
 */
static void linkage_murtagh(farray_t *fa, cluster_t *c, double *dist, double dm)
{
    double dmin; 
    long k, j, i, jj, jm, ii, im;

    /* Allocate stuff */
    char *done = calloc(1, sizeof(char) * fa->len);
    unsigned long *nn = malloc(sizeof(unsigned long) * fa->len);
    double *dnn = malloc(sizeof(double) * fa->len);
    
    if (!done || !nn || !dnn) {
        error("Could not allocate memory for clustering algorithm.");
        goto err;
    }
    
    /* Main loop */
    for (k = 0; k < fa->len - 1; k++) {
        #pragma omp parallel for default(shared) private(dmin, jj)
        for (i = 0; i < fa->len; i++) {
            if (done[i])
                continue;
            if (k > 0 && (nn[i] != im && nn[i] != jm))
                continue;
            dmin = DBL_MAX, jj = 0;
            for (j = i + 1; j < fa->len; j++) {
                if (done[j])
                    continue;
                if (D(i,j) >= dmin) 
                    continue;
                dmin = D(i,j), jj = j;
            }
            dnn[i] = dmin, nn[i] = jj;
        }
    
        /* Determine minimum */
        dmin = DBL_MAX, im = 0;
        for (i = 0; i < fa->len; i++) {
            if (done[i])
                continue;
            if (dnn[i] >= dmin) 
                continue;
            dmin = dnn[i], im = i;
        }
        jm = nn[im];
        
        if (dmin > dm)
            break;

        /* Update cluster assignments */
        for (i = 0; i < fa->len; i++)
            if (c->cluster[i] == jm)
                c->cluster[i] = im;
        done[jm] = TRUE;
        c->num--;
        
        /* Update distance matrix */
        dmin = DBL_MAX, ii = 0;
        for (i = 0; i < fa->len; i++) {
            if (done[i])
                continue;
            if (i == im)
                continue;
            D(im, k) = fmax(D(im, k), D(jm, k));
            D(k, im) = D(im, k);
            
            if (D(im, k) >= dmin)
                continue;
            dmin = D(im,k), ii = i;
        }
        dnn[im] = dmin;
        nn[im] = ii;
        
        if (verbose) 
            prog_bar(0, fa->len - 1, k);
    }

#if 0    
    for (i = 0; i < fa->len; i++)
        printf("%d ", c->cluster[i]);
    printf("\n");
#endif    
    
err:    
    /* Free remaining arrays */
    free(done);
    free(nn);
    free(dnn);
}

/**
 * Clusters a set of feature vectors using linkage clustering
 * @param a Array of feature vectors
 * @return clustering
 */
cluster_t *cluster_linkage(farray_t *fa) 
{
    assert(fa);
    double mindist;
    long i, reject;
    
    /* Get cluster configuration */
    config_lookup_float(&cfg, "cluster.min_dist", (double *) &mindist);
    config_lookup_int(&cfg, "cluster.reject_num", (long *) &reject);
    
    /* Allocate cluster structure */
    cluster_t *c = calloc(1, sizeof(cluster_t));
    c->cluster = malloc(sizeof(int) * fa->len);
    if (!c || !c->cluster) {
        error("Could not allocate cluster structure");
        cluster_destroy(c);
        return NULL;
    }

    /* Initialize cluster assignements */
    c->num = fa->len;
    for (i = 0; i < fa->len; i++)
        c->cluster[i] = i + 1;

    double *dist = malloc(sizeof(double) * fa->len * fa->len);
    /* Compute distances */
    farray_dist(fa, fa, dist); 

    if (verbose > 0) 
        printf("Clustering prototypes with minimum distance %4.2f "
               "and %ld rejection threshold.\n", mindist, reject);
    
    /* Run clustering */
    linkage_murtagh(fa, c, dist, mindist);

    if (verbose > 0) {
        prog_bar(0, 1, 1);
        printf("  Done. %ld clusters determined.\n", c->num);
    }

    free(dist);
    return c;
} 

/**
 * Destroy a cluster structure
 * @param  c Cluster structure
 */
void cluster_destroy(cluster_t *c)
{
    if (!c)
        return;
        
    if (c->cluster)
        free(c->cluster);
    free(c);
} 

/** @} */
