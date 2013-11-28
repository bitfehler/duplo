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

#include "StringUtil.h"

#include <ctype.h>

/**
 * Trim string
 *
 * @param input  string to trim
 * @return returns trimmed string
 */
std::string StringUtil::trim(const std::string& input)
{
	// If string is empty, there is nothing to look for.
	if (input.length() == 0) {
		return "";
	}

	// Set up temporary
	std::string final = input;

	// Remove spaces at beginning
	int i = 0;
	while (i < (int)input.length() && input[i] <= ' ') {
		i++;
	}

	// String full of spaces, return nothing.
	if (i >= (int)input.length()) {
		return "";
	}

	if (i > 0) {
		final = input.substr(i, input.length() - i);
	}

	// Remove spaces at end
	i = (int)final.length() - 1;
	while (i >= 0 && final[i] <= ' ') {
		i--;
	}

	final = final.substr(0, i + 1);

	// Return the new string
	return final;
}

std::string StringUtil::substitute(char s, char d, const std::string& str)
{
	std::string tmp = str;

	for (int i = 0; i < (int)tmp.size(); i++) {
		if (tmp[i] == s) {
			tmp[i] = d;
		}
	}

	return tmp;
}

// from:
//      http://www.codecomments.com/archive272-2005-4-473566.html
void StringUtil::StrSub(std::string& cp, const std::string& sub_this, const std::string& for_this, const int& num_times)
{
	int loc = 0;
	if (cp.empty()) {
		cp = sub_this;
		return;
	}
	for (int i = 0; i != num_times; i++) {
		loc = (int)cp.find(for_this, loc);
		if (loc >= 0) {
			cp.replace(loc, for_this.length(), sub_this);
			loc += (int)for_this.length();
		} else {
			return;
		}
	}
}
