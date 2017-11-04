//
// Assembly language kernels.
//
// FIXME: generally speaking, the kernels here could use some work, and some
// kernels remain to be written (see comments in 'class assembled_chunk').
//
// A performance puzzle: the assembly-language-kernel-enabled assembler runs 5 
// times faster on moose (3.5 GHz Haswell-E, gcc 4.4.7) than on my desktop 
// (2.4 GHz Haswell, gcc 4.8.3)!  This seems worth understanding and fixing, 
// but I haven't gotten to the bottom of it yet.  In the meantime, if you use 
// moose you should be fine, but you may find poor performance on other machines.  
//
// For a performance test, do 
//    ./run-vdif-assembler -t.
//
// This gives ~13 Gbps on moose and 2.5 Gbps on my desktop, where 6.4 Gbps is 
// the minimum needed to keep up with a real-time network capture.
//

#ifndef _CH_VDIF_ASSEMBLER_KERNELS_HPP
#define _CH_VDIF_ASSEMBLER_KERNELS_HPP

#include <stdint.h>
#include <sstream>

// Kernels in this file assume 128-bit SSE4.1 but not 256-bit AVX
#include "emmintrin.h"
#include "tmmintrin.h"
#include "smmintrin.h"


namespace ch_vdif_assembler {
#if 0
}; // pacify emacs c-mode
#endif


// useful for debugging
inline std::string str8(__m128i x)
{
    std::stringstream ret;

    ret << "[" << std::hex
	<< " " << _mm_extract_epi8(x,0)
	<< " " << _mm_extract_epi8(x,1)
	<< " " << _mm_extract_epi8(x,2)
	<< " " << _mm_extract_epi8(x,3)
	<< " " << _mm_extract_epi8(x,4)
	<< " " << _mm_extract_epi8(x,5)
	<< " " << _mm_extract_epi8(x,6)
	<< " " << _mm_extract_epi8(x,7)
	<< " " << _mm_extract_epi8(x,8)
	<< " " << _mm_extract_epi8(x,9)
	<< " " << _mm_extract_epi8(x,10)
	<< " " << _mm_extract_epi8(x,11)
	<< " " << _mm_extract_epi8(x,12)
	<< " " << _mm_extract_epi8(x,13)
	<< " " << _mm_extract_epi8(x,14)
	<< " " << _mm_extract_epi8(x,15)
	<< " ]";

    return ret.str();
}


inline std::string str16(__m128i x)
{
    std::stringstream ret;

    ret << "[" << std::hex
	<< " " << _mm_extract_epi16(x,0)
	<< " " << _mm_extract_epi16(x,1)
	<< " " << _mm_extract_epi16(x,2)
	<< " " << _mm_extract_epi16(x,3)
	<< " " << _mm_extract_epi16(x,4)
	<< " " << _mm_extract_epi16(x,5)
	<< " " << _mm_extract_epi16(x,6)
	<< " " << _mm_extract_epi16(x,7)
	<< " ]";

    return ret.str();
}


// useful for debugging
inline bool equal128(__m128i x, __m128i y)
{
    return ((_mm_extract_epi32(x,0) == _mm_extract_epi32(y,0)) &&
	    (_mm_extract_epi32(x,1) == _mm_extract_epi32(y,1)) &&
	    (_mm_extract_epi32(x,2) == _mm_extract_epi32(y,2)) &&
	    (_mm_extract_epi32(x,3) == _mm_extract_epi32(y,3)));
}


// -------------------------------------------------------------------------------------------------
//
// Assembler kernel


// 8-bit assembler, for reference
inline void _assemble8(uint8_t *dst, int stride, const uint8_t *src, int n)
{
     
    for (int i = 0; i < n; i++) {
	dst[i] = src[8*i];
	dst[i+stride] = src[8*i+1];
	dst[i+2*stride] = src[8*i+2];
	dst[i+3*stride] = src[8*i+3];
	dst[i+4*stride] = src[8*i+4];
	dst[i+5*stride] = src[8*i+5];
	dst[i+6*stride] = src[8*i+6];
	dst[i+7*stride] = src[8*i+7];
    }
     
}


