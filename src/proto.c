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

/* Local functions */
static proto_t *proto_create(farray_t *fa);

/**
 * Creates an empty structure of prototypes
 * @param a Array of feature vectors
 * @return Prototypes
 */
static proto_t *proto_create(farray_t *fa)
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
    p->dist = calloc(1, fa->len * sizeof(float));
    p->assign = calloc(1, fa->len * sizeof(int));
    p->len = fa->len;
    if (!p->protos || !p->dist || !p->assign) {
        error("Could not allocate prototype assignments");
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
 * Extracts a set of prototypes
 * @param a Array of feature vectors
 * @return Prototypes
 */
proto_t *proto_extract(farray_t *fa) 
{
    assert(fa);
    int i, j, k, num, far;
    double ratio, outl;
    float *ds;

    /* Allocate prototype structure */
    proto_t *p = proto_create(fa);
    if (!p)
        return NULL;
       
    /* Get prototype ratio */
    config_lookup_float(&cfg, "prototypes.ratio", (double *) &ratio);
    num = round(ratio * fa->len);
    if (num <= 0 || num > fa->len) {
        error("Prototype ratio %f invalid (too low/high)", ratio);
        proto_destroy(p);
        return NULL;
    }

    /* Get prototype percentile */
    config_lookup_float(&cfg, "prototypes.outliers", (double *) &outl);
    far = round((1 - outl) * fa->len);
    if (far < 0 || far >= fa->len) {
        error("Outlier amount %f invalid (too low/high)", outl);
        proto_destroy(p);
        return NULL;
    }

    /* Array for sorting */
    ds = malloc(fa->len * sizeof(float));
    
    if (verbose > 0)
        printf("Prototyping feature vectors with %d prototypes "
               "and %1.0f%% outliers.\n", num, outl * 100);

    for (i = 0; i < num; i++) {
        if (i == 0) {
            /* Select random prototype */
            j = rand() % fa->len;
        } else {
            /* Select farthest prototype (excluding outliers) */
            memcpy(ds, p->dist, fa->len * sizeof(float));
            qsort(ds, fa->len, sizeof(float), cmp_float);
            for (j = 0; j < fa->len && p->dist[j] != ds[far]; j++);            
        }

        /* Add prototype */
        fvec_t *pv = fvec_clone(fa->x[j]);
        farray_add(p->protos, pv, farray_get_label(fa, j));

        /* Update distances and assignments */
        #pragma omp parallel for shared(fa, pv, p)        
        for (k = 0; k < fa->len; k++) {
            float d = sqrt(2 - 2 * fvec_dot(pv, fa->x[k]));
            if (d < p->dist[k]) {
                p->dist[k] = d;
                p->assign[k] = i;
            }
        }
        
        if (verbose > 0)
            prog_bar(0, num - 1, i);
    }
        
    /* Free memory */
    free(ds); 
    
    if (verbose > 0)
        printf("\n");
    if (verbose > 1)
        farray_print(p->protos);

    return p;
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
    free(p);
}

/** @} */
