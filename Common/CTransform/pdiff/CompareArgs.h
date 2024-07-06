/*******************************************************************************
Compare Args
Copyright (C) 2006 Yangli Hector Yee

This program is free software; you can redistribute it and/or modify it under the terms of the
GNU General Public License as published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program;
if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*******************************************************************************/

#ifndef _COMPAREARGS_H
#define _COMPAREARGS_H

//#include <string>
#include "RGBAImage.h"

//class RGBAImage;

// Args to pass into the comparison function
class CompareArgs
{
public:
	CompareArgs();
	~CompareArgs();
#ifdef NOT_OPENHOLDEM
	bool Parse_Args(int argc, char **argv);	
	void Print_Args();
#endif
	
	RGBAImage		*ImgA;				// Image A
	RGBAImage		*ImgB;				// Image B
//	RGBAImage		*ImgDiff;			// Diff image
	bool			Verbose;			// Print lots of text or not
	float			FieldOfView;		// Field of view in degrees
	float			Gamma;				// The gamma to convert to linear color space
	float			Luminance;			// the display's luminance
	unsigned int	ThresholdPixels;	// How many pixels different to ignore
//	std::string		ErrorStr;			// Error string
	unsigned int	PixelsFailed;		// Number of pixels that are perceptually different
};

#endif