// This is ready to be ported to AVX2, by adding 8 permute instructions (see also XXX below)
inline void _assembler_kernel(__m128i &x0, __m128i &x1, __m128i &x2, __m128i &x3, __m128i &x4, __m128i &x5, __m128i &x6, __m128i &x7, const __m128i *src)
{
    __m128i a0 = _mm_loadu_si128(src);
    __m128i a1 = _mm_loadu_si128(src+1);
    __m128i a2 = _mm_loadu_si128(src+2);
    __m128i a3 = _mm_loadu_si128(src+3);
    __m128i a4 = _mm_loadu_si128(src+4);
    __m128i a5 = _mm_loadu_si128(src+5);
    __m128i a6 = _mm_loadu_si128(src+6);
    __m128i a7 = _mm_loadu_si128(src+7);

    static const __m128i ctl0 = _mm_set_epi8(15,7,14,6,13,5,12,4,11,3,10,2,9,1,8,0);
    static const __m128i ctl1 = _mm_set_epi8(14,6,15,7,12,4,13,5,10,2,11,3,8,0,9,1);

    // Note: _mm_shuffle_epi8 is expensive, so we use 8 calls, which is the minimum possible
    a0 = _mm_shuffle_epi8(a0, ctl0);
    a1 = _mm_shuffle_epi8(a1, ctl1);
    a2 = _mm_shuffle_epi8(a2, ctl0);
    a3 = _mm_shuffle_epi8(a3, ctl1);
    a4 = _mm_shuffle_epi8(a4, ctl0);
    a5 = _mm_shuffle_epi8(a5, ctl1);
    a6 = _mm_shuffle_epi8(a6, ctl0);
    a7 = _mm_shuffle_epi8(a7, ctl1);

    __m128i b0 = _mm_blend_epi16(a0, a1, 0xaa);  // (10101010)_2
    __m128i b1 = _mm_blend_epi16(a1, a0, 0xaa);
    __m128i b2 = _mm_blend_epi16(a2, a3, 0xaa);
    __m128i b3 = _mm_blend_epi16(a3, a2, 0xaa);
    __m128i b4 = _mm_blend_epi16(a4, a5, 0xaa);
    __m128i b5 = _mm_blend_epi16(a5, a4, 0xaa);
    __m128i b6 = _mm_blend_epi16(a6, a7, 0xaa);
    __m128i b7 = _mm_blend_epi16(a7, a6, 0xaa);

    b1 = _mm_shufflelo_epi16(_mm_shufflehi_epi16(b1, 0xb1), 0xb1);
    b5 = _mm_shufflelo_epi16(_mm_shufflehi_epi16(b5, 0xb1), 0xb1);

    b2 = _mm_shuffle_epi32(b2, 0xb1);   // (2301)_4
    b6 = _mm_shuffle_epi32(b6, 0xb1);   // (2301)_4

    b3 = _mm_shufflelo_epi16(_mm_shufflehi_epi16(b3, 0x1b), 0x1b);  // (0123)_4
    b7 = _mm_shufflelo_epi16(_mm_shufflehi_epi16(b7, 0x1b), 0x1b);

    // XXX when switching to AVX2, replace blend_epi16(0xcc) -> blend_epi32(0xa) for a small performance boost
    a0 = _mm_blend_epi16(b0, b2, 0xcc);  // (11001100)_2
    a2 = _mm_blend_epi16(b2, b0, 0xcc);
    a1 = _mm_blend_epi16(b1, b3, 0xcc);
    a3 = _mm_blend_epi16(b3, b1, 0xcc);
    a4 = _mm_blend_epi16(b4, b6, 0xcc);
    a6 = _mm_blend_epi16(b6, b4, 0xcc);
    a5 = _mm_blend_epi16(b5, b7, 0xcc);
    a7 = _mm_blend_epi16(b7, b5, 0xcc);

    a2 = _mm_shuffle_epi32(a2, 0xb1);   // (2301)_4
    a3 = _mm_shuffle_epi32(a3, 0xb1);   // (2301)_4

    a4 = _mm_shuffle_epi32(a4, 0x4e);   // (1032)_4
    a5 = _mm_shuffle_epi32(a5, 0x4e);   // (1032)_4

    a6 = _mm_shuffle_epi32(a6, 0x1b);   // (0123)_4
    a7 = _mm_shuffle_epi32(a7, 0x1b);   // (0123)_4

    // XXX when switching to AVX2, replace blend_epi16(0xf0) -> blend_epi32(0xc) for a small performance boost
    b0 = _mm_blend_epi16(a0, a4, 0xf0);  // (11110000)_2
    b4 = _mm_blend_epi16(a4, a0, 0xf0);  // (11110000)_2
    b1 = _mm_blend_epi16(a1, a5, 0xf0);  // (11110000)_2
    b5 = _mm_blend_epi16(a5, a1, 0xf0);  // (11110000)_2
    b2 = _mm_blend_epi16(a2, a6, 0xf0);  // (11110000)_2
    b6 = _mm_blend_epi16(a6, a2, 0xf0);  // (11110000)_2
    b3 = _mm_blend_epi16(a3, a7, 0xf0);  // (11110000)_2
    b7 = _mm_blend_epi16(a7, a3, 0xf0);  // (11110000)_2

    b4 = _mm_shuffle_epi32(b4, 0x4e);   // (1032)_4
    b5 = _mm_shuffle_epi32(b5, 0x4e);   // (1032)_4
    b6 = _mm_shuffle_epi32(b6, 0x4e);   // (1032)_4
    b7 = _mm_shuffle_epi32(b7, 0x4e);   // (1032)_4

    x0 = b0;
    x1 = b1;
    x2 = b2;
    x3 = b3;
    x4 = b4;
    x5 = b5;
    x6 = b6;
    x7 = b7;
}


