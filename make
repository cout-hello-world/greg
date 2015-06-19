#!/bin/bash
# Copyright 2015 Henry Elliott
# 
# This file is part of fact.
# 
# fact is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# fact is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with fact.  If not, see <http://www.gnu.org/licenses/>.
g++ --std=c++11 -pedantic -Wall -Wextra -Werror greg.cpp -o greg -lgmp -pthread -march=native -O2
