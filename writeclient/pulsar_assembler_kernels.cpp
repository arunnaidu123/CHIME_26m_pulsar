/*

_assembler_kernel_new: 

example input:


(s,p,c): It is a 8 byte word 

s: sample number or timestamp 
p: polarization (0-1)
c: channel (0-1023)


x0 = [(0,0,0), (0,0,128), (0,0,256), (0,0,384), (0,0,512), (0,0,640), (0,0,768), (0,0,896)
      (1,0,0), (1,0,128), (1,0,256), (1,0,384), (1,0,512), (1,0,640), (1,0,768), (1,0,896)]    

x1 = [(0,0,1), (0,0,129), (0,0,257), (0,0,385), (0,0,513), (0,0,641), (0,0,769), (0,0,897)
      (1,0,1), (1,0,129), (1,0,257), (1,0,385), (1,0,513), (1,0,641), (1,0,769), (1,0,897)]

x2 = [(0,0,2), (0,0,130), (0,0,258), (0,0,386), (0,0,514), (0,0,642), (0,0,770), (0,0,898)
      (1,0,2), (1,0,130), (1,0,258), (1,0,386), (1,0,514), (1,0,642), (1,0,770), (1,0,898)]

x3 = [(0,0,3), (0,0,131), (0,0,259), (0,0,387), (0,0,515), (0,0,643), (0,0,771), (0,0,899)
      (1,0,3), (1,0,131), (1,0,259), (1,0,387), (1,0,515), (1,0,643), (1,0,771), (1,0,899)]

x4 = [(0,1,0), (0,1,128), (0,1,256), (0,1,384), (0,1,512), (0,1,640), (0,1,768), (0,1,896)
      (1,1,0), (1,1,128), (1,1,256), (1,1,384), (1,1,512), (1,1,640), (1,1,768), (1,1,896)]    

x5 = [(0,1,1), (0,1,129), (0,1,257), (0,1,385), (0,1,513), (0,1,641), (0,1,769), (0,1,897)
      (1,1,1), (1,1,129), (1,1,257), (1,1,385), (1,1,513), (1,1,641), (1,1,769), (1,1,897)]

x6 = [(0,1,2), (0,1,130), (0,1,258), (0,1,386), (0,1,514), (0,1,642), (0,1,770), (0,1,898)
      (1,1,2), (1,1,130), (1,1,258), (1,1,386), (1,1,514), (1,1,642), (1,1,770), (1,1,898)]

x7 = [(0,1,3), (0,1,131), (0,1,259), (0,1,387), (0,1,515), (0,1,643), (0,1,771), (0,1,899)
      (1,1,3), (1,1,131), (1,1,259), (1,1,387), (1,1,515), (1,1,643), (1,1,771), (1,1,899)]



ouput:

d0 = [(0,0,0), (0,0,0), (0,1,0), (0,1,0), (0,0,1), (0,0,1), (0,1,1), (0,1,1)
      (0,0,2), (0,0,2), (0,1,2), (0,1,2), (0,0,3), (0,0,3), (0,1,3), (0,1,3)] 

d1 = [(0,0,128), (0,0,128), (0,1,128), (0,1,128), (0,0,129), (0,0,129), (0,1,129), (0,1,129)
      (0,0,130), (0,0,130), (0,1,130), (0,1,130), (0,0,131), (0,0,131), (0,1,131), (0,1,131)]
      
d2 = [(0,0,256), (0,0,256), (0,1,256), (0,1,256), (0,0,257), (0,0,257), (0,1,257), (0,1,257)
      (0,0,258), (0,0,258), (0,1,258), (0,1,258), (0,0,259), (0,0,259), (0,1,259), (0,1,259)]

d3 = [(0,0,384), (0,0,384), (0,1,384), (0,1,384), (0,0,385), (0,0,385), (0,1,385), (0,1,385)
      (0,0,386), (0,0,386), (0,1,386), (0,1,386), (0,0,387), (0,0,387), (0,1,387), (0,1,387)] 

d4 = [(0,0,512), (0,0,512), (0,1,512), (0,1,512), (0,0,513), (0,0,513), (0,1,513), (0,1,513)
      (0,0,514), (0,0,514), (0,1,514), (0,1,514), (0,0,515), (0,0,515), (0,1,515), (0,1,515)]
       
d5 = [(0,0,640), (0,0,640), (0,1,640), (0,1,640), (0,0,641), (0,0,641), (0,1,641), (0,1,641)
      (0,0,642), (0,0,642), (0,1,642), (0,1,642), (0,0,643), (0,0,643), (0,1,643), (0,1,643)]  
      
d6 = [(0,0,768), (0,0,768), (0,1,768), (0,1,768), (0,0,769), (0,0,769), (0,1,769), (0,1,769)
      (0,0,770), (0,0,770), (0,1,770), (0,1,770), (0,0,771), (0,0,771), (0,1,771), (0,1,771)]
 
d7 = [(0,0,896), (0,0,896), (0,1,896), (0,1,896), (0,0,897), (0,0,897), (0,1,897), (0,1,897)
      (0,0,898), (0,0,898), (0,1,898), (0,1,898), (0,0,899), (0,0,899), (0,1,899), (0,1,899)] 
      
d10 =[(1,0,0), (1,0,0), (1,1,0), (1,1,0), (1,0,1), (1,0,1), (1,1,1), (1,1,1)
      (1,0,2), (1,0,2), (1,1,2), (1,1,2), (1,0,3), (1,0,3), (1,1,3), (1,1,3)] 

d11 =[(1,0,128), (1,0,128), (1,1,128), (1,1,128), (1,0,129), (1,0,129), (1,1,129), (1,1,129)
      (1,0,130), (1,0,130), (1,1,130), (1,1,130), (1,0,131), (1,0,131), (1,1,131), (1,1,131)]
      
d12 =[(1,0,256), (1,0,256), (1,1,256), (1,1,256), (1,0,257), (1,0,257), (1,1,257), (1,1,257)
      (1,0,258), (1,0,258), (1,1,258), (1,1,258), (1,0,259), (1,0,259), (1,1,259), (1,1,259)]

d13 =[(1,0,384), (1,0,384), (1,1,384), (1,1,384), (1,0,385), (1,0,385), (1,1,385), (1,1,385)
      (1,0,386), (1,0,386), (1,1,386), (1,1,386), (1,0,387), (1,0,387), (1,1,387), (1,1,387)] 

d14 =[(1,0,512), (1,0,512), (1,1,512), (1,1,512), (1,0,513), (1,0,513), (1,1,513), (1,1,513)
      (1,0,514), (1,0,514), (1,1,514), (1,1,514), (1,0,515), (1,0,515), (1,1,515), (1,1,515)]
       
d15 =[(1,0,640), (1,0,640), (1,1,640), (1,1,640), (1,0,641), (1,0,641), (1,1,641), (1,1,641)
      (1,0,642), (1,0,642), (1,1,642), (1,1,642), (1,0,643), (1,0,643), (1,1,643), (1,1,643)]  
    
d16 =[(1,0,768), (1,0,768), (1,1,768), (1,1,768), (1,0,769), (1,0,769), (1,1,769), (1,1,769)
      (1,0,770), (1,0,770), (1,1,770), (1,1,770), (1,0,771), (1,0,771), (1,1,771), (1,1,771)]

d17  [(1,0,896), (1,0,896), (1,1,896), (1,1,896), (1,0,897), (1,0,897), (1,1,897), (1,1,897)
      (1,0,898), (1,0,898), (1,1,898), (1,1,898), (1,0,899), (1,0,899), (1,1,899), (1,1,899)] 

----------------------------------------------------------------------------------------------------------------


*/ 
#ifndef _PULSAR_ASSEMBLER_KERNELS_HPP
#define _PULSAR_ASSEMBLER_KERNELS_HPP

