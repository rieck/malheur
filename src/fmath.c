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

#include "config.h"
#include "common.h"
#include "fvec.h"
#include "fmath.h"
#include "util.h"

/* External variables */
extern int verbose;

/**
 * Normalize a feature vector to a particular norm value.
 * @param fv Feature vector 
 * @param n Normalization
 */
void fvec_normalize(fvec_t *fv, norm_t n)
{
    int i = 0;
    double s = 0;
    assert(fv);

    switch (n) {
    case NORM_BIN:
        for (i = 0; i < fv->len; i++)
            fv->val[i] = 1;
        break;
    case NORM_L1:
        s = fvec_norm1(fv);
        for (i = 0; i < fv->len; i++)
            fv->val[i] /= s;
        break;
    case NORM_L2:
        s = fvec_norm2(fv);
        for (i = 0; i < fv->len; i++)
            fv->val[i] /= s;
        break;
    }
}

/**
 * Multiplies vector with a scalar
 * @param fv Feature vector 
 * @param s Scalar value
 */
void fvec_mul(fvec_t *fv, double s)
{
    int i = 0;
    assert(fv);

    for (i = 0; i < fv->len; i++)
        fv->val[i] *= s;
}

/** 
 * Adds two feature vectors and create a new one (c = a + b * s)
 * @param fa Feature vector (a)
 * @param fb Feature vector (b)
 * @param s Scalar value
 * @return new feature vector
 */
fvec_t *fvec_adds(fvec_t *fa, fvec_t *fb, double s) 
{
    unsigned long i = 0, j = 0, len = 0;
    assert(fa && fb);
    fvec_t *f;
    
    /* Allocate feature vector (zero'd) */
    f = malloc(sizeof(fvec_t));
    if (!f) {
        error("Could not create feature vector.");
        return NULL;
    }

    /* Allocate arrays */
    f->dim = (feat_t *) malloc((fa->len + fb->len) * sizeof(feat_t));
    f->val = (float *) malloc((fa->len + fb->len) * sizeof(float));
    if (!f->dim || !f->val) {
        error("Could not allocate feature vector contents.");
        fvec_destroy(f);
        return NULL;
    }

    /* Loop over features in a and b */
    while (i < fa->len || j < fb->len) {
        if (i >= fa->len || fa->dim[i] > fb->dim[j]) {
            f->dim[len] = fb->dim[j];
            f->val[len++] = fb->val[j++] * s;
        } else if (j >= fb->len || fa->dim[i] < fb->dim[j]) {
            f->dim[len] = fa->dim[i];
            f->val[len++] = fa->val[i++];
        } else {
            f->dim[len] = fa->dim[i];
            f->val[len++] = fa->val[i++] + fb->val[j++] * s;
        }
    }

    /* Set new length and reallocate */
    f->len = len;
    fvec_shrink(f);
    
    return f;
}

/**
 * Computes the l1-norm of the feature vector
 * @param fv Feature vector 
 * @return sum of values 
 */
double fvec_norm1(fvec_t *fv)
{
    int i = 0;
    double s = 0;    
    assert(fv);

    for (i = 0; i < fv->len; i++)
        s += fv->val[i];
    
    return s;
}

/**
 * Computes the l2-norm of the feature vector
 * @param fv Feature vector 
 * @return sum of values 
 */
double fvec_norm2(fvec_t *fv)
{
    int i = 0;
    double s = 0;
    assert(fv);

    for (i = 0; i < fv->len; i++)
        s += pow(fv->val[i], 2);
    
    return sqrt(s);
}