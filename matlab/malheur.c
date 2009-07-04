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
#include "data.h"
#include "farray.h"
#include "fmath.h"

/* Mex signature */
#ifndef MEX_SIGNATURE
#define MEX_SIGNATURE \
    int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]
#endif   

/* Convenience definitions */
#define cmd     prhs[0]
#define in1     prhs[1]         /* Default input 1 */
#define in2     prhs[2]         /* Default input 2 */
#define in3     prhs[3]         /* Default input 3 */
#define out1    plhs[0]         /* Default output 1 */
#define out2    plhs[1]         /* Default output 2 */
#define out3    plhs[2]         /* Default output 3 */
#define out4    plhs[3]         /* Default output 4 */

/*
 * Global configuration
 */
config_t cfg;
int verbose = 1;

/*
 * Helper function to initialize scalar values 
 */
static mxArray* mxCreateScalar(double x) {
    mxArray *a = mxCreateDoubleMatrix(1, 1, mxREAL);
    double *ptr = mxGetPr(a);
    ptr[0] = x;
    return a;
}

/*
 * Print library version
 */
void mex_print_version(void)
{
    printf(" MALHEUR %s - Automatic Malware Analysis on Steroids\n"
           " Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)\n"
           " Berlin Institute of Technology (TU Berlin).\n", VERSION);
}

/**
 * Print a formated error/warning message.  
 * @param m Format string
 */
void mex_error(char *m, ...)
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
 * Load MIST reports to a cell array 
 */
void mex_load_mist(MEX_SIGNATURE)
{
    char *r, fn[1024];
    long i, len;
    mxArray *a;

    /* Check input */
    if (nrhs != 1 + 2 || nlhs != 1) 
        mex_error("Number of input/output arguments is invalid");
    if (!mxIsCell(in1))
        mex_error("First argument is not a cell array of file names");
    if (!mxIsChar(in2))
        mex_error("Second argument is not a config filename");

    /* Get input arguments */
    len = mxGetN(in1);    
    mxGetString(in2, fn, 1023);

    /* Init and load configuration */
    config_init(&cfg);
    if (config_read_file(&cfg, fn) != CONFIG_TRUE) {
        mex_error("Could not read configuration (%s in line %d)",
                  config_error_text(&cfg), config_error_line(&cfg));
    }          
    
    /* Check configuration */
    config_check(&cfg);
    if (verbose)
        config_print(&cfg);
        
    /* Get output variable */
    out1 = mxCreateCellMatrix(1, len);
    printf("Loading %d MIST reports ...\n", len);
    for (i = 0; i < len; i++) {
        /* Get file name */
        a = mxGetCell(in1, i);
        mxGetString(a, fn, 1023);

        /* Load report */
        r = data_load_file(fn, NULL);
        r = mist_preproc(r);
    
        /* Store report in cell array */
        mxSetCell(out1, i, mxCreateString(r));
        prog_bar(0, (double) len, (double) i);

        /* Free space */    
        free(r);
    }
    
    prog_bar(0.0, 1.0, 1.0);
    config_destroy(&cfg);
    
    printf("\nDone.\n");
}


/*
 * Extract features and compute pairwise dot product 
 */
void mex_dot_product(MEX_SIGNATURE)
{
    char cf[1024], df[1024];
    int i;
    mxArray *a;
    class_t *c;
    const char *fields[] = { "label", "name" };

    /* Check input */
    if (nrhs != 1 + 2 || nlhs < 1) 
        mex_error("Number of input/output arguments is invalid");
    if (!mxIsChar(in1))
        mex_error("First argument is not a dirname/archive");
    if (!mxIsChar(in2))
        mex_error("Second argument is not a config filename");

    /* Get input arguments */
    mxGetString(in1, df, 1023);
    mxGetString(in2, cf, 1023);

    /* Init and load configuration */
    config_init(&cfg);
    if (config_read_file(&cfg, cf) != CONFIG_TRUE) {
        mex_error("Could not read configuration (%s in line %d)",
                  config_error_text(&cfg), config_error_line(&cfg));
    }          
    
    /* Check configuration */
    config_check(&cfg);
    if (verbose)
        config_print(&cfg);

    /* Extract features */
    farray_t *fa = farray_extract(df);
    if (!fa)
        mex_error("Could not load data from '%s'", df);

    out1 = mxCreateNumericMatrix(fa->len, fa->len, mxDOUBLE_CLASS, mxREAL);    
    farray_dot(fa, fa, (double *) mxGetPr(out1));
    
    /* Copy labels */
    if (nlhs > 1) {
        out2 = mxCreateNumericMatrix(1, fa->len, mxDOUBLE_CLASS, mxREAL);
        double *y = mxGetPr(out2);
        for (i = 0; i < fa->len; i++)
            y[i] = fa->y[i];
    }
    
    /* Copy label names */
    if (nlhs > 2) {
        int n = HASH_CNT(hn, fa->class_name);
        out3 = mxCreateStructMatrix(1, n, 2, fields);
        for(i = 0, c = fa->class_name; c; i++, c = c->hn.next) {
            a = mxCreateScalar(c->index);
            mxSetField(out3, i, "label", a);
            a = mxCreateString(c->name);
            mxSetField(out3, i, "name", a);            
        }
    }

    /* Copy sources */
    if (nlhs > 3) {
        out4 = mxCreateCellMatrix(1, fa->len);
        for (i = 0; i < fa->len; i++) 
            if (fa->x[i]->src)
                mxSetCell(out4, i, mxCreateString(fa->x[i]->src));
    }
    
    /* Clean up */
    farray_destroy(fa);
    config_destroy(&cfg);    
    
    printf("Done.\n");
}


/*
 * Generic matlab wrapper function to Malheur functionality
 */
void mexFunction(MEX_SIGNATURE)
{
    char buf[256];

    if (nrhs == 0) {
        mex_print_version();
        return;
    }

    /* Get command */
    if (mxGetString(cmd, buf, 255)) 
        mex_error("Invalid Malheur command");

    /* Process commands */    
    if (!strcasecmp(buf, "version")) {
        mex_print_version();           
    } else if (!strcasecmp(buf, "load_mist")) {
        mex_load_mist(nlhs, plhs, nrhs, prhs);
    } else if (!strcasecmp(buf, "dot_product")) {
        mex_dot_product(nlhs, plhs, nrhs, prhs);
    } else {
        mex_error("Unknown Malheur command");
    }
}
