/*
 * MALHEUR - Automatic Analysis of Malware Behavior
 * Copyright (c) 2009-2012 Konrad Rieck (konrad@mlsec.org)
 * University of Goettingen, Berlin Institute of Technology 
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

/* 
 * Definitions for MIST parsing
 */
#define MIST_LEVEL      '|'       /* Delimiter for MIST levels */
#define MIST_INSTR      '\n'      /* Delimiter for MIST instructions */
#define MIST_COMMENT    '#'       /* Prefix for comments */
#define MIST_THREAD     "thread"  /* Comment string indicating new thread */

/* Functions */
char *mist_preproc(char *report);
char *mist_trunc_report(char *, int);
char *mist_trunc_thread(char *, int);
char *mist_trunc_level(char *, int);
char *mist_trim(char *);

#endif                          /* MIST_H */