inline void _assembler_store_full(__m128i *dst, int s, __m128i x0, __m128i x1, __m128i x2, __m128i x3, __m128i x4, __m128i x5, __m128i x6, __m128i x7)
{
    _mm_storeu_si128(dst, x0);
    _mm_storeu_si128(dst+s, x1);
    _mm_storeu_si128(dst+2*s, x2);
    _mm_storeu_si128(dst+3*s, x3);
    _mm_storeu_si128(dst+4*s, x4);
    _mm_storeu_si128(dst+5*s, x5);
    _mm_storeu_si128(dst+6*s, x6);
    _mm_storeu_si128(dst+7*s, x7);
}


//
// If we assumed AVX512, this routine could be implemented more efficiently
// and straightforwardly with _mm_mask_storeu_epi8().
//
inline void _assembler_store_partial(__m128i *dst, int s, int p, __m128i x0, __m128i x1, __m128i x2, __m128i x3, __m128i x4, __m128i x5, __m128i x6, __m128i x7)
{
    static const __m128i r = _mm_set_epi8(15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0);

    __m128i mask = _mm_cmplt_epi8(r, _mm_set1_epi8(p));

    x0 = _mm_and_si128(mask, x0);
    x1 = _mm_and_si128(mask, x1);
    x2 = _mm_and_si128(mask, x2);
    x3 = _mm_and_si128(mask, x3);
    x4 = _mm_and_si128(mask, x4);
    x5 = _mm_and_si128(mask, x5);
    x6 = _mm_and_si128(mask, x6);
    x7 = _mm_and_si128(mask, x7);

    __m128i y0 = _mm_loadu_si128(dst);
    __m128i y1 = _mm_loadu_si128(dst+s);
    __m128i y2 = _mm_loadu_si128(dst+2*s);
    __m128i y3 = _mm_loadu_si128(dst+3*s);
    __m128i y4 = _mm_loadu_si128(dst+4*s);
    __m128i y5 = _mm_loadu_si128(dst+5*s);
    __m128i y6 = _mm_loadu_si128(dst+6*s);
    __m128i y7 = _mm_loadu_si128(dst+7*s);
    
    y0 = _mm_andnot_si128(mask, y0);
    y1 = _mm_andnot_si128(mask, y1);
    y2 = _mm_andnot_si128(mask, y2);
    y3 = _mm_andnot_si128(mask, y3);
    y4 = _mm_andnot_si128(mask, y4);
    y5 = _mm_andnot_si128(mask, y5);
    y6 = _mm_andnot_si128(mask, y6);
    y7 = _mm_andnot_si128(mask, y7);
    
    y0 = _mm_or_si128(x0, y0);
    y1 = _mm_or_si128(x1, y1);
    y2 = _mm_or_si128(x2, y2);
    y3 = _mm_or_si128(x3, y3);
    y4 = _mm_or_si128(x4, y4);
    y5 = _mm_or_si128(x5, y5);
    y6 = _mm_or_si128(x6, y6);
    y7 = _mm_or_si128(x7, y7);

    _assembler_store_full(dst, s, y0, y1, y2, y3, y4, y5, y6, y7);
}


