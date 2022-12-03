#
# Makefile - Copyright (c) 2022 - Olivier Poncet
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# ----------------------------------------------------------------------------
# global environment
# ----------------------------------------------------------------------------

CC       = gcc
CFLAGS   = -g -O2 -Wall -std=c99
CXX      = g++
CXXFLAGS = -g -O2 -Wall -std=c++14
CPPFLAGS = -I.
LD       = g++
LDFLAGS  = -L.
CP       = cp
CPFLAGS  = -f
RM       = rm
RMFLAGS  = -f

# ----------------------------------------------------------------------------
# default rules
# ----------------------------------------------------------------------------

.c.o :
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $<

.cc.o :
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $<

# ----------------------------------------------------------------------------
# global targets
# ----------------------------------------------------------------------------

all : build

run : run_card
	@echo "=== $@ ok ==="

build : build_card
	@echo "=== $@ ok ==="

clean : clean_card
	@echo "=== $@ ok ==="

# ----------------------------------------------------------------------------
# Business Card Raytracer
# ----------------------------------------------------------------------------

CARD_PROGRAM = \
	card.bin \
	$(NULL)

CARD_OBJECTS = \
	card.o \
	$(NULL)

CARD_LIBS = \
	-lm \
	$(NULL)

run_card : build_card
	./$(CARD_PROGRAM)

build_card : $(CARD_PROGRAM)

clean_card :
	$(RM) $(RMFLAGS) $(CARD_OBJECTS) $(CARD_PROGRAM)
	$(RM) $(RMFLAGS) *.ppm

$(CARD_PROGRAM) : $(CARD_OBJECTS)
	$(LD) $(LDFLAGS) -o $(CARD_PROGRAM) $(CARD_OBJECTS) $(CARD_LIBS)

# ----------------------------------------------------------------------------
# dependencies
# ----------------------------------------------------------------------------

card.o : card.cc card.h

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
