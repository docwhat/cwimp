// 16 bit Mersenne Twister
// Originally designed by Makoto Matsumoto and Takuji Nishimura
// Accelerations by Shawn Cokus
// 16 bit version by Troy D. Gillette
/* Modified for use on a Palm by Christian Höltje */

#include<stdio.h>

/* Period parameters */  
#define N              (312)             // length of state vector
#define M              (197)             // a period parameter
#define K              (0x98BFU)         // a magic constant
#define hiBit(u)       ((u) & 0x8000U)   // mask all but highest   bit of u
#define loBit(u)       ((u) & 0x0001U)   // mask all but lowest    bit of u
#define loBits(u)      ((u) & 0x7FFFU)   // mask     the highest   bit of u
#define mixBits(u, v)  (hiBit(u)|loBits(v))  // move hi bit of u to hi bit of v

static unsigned short   state[N+1];     // state vector + 1 extra to not violate ANSI C
static unsigned short   *next;          // next random value is computed from here
static int      		left = -1;      // can *next++ this many times before reloading

void sgenrand(unsigned short seed)
{
    register unsigned short x = (seed | 1U) & 0xFFFFU, *s = state;
    register int    j;

    for(left=0, *s++=x, j=N; --j;
        *s++ = (x*=269U) & 0xFFFFU);
}

static unsigned short reloadMT(void)
{
	register unsigned short *p0=state, *p2=state+2, *pM=state+M, s0, s1;
	register int    j;
	
	if(left < -1)
		sgenrand(2176U);
	
	left=N-1, next=state+1;
	
	for(s0=state[0], s1=state[1], j=N-M+1; --j; s0=s1, s1=*p2++)
		*p0++ = *pM++ ^ (mixBits(s0, s1) >> 1) ^ (loBit(s1) ? K : 0U);
	
	for(pM=state, j=M; --j; s0=s1, s1=*p2++)
		*p0++ = *pM++ ^ (mixBits(s0, s1) >> 1) ^ (loBit(s1) ? K : 0U);
	
	s1=state[0], *p0 = *pM ^ (mixBits(s0, s1) >> 1) ^ (loBit(s1) ? K : 0U);
	s1 ^= (s1 >>  6);
	s1 ^= (s1 <<  3) & 0x9D60U;
	s1 ^= (s1 <<  7) & 0xEC00U;
	return(s1 ^ (s1 >> 9));
}

unsigned short genrand(void)
{
    unsigned short y;

    if(--left < 0)
        return(reloadMT());

    y  = *next++;
    y ^= (y >>  6);
    y ^= (y <<  3) & 0x9D2C5680U;
    y ^= (y <<  7) & 0xEFC60000U;
    return(y ^ (y >> 9));
}

