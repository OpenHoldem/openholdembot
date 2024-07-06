/* include/poker_config.h.  Generated from poker_config.h.in by configure.  */
/*
 * Copyright (C) 2004-2006 
 *           Michael Maurer <mjmaurer@yahoo.com>
 *           Loic Dachary <loic@dachary.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef WORDS_BIGENDIAN
/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
/* #undef WORDS_BIGENDIAN */
#endif /* WORDS_BIGENDIAN */

#ifndef HAVE_UINT64_T
/* Define if your compiler supports "uint64_t" for 64 bit integers */
#define HAVE_UINT64_T 1
#endif /* HAVE_UINT64_T */

#ifndef HAVE_LONG_LONG
/* Define if your compiler supports "long long" for 64 bit integers */
#define HAVE_LONG_LONG 1
#endif /* HAVE_LONG_LONG */

#ifndef HAVE_INT8
/* Define if type "int8" is defined already */
/* #undef HAVE_INT8 */
#endif /* HAVE_INT8 */

#ifndef SIZEOF_LONG
/* The size of a `long', as computed by sizeof. */
#define SIZEOF_LONG 4
#endif /* SIZEOF_LONG */

#ifndef HAVE_INTTYPES_H
/* Check if we have/need the inttypes include file */
#define HAVE_INTTYPES_H 1
#endif /* HAVE_INTTYPES_H */

#ifndef HAVE_STDINT_H
/* Check if we have/need the stdint include file */
#define HAVE_STDINT_H 1
#endif /* HAVE_STDINT_H */

#ifndef HAVE_SYS_TYPES_H
/* Check if we have/need the sys/types include file */
#define HAVE_SYS_TYPES_H 1
#endif /* HAVE_SYS_TYPES_H */

