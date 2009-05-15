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
 * Computes the embedding from a feature vector of counts
 * @param fv Feature vector 
 * @param n Normalization
 */
void fvec_norm(fvec_t *fv, norm_t n)
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
        for (i = 0; i < fv->len; i++)
            s += fv->val[i];
        for (i = 0; i < fv->len; i++)
            fv->val[i] /= s;
        break;
    case NORM_L2:
        for (i = 0; i < fv->len; i++)
            s += fv->val[i] * fv->val[i];
        for (i = 0; i < fv->len; i++)
            fv->val[i] /= sqrt(s);
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