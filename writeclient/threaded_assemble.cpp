#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <errno.h>
#include <cassert>
 
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include <time.h>

#include <emmintrin.h>
#include "ch_vdif_assembler_kernels.hpp"
#include "global.hpp"

int threaded_assemble(int id)
{
  if(id==1) assembled_new(chunk1.buf[5], chunk2.buf[0]);
  if(id==2) assembled_new(chunk1.buf[6], chunk2.buf[1]);
}
