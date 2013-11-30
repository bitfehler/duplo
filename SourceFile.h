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

#ifndef _SOURCEFILE_H_
#define _SOURCEFILE_H_

#include <string>
#include <vector>

#include "FileType.h"

class SourceLine;

#ifndef MIN
#define MIN(x, y) \
	((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x, y) \
	((x) > (y) ? (x) : (y))
#endif

class SourceFile {
protected:
	std::string m_absFileName;
	std::string m_fileName;
	FileType::FILETYPE m_FileType;

	int m_lines;
	bool m_ignorePrepStuff;

	std::vector<SourceLine*> m_sourceLines;

	bool isSourceLine(const std::string& line, const unsigned int minChars);
	void getCleanLine(const std::string& line, std::string& cleanedLine);
	std::string getBasename();

public:
	SourceFile(const std::string& fileName, const unsigned int minChars, const bool ignorePrepStuff);
	~SourceFile();

	const std::string& getFilename(bool abs=true) const;

	inline int getNumOfLines() const { return m_lines; }
	inline SourceLine* getLine(const int index) const { return m_sourceLines[index]; }
};

#endif
