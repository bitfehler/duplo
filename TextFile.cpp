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

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include "TextFile.h"

#include "StringUtil.h"

/** 
 * Creates a new text file. The file is accessed relative to current directory.
 */
TextFile::TextFile(const std::string& fileName)
    : m_fileName(fileName)
{
}

bool TextFile::readLines(std::vector<std::string>& lines)
{
	std::ifstream infile(m_fileName.c_str());
	if (!infile.good()) {
		std::cerr << "Error: Can't open file: " << m_fileName << ". File doesn't exist or access denied." << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(infile, line)) {
		lines.push_back(line);
	}
	return true;
}

/** 
 * Writes a std::string into a text file.
 */
bool TextFile::writeAll(const std::string& all)
{

	std::ofstream outFile(m_fileName.c_str(), std::ios::binary);
	if (outFile.is_open()) {
		outFile << all;
		outFile.close();
	} else {
		std::cout << "Error: Can't open file: " << m_fileName << ". File doesn't exist or access denied.\n";
		return false;
	}

	return true;
}
