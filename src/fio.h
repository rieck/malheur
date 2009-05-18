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

/* Type generic extraction function */
typedef fvec_t *(*fvec_extract_t)(char *, int);  
fvec_t *fio_load_file(char *name, fvec_extract_t);

#endif                          /* STRIO_H */
