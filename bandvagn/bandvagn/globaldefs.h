#ifndef globaldefs

#define F_CPU 8000000UL

#define SETBIT(ADDR,BIT) (ADDR |= (1<<BIT))
#define CLEARBIT(ADDR,BIT) (ADDR &= ~(1<<BIT))
#define FLIPBIT(ADDR,BIT) (ADDR ^= (1<<BIT))

#endif