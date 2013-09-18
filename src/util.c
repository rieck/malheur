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
/**
 * @defgroup util Utility functions
 * The module contains utility functions.
 * @author Konrad Rieck
 * @{
 */

#include "config.h"
#include "common.h"
 
#include <archive.h>
#include <archive_entry.h>

#include "fvec.h"
#include "util.h"

/** Progress bar (with NULL) */
static char pb_string[PROGBAR_LEN + 1];
/** Start timestamp measured */
static double pb_start = -1;

/**
 * Compares two index structures.
 * @param x Index structure x
 * @param y Index structure y
 * @return comparison using internal function (cmp)
 */
int cmp_index(const void *x, const void *y)
{
    index_t *xi = (index_t *) x;
    index_t *yi = (index_t *) y;

    assert(xi->cmp == yi->cmp);

    return xi->cmp(xi->ptr, yi->ptr);
}

/**
 * Compares two features values (hashs)
 * @param x feature X
 * @param y feature Y
 * @return result as a signed integer
 */
int cmp_feat(const void *x, const void *y)
{
    if (*((feat_t *) x) > *((feat_t *) y))
        return +1;
    if (*((feat_t *) x) < *((feat_t *) y))
        return -1;
    return 0;
}

/*
 * Sorts the provided array and also returns an array of corresponding
 * indices. The array's memory need to be free'd.
 * @param b Pointer to array
 * @param n Number of elements
 * @param w Size of each element
 * @param c Comparison function
 * @return array of indices according to sorting
 */
int *qsort_idx(void *b, size_t n, size_t w,
               int (*c) (const void *, const void *))
{
    int i;
    index_t *x = malloc(sizeof(index_t) * n);
    int *y = malloc(sizeof(int) * n);
    if (!x || !y) {
        error("Could not allocate memory for sorting");
        return NULL;
    }

    /* Prepare new array */
    for (i = 0; i < n; i++) {
        x[i].idx = i;
        x[i].ptr = ((char *) b) + i * w;
        x[i].cmp = c;
    }

    /* Sort new array */
    qsort(x, n, sizeof(index_t), cmp_index);

    /* Copy sorted indices */
    for (i = 0; i < n; i++)
        y[i] = x[i].idx;
    free(x);

    return y;
}

/**
 * Print a formated error/warning message. See the macros error and 
 * warning in util.h
 * @param p Prefix string, e.g. "Error"
 * @param f Function name
 * @param m Format string
 */
void err_msg(char *p, const char *f, char *m, ...)
{
    va_list ap;
    char s[256] = { " " };

    va_start(ap, m);
    vsnprintf(s, 256, m, ap);
    va_end(ap);

    fprintf(stderr, "%s: %s", p, s);
    if (errno)
        fprintf(stderr, ": %s", strerror(errno));
    fprintf(stderr, " [%s]\n", f);

    errno = 0;
}

/**
 * Print a progress bar in a given range.
 * @param a Minimum value 
 * @param b Maximum value
 * @param c Current value
 */
void prog_bar(long a, long b, long c)
{
    int i, first, last;
    double perc, ptime = 0, min, max, in;
    char *descr = "";
    
    min = (double) a;
    max = (double) b;
    in = (double) c;

    perc = (in - min) / (max - min);
    first = fabs(in - min) < 1e-10;
    last = fabs(in - max) < 1e-10;

    /* Start of progress */
    if (pb_start < 0 || (first && !last)) {
        pb_start = time_stamp();
        for (i = 0; i < PROGBAR_LEN; i++)
            pb_string[i] = PROGBAR_EMPTY;
        descr = "start";
        perc = 0.0;
    }

    /* End of progress */
    if (last) {
        for (i = 0; i < PROGBAR_LEN; i++)
            pb_string[i] = PROGBAR_FULL;
        ptime = time_stamp() - pb_start;
        descr = "total";
        perc = 1.0;
        pb_start = -1;
    }

    /* Middle of progress */
    if (!first && !last) {
        int len = (int) round(perc * PROGBAR_LEN);
        for (i = 0; i < len; i++)
            if (i < len - 1)
                pb_string[i] = PROGBAR_DONE;
            else
                pb_string[i] = PROGBAR_FRONT;
        ptime = (max - in) * (time_stamp() - pb_start) / (in - min);
        descr = "   in";
    }

    int mins = (int) floor(ptime / 60);
    int secs = (int) floor(ptime - mins * 60);
    pb_string[PROGBAR_LEN] = 0;

    printf("\r  [%s] %5.1f%%  %s %.2dm %.2ds ", pb_string,
           perc * 100, descr, mins, secs);

    if (last)
        printf("\n");

    fflush(stdout);
    fflush(stderr);
}


/**
 * Loads a textual file into a string. The string is allocated 
 * and need to be free'd later by the caller.
 * @param path Path to file
 * @param name file name or NULL
 * @return string 
 */
char *load_file(char *path, char *name)
{
    assert(path);
    long len, size = 0;
    char *str = NULL, file[1024];
    struct stat st;

#pragma omp critical (snprintf)
    {
        if (name)
            snprintf(file, 1024, "%s/%s", path, name);
        else
            snprintf(file, 1024, "%s", path);
    }

    /* Open file */
    FILE *fptr = fopen(file, "r");
    if (!fptr) {
        error("Could not open file '%s'", file);
        return NULL;
    }

    /* Allocate memory */
    stat(file, &st);
    size = st.st_size;
    str = malloc(sizeof(char) * (size + 1));
    if (!str) {
        error("Could not allocate memory for file data");
        return NULL;
    }

    /* Read data */
    len = fread(str, sizeof(char), size, fptr);
    fclose(fptr);

    if (len != size)
        warning("Could not read all data from file '%s'", file);

    str[size] = '\0';
    return str;
}

