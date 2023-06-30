#include <stdio.h>
#include <stdlib.h>
#include "libbmp.h"

int get_padding(int a){
    return a % 4;
}


// BMP_HEADER

void bmp_header_init_df (bmp_header *header, const int   width, const int   height){
    // bm signature length 2
    header->bfSize = (sizeof (bmp_pixel) * width + get_padding(width))
                     * abs (height); // size length 2, offset 2
    header->bfReserved = 0; // length 4
    header->bfOffBits = 54; // length 4 offset 10
    header->biSize = 40; // BITMAPINFOHEADER
    header->biWidth = width;
    header->biHeight = height;
    header->biPlanes = 1;
    header->biBitCount = 24;
    header->biCompression = 0;
    header->biSizeImage = 0;
    header->biXPelsPerMeter = 0;
    header->biYPelsPerMeter = 0;
    header->biClrUsed = 0;
    header->biClrImportant = 0;
}

enum bmp_error bmp_header_write (const bmp_header *header, FILE *img_file) {
    if (header == NULL)
    {
        return BMP_HEADER_NOT_INITIALIZED;
    }
    else if (img_file == NULL)
    {
        return BMP_FILE_NOT_OPENED;
    }

    // Since an adress must be passed to fwrite, create a variable!
    const unsigned short magic = BMP_MAGIC;
    fwrite (&magic, sizeof (magic), 1, img_file);

    // Use the type instead of the variable because its a pointer!
    fwrite (header, sizeof (bmp_header), 1, img_file);
    return BMP_OK;
}

enum bmp_error bmp_header_read (bmp_header *header, FILE *img_file){
    if (img_file == NULL)
    {
        return BMP_FILE_NOT_OPENED;
    }

    unsigned short magic;

    if (fread (&magic, sizeof (magic), 1, img_file) != 1 ||
        magic != BMP_MAGIC)
    {
        return BMP_INVALID_FILE;
    }

    if (fread (header, sizeof (bmp_header), 1, img_file) != 1)
    {
        return BMP_ERROR;
    }

    return BMP_OK;
}

// BMP_PIXEL

void bmp_pixel_init (bmp_pixel  *pxl, const unsigned char  red, const unsigned char  green, const unsigned char  blue){
    pxl->red = red;
    pxl->green = green;
    pxl->blue = blue;
}

// BMP_IMG

void bmp_img_alloc (bmp_img *img){
    const size_t h = abs (img->img_header.biHeight);

    // Allocate the required memory for the pixels:
    img->img_pixels = malloc (sizeof (bmp_pixel*) * h);

    for (size_t y = 0; y < h; y++)
    {
        img->img_pixels[y] = malloc (sizeof (bmp_pixel) * img->img_header.biWidth);
    }
}

void bmp_img_init_df (bmp_img  *img, const int  width, const int  height){
    bmp_header_init_df (&img->img_header, width, height);
    bmp_img_alloc (img);
}

void bmp_img_free (bmp_img *img){
    const size_t h = abs (img->img_header.biHeight);

    for (size_t y = 0; y < h; y++)
    {
        free (img->img_pixels[y]);
    }
    free (img->img_pixels);
}

enum bmp_error bmp_img_write (const bmp_img *img, const char  *filename){
    FILE *img_file = fopen (filename, "wb");

    if (img_file == NULL)
    {
        return BMP_FILE_NOT_OPENED;
    }

    const enum bmp_error err = bmp_header_write (&img->img_header, img_file);

    if (err != BMP_OK)
    {
        fclose (img_file);
        return err;
    }


    const size_t h = abs (img->img_header.biHeight);
    const size_t offset = (img->img_header.biHeight > 0 ? h - 1 : 0);

    const unsigned char padding[3] = {'\0', '\0', '\0'};

    for (size_t y = 0; y < h; y++)
    {
        fwrite (img->img_pixels[(offset - y)], sizeof (bmp_pixel), img->img_header.biWidth, img_file);
        fwrite (padding, sizeof (unsigned char), get_padding(img->img_header.biWidth), img_file);
    }

    fclose (img_file);
    return BMP_OK;
}

