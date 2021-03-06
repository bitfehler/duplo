/*
 * Duplo: a duplicate source code checker
 * Copyright (C) 2005-2013 the Duplo developers (see the AUTHORS file).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef _STRINGUTIL_H_
#define _STRINGUTIL_H_

#include <vector>
#include <string>

class StringUtil {
private:
public:
	static std::string trim(const std::string& input);
	static std::string substitute(char s, char d, const std::string& str);

	static void StrSub(std::string& cp, const std::string& sub_this, const std::string& for_this, const int& num_times);
};

#endif
