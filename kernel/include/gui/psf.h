#ifndef GUI_PSF_H_
#define GUI_PSF_H_

#include<stdint.h>

#define PSF_FONT_MAGIC 0x864ab572
 
typedef struct PSF_font {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
} PSF_font;

void init_psf();

#endif // GUI_PSF_H_