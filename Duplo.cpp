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

#include "Duplo.h"

#include <cstring>
#include <fstream>
#include <time.h>

#include "SourceFile.h"
#include "SourceLine.h"

#include "StringUtil.h"
#include "HashUtil.h"
#include "TextFile.h"

#define MATCH 1

Duplo::Duplo(const std::vector<std::string>& inputFiles, unsigned int minBlockSize, unsigned int minChars, bool ignorePrepStuff, bool ignoreSameFilename, bool xml, bool quiet)
    : m_inputFiles(inputFiles),
      m_minBlockSize(minBlockSize),
      m_minChars(minChars),
      m_ignorePrepStuff(ignorePrepStuff),
      m_ignoreSameFilename(ignoreSameFilename),
      m_maxLinesPerFile(0),
      m_DuplicateLines(0),
      m_Xml(xml),
      m_quiet(quiet),
      m_pMatrix(NULL)
{
}

Duplo::~Duplo()
{
	delete[] m_pMatrix;
	m_pMatrix = NULL;
}

void Duplo::reportSeq(int line1, int line2, int count, SourceFile* pSource1, SourceFile* pSource2, std::ostream& outFile)
{
	if (m_Xml) {
		outFile << "    <set LineCount=\"" << count << "\">" << std::endl;
		outFile << "        <block SourceFile=\"" << pSource1->getFilename() << "\" StartLineNumber=\"" << pSource1->getLine(line1)->getLineNumber() << "\"/>" << std::endl;
		outFile << "        <block SourceFile=\"" << pSource2->getFilename() << "\" StartLineNumber=\"" << pSource2->getLine(line2)->getLineNumber() << "\"/>" << std::endl;
		outFile << "        <lines xml:space=\"preserve\">" << std::endl;
		for (int j = 0; j < count; j++) {
			// replace various characters/ strings so that it doesn't upset the XML parser
			std::string tmpstr = pSource1->getLine(j + line1)->getLine();

			// " --> '
			StringUtil::StrSub(tmpstr, "\'", "\"", -1);

			// & --> &amp;
			StringUtil::StrSub(tmpstr, "&amp;", "&", -1);

			// < --> &lt;
			StringUtil::StrSub(tmpstr, "&lt;", "<", -1);

			// > --> &gt;
			StringUtil::StrSub(tmpstr, "&gt;", ">", -1);

			outFile << "            <line Text=\"" << tmpstr << "\"/>" << std::endl;
			m_DuplicateLines++;
		}
		outFile << "        </lines>" << std::endl;
		outFile << "    </set>" << std::endl;
	} else {
		outFile << pSource1->getFilename() << "(" << pSource1->getLine(line1)->getLineNumber() << ")" << std::endl;
		outFile << pSource2->getFilename() << "(" << pSource2->getLine(line2)->getLineNumber() << ")" << std::endl;
		for (int j = 0; j < count; j++) {
			outFile << pSource1->getLine(j + line1)->getLine() << std::endl;
			m_DuplicateLines++;
		}
		outFile << std::endl;
	}
}

int Duplo::process(SourceFile* pSource1, SourceFile* pSource2, std::ostream& outFile)
{
	const int m = pSource1->getNumOfLines();
	const int n = pSource2->getNumOfLines();

	// Reset matrix data
	memset(m_pMatrix, 0, m * n);

	// Compute matrix
	for (int y = 0; y < m; y++) {
		SourceLine* pSLine = pSource1->getLine(y);
		for (int x = 0; x < n; x++) {
			if (pSLine->equals(pSource2->getLine(x))) {
				m_pMatrix[x + n * y] = MATCH;
			}
		}
	}

	int blocks = 0;

	// Scan vertical part
	for (int y = 0; y < m; y++) {
		unsigned int seqLen = 0;
		int maxX = MIN(n, m - y);
		for (int x = 0; x < maxX; x++) {
			if (m_pMatrix[x + n * (y + x)] == MATCH) {
				seqLen++;
			} else {
				if (seqLen >= m_minBlockSize) {
					reportSeq(y + x - seqLen, x - seqLen, seqLen, pSource1, pSource2, outFile);
					blocks++;
				}
				seqLen = 0;
			}
		}

		if (seqLen >= m_minBlockSize) {
			reportSeq(m - seqLen, n - seqLen, seqLen, pSource1, pSource2, outFile);
			blocks++;
		}
	}

	// Scan horizontal part
	for (int x = 1; x < n; x++) {
		unsigned int seqLen = 0;
		int maxY = MIN(m, n - x);
		for (int y = 0; y < maxY; y++) {
			if (m_pMatrix[x + y + n * y] == MATCH) {
				seqLen++;
			} else {
				if (seqLen >= m_minBlockSize) {
					reportSeq(y - seqLen, x + y - seqLen, seqLen, pSource1, pSource2, outFile);
					blocks++;
				}
				seqLen = 0;
			}
		}

		if (seqLen >= m_minBlockSize) {
			reportSeq(m - seqLen, n - seqLen, seqLen, pSource1, pSource2, outFile);
			blocks++;
		}
	}

	return blocks;
}

