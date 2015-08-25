
MALHEUR - Automatic Analysis of Malware Behavior
==
 
Introduction 
--

Malheur is a tool for the automatic analysis of malware behavior (program
behavior recorded from malicious software in a sandbox environment).  It
has been designed to support the regular analysis of malicious software and
the development of detection and defense measures.  Malheur allows for
identifying novel classes of malware with similar behavior and assigning
unknown malware to discovered classes.  It supports four basic actions for
analysis which can be applied to reports of recorded behavior:

1. *Extraction of prototypes:*
    From a given set of reports, malheur identifies a subset of
    prototypes representative for the full data set. The prototypes
    provide a quick overview of recorded behavior and can be used to
    guide manual inspection.

2. *Clustering of behavior* 
    Malheur automatically identifies groups (clusters) of reports
    containing similar behavior. Clustering allows for discovering novel
    classes of malware and provides the basis for crafting specific
    detection and defense mechanisms, such as anti-virus signatures.

3. *Classification of behavior:* 
    Based on a set of previously clustered reports, malheur is able to
    assign unknown behavior to known groups of malware. Classification
    enables identifying novel and unknown variants of malware and can be
    used to filter program behavior prior to manual inspection.

4. *Incremental analysis:* 
    Malheur can be applied incrementally for analysis of large data
    sets. By processing reports in chunks, the run-time as well as
    memory requirements can be significantly reduced. This renders
    long-term application of malheur feasible, for example for daily
    analysis of incoming malware programs.

A detailed description of these techniques as well as technical
background on analysis of malicious software is provided in the
following articles:

+ "Automatic Analysis of Malware Behavior using Machine Learning."
  Konrad Rieck, Philipp Trinius, Carsten Willems, and Thorsten Holz
  Journal of Computer Security (JCS), 19 (4) 639-668, 2011.

+ "A Malware Instruction Set for Behavior-Based Analysis."
  Philipp Trinius, Carsten Willems, Thorsten Holz, and Konrad Rieck 
  Technical report TR-2009-07, University of Mannheim, 2009

Dependencies
--

+   libconfig >= 1.4, <http://www.hyperrealm.com/libconfig/>
+   libarchive >= 2.70,  <http://libarchive.github.com/>

#### Debian & Ubuntu Linux

The following packages need to be installed for compiling Malheur on Debian
and Ubuntu Linux

    gcc
    libconfig9-dev
    libarchive-dev

For bootstrapping Malheur from the GIT repository or manipulating the
automake/autoconf configuration, the following additional packages are
necessary.

    automake
    autoconf
    libtool

#### Mac OS X

For compiling Malheur on Mac OS X a working installation of Xcode is required
including `gcc`.  Additionally, the following packages need to be installed
via Homebrew

    libconfig
    libarchive (from homebrew-alt)

#### OpenBSD

For compiling Malheur on OpenBSD the following packages are required. Note
that you need to use `gmake` instead of `make` for building Malheur.

    gmake
    libconfig
    libarchive

For bootstrapping Malheur from the GIT repository, the following packages
need be additionally installed

    autoconf
    automake
    libtool

Compilation & Installation
--

From GIT repository first run

    $ ./bootstrap

From tarball run

    $ ./configure [options]
    $ make
    $ make check
    $ make install

Options for configure

    --prefix=PATH           Set directory prefix for installation

By default Malheur is installed into /usr/local. If you prefer
a different location, use this option to select an installation
directory.

License
--

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.  This program is distributed
without any warranty. See the GNU General Public License for more
details.

Copyright
--

Copyright (c) 2009-2012 Konrad Rieck (konrad@mlsec.org)   
University of Goettingen, Berlin Institute of Technology
