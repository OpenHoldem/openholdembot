#
# Copyright (C) 2004-2006 Mekensleep
#
# Mekensleep
# 24 rue vieille du temple
# 75004 Paris
#       licensing@mekensleep.com
#
#  This package is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; version 2 dated June, 1991.
#
#  This package is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this package; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
#  MA 02110-1301, USA.
#
# Authors:
#  Loic Dachary <loic@dachary.org>
#
# 
MAINTAINERCLEANFILES = \
	Makefile.in \
	$(TABLES)

INCLUDES = -I$(top_srcdir)/include

lib_LTLIBRARIES = libpoker-eval.la

EXTRA_DIST = \
	evx_preamble.cfrag \
	${POKER_EVAL_SOURCES}

BASIC_TABLES = \
        t_nbits.c t_topbit.c t_topcard.c t_topfivecards.c t_topfivebits.c \
        t_toptwobits.c t_cardmasks.c t_maskrank.c t_straight.c 
JOKER_TABLES = t_jokercardmasks.c t_jokerstraight.c
PACKED_TABLES = \
        t_nbitsandstr.c
EVX_TABLES = \
        t_evx_pairval.c t_evx_strval.c t_evx_tripsval.c t_evx_flushcards.c
ASTUD_TABLES = t_astudcardmasks.c
LOWBALL_TABLES = t_botfivecards.c t_botcard.c t_botfivecardsj.c

TABLES = \
	$(BASIC_TABLES) \
	$(JOKER_TABLES) \
	$(PACKED_TABLES) \
	$(EVX_TABLES) \
	$(ASTUD_TABLES) \
	$(LOWBALL_TABLES)

POKER_EVAL_SOURCES = \
	combinations.c \
	deck.c \
	deck_astud.c \
	deck_joker.c \
	deck_std.c \
	enumerate.c \
	enumord.c \
	evx.c \
	lowball.c \
	rules_astud.c \
	rules_joker.c \
	rules_std.c \
	poker_wrapper.c

nodist_libpoker_eval_la_SOURCES = \
	$(POKER_EVAL_SOURCES) \
	$(TABLES)
libpoker_eval_la_CPPFLAGS = ${GCOV_CPPFLAGS}

noinst_HEADERS = \
	mktable.h

MKTABLE_HELPERS = mktable.c mktable_utils.c

noinst_PROGRAMS = \
	mktab_basic \
	mktab_joker \
	mktab_packed \
	mktab_evx \
	mktab_astud \
	mktab_lowball \
	evx_gen5 \
	evx_gen7

mktab_basic_SOURCES = mktab_basic.c $(MKTABLE_HELPERS)

${BASIC_TABLES}: mktab_basic$(EXEEXT)
	./mktab_basic

mktab_joker_SOURCES = mktab_joker.c $(MKTABLE_HELPERS)

${JOKER_TABLES}: mktab_joker$(EXEEXT) 
	./mktab_joker

mktab_packed_SOURCES = mktab_packed.c $(MKTABLE_HELPERS)

${PACKED_TABLES}: mktab_packed$(EXEEXT)
	./mktab_packed

mktab_evx_SOURCES = mktab_evx.c $(MKTABLE_HELPERS)

${EVX_TABLES}: mktab_evx$(EXEEXT) 
	./mktab_evx

mktab_astud_SOURCES = mktab_astud.c $(MKTABLE_HELPERS)

${ASTUD_TABLES}: mktab_astud$(EXEEXT) 
	./mktab_astud

mktab_lowball_SOURCES = mktab_lowball.c $(MKTABLE_HELPERS)

${LOWBALL_TABLES}: mktab_lowball$(EXEEXT) 
	./mktab_lowball

evx_gen5_SOURCES = evx_generate.c 
evx_gen5_CPPFLAGS = -DCARDS_DEALT=5
evx_gen7_SOURCES = evx_generate.c
evx_gen7_CPPFLAGS = -DCARDS_DEALT=7

all-local: evx_gen5 evx_gen7 evx_preamble.cfrag
	if [ -w $(top_srcdir)/include/inlines/evx5.h ] ; then \
		./evx_gen5 > $(top_srcdir)/include/inlines/evx5.h ; \
	fi
	if [ -w $(top_srcdir)/include/inlines/evx7.h ] ; then \
		./evx_gen7 > $(top_srcdir)/include/inlines/evx7.h ; \
	fi

clean-local:
	rm -f t_*.c
	-rm -f *.gcov *.gcda *.gcno

libpoker_eval_la_LDFLAGS = -no-undefined -version-info 1:0:0
