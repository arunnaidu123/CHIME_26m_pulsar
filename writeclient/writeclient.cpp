/* Before running the the program the nodes must be configureed with following settings.
 *
 * 1) The default memlock limit   must be incresed in the /etc/security/limits.conf add the following
 *     chime    hard   memlock           unlimited
 *     chime    soft   memlock           unlimited
 *
 * 2) The default receive and tramsmit buffer limits must be increased in the file /etc/sysctl.conf
 *     add he following three lines.
 *
 *     net.core.rmem_max = 268435456
 *     net.core.wmem_max = 268435456
 *     net.core.netdev_max_backlog = 65536
 *
 *  3) disable irqbalance and increase the MTU to 9000 and manually bind the cpu cores to the 10 gig port.
 *
 *  4) MTU value should be changed in and make BOOT=yes
 *     /etc/sysconfig/network-scripts/ifcfg-
*/


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
#include "ch_vdif_assembler_kernels.hpp"
#include "global.hpp"
#include <sched.h>
#include <pthread.h>

int network_thread(char* argv[], dada_hdu_t* hdu, float duration);
int assemble_thread(float duration);
int threaded_assemble(int t);



int assemble(int8_t *buf, int8_t *assembled_buf);
struct data_chunk chunk1; // Two second buffer
struct data_chunk chunk2; // Two second buffer

//int packet_assembler();
//struct data_chunk chunk2(800000000*2); // Two second buffer

int main(int argc, char* argv[])
{
  
  chunk1.allocate_data_chunk(268435456,50);
  chunk2.allocate_data_chunk(536870912,20); 
  
  dada_hdu_t* hdu;
  multilog_t* log;
   
  key_t key = DADA_DEFAULT_BLOCK_KEY;
  
  char *data;
  
  
  log = multilog_open ("dada_chime_1", 1);
  multilog_add (log, stderr);
  
   
  hdu = dada_hdu_create (log);
  dada_hdu_set_key(hdu, key);
  
  /*
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
  
  */
  if (dada_hdu_connect (hdu) < 0)
    printf("something is wrong 1\n");

  

  float duration = (float)(atoi(argv[2]));

  std::thread t1(network_thread,argv,hdu,duration);
  std::thread t2(assemble_thread,duration);

  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(4, &cpuset);
  int rc = pthread_setaffinity_np(t1.native_handle(),
                                    sizeof(cpu_set_t), &cpuset);
  if (rc != 0) {
    std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
  }


  /*
 
  char *header, *hdr_location;
  header = (char*) malloc(sizeof(char)*4096);
   
  FILE *hdr_file;
  int hdr_size = 0;

  hdr_file = fopen(argv[1],"rb");
  if(hdr_file==NULL)
  {
    printf("Cannot open the header file \n");
    exit(0);
  }
  
  fseek(hdr_file,0,SEEK_END);
  hdr_size = ftell(hdr_file);
  fseek(hdr_file,0,SEEK_SET);

  fread(header,1,hdr_size,hdr_file);
  
  header[hdr_size-1] = '\0';
  
  printf("Header size is = %d \n",hdr_size);
  if(hdr_size>=4096) 
  {
    printf("size is larger than required \n");
    exit(0);
  }
  
  ipcbuf_lock_write (hdu->header_block);
  hdr_location = ipcbuf_get_next_write (hdu->header_block);
  memcpy(hdr_location,header,hdr_size);
  ipcbuf_mark_filled (hdu->header_block, hdr_size);
  
  ipcbuf_unlock_write (hdu->header_block);

  fclose(hdr_file);

   
  */
  ipcio_open (hdu->data_block, 'W');
  int stop =0;
  //float duration = (float)(atoi(argv[2])); 
  long count=0;
  while(!stop)
  {
    
    std::unique_lock<std::mutex> lk2(chunk2.m);
    chunk2.cv.wait(lk2, []{return chunk2.readFlag;});
     
   //ipcio_write (hdu->data_block, reinterpret_cast<char*>(chunk2.buf[chunk2.get_index(n)]), 320000000); 
   for(int n=chunk1.nscratch; n<2*chunk1.nscratch; n++) ipcio_write (hdu->data_block, reinterpret_cast<char*>(chunk2.buf[chunk2.get_index(n)]), 320000000); 
     
    chunk2.writeFlag = true;
    chunk2.readFlag = false;
    chunk2.cv.notify_one();
    count++;

    if(duration<count*0.2*chunk1.nscratch) stop=1;
    
    
     
  } 
  //ipcio_stop(hdu->data_block);
  
  ipcio_close (hdu->data_block);
  

  if (dada_hdu_disconnect (hdu) < 0)
    printf("something is wrong 2\n");

  t1.join();
  t2.join();
  
  //ipcio_destroy(hdu->data_block);
  //ipcbuf_destroy(hdu->header_block);
  return 0;
}


int assemble(int8_t *buf, int8_t *assembled_buf)
{
	long block_size = 625*4096;
	
	for(int sample=0;sample<2*625*625;sample++)
	{
		for(int channel=0; channel<1024; channel++)
		{
			for(int pol=0; pol<2; pol++)
			{
				long block = sample/625;
				long block_sample = sample%625;
				long block_channel = channel%128;
				long c = channel/128;
				long location = block*block_size+(pol*block_size/2)+block_channel*8*625+block_sample*8+c;
				long a_location = sample*4096+channel*2048+pol;
				assembled_buf[a_location] = buf[location];
		  }
	  }
  }
  
  return 0;
}				
