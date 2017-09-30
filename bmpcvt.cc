#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <assert.h>

#include "bmdisp.h"

// Windows BMP file structs

struct BITMAPFILEHEADER 
{
    short   bfType;
    long    bfSize;
    short   bfReserved1;
    short   bfReserved2;
    long   bfOffBits;
};

struct BITMAPINFOHEADER 
{
    long  biSize;
    long  biWidth;
    long  biHeight;
    short biPlanes;
    short biBitCount;
    long  biCompression;
    long  biSizeImage;
    long  biXPelsPerMeter;
    long  biYPelsPerMeter;
    long  biClrUsed;
    long  biClrImportant;
};

struct RGBQUAD 
{
    char    rgbBlue;
    char    rgbGreen;
    char    rgbRed;
    char    rgbReserved;
};

BMDisplay::BMDisplay(int width_in, int height_in)
    : width(width_in), height(height_in)
{
    scrmap = new unsigned char[width*height/8];
}

void BMDisplay::Clear()
{
    for (int i = width*height/8; --i >= 0;)
	scrmap[i] = 0;
}

void BMDisplay::Clear(int left, int top, int w, int h)
{
    for (int y = top; y < (top+h); y++)
	for (int x = left; x < (left+w); x++)
	    ClrPixel(x, y);
}

void BMDisplay::Set(int left, int top, int w, int h)
{
    for (int y = top; y < (top+h); y++)
	for (int x = left; x < (left+w); x++)
	    SetPixel(x, y);
}

void BMDisplay::SaveBMP(char *fname)
{
    BITMAPFILEHEADER hdr;
    BITMAPINFOHEADER hdri;
    RGBQUAD  cols[2];
    FILE *fp;
    for (int cnt = 0; ; cnt++)
    {
	char fn[20];
	assert(cnt < 100);
	sprintf(fn, "%.6s%d.BMP", fname, cnt);
	if (access(fn, 0) != 0)
	{
	    fp = fopen(fn, "wb");
	    if (fp) break;
	    else return; // couldn't open dump file!
	}
    }
    // write the header
    int bytewidth = 2*((width+15)/16)+2;

    hdr.bfType = 19778;
    hdr.bfSize = 62+bytewidth*height; // size in bytes
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;
    hdr.bfOffBits = 62; // offset of actual bitmap
    fwrite(&hdr, sizeof(hdr), 1, fp);

    // write the header info

    hdri.biSize = 40;		// bytes required by hdr
    hdri.biWidth = width;	// width of bitmap in pixels
    hdri.biHeight = height;	// height of bitmap in pixels
    hdri.biPlanes = 1;		// must be one
    hdri.biBitCount = 1;	// bits per pixel
    hdri.biCompression = 0;	// no compression
    hdri.biSizeImage = bytewidth*height; // bytes in bitmap
    hdri.biXPelsPerMeter = 0;
    hdri.biYPelsPerMeter = 0;
    hdri.biClrUsed = 0;
    hdri.biClrImportant = 0;
    fwrite(&hdri, sizeof(hdri), 1, fp);

    // write the color info

    cols[0].rgbBlue = 255;	cols[0].rgbGreen = 255;
    cols[0].rgbRed = 255;	cols[0].rgbReserved = 0;
    cols[1].rgbBlue = 0;	cols[1].rgbGreen = 0;
    cols[1].rgbRed = 0;		cols[1].rgbReserved = 0;
    fwrite(cols, sizeof(RGBQUAD), 2, fp);

    for (int r = 0; r < height; r++)
        fwrite(scrmap+r*width/8, sizeof(char), bytewidth, fp);
    fclose(fp);
}

