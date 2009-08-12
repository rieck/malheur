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

#include <mex.h>
#include <stdarg.h>
#include <string.h>
#include <libconfig.h>
#include <strings.h>

#include "mist.h"
#include "mconfig.h"
#include "uthash.h"
#include "util.h"
#include "farray.h"
#include "proto.h"
#include "malmex.h"

/*
 * Global configuration
 */
extern config_t cfg;

/*
 * Helper function to initialize scalar values 
 */
mxArray* mxCreateScalar(double x) {
    mxArray *a = mxCreateDoubleMatrix(1, 1, mxREAL);
    double *ptr = mxGetPr(a);
    ptr[0] = x;
    return a;
}

/**
 * Print a formated error/warning message.  
 * @param m Format string
 */
void mal_error(char *m, ...)
{
    va_list ap;
    char s[256] = { " " };

    config_destroy(&cfg);

    va_start(ap, m);
    vsnprintf(s, 256, m, ap);
    va_end(ap);

    mexErrMsgIdAndTxt("malheur:generic", s);
}

/*
 * Create a data struct from a given array of feature vectors
 */
mxArray *mal_data_struct(farray_t *fa) 
{
    const char *fields1[] = { "labels", "names", "sources" };
    const char *fields2[] = { "label", "name" };
    mxArray *a, *b, *data;
    double *f;
    int i;
    label_t *c;
    
    data = mxCreateStructMatrix(1, 1, 3, fields1);

    /* Copy labels */
    a = mxCreateNumericMatrix(1, fa->len, mxDOUBLE_CLASS, mxREAL);
    f = mxGetPr(a);
    for (i = 0; i < fa->len; i++)
        f[i] = fa->y[i];
    mxSetField(data, 0, "labels", a);
    
    /* Copy label names */
    int n = HASH_CNT(hn, fa->label_name);
    a = mxCreateStructMatrix(1, n, 2, fields2);
    for(i = 0, c = fa->label_name; c; i++, c = c->hn.next) {
        b = mxCreateScalar(c->index);
        mxSetField(a, i, "label", b);
        b = mxCreateString(c->name);
        mxSetField(a, i, "name", b);            
    }
    mxSetField(data, 0, "names", a);

    /* Copy sources */
    a = mxCreateCellMatrix(1, fa->len);
    for (i = 0; i < fa->len; i++) 
        if (fa->x[i]->src)
            mxSetCell(a, i, mxCreateString(fa->x[i]->src));
    mxSetField(data, 0, "sources", a);
    
    return data;
}

/*
 * Create a prototype struct
 */
mxArray *mal_proto_struct(proto_t *p) 
{
    const char *fields[] = { "indices", "assign" };
    mxArray *a, *proto;
    double *f;
    int i, j;
    
    proto = mxCreateStructMatrix(1, 1, 3, fields);

    /* Copy prototype indices */
    a = mxCreateNumericMatrix(1, p->protos->len, mxDOUBLE_CLASS, mxREAL);
    f = mxGetPr(a);
    for (i = j = 0; i < p->len; i++) {
        if (p->assign[i] & PA_PROTO_MASK)
            f[j++] = i;
    }
    mxSetField(proto, 0, "indices", a);
    
    /* Copy prototype assignments */
    a = mxCreateNumericMatrix(1, p->len, mxDOUBLE_CLASS, mxREAL);
    f = mxGetPr(a);
    for (i = 0; i < p->len; i++)
        f[i] = p->assign[i] & PA_ASSIGN_MASK;
    mxSetField(proto, 0, "assign", a);
    
    return proto;
}   
