#ifndef RGB565_H
#define RGB565_H

/* params: 3 unsigned chars */
/* returns: short */
#define RGB565_MAKE(R, G, B) (short)                                           \
    (((R & 0xF8) << 8 | ((G & 0xFC) << 3) | (B >> 3)))                         \

/* params: short */
/* returns : unsigned char */
#define RGB565_GETR(c) (unsigned char)((c >> 11) & 0x1F)
#define RGB565_GETG(c) (unsigned char)((c >> 5 ) & 0x3F)
#define RGB565_GETB(c) (unsigned char)( c        & 0x1F)

#endif /* RGB565_H */
