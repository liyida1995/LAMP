# Level Associated Mapping-based Preconditioning (LAMP)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.6459727.svg)](https://doi.org/10.5281/zenodo.6459727)
## Overview
The main idea of LAMP is to rebuild AMR hierarchy and precondition the data before compression and aim to further improve the storage efficiency of AMR applications.

## Instruction for LAMP:
1. Get the AMR applications, lossy compressors (including SZ and ZFP).
2. Set the compressors and set the Makefile in LAMP/src/.
3. Follow the scripts in LAMP/scripts/ for the results of CR_comparation, parallel_LAMP, etc.
4. We also provide the python scripts to plot the result figures.

## Evaluation Platform
Our evaluation platform runs Linux Ubuntu 20.04 LTS. We compile source code using gcc version 9.3.0 with the “-O2” compiler option. Specifically, we run our proposed method (LAMP) and AMR applications on a cluster with 7 compute nodes, 1 I/O node, and 15 TB of storage. Each compute node has 32 cores of Intel(R) Xeon(R) Silver 4110 CPU @ 2.10GHz. The I/O node has 6 cores of 11th Gen Intel(R) Core(TM) i5-11400F @ 2.60GHz. The storage performance of LAMP is tested on a SATA, 7200RPM HDD.

## AMR Applications
We evaluate the effectiveness of LAMP with three real AMR applications in Chombo. These applications can be downloaded from the address: http://davis.lbl.gov/Manuals/BISICLES-DOCS/index.html.

## Extract Data and Boxes for AMR Applications
For AMR applications, the data (e.g., Points, Regions and Rectangular Arrays, etc.) and the metadata of mesh hierarchy (e.g., Unions of Rectangles, Layouts, etc.) are maintained in the main memory. In-situ analysis can easily visualize the results with the help of mesh hierarchy.

To this end, we designed the filetool to extract the key components (data points and boxes) for AMR applications.

Specifically, you need to copy the files in LAMP/filetool/ to the directory of $BISICLES/code/filetool/, and compile.

## Data Compression
We utilize two lossy compressors, including ZFP (0.5.5) and SZ (2.1.12) to evaluate how much LAMP can improve the compression ratio. In particular, we use SZ with the default mode and absolute error bound. For ZFP, we use its absolute error bound. These compressors can be downloaded from the following addresses:
(1)	ZFP: https://github.com/LLNL/zfp.
(2)	SZ: https://github.com/szcompressor/SZ.

## Testing Examples
Examples can be found in the LAMP/example.

You can use the executable 'sh test.sh' command to do the compression. Please see the user guide for details.


## Overall Results
The baseline compression stores AMR data level by level. In contrast, LAMP preconditions data and makes data smoother than the baseline. The experimental results show that LAMP yields higher compression ratios for both ZFP and SZ, compared to the baseline. Besides, the runtime overhead of LAMP is negligible compared to the baseline.
