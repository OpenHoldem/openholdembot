/*
 * Copyright (C) 2004-2006 Mekensleep
 *
 * Mekensleep
 * 24 rue vieille du temple
 * 75004 Paris
 *       licensing@mekensleep.com
 *
 *  This package is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 dated June, 1991.
 *
 *  This package is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this package; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 *  MA 02110-1301, USA.
 *
 * Authors:
 *  Loic Dachary <loic@dachary.org>
 *
 */ 

#ifndef LIBPOKEREVAL_EXPORT
#define LIBPOKEREVAL_EXPORT 1


#if defined(_MSC_VER)
    #pragma warning( disable : 4244 )
    #pragma warning( disable : 4251 )
    #pragma warning( disable : 4267 )
    #pragma warning( disable : 4275 )
    #pragma warning( disable : 4290 )
    #pragma warning( disable : 4786 )
    #pragma warning( disable : 4305 )
#endif

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)

	#ifndef POKERVAL_DLL
		#ifdef __cplusplus
			#define POKEREVAL_EXPORT "C"
		#else
			#define POKEREVAL_EXPORT
		#endif
	#else
		#ifdef POKEREVAL_LIBRARY
			#ifdef __cplusplus
				#define POKEREVAL_EXPORT "C" __declspec(dllexport)
			#else
				#define POKEREVAL_EXPORT "C"
			#endif
		#else
			#ifdef __cplusplus
				#define POKEREVAL_EXPORT "C" __declspec(dllimport)
			#else
				#define POKEREVAL_EXPORT "C"
			#endif
		#endif
	#endif

#else
	#ifdef __cplusplus
		#define POKEREVAL_EXPORT "C"
	#else
		#define POKEREVAL_EXPORT
	#endif
#endif

#endif
