# Level Associated Mapping-based Preconditioning (LAMP) for AMR data

## Overview
The main idea of LAMP is to rebuild AMR hierarchy and precondition the data before compression and aim to further improve the storage efficiency of AMR applications.

## Evaluation Platforms
We ran our proposed method (LAMP) and AMR applications on a cluster with 7 compute nodes, 1 I/O node, and 15 TB of storage. Each compute node has 32 cores of Intel(R) Xeon(R) Silver 4110 CPU @ 2.10GHz. The I/O node has 6 cores of 11th Gen Intel(R) Core(TM) i5-11400F @ 2.60GHz. The storage performance of LAMP is tested on a SATA, 7200RPM HDD.

## AMR Applications
We evaluate the effectiveness of LAMP with three real AMR applications in Chombo. These applications can be downloaded from the address: http://davis.lbl.gov/Manuals/BISICLES-DOCS/index.html.

## Data Compression
We utilize two lossy compressors, including ZFP (0.5.5) and SZ (2.1.12) to evaluate how much LAMP can improve the compression ratio. In particular, we use SZ with the default mode and absolute error bound. For ZFP, we use its absolute error bound. These compressors can be downloaded from the following addresses:
(1)	ZFP: https://github.com/LLNL/zfp.
(2)	SZ: https://github.com/szcompressor/SZ.

## Overall Results
The baseline compression stores AMR data level by level. In contrast, LAMP preconditions data and makes data smoother than the baseline. The experimental results show that LAMP yields higher compression ratios for both ZFP and SZ, compare to the baseline. Besides, the runtime overhead of LAMP is negligible compared to the baseline.