void Duplo::run(std::ostream& out)
{
	if (m_Xml) {
		out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
		out << "<?xml-stylesheet href=\"duplo.xsl\" type=\"text/xsl\"?>" << std::endl;
		out << "<duplo version=\"" << VERSION << "\">" << std::endl;
		out << "    <check Min_block_size=\"" << m_minBlockSize << "\" Min_char_line=\"" << m_minChars << "\" Ignore_prepro=\"" << (m_ignorePrepStuff ? "true" : "false") << "\" Ignore_same_filename=\"" << (m_ignoreSameFilename ? "true" : "false") << "\">" << std::endl;
	}

	clock_t start, finish;
	double duration;

	start = clock();

	if (!m_quiet)
		std::cout << "Loading and hashing files ... ";
	std::cout.flush();

	std::vector<SourceFile*> sourceFiles;

	int files = 0;
	int locsTotal = 0;

	// Create vector with all source files
	for (int i = 0; i < (int)m_inputFiles.size(); i++) {
		if (m_inputFiles[i].size() > 5) {
			SourceFile* pSourceFile = new SourceFile(m_inputFiles[i], m_minChars, m_ignorePrepStuff);
			int numLines = pSourceFile->getNumOfLines();
			if (numLines > 0) {
				files++;
				sourceFiles.push_back(pSourceFile);
				locsTotal += numLines;
				if (m_maxLinesPerFile < numLines) {
					m_maxLinesPerFile = numLines;
				}
			} else {
				delete pSourceFile;
			}
		}
	}

	if (!m_quiet)
		std::cout << "done.\n\n";

	// Generate matrix large enough for all files
	m_pMatrix = new unsigned char[m_maxLinesPerFile * m_maxLinesPerFile];

	int blocksTotal = 0;

	// Compare each file with each other
	for (int i = 0; i < (int)sourceFiles.size(); i++) {
		if (!m_quiet)
			std::cout << sourceFiles[i]->getFilename();
		int blocks = 0;

		for (int j = 0; j < i; j++) {
			if (m_ignoreSameFilename || sourceFiles[i]->getFilename(false) != sourceFiles[j]->getFilename(false)) {
				blocks += process(sourceFiles[i], sourceFiles[j], out);
			}
		}

		if (blocks > 0) {
			if (!m_quiet)
				std::cout << " found " << blocks << " block(s)" << std::endl;
		} else {
			if (!m_quiet)
				std::cout << " nothing found" << std::endl;
		}

		blocksTotal += blocks;
	}

	// Clean up
	for (int i = 0; i < (int)sourceFiles.size(); i++) {
		delete sourceFiles[i];
	}
	sourceFiles.clear();

	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	if (!m_quiet)
		std::cout << "Time: " << duration << " seconds" << std::endl;

	if (m_Xml) {
		out << "        <summary Num_files=\"" << files << "\" Duplicate_blocks=\"" << blocksTotal << "\" Total_lines_of_code=\"" << locsTotal << "\" Duplicate_lines_of_code=\"" << m_DuplicateLines << "\" Time=\"" << duration << "\"/>" << std::endl;
		out << "    </check>" << std::endl;
		out << "</duplo>" << std::endl;
	} else {
		out << "Configuration: " << std::endl;
		out << "  Number of files: " << files << std::endl;
		out << "  Minimal block size: " << m_minBlockSize << std::endl;
		out << "  Minimal characters in line: " << m_minChars << std::endl;
		out << "  Ignore preprocessor directives: " << m_ignorePrepStuff << std::endl;
		out << "  Ignore same filenames: " << m_ignoreSameFilename << std::endl;
		out << std::endl;
		out << "Results: " << std::endl;
		out << "  Lines of code: " << locsTotal << std::endl;
		out << "  Duplicate lines of code: " << m_DuplicateLines << std::endl;
		out << "  Total " << blocksTotal << " duplicate block(s) found." << std::endl << std::endl;
		out << "  Time: " << duration << " seconds" << std::endl;
	}
}
