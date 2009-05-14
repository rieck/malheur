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
#include "config.h"
#include "mist.h"
#include "util.h"
#include "strings.h"

/* Mex signature */
#ifndef MEX_SIGNATURE
#define MEX_SIGNATURE \
    int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]
#endif   

/* Convenience definitions */
#define in1     prhs[1]         /* Default input 1 */
#define in2     prhs[2]         /* Default input 2 */
#define out     plhs[0]         /* Default output */

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
    int level, i, len, j = 0;
    char *r, fn[1024];
    mxArray *a;

    /* Check input */
    if (nrhs != 3)
        error("Number of input arguments is invalid");
    if (nlhs != 1)
        error("Number of output arguments is invalid");
    if (!mxIsCell(in1))
        error("First argument is not a cell array of file names");
    if (!mxIsNumeric(in2))
        error("Second argument is not a level number");

    /* Get input arguments */
    level = (int) mxGetScalar(in2);
    len = mxGetN(in1);
    
    /* Get output variable */
    out = mxCreateCellMatrix(1, len);

    printf("Loading %d MIST reports with level %d ...\n", len, level);

#ifdef MALHEUR_OPENMP
    #pragma omp parallel for private(fn,a,r)   
#endif    
    for (i = 0; i < len; i++) {
        /* Get file name */
        a = mxGetCell(in1, i);
        mxGetString(a, fn, 1023);
        /* Load report */
        r = mist_load_report(fn, level);
        
#ifdef MALHEUR_OPENMP        
        #pragma omp critical
#endif        
        {
            progbar(0, (double) len, (double) j++);
            /* Store report in cell array */
            mxSetCell(out, i, mxCreateString(r));
        }
        /* Free space */    
        free(r);
    }
    
    progbar(0.0, 1.0, 1.0);
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
