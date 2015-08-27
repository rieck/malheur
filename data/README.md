# MIST archives

This directory contains all MIST archives from the original Malheur dataset. 
Unfortunately, the full dataset is not available any more.

## Archive Content 

The dataset includes all MIST archives of the so-called _reference dataset_
and the _application dataset_ used in the JCS article (see below).  Each
archive contains behavior reports of malware programs.  The reports have
been generated using CWSandbox and are provided in the so-called MIST
format, a weird representation of program behavior that was considered cool
in the late 2000s.


A detailed description of the dataset and the MIST format are available
in the following two papers: 

  1. Automatic Analysis of Malware Behavior using Machine Learning
     Konrad Rieck, Philipp Trinius, Carsten Willems, and Thorsten Holz.
     Journal of Computer Security (JCS), 19 (4) 639-668, 2011.

  2. A Malware Instruction Set for Behavior-Based Analysis
     Philipp Trinius, Carsten Willems, Thorsten Holz, and Konrad Rieck
     TR-2009-07, University of Mannheim, 2009

## Common Questions

  1. _Where is the rest of the dataset?_  The original dataset was published
     long time ago in 2011 along with the JCS article.  The files were
     hosted at the University of Mannheim, Germany.  With the move of the
     security research group to Erlangen, however, the server storing the
     data went offline and now likely serves a different purpose. The
     dataset is lost. :(

  2.  _Where are the benign behavior reports?_ Malheur has never been
     designed for detecting malware.  The main purpose of the tool is to
     group malware into families according to its behavior.  As a
     consequence, benign behavior has never been recorded in our
     experiments.

  3. _Can I use the dataset in my research?_ Yes.  However, keep in mind
     that this is an old and rusty dataset.  It is stored in a weird format
     and contains behavior of malware families that died several years ago.
     You can do some research with this data, but it is highly recommended
     that you look for something better. 

