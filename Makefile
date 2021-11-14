GFORTRAN    = gfortran -Wall
CC          = gcc -O3 -g
#CC          = gcc -g
MPI_FORTRAN = mpif90 -Wall
MPI_CC      = mpicc
LD = -lm

.SUFFIXES : .o .c

Target:=lampcompress

all: $(Target)


$(Target): %:%.o
        $(CC) $(LD) $< -o $@  -L/home/liyida/Compressor/zfp/build/lib/ -lzfp -L/home/liyida/Compressor/SZ/lib   -lSZ  -lzstd -lzlib
%.o : %.c
        $(CC) -c $(*F).c -I/home/liyida/Compressor/zfp/include/  -I/home/liyida/Compressor/SZ/include/

clean :
        /bin/rm -f *.o $(Target)
