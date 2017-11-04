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




void offset_decode(int8_t &real, int8_t &imag, uint8_t t)
{
  uint8_t b;
  
  b = t&0xf0;
  
  real = (b>>4);
  real = real&0x0f;
  imag = t&0x0f;
  real -= 8;
  imag -= 8;
}

/*
buf: one second buffer where the assembled data is copied
freq_major: frequency channel corresponding to the first byte in the packet
packet: packet without the header (5000 bytes).
sample: sample corresponding to first byte in the packet (in one second buffer there are 625*625 samples.)
pol: polarization (0 or 1)

*/

int move_data(int8_t *buf, uint8_t *packet, uint32_t freq_major, uint32_t sample, uint32_t pol)
{
  for(uint32_t s=0;s<625;s++) // 625 time stamps in the packet
  {
  	for(uint32_t c=0;c<8;c++) // 8 channels in the packet per time stamps
  	{
  		uint32_t location = (sample+s)*4096+4*(freq_major+(c*128))+2*pol;
  		offset_decode(buf[location],buf[location+1],packet[s*8+c]); 
    }
  }
  return 0;
}



