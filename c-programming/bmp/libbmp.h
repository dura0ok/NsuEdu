#pragma once
#define BMP_MAGIC 19778


enum bmp_error
{
    BMP_FILE_NOT_OPENED = -4,
    BMP_HEADER_NOT_INITIALIZED,
    BMP_INVALID_FILE,
    BMP_ERROR,
    BMP_OK = 0
};

#pragma pack(push, 1)
typedef struct T_bmp_header
{
    unsigned int   bfSize;
    unsigned int   bfReserved;
    unsigned int   bfOffBits;

    unsigned int   biSize;
    int            biWidth;
    int            biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int   biCompression;
    unsigned int   biSizeImage;
    int            biXPelsPerMeter;
    int            biYPelsPerMeter;
    unsigned int   biClrUsed;
    unsigned int   biClrImportant;
} bmp_header;
#pragma pack(pop)

typedef struct T_bmp_pixel
{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
} bmp_pixel;


typedef struct T_bmp_img
{
    bmp_header   img_header;
    bmp_pixel  **img_pixels;
} bmp_img;


void bmp_header_init_df (bmp_header *header, int   width, int height);

enum bmp_error  bmp_header_write (const bmp_header*, FILE*);

// BMP_PIXEL
void bmp_pixel_init (bmp_pixel  *pxl, unsigned char  red, unsigned char  green, unsigned char  blue);

// BMP_IMG
void bmp_img_alloc (bmp_img *img);
void bmp_img_init_df (bmp_img  *img, int  width, int  height);
void bmp_img_free (bmp_img *img);

enum bmp_error bmp_img_write (const bmp_img *img, const char  *filename);
