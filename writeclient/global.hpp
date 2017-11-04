#include <iostream>
#include <cstring>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>


//
// The 32-bit FPGA counts used as timestamps "wrap around" every 3 hours.

// This helper class converts a stream of unsigned 32-bit FPGA counts to
// a stream of signed 64-bit timestamps without wraparound.  It correctly
// handles out-of-order timestamps as long as the "jitter" isn't more than
// 2^31 timestamps (which is an unrealistic case).


class timestamp_unwrapper 
{

private:
    int64_t last_timestamp;

public:
    timestamp_unwrapper()
    {
      // this initial value will mean that the first timestamp is in the range [0,2^32-1]
      last_timestamp = int64_t(1) << 31;
    }
        
    inline int64_t unwrap(uint32_t x)
    {
      int32_t delta = (int32_t)x - (int32_t)last_timestamp;
      last_timestamp += (int64_t)delta;
      return last_timestamp;
    }
};


// Allocate memmory aligned with the 64 byte lines. Useful for fast memory transfers
template<typename T>
inline T *aligned_alloc(size_t nelts)
{
    if (nelts == 0)
        return NULL;

  // align to 64-byte cache lines
  void *p = NULL;
  if (posix_memalign(&p, 64, nelts * sizeof(T)) != 0)
  throw std::runtime_error("couldn't allocate memory");
  
  memset(p, 0x88, nelts * sizeof(T));
  return reinterpret_cast<T *> (p);
}

// Allocates buffer in segments. Useful if we are planning for allocating large buffers
// this structure is used through out the code.
// These buffers are not as efficient as the psrdada buffers but will suffice this purpose
struct data_chunk
{
  std::condition_variable cv;
  std::mutex m;
  uint8_t **buf;
  int nbufs=1;
  long nbytes=1;
  bool readFlag=false;
  bool writeFlag=true;
  uint32_t offset=0;
  uint32_t nscratch = 10; 
  // allocated n chunks in size n bytes each.
  int allocate_data_chunk (long bytes, int n)
  {
    nbytes = bytes;
    nbufs = n;
    buf = new uint8_t*[nbufs];
    for(int i=0;i<nbufs;i++) 
    {
      buf[i] = aligned_alloc<uint8_t>(nbytes);
    }
    return 0;
  }
  ~data_chunk()
  {
    for(int i=0; i<nbufs; i++) delete buf[i];
    delete buf;
  }
  
  // Idea is to use this data chunks as ring buffers.
  // It is better use the these functions to increment the offset.
  int increment_offset(int n)
  {
    offset = offset+n;
    if(offset>=nbufs) offset=offset%nbufs;
    return 0;
  }
  // the index of the chunk is determined from the offset
  int get_index(int n)
  {
    int value = offset+n;
    if(value>=nbufs) value = value%nbufs;
    return value;
  }
};


extern struct data_chunk chunk1;
extern struct data_chunk chunk2;


 
