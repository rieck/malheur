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

#ifndef MIST_H
#define MIST_H

/** Delimiters for instructions */
#define MIST_DELIM      '\n'
#define MIST_LEVEL      ':'
#define MIST_COMMENT    '#'

/* Functions */
char *mist_load_report(char *, int);
char *mist_trunc_level(char *, int);

#endif                          /* MIST_H */
