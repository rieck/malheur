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

#ifndef EXPORT_H
#define EXPORT_H

#include "farray.h"
#include "proto.h"

/* Simple CSS style */
#define BODY    "<style> " \
                " h1,h2,h3 {font-family: georgia; color: #511;}\n " \
                " body {font-size: 11pt; font-family: verdana;}" \
                " a {text-decoration: none; color: #151;}" \
                " </style>" 

/* Brain-damaged macros for HTML tables */
#define TS      "<tr><td>&nbsp;"
#define TM      "&nbsp;</td><td valign='right'>&nbsp;"
#define TE      "</td></tr>\n"

/* URL template for cwsandbox.org */
#define CWS_URL   "https://cwsandbox.org/?site=1&page=report&format=xml"

/* I/O functions */
void export_proto_html(proto_t *p, farray_t *fa, char *file);
void export_proto_text(proto_t *p, farray_t *fa, char *file);
void export_distance_html(double *d, farray_t *f, char *file);
void export_distance_text(double *d, farray_t *f, char *file);


#endif                          /* EXPORT_H */
