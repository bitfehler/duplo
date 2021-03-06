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

#include "SourceFile.h"

#include <assert.h>
#include <algorithm>

#include "TextFile.h"
#include "SourceLine.h"
#include "StringUtil.h"

#if defined(__CYGWIN__) || defined(_WIN32)
#define PATH_SEP '\\'
#else
#define PATH_SEP '/'
#endif

SourceFile::SourceFile(const std::string& fileName, const unsigned int minChars, const bool ignorePrepStuff)
    : m_absFileName(fileName),
      m_FileType(FileType::GetFileType(fileName)),
      m_ignorePrepStuff(ignorePrepStuff)
{
	m_fileName = getBasename();
	TextFile listOfFiles(m_absFileName.c_str());

	std::vector<std::string> lines;
	listOfFiles.readLines(lines);

	int openBlockComments = 0;
	for (int i = 0; i < (int)lines.size(); i++) {
		std::string& line = lines[i];
		std::string tmp;

		tmp.reserve(line.size());

		// Remove block comments
		if (FileType::FILETYPE_C == m_FileType || FileType::FILETYPE_CPP == m_FileType || FileType::FILETYPE_CXX == m_FileType || FileType::FILETYPE_H == m_FileType || FileType::FILETYPE_HPP == m_FileType || FileType::FILETYPE_JAVA == m_FileType || FileType::FILETYPE_CS == m_FileType) {
			int lineSize = (int)line.size();
			for (int j = 0; j < (int)line.size(); j++) {
				if (line[j] == '/' && line[MIN(lineSize - 1, j + 1)] == '*') {
					openBlockComments++;
				}

				if (openBlockComments <= 0) {
					tmp.push_back(line[j]);
				}

				if (line[MAX(0, j - 1)] == '*' && line[j] == '/') {
					openBlockComments--;
				}
			}
		}
		if (FileType::FILETYPE_VB == m_FileType) {
			tmp = line;
		}

		std::string cleaned;
		getCleanLine(tmp, cleaned);

		if (isSourceLine(cleaned, minChars)) {
			m_sourceLines.push_back(new SourceLine(cleaned, i));
		}
	}
	m_lines = (int)m_sourceLines.size();
}

SourceFile::~SourceFile()
{
	for (unsigned int i = 0; i < m_sourceLines.size(); ++i) {
		delete m_sourceLines[i];
	}
}

std::string SourceFile::getBasename()
{
	std::string::size_type idx = m_absFileName.rfind(PATH_SEP);
	if (idx != std::string::npos) {
		return m_absFileName.substr(idx + 1, m_absFileName.size() - idx - 1);
	}
	return m_absFileName;
}

void SourceFile::getCleanLine(const std::string& line, std::string& cleanedLine)
{
	// Remove single line comments
	cleanedLine.reserve(line.size());
	int lineSize = (int)line.size();
	for (int i = 0; i < (int)line.size(); i++) {
		switch (m_FileType) {
		case FileType::FILETYPE_C:
		case FileType::FILETYPE_CPP:
		case FileType::FILETYPE_CXX:
		case FileType::FILETYPE_H:
		case FileType::FILETYPE_HPP:
		case FileType::FILETYPE_JAVA:
		case FileType::FILETYPE_CS:
			if (i < lineSize - 2 && line[i] == '/' && line[i + 1] == '/') {
				return;
			}
			break;

		case FileType::FILETYPE_VB:
			if (i < lineSize - 1 && line[i] == '\'') {
				return;
			}
			break;
		default:
			return;
		}
		cleanedLine.push_back(line[i]);
	}
}

bool SourceFile::isSourceLine(const std::string& line, const unsigned int minChars)
{
	std::string tmp = StringUtil::trim(line);

	// filter min size lines
	if (tmp.size() < minChars) {
		return false;
	}

	std::transform(tmp.begin(), tmp.end(), tmp.begin(), (int (*)(int))tolower);

	if (m_ignorePrepStuff) {
		switch (m_FileType) {
		case FileType::FILETYPE_C:
		case FileType::FILETYPE_CPP:
		case FileType::FILETYPE_CXX:
		case FileType::FILETYPE_H:
		case FileType::FILETYPE_HPP:
		case FileType::FILETYPE_JAVA:
			if (tmp[0] == '#') {
				return false;
			}
			break;

		case FileType::FILETYPE_CS: {
			if (tmp[0] == '#') {
				return false;
			}
			// look for preprocessor marker in start of string
			const std::string PreProc_CS = "using";

			return std::string::npos == tmp.find(PreProc_CS.c_str(), 0, PreProc_CS.length());
		} break;

		case FileType::FILETYPE_VB: {
			// look for preprocessor marker in start of string
			const std::string PreProc_VB = "imports";

			return std::string::npos == tmp.find(PreProc_VB.c_str(), 0, PreProc_VB.length());
		} break;
		default:
			return false;
		}
	}

	bool bRet = (tmp.size() >= minChars);
	assert(bRet);

	return bRet;
}

const std::string& SourceFile::getFilename(bool abs) const
{
	return abs ? m_absFileName : m_fileName;
}
