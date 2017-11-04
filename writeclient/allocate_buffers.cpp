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
#include "ipcbuf.h"
#include "ipcutil.h"
#include "dada_hdu.h"
#include "dada_def.h"
#include "dada_cuda.h"

int main(int argc, char* argv[])
{

  dada_hdu_t* hdu;
  multilog_t* log;

  key_t key = DADA_DEFAULT_BLOCK_KEY;
  
  log = multilog_open ("dada_chime_1", 1);
  multilog_add (log, stderr);


  hdu = dada_hdu_create (log);
  dada_hdu_set_key(hdu, key);


  ipcbuf_t ipcbuf_init = IPCBUF_INIT;
  ipcio_t ipcio_init = IPCIO_INIT;

  assert (hdu != 0);

  if (hdu->data_block) {
    fprintf (stderr, "dada_hdu_connect: already connected\n");
    return -1;
  }

  hdu->header_block = new ipcbuf_t[1];
  assert (hdu->header_block != 0);
  *(hdu->header_block) = ipcbuf_init;

  hdu->data_block = new ipcio_t[1];

  assert (hdu->data_block != 0);
  *(hdu->data_block) = ipcio_init;

  ipcbuf_create (hdu->header_block, hdu->header_block_key, 1, 4096, 1);
  ipcbuf_create ((ipcbuf_t *)hdu->data_block, hdu->data_block_key, 8, 1024*1026*1024, 1);

   if (dada_hdu_disconnect (hdu) < 0)
    printf("something is wrong 0\n");

   return 0;
}
   
