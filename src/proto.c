/*
 * MALHEUR - Automatic Analysis of Malware Behavior
 * Copyright (c) 2009-2010 Konrad Rieck (konrad.rieck@tu-berlin.de)
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
 * @defgroup proto Prototype extraction
 * The module contains functions for extracting prototype feature vectors.s
 * @author Konrad Rieck (konrad.rieck@tu-berlin.de)
 * @{
 */

#include "config.h"
#include "common.h"
#include "fmath.h"
#include "util.h"
#include "proto.h"

/* External variables */
extern int verbose;
extern config_t cfg;

/**
 * Extracts prototypes using an extended version of Gonzalez' algorithm.
 * @param fa Array of feature vectors
 * @param as Empty assignment structure
 * @param n Maximum number of prototypes
 * @param m Minimum distance between prototypes
 * @return Prototypes
 */
static farray_t *proto_gonzalez(farray_t *fa, assign_t *as, long n, double m)
{
    assert(fa);
    int i, j, k;

    /* Allocate prototype structure and distance arrays */
    farray_t *pr = farray_create(fa->src);
    if (!pr) {
        error("Could not allocate memory for prototype extraction");
        return NULL;
    }

    /* Init distances to maximum value */
    for (i = 0; i < fa->len; i++)
        as->dist[i] = DBL_MAX;

    /* Check for maximum number of protos */
    if (n == 0)
        n = as->len;
        
    /* Get a fixed first element */
    int fixed = farray_get_fixed(fa);
    
    /* Loop over feature vectors. First prototype: j = 0. */
    for (i = 0; i < n; i++) {
        /* Determine largest distance */
        if (i > 0)
            j = array_max(as->dist, as->len);
        else 
            j = fixed;

        /* Check for minimum distance between prototypes */
        if (as->dist[j] < m)
            break;

        /* Add prototype */
        fvec_t *pv = fvec_clone(fa->x[j]);
        farray_add(pr, pv, farray_get_label(fa, j));

        /* Update distances and assignments */
#pragma omp parallel for shared(fa, pv)
        for (k = 0; k < fa->len; k++) {
            double d = fvec_dist(pv, fa->x[k]);
            if (d < as->dist[k]) {
                as->dist[k] = d;
                as->proto[k] = pr->len - 1;
                as->label[k] = pr->y[pr->len - 1];
            }
        }

        if (verbose)
            prog_bar(0, n, i);
    }

    /* Update progress bar */
    if (verbose)
        prog_bar(0, n, n);

    return pr;
}

/**
 * Extracts a set of prototypes using the prototype algorithm.
 * Prototype algorithm is run multiple times and the smallest set
 * of prototypes is returned.
 * @param fa Array of feature vectors
 * @param as Pointer for new assignment structure
 * @return Prototypes
 */
farray_t *proto_extract(farray_t *fa, assign_t **as)
{
    assert(fa);
    farray_t *p;
    int maxnum;
    double maxdist;

    /* Get configuration */
    config_lookup_float(&cfg, "prototypes.max_dist", (double *) &maxdist);
    config_lookup_int(&cfg, "prototypes.max_num", (int *) &maxnum);

    if (verbose > 0)
        printf("Extracting prototypes with maximum distance %4.2f.\n",
               maxdist);

    /* Create assignments */
    *as = assign_create(fa);

    /* Extract prototypes */
    p = proto_gonzalez(fa, *as, maxnum, maxdist);

    if (verbose > 0)
        printf("  Done. %lu prototypes using %.2fMb extracted.\n",
               p->len, p->mem / 1e6);

    return p;
}


/**
 * Assign a set of vector to prototypes
 * @param fa Feature vectors
 * @param p Prototype vectors
 */
assign_t *proto_assign(farray_t *fa, farray_t *p)
{
    assert(fa && p);
    int i, k, j, cnt = 0;
    double d = 0;

    assign_t *c = assign_create(fa);

    if (verbose > 0)
        printf("Assigning feature vectors to %lu prototypes.\n", p->len);

#pragma omp parallel for shared(fa,c,p) private(k,j)
    for (i = 0; i < fa->len; i++) {
        double min = DBL_MAX;
        for (k = 0, j = 0; k < p->len; k++) {
            d = fvec_dist(fa->x[i], p->x[k]);
            if (d < min) {
                min = d;
                j = k;
            }
        }

        /* Compute assignments */
        c->proto[i] = j;
        c->dist[i] = min;
        c->label[i] = p->y[j];

#pragma omp critical (cnt)
        if (verbose)
            prog_bar(0, fa->len, ++cnt);
    }

    if (verbose > 0)
        printf("  Done. Assigened %lu feature vectors to %lu prototypes.\n",
               fa->len, p->len);

    return c;
}

/** @} */
