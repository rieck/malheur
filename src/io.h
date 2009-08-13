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
char *io_load_file(char *, char *);
void io_dir_entries(char *dir, int *, int *);
void io_arc_entries(char *arc, int *, int *);
char *io_preprocess(char *);
void io_save_kernel(double *, farray_t *, char *);
void io_export_proto(proto_t *p, farray_t *fa, char *file);

#endif                          /* STRIO_H */
