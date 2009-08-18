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

#include <archive.h>
#include <archive_entry.h>

#include "config.h"
#include "common.h"
#include "fvec.h"
#include "util.h"

/** Progress bar (with NULL) */
static char pb_string[PROGBAR_LEN + 1];
/** Start timestamp measured */
static double pb_start = 0;

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

/**
 * Compares two double values
 * @param x double X
 * @param y double Y
 * @return result as a signed integer
 */
int cmp_double(const void *x, const void *y)
{
    if (*((double *) x) > *((double *) y))
        return +1;
    if (*((double *) x) < *((double *) y))
        return -1;
    return 0;
}

/**
 * Compares two unsigned integer values
 * @param x double X
 * @param y double Y
 * @return result as a signed integer
 */
int cmp_uint(const void *x, const void *y)
{
    if (*((unsigned int *) x) > *((unsigned int *) y))
        return +1;
    if (*((unsigned int *) x) < *((unsigned int *) y))
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
int *qsort_idx(void *b, size_t n, size_t w, int (*c)(const void *, const void *))
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
 * @param min Minimum value 
 * @param max Maximum value
 * @param in Current value
 */
void prog_bar(double min, double max, double in)
{
    int i, first, last;
    double perc, ptime = 0;
    char *descr = "";

#ifdef DEBUG
    /* Skip progress bars when debugging */
    return;
#endif

    perc = (in - min) / (max - min);
    first = fabs(in - min) < 1e-10;
    last = fabs(in - max) < 1e-10;

    /* Start of progress */
    if (pb_start == 0 || (first && !last)) {
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
        pb_start = 0;
    }
    
    /* Middle of progress */
    if (!first && !last) {    
        int len = round(perc * PROGBAR_LEN);
        for (i = 0; i < len; i++)
            if (i < len - 1)
                pb_string[i] = PROGBAR_DONE;
            else
                pb_string[i] = PROGBAR_FRONT;
        ptime = (max - in) * (time_stamp() - pb_start) / (in - min);
        descr = "   in";     
    }

    int mins = floor(ptime / 60);
    int secs = floor(ptime - mins * 60);
    pb_string[PROGBAR_LEN] = 0;

    printf("\r  [%s] %5.1f%%  %s %.2dm %.2ds", pb_string,
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
            ++*fnum;
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
    archive_read_support_compression_all(a);
    archive_read_support_format_all(a);
    archive_read_open_filename(a, arc, 4096);
    
    /* Jump through archive */
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        const struct stat *s = archive_entry_stat(entry);
        if (S_ISREG(s->st_mode))      
            ++*fnum;
        ++*total;    
        archive_read_data_skip(a);
    }
    archive_read_finish(a);
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
            str[k] = r;
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
 * Checks and fixes the range of a value 
 * @param a Value to check
 * @param mi Minimum value
 * @param ma Maximum value
 * @return fixed value
 */
int check_range(int a, int mi, int ma)
{
    a = a < mi ? mi : a;
    a = a > ma ? ma : a;
    return a;
} 
 

#ifndef HAVE_FUNC_LOG2
/** 
 * Logarithm of x to base 2
 * @param x input value
 * @return logarithm 
 */
double log2(double x)
{
    return log10(x) * 3.32192809488736234;
}
#endif

#ifndef HAVE_FUNC_ROUND
/** 
 * Round function 
 * @param x input value
 * @return integer number
 */
long round(double x)
{
    double f = floor(x);
    if (s - f >= 0.5)
        return (long) f + 1;
    return (long) f;
}
#endif

