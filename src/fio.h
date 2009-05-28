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

#ifndef FIO_H
#define FIO_H

#include "fvec.h"

/* Default block size */
#define FIO_BLOCK               4096

/* I/O functions */
char *fio_load_file(char *, char *);
long fio_count_files(char *dir);
long fio_count_archive(char *arc);
char *fio_preproc(char *);

#endif                          /* STRIO_H */
