#include "poker_defs.h"

/* 
 * Table StdDeck_maskRankTable
 */

/*
StdDeck_masksRanksTable[].  Maps card ranks (2..A) to a cardmask which
has all the bits set except the bits corresponding to the cards whose
have the input rank.  This is a quick way to mask off all the cards of
a specific rank.
 */

StdDeck_CardMask StdDeck_maskRankTable[13] = { 
      { 0xfffefffefffefffei64 }  ,
      { 0xfffdfffdfffdfffdi64 }  ,
      { 0xfffbfffbfffbfffbi64 }  ,
      { 0xfff7fff7fff7fff7i64 }  ,
      { 0xffefffefffefffefi64 }  ,
      { 0xffdfffdfffdfffdfi64 }  ,
      { 0xffbfffbfffbfffbfi64 }  ,
      { 0xff7fff7fff7fff7fi64 }  ,
      { 0xfefffefffefffeffi64 }  ,
      { 0xfdfffdfffdfffdffi64 }  ,
      { 0xfbfffbfffbfffbffi64 }  ,
      { 0xf7fff7fff7fff7ffi64 }  ,
      { 0xefffefffefffefffi64 }  
};
