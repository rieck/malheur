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

/* Macro for convenient matrix access */
#define D(x,y)      d[tria_pos(x,y,c->len)]

/**
 * Linkage clustering algorithm by Mutargh. The algorithm has a 
 * worst-case run-time of O(n^3) but usually runs in O(n^2). Note
 * that in the generic case linkage clustering has a worst-case 
 * time complexity of O(n^2 log n).
 * @param c Clustering structure
 * @param d Minimum distance
 * @param m Clustering mode
 */
static 
void linkage_murtagh(cluster_t *c, double *d, double dm, char m)
{
    assert(c && d);
    double dmin, dnew; 
    long k, j, i, jj, jm, ii, im;

    /* Allocate stuff */
    char *done = calloc(1, sizeof(char) * c->len);
    unsigned long *nn = malloc(sizeof(unsigned long) * c->len);
    double *dnn = malloc(sizeof(double) * c->len);
    if (!done || !nn || !dnn) {
        error("Could not allocate memory for clustering algorithm.");
        goto err;
    }
    
    /* Main loop */
    for (k = 0; k < c->len - 1; k++) {
        /* Update nearest neighbors for each point */
        #pragma omp parallel for default(shared) private(dmin, jj)
        for (i = 0; i < c->len; i++) {
            if (done[i] || (k > 0 && (nn[i] != im && nn[i] != jm)))
                continue;
            dmin = DBL_MAX, jj = 0;
            for (j = i + 1; j < c->len; j++) {
                if (done[j] || D(i,j) >= dmin) 
                    continue;
                dmin = D(i,j), jj = j;
            }
            dnn[i] = dmin, nn[i] = jj;
        }
    
        /* Determine smalled distance */
        dmin = DBL_MAX, im = 0;
        for (i = 0; i < c->len; i++) {
            if (done[i] || dnn[i] >= dmin) 
                continue;
            dmin = dnn[i], im = i;
        }
        jm = nn[im];
        
        /* Check for minimum distance */
        if (dmin > dm)
            break;

        /* Update */
        done[jm] = TRUE;
        c->num--;
        
        /* Update clusters and distance matrix */
        int cm = c->cluster[jm];
        #pragma omp parallel for default(shared) private(dnew)
        for (i = 0; i < c->len; i++) {
            /* Update cluster assignments */
            if (c->cluster[i] == cm)
                c->cluster[i] = c->cluster[im];
            if (done[i] || i == im)
                continue;
            
            switch(m) {
                /* Single linkage */
                case 's':
                    dnew = fmin(D(im, i), D(jm, i));
                    break;
                /* Average linkage */
                case 'a':
                    dnew = (D(im, i) + D(jm, i)) / 2;
                    break;
                /* Complete linkage */
                default:
                case 'c':
                    dnew = fmax(D(im, i), D(jm, i));
                    break;
            }
            d[tria_pos(i,im,c->len)] = dnew;
        }
        
        /* Update nearest neighbors */
        dmin = DBL_MAX, ii = 0;
        for (i = 0; i < c->len; i++) {
            if (done[i] || i == m || D(im, i) >= dmin)
                continue;
            dmin = D(im, i), ii = i;
        }
        dnn[im] = dmin;
        nn[im] = ii;
        
        if (verbose) 
            prog_bar(0, c->len - 1, k);
    }
    
err:    
    /* Free remaining arrays */
    free(done);
    free(nn);
    free(dnn);
}

/**
 * Initializes an empty clustering 
 * @param n Number of points
 * @return clustering structure
 */
static cluster_t *cluster_create(int n)
{
    int i;

    /* Allocate cluster structure */
    cluster_t *c = calloc(1, sizeof(cluster_t));
    if (!c) {
        error("Could not allocate cluster structure");
        return NULL;
    }

    /* Allocate cluster assignments */
    c->cluster = malloc(sizeof(int) * n);
    if (!c->cluster) {
        error("Could not allocate cluster assignments");
        cluster_destroy(c);
        return NULL;
    }

    /* Initialize cluster assignements */
    c->num = n;
    c->len = n;
    for (i = 0; i < n; i++)
        c->cluster[i] = i + 1;
        
    return c;
}

/**
 * Clusters a set of feature vectors using linkage clustering
 * @param a Array of feature vectors
 * @return clustering
 */
cluster_t *cluster_linkage(farray_t *fa) 
{
    assert(fa);
    double dmin;
    long rej;
    const char *mode;
    
    /* Get cluster configuration */
    config_lookup_float(&cfg, "cluster.min_dist", (double *) &dmin);
    config_lookup_int(&cfg, "cluster.reject_num", (long *) &rej);
    config_lookup_string(&cfg, "cluster.link_mode", &mode);

    /* Allocate cluster structure */
    cluster_t *c = cluster_create(fa->len);

    /* Compute distances */
    double *dist = malloc(sizeof(double) * tria_size(fa->len));
    farray_dist_tria(fa, dist); 

    if (verbose > 0) 
        printf("Clustering (%s linkage) with minimum distance %4.2f.\n",
               mode, dmin);
    
    /* Run clustering */
    linkage_murtagh(c, dist, dmin, mode[0]);

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