#include <stdint.h>
#include <sstream>

#include "emmintrin.h"
#include "tmmintrin.h"
#include "smmintrin.h"


namespace pulsar_vdif_assembler {
 



inline void _assembler_kernel_new(__m128i &x0, __m128i &x1, __m128i &x2, __m128i &x3, __m128i &x4, __m128i &x5, __m128i &x6, 
      __m128i &x7, __m128i &x10, __m128i &x11, __m128i &x12, __m128i &x13, __m128i &x14, __m128i &x15, __m128i &x16, 
      __m128i &x17, const __m128i *s0, const __m128i *s1, const __m128i *s2, const __m128i *s3, const __m128i *s4, 
      const __m128i *s5, const __m128i *s6, const __m128i *s7)
{
    __m128i a0 = _mm_loadu_si128(s0);
    __m128i a1 = _mm_loadu_si128(s1);
    __m128i a2 = _mm_loadu_si128(s2);
    __m128i a3 = _mm_loadu_si128(s3);
    __m128i a4 = _mm_loadu_si128(s4);
    __m128i a5 = _mm_loadu_si128(s5);
    __m128i a6 = _mm_loadu_si128(s6);
    __m128i a7 = _mm_loadu_si128(s7);
    
    
    static const __m128i ctl0 = _mm_set_epi8(7,7,6,6,5,5,4,4,3,3,2,2,1,1,0,0);
    static const __m128i ctl1 = _mm_set_epi8(15,15,14,14,13,13,12,12,11,11,10,10,9,9,8,8);
    
    //pol 0 sample 0
    
    __m128i b0 = _mm_shuffle_epi8(a0, ctl0);
    __m128i b1 = _mm_shuffle_epi8(a1, ctl0);
    __m128i b2 = _mm_shuffle_epi8(a2, ctl0);
    __m128i b3 = _mm_shuffle_epi8(a3, ctl0);
    
    //pol 1 sample 0
    __m128i b4 = _mm_shuffle_epi8(a4, ctl0);
    __m128i b5 = _mm_shuffle_epi8(a5, ctl0);
    __m128i b6 = _mm_shuffle_epi8(a6, ctl0);
    __m128i b7 = _mm_shuffle_epi8(a7, ctl0);
    
    //pol 0 sample 1
    __m128i b8 = _mm_shuffle_epi8(a0, ctl1);
    __m128i b9 = _mm_shuffle_epi8(a1, ctl1);
    __m128i b10 = _mm_shuffle_epi8(a2, ctl1);
    __m128i b11 = _mm_shuffle_epi8(a3, ctl1);
    
    //pol 1 sample 1
    __m128i b12 = _mm_shuffle_epi8(a4, ctl1);
    __m128i b13 = _mm_shuffle_epi8(a5, ctl1);
    __m128i b14 = _mm_shuffle_epi8(a6, ctl1);
    __m128i b15 = _mm_shuffle_epi8(a7, ctl1);
    
    
    //shuffled pol 1 sample 0
    __m128i c4 = _mm_shufflehi_epi16(_mm_shufflelo_epi16(b4, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1));
    __m128i c5 = _mm_shufflehi_epi16(_mm_shufflelo_epi16(b5, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1));
    __m128i c6 = _mm_shufflehi_epi16(_mm_shufflelo_epi16(b6, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1)); 
    __m128i c7 = _mm_shufflehi_epi16(_mm_shufflelo_epi16(b7, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1));
    
    //shuffled pol1 sample 1
    __m128i c12 = _mm_shufflehi_epi16(_mm_shufflelo_epi16(b12, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1));
    __m128i c13 = _mm_shufflehi_epi16(_mm_shufflelo_epi16(b13, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1));
    __m128i c14 = _mm_shufflehi_epi16(_mm_shufflelo_epi16(b14, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1)); 
    __m128i c15 = _mm_shufflehi_epi16(_mm_shufflelo_epi16(b15, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1));
    
    
    
    __m128i d0 = _mm_blend_epi16(b0, c4, 0xaa);  
    __m128i d1 = _mm_blend_epi16(b1, c5, 0xaa);
    __m128i d2 = _mm_blend_epi16(b2, c6, 0xaa);
    __m128i d3 = _mm_blend_epi16(b3, c7, 0xaa);
    __m128i d4 = _mm_blend_epi16(b0, c4, 0x55);
    __m128i d5 = _mm_blend_epi16(b1, c5, 0x55);
    __m128i d6 = _mm_blend_epi16(b2, c6, 0x55);  
    __m128i d7 = _mm_blend_epi16(b3, c7, 0x55);
      
       
    __m128i d10 = _mm_blend_epi16(b8, c12, 0xaa);  
    __m128i d11 = _mm_blend_epi16(b9, c13, 0xaa);
    __m128i d12 = _mm_blend_epi16(b10, c14, 0xaa);
    __m128i d13 = _mm_blend_epi16(b11, c15, 0xaa);
    __m128i d14 = _mm_blend_epi16(b8, c12, 0x55);
    __m128i d15 = _mm_blend_epi16(b9, c13, 0x55);
    __m128i d16 = _mm_blend_epi16(b10, c14, 0x55);
    __m128i d17 = _mm_blend_epi16(b11, c15, 0x55);
    
    
    d4 = _mm_shufflehi_epi16(_mm_shufflelo_epi16(d4, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1));
    d5 = _mm_shufflehi_epi16(_mm_shufflelo_epi16(d5, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1));
    d6 = _mm_shufflehi_epi16(_mm_shufflelo_epi16(d6, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1));
    d7 = _mm_shufflehi_epi16(_mm_shufflelo_epi16(d7, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1));
    d14 = _mm_shufflehi_epi16(_mm_shufflelo_epi16(d14, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1));
    d15 = _mm_shufflehi_epi16(_mm_shufflelo_epi16(d15, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1));
    d16 = _mm_shufflehi_epi16(_mm_shufflelo_epi16(d16, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1));
    d17 = _mm_shufflehi_epi16(_mm_shufflelo_epi16(d17, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1));
     
    
    b0 = _mm_blend_epi16(d0, _mm_shuffle_epi32(d1, _MM_SHUFFLE(2, 3, 0, 1)),0xcc);
    b1 = _mm_blend_epi16(d2, _mm_shuffle_epi32(d3, _MM_SHUFFLE(2, 3, 0, 1)),0xcc);
    b2 = _mm_blend_epi16(d4, _mm_shuffle_epi32(d5, _MM_SHUFFLE(2, 3, 0, 1)),0xcc);
    b3 = _mm_blend_epi16(d6, _mm_shuffle_epi32(d7, _MM_SHUFFLE(2, 3, 0, 1)),0xcc);
   
    b4 = _mm_blend_epi16(d1, _mm_shuffle_epi32(d0, _MM_SHUFFLE(2, 3, 0, 1)),0x33);
    b5 = _mm_blend_epi16(d3, _mm_shuffle_epi32(d2, _MM_SHUFFLE(2, 3, 0, 1)),0x33);
    b6 = _mm_blend_epi16(d5, _mm_shuffle_epi32(d4, _MM_SHUFFLE(2, 3, 0, 1)),0x33);
    b7 = _mm_blend_epi16(d7, _mm_shuffle_epi32(d6, _MM_SHUFFLE(2, 3, 0, 1)),0x33);
    
    
    b8 = _mm_blend_epi16(d10, _mm_shuffle_epi32(d11, _MM_SHUFFLE(2, 3, 0, 1)),0xcc);
    b9 = _mm_blend_epi16(d12, _mm_shuffle_epi32(d13, _MM_SHUFFLE(2, 3, 0, 1)),0xcc);
    b10 = _mm_blend_epi16(d14, _mm_shuffle_epi32(d15, _MM_SHUFFLE(2, 3, 0, 1)),0xcc);
    b11 = _mm_blend_epi16(d16, _mm_shuffle_epi32(d17, _MM_SHUFFLE(2, 3, 0, 1)),0xcc);
    
    b12 = _mm_blend_epi16(d11, _mm_shuffle_epi32(d10, _MM_SHUFFLE(2, 3, 0, 1)),0x33);
    b13 = _mm_blend_epi16(d13, _mm_shuffle_epi32(d12, _MM_SHUFFLE(2, 3, 0, 1)),0x33);
    b14 = _mm_blend_epi16(d15, _mm_shuffle_epi32(d14, _MM_SHUFFLE(2, 3, 0, 1)),0x33);
    b15 = _mm_blend_epi16(d17, _mm_shuffle_epi32(d16, _MM_SHUFFLE(2, 3, 0, 1)),0x33);
    
    d0 =  _mm_blend_epi16(b0, _mm_shuffle_epi32(b1, _MM_SHUFFLE(1, 0, 3, 2)),0xf0);
    d1 =  _mm_blend_epi16(b2, _mm_shuffle_epi32(b3, _MM_SHUFFLE(1, 0, 3, 2)),0xf0);
    d2 =  _mm_blend_epi16(b4, _mm_shuffle_epi32(b5, _MM_SHUFFLE(1, 0, 3, 2)),0xf0);
    d3 =  _mm_blend_epi16(b6, _mm_shuffle_epi32(b7, _MM_SHUFFLE(1, 0, 3, 2)),0xf0);
    
    d4 = _mm_blend_epi16(b1, _mm_shuffle_epi32(b0, _MM_SHUFFLE(1, 0, 3, 2)),0x0f);
    d5 = _mm_blend_epi16(b3, _mm_shuffle_epi32(b2, _MM_SHUFFLE(1, 0, 3, 2)),0x0f);
    d6 = _mm_blend_epi16(b5, _mm_shuffle_epi32(b4, _MM_SHUFFLE(1, 0, 3, 2)),0x0f);
    d7 = _mm_blend_epi16(b7, _mm_shuffle_epi32(b6, _MM_SHUFFLE(1, 0, 3, 2)),0x0f);
    
    d10 =  _mm_blend_epi16(b8, _mm_shuffle_epi32(b9, _MM_SHUFFLE(1, 0, 3, 2)),0xf0);
    d11 =  _mm_blend_epi16(b10, _mm_shuffle_epi32(b11, _MM_SHUFFLE(1, 0, 3, 2)),0xf0);
    d12 =  _mm_blend_epi16(b12, _mm_shuffle_epi32(b13, _MM_SHUFFLE(1, 0, 3, 2)),0xf0);
    d13 =  _mm_blend_epi16(b14, _mm_shuffle_epi32(b15, _MM_SHUFFLE(1, 0, 3, 2)),0xf0);
    
    d14 = _mm_blend_epi16(b9, _mm_shuffle_epi32(b8, _MM_SHUFFLE(1, 0, 3, 2)),0x0f);
    d15 = _mm_blend_epi16(b11, _mm_shuffle_epi32(b10, _MM_SHUFFLE(1, 0, 3, 2)),0x0f);
    d16 = _mm_blend_epi16(b13, _mm_shuffle_epi32(b12, _MM_SHUFFLE(1, 0, 3, 2)),0x0f);
    d17 = _mm_blend_epi16(b15, _mm_shuffle_epi32(b14, _MM_SHUFFLE(1, 0, 3, 2)),0x0f);
    
     
    
    x0 = d0;
    x1 = d1;
    x2 = d2;
    x3 = d3;
    x4 = d4;
    x5 = d5;
    x6 = d6;
    x7 = d7;
    
    x10 = d10;
    x11 = d11;
    x12 = d12;
    x13 = d13;
    x14 = d14;
    x15 = d15;
    x16 = d16;
    x17 = d17;
    
}

inline void _assembler_store_dst(uint8_t *dst, int channel, 
           __m128i x0, __m128i x1, __m128i x2, __m128i x3, __m128i x4, __m128i x5, __m128i x6, __m128i x7,
           __m128i x10, __m128i x11, __m128i x12, __m128i x13, __m128i x14, __m128i x15, __m128i x16, __m128i x17)
{
    int location = channel*4;
     
     
    
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location]), x0);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*1]), x1);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*2]), x2);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*3]), x3);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*4]), x4);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*5]), x5);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*6]), x6);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*7]), x7);
    
    location = 4096+channel*4;
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location]), x10);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*1]), x11);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*2]), x12);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*3]), x13);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*4]), x14);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*5]), x15);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*6]), x16);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*7]), x17);
}

