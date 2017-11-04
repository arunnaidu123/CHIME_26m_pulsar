CC=/usr/local/cuda-8.0/bin/nvcc
CFLAGS=-I/usr/local/cuda-8.0/include/ -L/usr/local/cuda-8.0/lib64 -L/usr/local/pgplot/
DEPS = global.cuh struct.cuh
CPP = g++ -g -Wall
ALL_CFLAGS = -ccbin g++ -I/usr/local/cuda-8.0/include/  -m64 \
	-gencode arch=compute_35,code=\"sm_35,compute_35\" 
LD_FLAGS = -lm -lpthread -lcpgplot -lcufft -std=c++11 -fPIC -march=native -ffast-math -funroll-loops
OBJ = writeclient.o \
    ipcutil.o ipcbuf.o tmutil.o dada_hdu.o multilog.o ipcio.o ascii_header.o \
    ipcbuf_cuda.o dada_cuda.o ipcio_cuda.o time_convert.o network_thread.o

OBJ1 = allocate_buffers.o \
    ipcutil.o ipcbuf.o tmutil.o dada_hdu.o multilog.o ipcio.o ascii_header.o \
    ipcbuf_cuda.o dada_cuda.o ipcio_cuda.o

FILES = \
    

%.o: %.c 
	$(CC) $(ALL_CFLAGS) -o $@ -c $< 
	
%.o: %.cu 
	$(CC) $(ALL_CFLAGS) -o $@ -c $<

%.o: %.cpp
	$(CPP) $(CFLAGS) -o $@ -c $< $(LD_FLAGS)

all: writeclient allocate_buffers

writeclient: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

allocate_buffers: $(OBJ1)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f -r ./*.o
	rm -f -r ./*~

