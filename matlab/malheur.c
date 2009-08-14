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
#include "util.h"
#include "fmath.h"
#include "proto.h"
#include "malmex.h"

/* Mex signature */
#ifndef MEX_SIGNATURE
#define MEX_SIGNATURE \
    int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]
#endif   

/* Convenience definitions: Input */
#define cmd     prhs[0]
#define in1     prhs[1]         /* Default input 1 */
#define in2     prhs[2]         /* Default input 2 */
#define in3     prhs[3]         /* Default input 3 */

/* Convenience definitions: Output */
#define out1    plhs[0]         /* Default output 1 */
#define out2    plhs[1]         /* Default output 2 */

/* Macros for faking a configuration */
#define config_set_string(c,x,s) \
      config_setting_set_string(config_lookup(c,x),s)
#define config_set_int(c,x,s) \
      config_setting_set_int(config_lookup(c,x),s)
#define config_set_float(c,x,s) \
      config_setting_set_float(config_lookup(c,x),s)

/*
 * Global configuration
 */
config_t cfg;
int verbose = 1;

/*
 * Print library version
 */
void mex_print_version(void)
{
    printf(" MALHEUR %s - Automatic Malware Analysis on Steroids\n"
           " Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)\n"
           " Berlin Institute of Technology (TU Berlin).\n", VERSION);
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
        mal_error("Number of input/output arguments is invalid");
    if (!mxIsCell(in1))
        mal_error("First argument is not a cell array of file names");
    if (!mxIsChar(in2))
        mal_error("Second argument is not a config filename");

    /* Get input arguments */
    len = mxGetN(in1);    
    mxGetString(in2, fn, 1023);

    /* Init and load configuration */
    config_init(&cfg);
    if (config_read_file(&cfg, fn) != CONFIG_TRUE) {
        mal_error("Could not read configuration (%s in line %d)",
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
        r = load_file(fn, NULL);
        r = mist_preproc(r);
    
        /* Store report in cell array */
        mxSetCell(out1, i, mxCreateString(r));
        prog_bar(0, (double) len, (double) i);

        /* Free space */    
        free(r);
    }
    
    prog_bar(0.0, 1.0, 1.0);
    config_destroy(&cfg);
}

/*
 * Extract features and compute pairwise dot product 
 */
void mex_kernel(MEX_SIGNATURE)
{
    char cf[1024], df[1024];

    /* Check input */
    if (nrhs != 1 + 2 || nlhs < 1) 
        mal_error("Number of input/output arguments is invalid");
    if (!mxIsChar(in1))
        mal_error("First argument is not a dirname/archive");
    if (!mxIsChar(in2))
        mal_error("Second argument is not a config filename");

    /* Get input arguments */
    mxGetString(in1, df, 1023);
    mxGetString(in2, cf, 1023);

    /* Init and load configuration */
    config_init(&cfg);
    if (config_read_file(&cfg, cf) != CONFIG_TRUE) {
        mal_error("Could not read configuration (%s in line %d)",
                  config_error_text(&cfg), config_error_line(&cfg));
    }          
    
    /* Check configuration */
    config_check(&cfg);
    if (verbose)
        config_print(&cfg);

    /* Extract features */
    farray_t *fa = farray_extract(df);
    if (!fa)
        mal_error("Could not load data from '%s'", df);

    /* Compute dot product */
    out1 = mxCreateNumericMatrix(fa->len, fa->len, mxDOUBLE_CLASS, mxREAL);    
    farray_dot(fa, fa, (double *) mxGetPr(out1));

    /* Create data struct */
    out2 = mal_data_struct(fa);
         
    /* Clean up */
    farray_destroy(fa);
    config_destroy(&cfg);    
}

/*
 * Extract features and compute quantile prototypes
 */
void mex_prototype(MEX_SIGNATURE)
{
    const char *fields[] = { "indices", "assign" };
    char cf[1024], df[1024];
    double *rs = NULL; 
    int i, ns = 0;

    /* Check input */
    if (nrhs < 1 + 2 || nlhs < 1) 
        mal_error("Number of input/output arguments is invalid");
    if (!mxIsChar(in1))
        mal_error("First argument is not a dirname/archive");
    if (!mxIsChar(in2))
        mal_error("Second argument is not a config filename");

    /* Get input arguments */
    mxGetString(in1, df, 1023);
    mxGetString(in2, cf, 1023);

    /* Init and load configuration */
    config_init(&cfg);
    if (config_read_file(&cfg, cf) != CONFIG_TRUE) {
        mal_error("Could not read configuration (%s in line %d)",
                  config_error_text(&cfg), config_error_line(&cfg));
    }          

    /* Check configuration */
    config_check(&cfg);
    if (verbose)
        config_print(&cfg);
        
    /* Check for optional input */
    if (nrhs == 4 && mxGetClassID(in3) == mxDOUBLE_CLASS) {
        ns = mxGetNumberOfElements(in3);
        rs = malloc(ns * sizeof(double));
        memcpy(rs, mxGetData(in3), ns * sizeof(double));
    } else {
        ns = 1;
        rs = malloc(ns * sizeof(double));
        config_lookup_float(&cfg, "prototypes.ratio", rs);
    }        

    /* Extract features */
    farray_t *fa = farray_extract(df);
    if (!fa)
        mal_error("Could not load data from '%s'", df);

    /* Extract prototypes */
    out1 = mxCreateStructMatrix(1, ns, 3, fields);
    for (i = 0; i < ns; i++) {
        config_set_float(&cfg, "prototypes.ratio", rs[i]);  
    
        proto_t *pr = proto_extract(fa);
        if (!pr)
            mal_error("Could not prototype feature vectors.");
            
        mal_proto_struct(out1, i, proto_extract(fa));
        proto_destroy(pr);
    }
    out2 = mal_data_struct(fa);
    
    /* Clean up */
    free(rs);
    farray_destroy(fa);
    config_destroy(&cfg);    
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
        mal_error("Invalid Malheur command");

    /* Process commands */    
    if (!strcasecmp(buf, "version")) {
        mex_print_version();           
    } else if (!strcasecmp(buf, "load_mist")) {
        mex_load_mist(nlhs, plhs, nrhs, prhs);
    } else if (!strcasecmp(buf, "kernel")) {
        mex_kernel(nlhs, plhs, nrhs, prhs);
    } else if (!strcasecmp(buf, "prototype")) {
        mex_prototype(nlhs, plhs, nrhs, prhs);
    } else {
        mal_error("Unknown Malheur command");
    }
}
