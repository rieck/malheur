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

#ifndef DATA_H
#define DATA_H

#define CSS_FONT  "style='font-family: verdana; font-size: 10pt;'"

#include "farray.h"
#include "proto.h"

/* I/O functions */
char *data_load_file(char *, char *);
void data_dir_entries(char *dir, int *, int *);
void data_archive_entries(char *arc, int *, int *);
char *data_preproc(char *);
void data_export_kernel(double *, farray_t *, char *);
void data_export_proto(proto_t *, farray_t *, char *);


#endif                          /* STRIO_H */
