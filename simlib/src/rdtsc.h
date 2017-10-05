/*
 *  rdtsc.h
 *  -----------------------------------------------------------------
 *  function for reading 64 bit timer (x86: Pentium, K6, etc.)
 *  for GNU C/C++ only
 *
 */
#ifndef __GNUC__
#error "Use GNU C, please"
#endif

// universal x86, x86-64 code:
static __inline__ unsigned long long rdtsc(void)
{
    unsigned a, d;
    __asm volatile ( "rdtsc" : "=a" (a), "=d" (d) ); // compiler-dependent
    return ((unsigned long long) a) | (((unsigned long long) d) << 32);
}

