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
# SUBDIRS
# ----------------------------------------------------------------------------

SUBDIRS = \
	src \
	$(NULL)

# ----------------------------------------------------------------------------
# global targets
# ----------------------------------------------------------------------------

all :
	@for SUBDIR in $(SUBDIRS); \
	do \
		(cd $${SUBDIR} && make all) \
	done

clean :
	@for SUBDIR in $(SUBDIRS); \
	do \
		(cd $${SUBDIR} && make clean) \
	done

run :
	@for SUBDIR in $(SUBDIRS); \
	do \
		(cd $${SUBDIR} && make run) \
	done

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
