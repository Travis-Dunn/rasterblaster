#ifndef SF610_H
#define SF610_H

/* 16 bit fixed point type for mesh storage 
 * 6.10 format gives range -32.0f to +31.999f with approx. 0.001 precision */

/* params: float */
/* returns: short */
#define FLOAT_TO_SF610(f)     ((short)((f) * 0x0400))

/* params: float */
/* returns: short */
#define SF610_TO_FLOAT(x)     ((float)(x) / 1024.f)

#endif /* SF610_H */
