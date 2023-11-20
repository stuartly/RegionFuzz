/* Compare numeric strings.

   Copyright (C) 2005-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Paul Eggert.  */

#include <config.h>

#include "strnumcmp-in.h"

/* Externally-visible name for numcompare.  */

int _GL_ATTRIBUTE_PURE
strnumcmp (char const *a, char const *b,
           int decimal_point, int thousands_sep)
{
  return numcompare (a, b, decimal_point, thousands_sep);
}