void BMDisplay::Convert(unsigned char *vector, char *fname)
{
    int tot = (width*height/8);
    while (tot-- > 0)
    {
	// we need to reverse the order
	unsigned v = 0;
	for (int j = 0; j < 8; j++)
	    if ((vector[tot] & (1 << j)) != 0)
		v |= (0x80 >> j);
	scrmap[tot] = v;
    }
//    Clear();
//    for (int r = 0; r < height; r++)
//    {
//        for (int c = 0; c < width; c++)
//	{
//	    int pix = r*width+c;
//	    unsigned char m = vector[pix/8];
//	    int v = (m & (0x80>>(pix%8))) != 0;
//	    if (v) SetPixel(c, r);
//	}
//    }
    SaveBMP(fname);
}

BMDisplay::~BMDisplay()
{
    delete [] scrmap;
}

static unsigned char whitestone_bits[] = {
   0xc0, 0x03, 0xf0, 0x0f, 0xf8, 0x1f, 0xfc, 0x3f, 0xfe, 0x7f, 0xfe, 0x7f,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0xff, 0xef, 0xfe, 0x77, 0xfe, 0x7b,
   0xfc, 0x3c, 0xf8, 0x1f, 0xf0, 0x0f, 0xc0, 0x03};

static unsigned char black_bits[] = {
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0};

static unsigned char blackstone_bits[] = {
   0xc0, 0x03, 0x30, 0x0c, 0x08, 0x10, 0x04, 0x20, 0x02, 0x40, 0x02, 0x40,
   0x01, 0x80, 0x01, 0x80, 0x01, 0x90, 0x01, 0x90, 0x02, 0x48, 0x02, 0x44,
   0x04, 0x23, 0x08, 0x10, 0x30, 0x0c, 0xc0, 0x03};

static unsigned char graystone_bits[] = {
   0xc0, 0x03, 0xb0, 0x0e, 0x58, 0x15, 0xac, 0x2a, 0x56, 0x55, 0xaa, 0x6a,
   0x55, 0xd5, 0xab, 0xaa, 0x55, 0xd5, 0xab, 0xba, 0x56, 0x5d, 0xaa, 0x6e,
   0x54, 0x37, 0xa8, 0x1a, 0x70, 0x0d, 0xc0, 0x03};

static unsigned char grid_bits[] = {
   0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
   0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
   0xff, 0xff, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
   0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00
};

static unsigned char wood_bits[] = {
	0xf6, 0xbb, 0x60, 	/* #### ## # ### ## ## */
	0xdf, 0xeb, 0xe0, 	/* ## ######## # ##### */
	0xbf, 0xbf, 0xe0, 	/* # ####### ######### */
	0xff, 0xfb, 0x60, 	/* ############# ## ## */
	0xdb, 0xee, 0xe0, 	/* ## ## ##### ### ### */
	0xfe, 0xff, 0xe0, 	/* ####### ########### */
	0xdf, 0xf6, 0xe0, 	/* ## ######### ## ### */
	0xfe, 0xfd, 0xe0, 	/* ####### ###### #### */
	0xdb, 0xb7, 0xe0, 	/* ## ## ### ## ###### */
	0xda, 0xff, 0xa0, 	/* ## ## # ######### # */
	0xff, 0xdd, 0xe0, 	/* ########## ### #### */
	0xaf, 0x7f, 0xa0, 	/* # # #### ######## # */
	0xff, 0xfd, 0xe0, 	/* ############## #### */
	0xd7, 0x7e, 0xe0, 	/* ## # ### ###### ### */
	0xff, 0xff, 0xe0, 	/* ################### */
	0x6b, 0x77, 0xe0, 	/*  ## # ## ### ###### */
	0xaf, 0xb7, 0x60, 	/* # # ##### ## ### ## */
	0xfd, 0xef, 0xe0, 	/* ###### #### ####### */
	0xfb, 0xbf, 0xe0  	/* ##### ### ######### */
} ;

main()
{
    BMDisplay *d = new BMDisplay(16, 16);
    d->Convert(blackstone_bits, "bstone");
    d->Convert(whitestone_bits, "wstone");
    d->Convert(graystone_bits, "gstone");
    d->Convert(grid_bits, "vertex");
    d->Convert(black_bits, "empty");
    delete d;
    d = new BMDisplay(20, 20);
    d->Convert(wood_bits, "wood");
    delete d;
}

