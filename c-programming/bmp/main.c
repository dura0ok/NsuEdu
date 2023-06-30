#include <stdio.h>
#include "libbmp.h"
#include "complex_number.h"
#include "string.h"
#include "stdlib.h"

void draw(int height, int width, double left, double right, double top, double bottom, bmp_img *img) {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            double a = left + (double)(right - left) * x / width;
            double b = bottom + (double)(top - bottom) * y / height;

            struct T_complex_number c = {.real = a, .complex = b};
            struct T_complex_number z = {.real = 0, .complex = 0};
            int iteration_counter  = 0;
            do {
                square(&z);
                add(&z, c);
                if(magnitude(z) > 2.0) break;
                iteration_counter++;
            } while (iteration_counter < 100);

            if(iteration_counter < 100){
                bmp_pixel_init (&(*img).img_pixels[y][x], iteration_counter, iteration_counter, iteration_counter);
            }else{
                bmp_pixel_init (&(*img).img_pixels[y][x], 255, 255, 255);
            }
        }
    }
}

int main (int argc, char * argv[]){
    unsigned int height, width;
    double left, right, top, bottom;

    printf("Height width: \n");
    scanf("%u %u", &height, &width);
    printf("Left right top bottom space: \n");
    scanf("%lf %lf %lf %lf", &left, &right, &top, &bottom);

    bmp_img img;
    bmp_img_init_df (&img, width, height);

    draw(height, width, left, right, top, bottom, &img);

    bmp_img_write (&img, "test.bmp");
    bmp_img_free (&img);
    return 0;
}
