/*
RGBAImage.cpp
Copyright (C) 2006 Yangli Hector Yee

This program is free software; you can redistribute it and/or modify it under the terms of the
GNU General Public License as published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program;
if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "stdafx.h"

#include "RGBAImage.h"
#ifdef NOT_OPENHOLDEM
#include "png.h"
#include "tiff.h"
#include "tiffio.h"
#endif

// Reads Tiff Images
#ifdef NOT_OPENHOLDEM
RGBAImage* RGBAImage::ReadTiff(char *filename)
{
	RGBAImage *fimg = 0;
	
    TIFF* tif = TIFFOpen(filename, "r");
	char emsg[1024];
	emsg[0] = 0;
    if (tif) {
		TIFFRGBAImage img;
		
		if (TIFFRGBAImageBegin(&img, tif, 0, emsg)) {
			size_t npixels;
			uint32* raster;

			npixels = img.width * img.height;
			raster = (uint32*) _TIFFmalloc(npixels * sizeof (uint32));
			if (raster != NULL) {
				if (TIFFRGBAImageGet(&img, raster, img.width, img.height)) {
					// result is in ABGR
					fimg = new RGBAImage(img.width, img.height);
					for (int y = img.height - 1; y >= 0; y--) {
						for (int x = 0; x < img.width; x++) {
						   fimg->Set(x,img.height - (y+1), raster[x + y * img.width]);
						}
					}
				}
			_TIFFfree(raster);
			}
	    }
	    TIFFRGBAImageEnd(&img);
	}
	return fimg;
}

// This portion was written by Scott Corley
RGBAImage* RGBAImage::ReadPNG(char *filename)
{
	RGBAImage *fimg = 0;
	FILE *fp=fopen(filename, "rb");
	if (!fp)
	{
		return NULL;
	}
	png_byte header[8];

	fread(header, 1, 8, fp);
	bool is_png = !png_sig_cmp(header, 0, 8);
	if (!is_png)
	{
		return NULL;
	}

    png_structp png_ptr = png_create_read_struct
       (PNG_LIBPNG_VER_STRING, (png_voidp)NULL,
        NULL, NULL);
    if (!png_ptr)
        return (NULL);

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr,
				(png_infopp)NULL, (png_infopp)NULL);
        return (NULL);
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr,
				(png_infopp)NULL);
        return (NULL);
    }

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_png(png_ptr, info_ptr, 0, NULL);

	png_bytep *row_pointers;
	row_pointers = png_get_rows(png_ptr, info_ptr);

	fimg = new RGBAImage(png_ptr->width, png_ptr->height);
	for (int y = 0; y < (int) png_ptr->height; y++) {
		for (int x = 0; x < (int) png_ptr->width; x++) {
			uint32 value = 0;
			if (png_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
				value = ((uint32)row_pointers[y][x*4]) | (((uint32)row_pointers[y][x*4+1])<<8) | (((uint32)row_pointers[y][x*4+2])<<16) |(((uint32)row_pointers[y][x*4+3])<<24);
			else if (png_ptr->color_type == PNG_COLOR_TYPE_RGB)
				value = ((uint32)row_pointers[y][x*3] /*B*/) | (((uint32)row_pointers[y][x*3+1] /*G*/)<<8) | (((uint32)row_pointers[y][x*3+2]/*R*/)<<16) | (0xFFUL << 24);
		   fimg->Set(x,y, value);
		}
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	return fimg;
}
#endif

bool RGBAImage::WritePPM()
{	
	if (Width <= 0) return false;
	if (Height <=0 ) return false;
	FILE *out;
	fopen_s(&out, Name.c_str(), "wb");
	if (!out) return false;
	fprintf(out, "P6\n%d %d 255\n", Width, Height);
	for (int y = 0; y < Height; y++) {
		for (int x = 0; x < Width; x++) {
			int i = x + y * Width;
			unsigned char r = Get_Red(i);
			unsigned char g = Get_Green(i);
			unsigned char b = Get_Blue(i);
			fwrite(&r, 1, 1, out);
			fwrite(&g, 1, 1, out);
			fwrite(&b, 1, 1, out);
		}
	}
	fclose(out);
	return true;
}
