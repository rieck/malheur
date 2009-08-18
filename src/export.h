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
#define TS      "<tr><td> &nbsp;&nbsp; "
#define TM      " &nbsp;&nbsp; </td><td valign='right'>"
#define TE      "</td></tr>\n"
#define TABS    "<table cellpadding='0' cellspacing='0' style='font-size: 11pt;'>"
#define TABE    "</table>\n"

/* URL template for cwsandbox.org */
#define CWS_URL   "https://pcert.cwsandbox.org/?page=analysis&format=xml"

/* I/O functions */
void export_proto(proto_t *p, farray_t *fa, char *file);
void export_kernel(double *d, farray_t *f, char *file);


#endif                          /* EXPORT_H */
