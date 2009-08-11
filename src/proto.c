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
 * @defgroup proto Extraction of prototypes
 * The module contains functions for extracting prototypical feature
 * vectors 
 * @author Konrad Rieck (rieck@cs.tu-berlin.de)
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
 * Creates an empty structure of prototypes
 * @param a Array of feature vectors
 * @param n Number of prototypes
 * @return Prototypes
 */
static proto_t *proto_create(farray_t *fa, int n)
{
    assert(fa);
    int i;

    /* Allocate prototype structure */
    proto_t *p = malloc(sizeof(proto_t));    
    if (!p) {
        error("Could not allocate prototype structure");
        return NULL;
    }

    /* Allocate structure fields */
    p->protos = farray_create(fa->src);
    p->dist = calloc(fa->len, sizeof(float));
    p->assign = calloc(fa->len, sizeof(int));
    p->indices = calloc(n, sizeof(int));
    p->len = fa->len;
    
    if (!p->protos || !p->dist || !p->assign || !p->indices) {
        error("Could not allocate prototype structure");
        proto_destroy(p);
        return NULL;
    }
    
    /* Create initial assignment */
    for (i = 0; i < fa->len; i++) {
        p->assign[i] = -1;
        p->dist[i] = DBL_MAX;
    }
    
    return p;
}

/**
 * Extracts a set of prototypes using the quantile prototype algorithm.
 * @param a Array of feature vectors
 * @return Prototypes
 */
proto_t *proto_extract(farray_t *fa) 
{
    assert(fa);
    int i, j, k, p, n;
    double ratio, outl;
    float *ds;

    /* Get prototype ratio */
    config_lookup_float(&cfg, "prototypes.ratio", (double *) &ratio);
    n = round(ratio * fa->len);
    n = n < 1 ? 1 : n;
    n = n > fa->len ? fa->len : n;

    /* Get prototype quantile */
    config_lookup_float(&cfg, "prototypes.outliers", (double *) &outl);
    p = round((1 - outl) * fa->len);
    p = p < 0 ? 0 : p;
    p = p > fa->len - 1 ? fa->len - 1 : p;

    /* Allocate prototype structure */
    proto_t *pr = proto_create(fa, n);
    if (!pr)
        return NULL;
       
    /* Array for sorting */
    ds = malloc(fa->len * sizeof(float));
    
    if (verbose > 0)
        printf("Prototyping feature vectors with %d prototypes "
               "and %1.0f%% outliers.\n", n, outl * 100);

    for (i = 0; i < n; i++) {
        if (i == 0) {
            /* Select random prototype */
            j = rand() % fa->len;
        } else {
            /* Select p-quantile prototype */
            memcpy(ds, pr->dist, fa->len * sizeof(float));
            qsort(ds, fa->len, sizeof(float), cmp_float);
            for (j = 0; j < fa->len && pr->dist[j] != ds[p]; j++);            
        }

        /* Add prototype */
        fvec_t *pv = fvec_clone(fa->x[j]);
        farray_add(pr->protos, pv, farray_get_label(fa, j));
        pr->indices[i] = j;

        /* Update distances and assignments */
        #pragma omp parallel for shared(fa, pv, p)        
        for (k = 0; k < fa->len; k++) {
            float d = sqrt(2 - 2 * fvec_dot(pv, fa->x[k]));
            if (d < pr->dist[k]) {
                pr->dist[k] = d;
                pr->assign[k] = i;
            }
        }
        
        if (verbose > 0)
            prog_bar(0, n - 1, i);
    }
        
    /* Free memory */
    free(ds); 
    
    if (verbose > 1)
        farray_print(pr->protos);

    return pr;
} 

/**
 * Destroys a structure containing prototypes and frees its memory. 
 * @param p PrototypeS
 */
void proto_destroy(proto_t *p)
{
    if (!p)
        return;
    if (p->protos)
        farray_destroy(p->protos);        
    if (p->assign)
        free(p->assign);
    if (p->dist)
        free(p->dist);
    if (p->indices)
        free(p->indices);
    free(p);
}

/** @} */