inline void _assembler_store_dst_partial(uint8_t *dst, int channel, 
           __m128i x0, __m128i x1, __m128i x2, __m128i x3, __m128i x4, __m128i x5, __m128i x6, __m128i x7)
{
    int location = channel*4;
    
    
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location]), x0);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*1]), x1);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*2]), x2);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*3]), x3);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*4]), x4);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*5]), x5);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*6]), x6);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*7]), x7);
     
     
}

inline void prefetch(uint8_t *dst, int channel)
{
  int location = channel*4;
   
	  // Prefetch to L1 cache (_MM_HINT_T0)
	  _mm_prefetch(&dst[location], _MM_HINT_T0);
	  _mm_prefetch(&dst[location+512*1], _MM_HINT_T0);
	  _mm_prefetch(&dst[location+512*2], _MM_HINT_T0);
	  _mm_prefetch(&dst[location+512*3], _MM_HINT_T0);
	  _mm_prefetch(&dst[location+512*4], _MM_HINT_T0);
	  _mm_prefetch(&dst[location+512*5], _MM_HINT_T0);
	  _mm_prefetch(&dst[location+512*6], _MM_HINT_T0);
	  _mm_prefetch(&dst[location+512*7], _MM_HINT_T0);
	  
	  location = 4096+channel*4;
	  
	  // Prefetch to L1 cache (_MM_HINT_T0)
	  _mm_prefetch(&dst[location], _MM_HINT_T0);
	  _mm_prefetch(&dst[location+512*1], _MM_HINT_T0);
	  _mm_prefetch(&dst[location+512*2], _MM_HINT_T0);
	  _mm_prefetch(&dst[location+512*3], _MM_HINT_T0);
	  _mm_prefetch(&dst[location+512*4], _MM_HINT_T0);
	  _mm_prefetch(&dst[location+512*5], _MM_HINT_T0);
	  _mm_prefetch(&dst[location+512*6], _MM_HINT_T0);
	  _mm_prefetch(&dst[location+512*7], _MM_HINT_T0);
	
}



