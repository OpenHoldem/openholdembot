/*
Comapre Args
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

#include "CompareArgs.h"
#include "RGBAImage.h"
#include <stdio.h>

static const char* copyright = 
"PerceptualDiff version 1.0, Copyright (C) 2006 Yangli Hector Yee\n\
PerceptualDiff comes with ABSOLUTELY NO WARRANTY;\n\
This is free software, and you are welcome\n\
to redistribute it under certain conditions;\n\
See the GPL page for details: http://www.gnu.org/copyleft/gpl.html\n\n";

static const char *usage =
"PeceptualDiff image1.tif image2.tif\n\n\
   Compares image1.tif and image2.tif using a perceptually based image metric\n\
   Options:\n\
\t-verbose       : Turns on verbose mode\n\
\t-fov deg       : Field of view in degrees (0.1 to 89.9)\n\
\t-threshold p	 : #pixels p below which differences are ignored\n\
\t-gamma g       : Value to convert rgb into linear space (default 2.2)\n\
\t-luminance l   : White luminance (default 100.0 cdm^-2)\n\
\t-output o.ppm  : Write difference to the file o.ppm\n\
\n\
\n Note: Input files can also be in the PNG format\
\n";

CompareArgs::CompareArgs()
{
	ImgA = NULL;
	ImgB = NULL;
//	ImgDiff = NULL;
	Verbose = false;
	FieldOfView = 45.0f;
	Gamma = 2.2f;
	ThresholdPixels = 100;
	Luminance = 100.0f;
	PixelsFailed = 0;
}

CompareArgs::~CompareArgs()
{
	if (ImgA) delete ImgA;
	if (ImgB) delete ImgB;
//	if (ImgDiff) delete ImgDiff;
}

#ifdef NOT_OPENHOLDEM
bool CompareArgs::Parse_Args(int argc, char **argv)
{
	if (argc < 3) {
		ErrorStr = copyright;
		ErrorStr += usage;
		return false;
	}
	for (int i = 0; i < argc; i++) {
		if (i == 1) {
			ImgA = RGBAImage::ReadTiff(argv[1]);
			if (!ImgA) {
				ImgA = RGBAImage::ReadPNG(argv[1]);
				if (!ImgA)
				{
					ErrorStr = "FAIL: Cannot open ";
					ErrorStr += argv[1];
					ErrorStr += "\n";
					return false;
				}
			}
		} else if (i == 2) {			
			ImgB = RGBAImage::ReadTiff(argv[2]);
			if (!ImgB) {
				ImgB = RGBAImage::ReadPNG(argv[2]);
				if (!ImgB)
				{
					ErrorStr = "FAIL: Cannot open ";
					ErrorStr += argv[2];
					ErrorStr += "\n";
					return false;
				}
			}
		} else {
			if (strstr(argv[i], "-fov")) {
				if (i + 1 < argc) {
					FieldOfView = (float) atof(argv[i + 1]);
				}
			} else if (strstr(argv[i], "-verbose")) {
				Verbose = true;
			} else 	if (strstr(argv[i], "-threshold")) {
				if (i + 1 < argc) {
					ThresholdPixels = atoi(argv[i + 1]);
				}
			} else 	if (strstr(argv[i], "-gamma")) {
				if (i + 1 < argc) {
					Gamma = (float) atof(argv[i + 1]);
				}
			}else 	if (strstr(argv[i], "-luminance")) {
				if (i + 1 < argc) {
					Luminance = (float) atof(argv[i + 1]);
				}
			}else 	if (strstr(argv[i], "-output")) {
				if (i + 1 < argc) {
					ImgDiff = new RGBAImage(ImgA->Get_Width(), ImgA->Get_Height(), argv[i+1]);
				}
			}
		}
	} // i
	return true;
}

void CompareArgs::Print_Args()
{
	printf("Field of view is %f degrees\n", FieldOfView);
	printf("Threshold pixels is %d pixels\n", ThresholdPixels);
	printf("The Gamma is %f\n", Gamma);
	printf("The Display's luminance is %f candela per meter squared\n", Luminance);
}
#endif