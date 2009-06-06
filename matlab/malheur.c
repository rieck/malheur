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

/* Matlab message identifier */
#define error(...) mexErrMsgIdAndTxt("malheur:generic", __VA_ARGS__)

#include <mex.h>
#include <string.h>
#include <libconfig.h>
#include "config.h"
#include "mist.h"
#include "util.h"
#include "data.h"
#include "strings.h"

/* Mex signature */
#ifndef MEX_SIGNATURE
#define MEX_SIGNATURE \
    int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]
#endif   

/* Convenience definitions */
#define in1     prhs[1]         /* Default input 1 */
#define in2     prhs[2]         /* Default input 2 */
#define in3     prhs[3]         /* Default input 3 */
#define out     plhs[0]         /* Default output */

/*
 * Global configuration
 */
config_t cfg;

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
    long level, i, len, rlen, tlen;
    mxArray *a;

    /* Check input */
    if (nrhs != 2 || nlhs != 1)
        error("Number of input/output arguments is invalid");
    if (!mxIsCell(in1))
        error("First argument is not a cell array of file names");
    if (!mxIsChar(in2))
        error("Second argument is not a filename");

    /* Get input arguments */
    len = mxGetN(in1);    
    mxGetString(in2, fn, 1023);

    /* Init and load configuration */
    config_init(&cfg);
    if (config_read_file(&cfg, fn) != CONFIG_TRUE)
        error("Could not read configuration (%s in line %d)",
              config_error_text(&cfg), config_error_line(&cfg));
    
    /* Check configuration */
    char *err = check_config(&cfg);
    if (err)
        error(err);
    
    /* Get MIST configuration */
    config_lookup_int(&cfg, "input.mist_level", (long *) &level);  
    config_lookup_int(&cfg, "input.mist_report_len", (long *) &rlen);
    config_lookup_int(&cfg, "input.mist_thread_len", (long *) &tlen);
    printf("Loading %d MIST reports (level: %d, rlen: %d, tlen: %d)\n", 
           len, level, rlen, tlen);
    
    /* Get output variable */
    out = mxCreateCellMatrix(1, len);
    for (i = 0; i < len; i++) {
        /* Get file name */
        a = mxGetCell(in1, i);
        mxGetString(a, fn, 1023);

        /* Load report */
        r = data_load_file(fn, NULL);
        r = mist_preproc(r);
    
        /* Store report in cell array */
        mxSetCell(out, i, mxCreateString(r));
        prog_bar(0, (double) len, (double) i);

        /* Free space */    
        free(r);
    }
    
    prog_bar(0.0, 1.0, 1.0);
    config_destroy(&cfg);
    
    printf("\nDone.\n");
}


/*
 * Generic matlab wrapper function to Malheur functionality
 */
void mexFunction(MEX_SIGNATURE)
{
    char cmd[256];

    if (nrhs == 0) {
        mex_print_version();
        return;
    }

    /* Get command */
    if (mxGetString(prhs[0], cmd, 255)) 
        error("Invalid Malheur command");

    /* Process commands */    
    if (!strcasecmp(cmd, "version")) {
        mex_print_version();           
    } else if (!strcasecmp(cmd, "load_mist")) {
        mex_load_mist(nlhs, plhs, nrhs, prhs);
    } else {
        error("Unknown Malheur command");
    }
}