inline void assembled_new(uint8_t *src_buf, uint8_t *dst_buf)
{
  
  __m128i x0, x1, x2, x3,x4, x5,x6, x7, x10, x11, x12, x13, x14, x15, x16, x17;
   
  uint8_t *temp_src = new uint8_t[16*8];
  
  for(int sample=0;sample<390625;sample+=625)
  {
  	for(int freq_major=0; freq_major<128; freq_major+=4)
  	{
  		for(int s=0; s<624;s+=2)
  		{
      	prefetch(&dst_buf[(sample+s)*4096],freq_major);
      	
      	_assembler_kernel_new(x0, x1, x2, x3, x4, x5, x6, x7, x10, x11, x12, x13, x14, x15, x16, x17, 
      	 reinterpret_cast<__m128i *> (&src_buf[sample*2048+freq_major*10000+s*8]), 
      	 reinterpret_cast<__m128i *> (&src_buf[sample*2048+(freq_major+1)*10000+(s)*8]),
      	 reinterpret_cast<__m128i *> (&src_buf[sample*2048+(freq_major+2)*10000+(s)*8]),
      	 reinterpret_cast<__m128i *> (&src_buf[sample*2048+(freq_major+3)*10000+(s)*8]),
      	 reinterpret_cast<__m128i *> (&src_buf[sample*2048+freq_major*10000+s*8+5000]), 
      	 reinterpret_cast<__m128i *> (&src_buf[sample*2048+(freq_major+1)*10000+(s)*8+5000]),
      	 reinterpret_cast<__m128i *> (&src_buf[sample*2048+(freq_major+2)*10000+(s)*8+5000]),
      	 reinterpret_cast<__m128i *> (&src_buf[sample*2048+(freq_major+3)*10000+(s)*8+5000]));
      	 
      	 _assembler_store_dst(&dst_buf[(sample+s)*4096], freq_major, x0, x1, x2, x3, x4, x5, x6, x7,
           x10, x11, x12, x13, x14, x15, x16, x17); 
      }
      
      for(int i=0; i<8; i++)
      {
        int s=624;
        int pol = i/4;
        int c = i%4;
        std::memcpy(&temp_src[16*i],&src_buf[sample*2048+(freq_major+c)*10000+s*8+pol*5000],8);
      }
      
      prefetch(&dst_buf[(sample+624)*4096],freq_major);
      
      _assembler_kernel_new(x0, x1, x2, x3, x4, x5, x6, x7, x10, x11, x12, x13, x14, x15, x16, x17, 
      	 reinterpret_cast<__m128i *> (&temp_src[0]), 
      	 reinterpret_cast<__m128i *> (&temp_src[16*1]),
      	 reinterpret_cast<__m128i *> (&temp_src[16*2]),
      	 reinterpret_cast<__m128i *> (&temp_src[16*3]),
      	 reinterpret_cast<__m128i *> (&temp_src[16*4]),
      	 reinterpret_cast<__m128i *> (&temp_src[16*5]),
      	 reinterpret_cast<__m128i *> (&temp_src[16*6]),
      	 reinterpret_cast<__m128i *> (&temp_src[16*7]));
      	    
      _assembler_store_dst_partial(&dst_buf[(sample+624)*4096], freq_major, x0, x1, x2, x3, x4, x5, x6, x7);   
    }
    
  }
  
}		

 
#endif 