// 128-bit assembler: should do the same thing as _assemble8() above
inline void _assemble128(uint8_t *dst, int stride, const uint8_t *src, int n)
{
    // this case occurs about half the time, so we put the test up front
    if (n <= 0)
	return;

    // OK to assume evenly divisible
    int s = stride/16;

    __m128i x0, x1, x2, x3, x4, x5, x6, x7;
    _assembler_kernel(x0, x1, x2, x3, x4, x5, x6, x7, reinterpret_cast<const __m128i *> (src));

    if (n < 16) {
	_assembler_store_partial(reinterpret_cast<__m128i *> (dst), s, n, x0, x1, x2, x3, x4, x5, x6, x7);
	return;
    }

    _assembler_store_full(reinterpret_cast<__m128i *> (dst), s, x0, x1, x2, x3, x4, x5, x6, x7);

    int n16 = (n-1)/16;
    int m = n - 16*n16;
    dst += m;
    src += 8*m;

    for (int i = 0; i < n16; i++) {
	_assembler_kernel(x0, x1, x2, x3, x4, x5, x6, x7, reinterpret_cast<const __m128i *> (src + 128*i));
	_assembler_store_full(reinterpret_cast<__m128i *> (dst + 16*i), s, x0, x1, x2, x3, x4, x5, x6, x7);
    }
}


// -------------------------------------------------------------------------------------------------


// only used in slow reference kernels, not fast assembly-language kernels
static inline void offset_decode(int &re, int &im, uint8_t byte)
{
    re = (int)((byte & 0xf0) >> 4) - 8;
    im = (int)(byte & 0x0f) - 8;
}

static inline void copy_packet(uint8_t *buffer, uint8_t *packet)
{
  for(int i=0; i<312; i++)
  {
    __m128i x = _mm_loadu_si128(reinterpret_cast<const __m128i *> (&packet[i*16]));
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&buffer[i*16]), x);
  }
  
  std::memcpy(&buffer[4992],&packet[4992],8);   

}



inline void _sum16_auto_correlations_reference(int &sum, int &count, const uint8_t *buf)
{
    sum = count = 0;
    
    for (int i = 0; i < 16; i++) {
	if (buf[i] != 0) {
	    int re, im;
	    offset_decode(re, im, buf[i]);
	    sum += (re*re + im*im);
	    count += 1;
	}
    }
}

inline void _sum16_auto_correlations(int &sum, int &count, const uint8_t *buf)
{
    __m128i x = _mm_loadu_si128(reinterpret_cast<const __m128i *> (buf));
    __m128i mask_invalid = _mm_cmpeq_epi8(x, _mm_set1_epi8(0));
    
    // take "horizontal" sum of mask to get the count
    __m128i s = _mm_andnot_si128(mask_invalid, _mm_set1_epi8(1));
    s = _mm_add_epi8(s, _mm_srli_si128(s,1));
    s = _mm_add_epi8(s, _mm_srli_si128(s,2));
    s = _mm_add_epi8(s, _mm_srli_si128(s,4));
    s = _mm_add_epi8(s, _mm_srli_si128(s,8));
    count = _mm_extract_epi8(s, 0);

    // replace invalid bytes by 0x88
    x = _mm_andnot_si128(mask_invalid, x);
    x = _mm_or_si128(x, _mm_and_si128(mask_invalid, _mm_set1_epi8(0x88)));

    // Each of the next 4 stanzas computes eight 16-bit numbers

    __m128i xim_lo = _mm_and_si128(x, _mm_set1_epi16(0x000f));
    xim_lo = _mm_sub_epi16(xim_lo, _mm_set1_epi16(0x0008));
    __m128i xim2_lo = _mm_mullo_epi16(xim_lo, xim_lo);

    __m128i xim_hi = _mm_and_si128(x, _mm_set1_epi16(0x0f00));
    xim_hi = _mm_sub_epi16(xim_hi, _mm_set1_epi16(0x0800));
    __m128i xim2_hi = _mm_mulhi_epi16(xim_hi, xim_hi);

    __m128i xre_lo = _mm_and_si128(x, _mm_set1_epi16(0x00f0));
    xre_lo = _mm_srli_epi16(xre_lo, 4);
    xre_lo = _mm_sub_epi16(xre_lo, _mm_set1_epi16(0x0008));
    __m128i xre2_lo = _mm_mullo_epi16(xre_lo, xre_lo);

    __m128i xre_hi = _mm_and_si128(x, _mm_set1_epi16(0xf000));
    xre_hi = _mm_srli_epi16(xre_hi, 12);
    xre_hi = _mm_sub_epi16(xre_hi, _mm_set1_epi16(0x0008));
    __m128i xre2_hi = _mm_mullo_epi16(xre_hi, xre_hi);

    __m128i xim2 = _mm_add_epi16(xim2_lo, xim2_hi);
    __m128i xre2 = _mm_add_epi16(xre2_lo, xre2_hi);
    __m128i x2 = _mm_add_epi16(xre2, xim2);

    // "horizontal" sum of 8 16-bit integers
    x2 = _mm_add_epi16(x2, _mm_srli_si128(x2,2));
    x2 = _mm_add_epi16(x2, _mm_srli_si128(x2,4));
    x2 = _mm_add_epi16(x2, _mm_srli_si128(x2,8));
    sum = _mm_extract_epi16(x2, 0);
}


}  // namespace ch_vdif_assembler

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


