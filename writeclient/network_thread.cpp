#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdexcept>
#include <netinet/in.h>
#include <unistd.h>
#include <chrono>
#include <memory>
#include <fstream>
#include <vector>
#include "global.hpp"
#include "ch_vdif_assembler_kernels.hpp"
#include <sched.h>
#include <pthread.h>
#include "time_convert.h"

#include "ipcbuf.h"
#include "ipcutil.h"
#include "dada_hdu.h"
#include "dada_def.h"
#include "dada_cuda.h"
#include "ascii_header.h"
#include <time.h>

template<class T>
char* as_bytes(T& i)
{ 
  void* addr = &i;
  return static_cast<char *>(addr);
}


using namespace std;
using namespace ch_vdif_assembler;
//void network_thread()
int decode(uint8_t *packet, int8_t *decoded_packet);
int move_data(int8_t *buf, uint8_t *packet, uint32_t freq_major, uint32_t sample, uint32_t pol);
int shift_pointers(uint8_t **buf, int num_segments, int rotations); 
int threaded_assemble(int id);
int set_header(char* argv[], dada_hdu_t* hdu, char* utc);
int seconds2utc(long seconds, char* utc);

int network_thread(char* argv[], dada_hdu* hdu, float duration)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  std::cout << "Thread # network thread : on CPU " << sched_getcpu() << "\n"; 

  uint8_t *packet;
  uint8_t *mask = new uint8_t[16];
  mask[0] = 1;
  mask[1] = 1;
  mask[2] = 1;
  mask[3] = 1; 
  mask[4] = 1; 
  mask[5] = 1;
  mask[6] = 1;
  mask[7] = 1;
  mask[8] = 1;
  mask[9] = 1;
  mask[10] = 1;
  mask[11] = 1;
  mask[12] = 0;
  mask[13] = 1;
  mask[14] = 1;
  mask[15] = 1;
  
  //packet starts from 32 bytes
  packet = aligned_alloc<uint8_t>(5088); //effort to alligned the allocated memory with the 64 bit catchline
  
  class timestamp_unwrapper ts;

  static const int packet_size = 5032;
  static const int max_events = 100;
  static const int udp_port_number = 10251;
  

  int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);      
  if (sock_fd < 0)
  {
    cout << "network thread: socket() failed: " << strerror(errno) << endl;
    throw runtime_error(strerror(errno));
  }
  
  struct sockaddr_in server_address;
  memset(&server_address, 0, sizeof(server_address));
  
  server_address.sin_family = AF_INET;
  inet_pton(AF_INET, "10.11.1.200", &server_address.sin_addr);
  server_address.sin_port = htons(udp_port_number);
  
  if (::bind(sock_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
  {
    cout << "network thread: bind() failed: " << strerror(errno) << endl;
    throw runtime_error(strerror(errno));
  }
  
  int n = 16*1024*1024;
  if (setsockopt(sock_fd, SOL_SOCKET, SO_RCVBUF, (void *) &n, sizeof(n)) < 0)
  {
    cout << "network thread: setsockopt() failed: " << strerror(errno) << endl;
    throw runtime_error(strerror(errno));
  }
  
  struct epoll_event events[max_events];
  struct epoll_event ev;
  int epoll_fd = epoll_create(10);
  
  if (epoll_fd < 0)
  {
    cout << "network thread: epoll_create() failed: " << strerror(errno) << endl;
    throw runtime_error(strerror(errno));
  }
  
  ev.events = EPOLLIN;
  ev.data.fd = sock_fd;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &ev) < 0)
  {
    cout << "network thread: epoll_ctl() failed: " << strerror(errno) << endl;
    throw runtime_error(strerror(errno));
  }
  //struct timeval tv0 = get_time();
    
  
  uint64_t total_events=0, missed_events=0;  
  uint32_t *header;
  uint64_t start_sample, first_sample;
  int flag=0;
  long count=0;
  int stop=0;
  while (!stop)
  {
    uint32_t num_events = epoll_wait(epoll_fd, events, max_events, -1);

    if (num_events < 0)
    {
      cout << "network thread: epoll_wait() failed: " << strerror(errno) << endl;
      throw runtime_error(strerror(errno));
    }

      
    
    //std::cout<<"total_events: "<<total_events<< "  \n";   

    for (uint32_t i = 0; i < num_events; i++)
    {
      if (events[i].data.fd != sock_fd)
        continue;

      ssize_t bytes_read = read(sock_fd, &packet[32], packet_size);

      if (bytes_read != packet_size)
      {
        cout<<"Something is wrong. Please check \n";
        cout.flush();
      }
      
      
        
      header = reinterpret_cast<uint32_t*> (&packet[32]);
      uint64_t seq_id = ts.unwrap(header[5]);
      
       
      if(flag==0)
      {
        start_sample = seq_id;
        first_sample = start_sample;
        uint32_t h_time = ((header[0] & 0x0fffffff));
        uint32_t h_epoch = ((header[1] & 0x0f000000) >> 24);
        uint32_t h_frame = ((header[1] & 0x00ffffff));
        
        long seconds = 1483228800 + 4*86400+h_time; 
        char *utc = new char[1000];
        seconds2utc(seconds, utc);
        cout<< "time: "<<h_time<<" epoch: "<<utc<<" frame: "<<h_frame<<endl;
        printf("%s\n",utc); 
        set_header(argv, hdu, utc);
      }
      flag=1;

      int32_t slot_id = ((header[3] & 0x0ff00000) >> 20);
      int32_t link_id = ((header[3] & 0x000f0000) >> 16);
      int32_t pol = ((header[3] & 0x0000ffff));
      int32_t freq_major = (slot_id + 16*link_id);
      //std::cout<<"seq_id: "<<seq_id<<" header_id "<<header[5]<<" slot_id "<<slot_id<<" link_id "<<link_id<<" pol "<<pol<<"\n";      
      if(seq_id>=start_sample && mask[slot_id]!=0)
      {
        uint32_t sample = (seq_id-start_sample);
        uint32_t segment = sample/78125;
        sample = sample%78125;
        uint32_t jump = sample/625;
        if(segment<chunk1.nbufs-chunk1.nscratch)
        {
          //std::memcpy(&chunk1.buf[chunk1.get_index(segment)][jump*1294336+freq_major*10112+pol*5056],&packet[64],5056);
          memcpy_packet(&chunk1.buf[chunk1.get_index(segment)][jump*1294336+freq_major*10112+pol*5056],&packet[64]);
          total_events++;
          
          /* 
          if(total_events%160000==0)
          {
            std::cout<<"Estimated packet loss: "<<(1-(long double) total_events/(((long double)(seq_id-first_sample)/625.0)*256.0))*100<<" % \n"
                      <<"Missed :"<<missed_events<<"        \n";
             std::cout.flush();
          }
          */
          
        }
        else missed_events++;
 
        if(segment>(chunk1.nbufs-chunk1.nscratch)/2)
        {
          std::unique_lock<std::mutex> lk1(chunk1.m);
          chunk1.cv.wait(lk1, []{return chunk1.writeFlag;});
          chunk1.increment_offset(chunk1.nscratch);
          chunk1.writeFlag = false;
          chunk1.readFlag = true;
          chunk1.cv.notify_one();
          start_sample+=(chunk1.nscratch)*78125;
   
          if(duration<count*0.2*chunk1.nscratch) stop=1;
          
          count++;
 
        }
      }
      

    }
   


 }
 return 0;
}   


