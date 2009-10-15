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

#ifndef COMMON_H
#define COMMON_H

#define __USE_BSD               /* Also for dirent under Linux */
#define _BSD_SOURCE             /* For setdup under Linux */
#define __USE_POSIX             /* For readdir_r under Linux */

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <float.h>
#include <dirent.h>
#include <assert.h>
#include <ctype.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <pthread.h>
#include <zlib.h>
#include <libconfig.h>

/* Some useful definitions */
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif                          /* COMMON_H */