inline void decode_dst(uint8_t *dst)
{
  for(int i=0; i<8; i++)
  {
    dst[2*i] = ((int8_t)((dst[2*i]&0xf0)>>4)-8);
    dst[2*i+1] = ((int8_t)(dst[2*i+1]&0x0f)-8);
  }
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
    
    //for(int i=0; i<8; i++) decode_dst(&dst[location+512*i]);
    location = 4096+channel*4;
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location]), x10);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*1]), x11);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*2]), x12);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*3]), x13);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*4]), x14);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*5]), x15);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*6]), x16);
    _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[location+512*7]), x17);
    //for(int i=0; i<8; i++) decode_dst(&dst[location+512*i]);
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

  for(int jump=0;jump<125;jump++)
  {
        for(int s=0; s<625;s+=2)
                {

        for(int freq_major=0; freq_major<128; freq_major+=4)
        {

        int sample = jump*625;
        if(freq_major%16==0) prefetch(&dst_buf[(sample+s)*4096],freq_major);

        _assembler_kernel_new(x0, x1, x2, x3, x4, x5, x6, x7, x10, x11, x12, x13, x14, x15, x16, x17,
         reinterpret_cast<__m128i *> (&src_buf[1294336*jump+freq_major*10112+s*8]),
         reinterpret_cast<__m128i *> (&src_buf[1294336*jump+(freq_major+1)*10112+(s)*8]),
         reinterpret_cast<__m128i *> (&src_buf[1294336*jump+(freq_major+2)*10112+(s)*8]),
         reinterpret_cast<__m128i *> (&src_buf[1294336*jump+(freq_major+3)*10112+(s)*8]),
         reinterpret_cast<__m128i *> (&src_buf[1294336*jump+freq_major*10112+s*8+5056]),
         reinterpret_cast<__m128i *> (&src_buf[1294336*jump+(freq_major+1)*10112+(s)*8+5056]),
         reinterpret_cast<__m128i *> (&src_buf[1294336*jump+(freq_major+2)*10112+(s)*8+5056]),
         reinterpret_cast<__m128i *> (&src_buf[1294336*jump+(freq_major+3)*10112+(s)*8+5056]));

        if(s<624)  _assembler_store_dst(&dst_buf[(sample+s)*4096], freq_major, x0, x1, x2, x3, x4, x5, x6, x7,
           x10, x11, x12, x13, x14, x15, x16, x17);
        else _assembler_store_dst_partial(&dst_buf[(sample+624)*4096], freq_major, x0, x1, x2, x3, x4, x5, x6, x7);
      }

     }

  }

}






inline void prefetch_packet(uint8_t *dst, int i)
{
  int location = i*64;
   // Prefetch to L1 cache (_MM_HINT_T0)
  _mm_prefetch(&dst[location], _MM_HINT_T0);
}



inline void memcpy_packet(uint8_t *dst, uint8_t *src)
{
        
    for(int i=0;i<79;i++)
    {
      __m128i a0  = _mm_loadu_si128(reinterpret_cast<__m128i *> (&src[i*64]));
      __m128i a1  = _mm_loadu_si128(reinterpret_cast<__m128i *> (&src[i*64+16]));
      __m128i a2  = _mm_loadu_si128(reinterpret_cast<__m128i *> (&src[i*64+32]));
      __m128i a3  = _mm_loadu_si128(reinterpret_cast<__m128i *> (&src[i*64+48]));
      
      //prefetch_packet(dst,i);

      _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[i*64]), a0);
      _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[i*64+16]), a1);
      _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[i*64+32]), a2);
      _mm_storeu_si128(reinterpret_cast<__m128i *> (&dst[i*64+48]), a3);
    }
    return;
}  
    
inline void alt_memset(uint8_t *src,int N)
{
  static const __m128i value = _mm_set_epi8(0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88);
  for(int i=0;i<N/16;i++) _mm_storeu_si128(reinterpret_cast<__m128i *> (&src[i*16]), value);
  return;

}    
 
#endif  // _CH_VDIF_ASSEMBLER_KERNELS_HPP
