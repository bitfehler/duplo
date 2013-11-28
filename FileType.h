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

#ifndef _FILETYPE_H_
#define _FILETYPE_H_

#include <vector>
#include <string>

class FileType {
public:
	enum FILETYPE {
		FILETYPE_UNKNOWN,
		FILETYPE_C,
		FILETYPE_CPP,
		FILETYPE_CXX,
		FILETYPE_H,
		FILETYPE_HPP,
		FILETYPE_JAVA,
		FILETYPE_CS,
		FILETYPE_VB
	};

public:
	static FileType::FILETYPE GetFileType(const std::string& FileName);
};

#endif
