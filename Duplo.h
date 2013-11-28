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

#ifndef _DUPLO_H_
#define _DUPLO_H_

/* The default minimum number of identical lines required for a match. */
#define MIN_BLOCK_SIZE 4

/* The default minimum number of characters for a line to be part of a match. */
#define MIN_CHARS 3

#include <string>
#include <vector>
#include <iostream>

class SourceFile;

const std::string VERSION = "0.2.0";

class Duplo {
protected:
	const std::vector<std::string>& m_inputFiles;
	unsigned int m_minBlockSize;
	unsigned int m_minChars;
	bool m_ignorePrepStuff;
	bool m_ignoreSameFilename;
	int m_maxLinesPerFile;
	int m_DuplicateLines;
	bool m_Xml;
	bool m_quiet;
	unsigned char* m_pMatrix;

	void reportSeq(int line1, int line2, int count, SourceFile* pSource1, SourceFile* pSource2, std::ostream& outFile);
	int process(SourceFile* pSource1, SourceFile* pSource2, std::ostream& outFile);

	const std::string getFilenamePart(const std::string& fullpath);
	bool isSameFilename(const std::string& filename1, const std::string& filename2);

public:
	Duplo(const std::vector<std::string>& inputFiles, unsigned int minBlockSize, unsigned int minChars, bool ignorePrepStuff, bool ignoreSameFilename, bool Xml, bool quiet);
	~Duplo();
	void run(std::ostream& out);
};

#endif
