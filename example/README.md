===Example file===

test_compression.c

We provide an example of AMR data (MISMIP3D) for illustrating how the data being compressed was generated. 

The test data is stored in LAMP/example/testdata, you can use the executable 'sh test.sh' command to do the compression.

===compression prototypes===

int zfp1_compress(double * array, int nx, double tolerance, char filename[50]);

Return: the compressed bytes for ZFP

char *SZ_compress(int dataType, void *data, ulong *outSize, int r5, int r4, int r3, int r2, int r1);

dataType: SZ_DOUBLE

void *data: double* data

ulong *outSize: size of output compressed data (in byte)

int r5: size of dimension 5

int r4: size of dimension 4

int r3: size of dimension 3

int r2: size of dimension 2

int r1: size of dimension 1

Return: the compressed bytes for SZ

## Note
More detailed instructions on how to build the LAMP can be found in the LAMP/src.