/**
 * Simple copy function. This function is likely inefficient and 
 * should only be used to copy small files, e.g. stuff from /etc
 * @param src Name of source file
 * @param dst Name of destination fie
 * @return true on success, false otherwise
 */ 
int copy_file(char *src, char *dst)
{
    FILE *sf, *df;
    int c;
    
    sf = fopen(src, "r");
    if (!sf) {
        error("Could not open file '%s'.", src);
        return FALSE;
    }

    df = fopen(dst, "w");
    if (!df) {
        error("Could not open file '%s'.", dst);
        return FALSE;
    }
    
    while ((c = getc(sf)) != EOF)
        putc(c, df);

    fclose(sf);
    fclose(df);
    
    return TRUE;
}


/**
 * Returns the number of entries in a directory. 
 * @param dir Directory to analyse
 * @param fnum Return pointer for number of regular files
 * @param total Return pointer for number of total files
 */
void list_dir_entries(char *dir, int *fnum, int *total)
{
    struct dirent *dp;
    DIR *d;

    *fnum = 0;
    *total = 0;

    d = opendir(dir);
    while (d && (dp = readdir(d)) != NULL) {
        if (dp->d_type == DT_REG)
            ++ * fnum;
        ++*total;
    }
    closedir(d);
}

/**
 * Returns the number of file entries in an archive.
 * @param arc archive containing files
 * @param fnum Return pointer for number of regular files
 * @param total Return pointer for number of total files
 */
void list_arc_entries(char *arc, int *fnum, int *total)
{
    struct archive *a;
    struct archive_entry *entry;
    assert(arc);

    *fnum = 0;
    *total = 0;

    /* Open archive */
    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    archive_read_open_filename(a, arc, 4096);

    /* Jump through archive */
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        const struct stat *s = archive_entry_stat(entry);
        if (S_ISREG(s->st_mode))
            ++ * fnum;

        ++*total;
        archive_read_data_skip(a);
    }
    archive_read_close(a);
}


/**
 * Return a timestamp of the real time
 * @return time stamp
 */
double time_stamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

/**
 * Decode a string with URI encoding. The function operates 
 * in-place. A trailing NULL character is appended to the string.
 * @param str Stirng to escape.
 * @return length of decoded sequence
 */
int decode_string(char *str)
{
    int j, k, r;
    char hex[5] = "0x00";

    /* Loop over string */
    for (j = 0, k = 0; j < strlen(str); j++, k++) {
        if (str[j] != '%') {
            str[k] = str[j];
        } else {
            /* Check for truncated string */
            if (strlen(str) - j < 2)
                break;

            /* Parse hexadecimal number */
            hex[2] = str[++j];
            hex[3] = str[++j];
            sscanf(hex, "%x", (unsigned int *) &r);
            str[k] = (char) r;
        }
    }

    return k;
}

/**
 * Extracts the suffix from a file name. If the file does not
 * have a suffix, the function returns "unknown". 
 */
char *file_suffix(char *file)
{
    char *name = file + strlen(file) - 1;

    /* Determine dot in file name */
    while (name != file && *name != '.')
        name--;

    /* Check for files with no suffix */
    if (name == file)
        name = "unknown";
    else
        name++;

    return name;
}

/**
 * Print version and copyright information
 */
void malheur_version(FILE *f)
{
    fprintf(f,
            "# MALHEUR (%d.%d.%d) - Automatic Analysis of Malware Behavior\n"
            "# Copyright (c) 2009-2012 Konrad Rieck (konrad@mlsec.org)\n"
            "# University of Goettingen, Berlin Institute of Technology\n",
            MALHEUR_MAJOR, MALHEUR_MINOR, MALHEUR_PATCH);
}

/** 
 * Size of upper triangle of a symmatrix matrix
 * @param n length of one dimension
 * @return size of triangle
 */
long tria_size(long n)
{
    return n * (n + 1) / 2;
}

/** 
 * Index of point in the upper triangle of a symmetric matrix
 * @param x coordinate in x axis
 * @param y coordinate in y axis
 * @param n length of one dimension 
 */
long tria_pos(long x, long y, long n)
{
    if (y < x)
        return tria_size(y) + (n - y) * y + (x - y);
    else
        return tria_size(x) + (n - x) * x + (y - x);
}

/**
 * Determine the maximum of an array
 * @param a array
 * @param l length of array 
 * @return index to maximum value
 */
int array_max(double *a, int l)
{
    assert(a && l > 0);
    int i, k = 0;
    double dm = DBL_MIN;

    for (i = 0; i < l; i++) {
        if (a[i] <= dm)
            continue;
        dm = a[i], k = i;
    }

    return k;
}

/**
 * Determine the minimum of an array
 * @param a array
 * @param l length of array 
 * @return index to minimum value
 */
int array_min(double *a, int l)
{
    assert(a && l > 0);
    int i, k = 0;
    double dm = DBL_MAX;

    for (i = 0; i < l; i++) {
        if (a[i] >= dm)
            continue;
        dm = a[i], k = i;
    }

    return k;
}

/** @} */

