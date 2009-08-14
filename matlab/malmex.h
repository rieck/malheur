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

#ifndef _MALMEX_H
#define _MALMEX_H

/* Malheur-MEX functions */
void mal_error(char *m, ...);
mxArray *mal_data_struct(farray_t *fa);
void mal_proto_struct(mxArray *, int, proto_t *p);

/* Helper functions */
mxArray* mxCreateScalar(double x);

#endif