int assemble_thread(float duration)
{
  std::thread t[2];
  for(uint32_t i=0;i<2;i++) t[i] = std::thread(threaded_assemble,i);
  for(uint32_t i=0;i<2;i++) t[i].join();
  
  int stop = 0;
  long count =0;

  while(!stop)
  {
    std::chrono::system_clock::time_point start = std::chrono::high_resolution_clock::now();
  	   
    std::unique_lock<std::mutex> lk1(chunk1.m);
    chunk1.cv.wait(lk1, []{return chunk1.readFlag;});
     
    
     
    std::unique_lock<std::mutex> lk2(chunk2.m);
    chunk2.cv.wait(lk2, []{return chunk2.writeFlag;});
    
        
    //std::thread t[2];
    //std::chrono::system_clock::time_point start = std::chrono::high_resolution_clock::now();
    
    for(uint32_t i=0;i<2;i++) t[i] = std::thread(threaded_assemble,i*chunk2.nscratch/2);
    for(uint32_t i=0;i<2;i++) t[i].join();
    
    
    long nsec = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start).count();
    long double gbps = (1600000000.0)/(long double)nsec;
    std::cout<<"gbps:  "<<8*gbps<<"        \r";
    std::cout.flush(); 
    
     
    //threaded_assemble(0);
    

    chunk2.increment_offset(chunk2.nscratch);
    
    chunk2.writeFlag = false;
    chunk2.readFlag = true;
    chunk2.cv.notify_one();
    
    
     
    chunk1.writeFlag = true;
    chunk1.readFlag = false;
    chunk1.cv.notify_one();

    
    if(duration<count*0.2*chunk1.nscratch) stop=1;
    count++;
    
  }
  return 0;
}

int threaded_assemble(int t)
{
   for(int n=t;n<t+5;n++)
   {
       assembled_new(chunk1.buf[chunk1.get_index(n+chunk1.nbufs-chunk1.nscratch)], chunk2.buf[chunk2.get_index(n)]); 
       std::memset(chunk1.buf[chunk1.get_index(n+chunk1.nbufs-chunk1.nscratch)],0x88,chunk1.nbytes);
    }
    
    
  return 0;
}

int set_header(char* argv[], dada_hdu_t* hdu, char *utc)
{  
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
 
  //ascii_header_set (header,"SOURCE","%s",argv[3]); 
  ascii_header_set (header,"UTC_START","%s",utc);
  
  //ascii_header_set (header,"MJD_START","%lf",mjd);

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

}


int seconds2utc(long seconds, char* utc)
{
  long now = seconds;
  struct tm  ts;

  // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
  ts = *gmtime(&now);
  strftime(utc, 80, "%Y-%m-%d-%H:%M:%S", &ts);
  printf("%s\n", utc);
  return 0;
}

