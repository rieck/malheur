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

    /* Allocate prototype structure */
    proto_t *p = malloc(sizeof(proto_t));    
    if (!p) {
        error("Could not allocate prototype structure");
        return NULL;
    }

    /* Allocate structure fields */
    p->protos = farray_create(fa->src);
    p->assign = calloc(fa->len, sizeof(uint32_t));
    p->len = fa->len;
    
    if (!p->protos || !p->assign) {
        error("Could not allocate prototype structure");
        proto_destroy(p);
        return NULL;
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
    float *ds, *di;

    /* Get prototype ratio */
    config_lookup_float(&cfg, "prototypes.ratio", (double *) &ratio);
    n = check_range(round(ratio * fa->len), 1, fa->len);
    
    /* Get prototype quantile */
    config_lookup_float(&cfg, "prototypes.outliers", (double *) &outl);
    p = check_range(round((1 - outl) * fa->len), 0, fa->len - 1);

    /* Allocate prototype structure and distance arrays */
    proto_t *pr = proto_create(fa, n);
    ds = malloc(fa->len * sizeof(float));
    di = malloc(fa->len * sizeof(float));
    if (!pr || !ds || !di) {
        error("Could not allocate memory for prototype extraction");
        return NULL;
    }

    /* Init distances to maximum value */
    for (i = 0; i < fa->len; i++)
        di[i] = DBL_MAX;    
    
    if (verbose > 0)
        printf("Extracting %d prototypes from feature vectors with"
               " %1.0f%% outliers.\n", n, outl * 100);

    for (i = 0; i < n; i++) {
        if (i == 0) {
            /* Select random prototype */
            j = rand() % fa->len;
        } else {
            /* Select p-quantile prototype */
            memcpy(ds, di, fa->len * sizeof(float));
            qsort(ds, fa->len, sizeof(float), cmp_float);
            for (j = 0; j < fa->len && di[j] != ds[p]; j++);            
        }

        /* Add prototype */
        fvec_t *pv = fvec_clone(fa->x[j]);
        farray_add(pr->protos, pv, farray_get_label(fa, j));

        /* Update distances and assignments */
        #pragma omp parallel for shared(fa, pv, p)        
        for (k = 0; k < fa->len; k++) {
            float d = sqrt(2 - 2 * fvec_dot(pv, fa->x[k]));
            if (d < di[k]) {
                di[k] = d;
                /* Store assignments, MSB indicates prototype itself */
                pr->assign[k] = i & PA_ASSIGN_MASK;
                pr->assign[k] |= k == j ? PA_PROTO_MASK : 0;
            }
        }
        
        if (verbose > 0)
            prog_bar(0, n - 1, i);
    }
        
    /* Free memory */
    free(ds); 
    free(di);
    
    return pr;
} 

/**
 * Destroys a structure containing prototypes and frees its memory. 
 * @param p Prototypes
 */
void proto_destroy(proto_t *p)
{
    if (!p)
        return;
    if (p->protos)
        farray_destroy(p->protos);        
    if (p->assign)
        free(p->assign);
    free(p);
}

/**
 * Prints a structure containing prototypes
 * @param p Prototypes
 */
void proto_print(proto_t *p)
{
    /* FIXME */
} 

/**
 * Saves a structure of prototype to a file stram
 * @param p Prototypes
 * @param z Stream pointer
 */
void proto_save(proto_t *p, gzFile *z)
{
    /* FIXME */
}

/**
 * Loads a structure of prototype from a file stram
 * @param z Stream pointer
 * @return Prototypes
 */
proto_t *proto_load(gzFile *z)
{
    /* FIXME */
    return NULL;
}

/** @} */
