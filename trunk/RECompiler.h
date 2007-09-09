 /***************************************************************************
 *   This file is part of slex - A Lexical Analyzer Generator.             *
 *   Copyright (C) 2007 by                                                 *
 *   Tao Fei (Filia.Tao@gmail.com)                                         *
 *   Tang Junjie (junjietang.jie@gmail.com)                                *
 *   Song Mingzhou (songmingzhou@hotmail.com)                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
#ifndef RECOMPILER_H_
#define RECOMPILER_H_

#include <string>
#include "FA.h"

const int  ANY  = -2;
const int  SEOF = -1;
const int  NONE = -3;
const int  META = -4;

using namespace std;

typedef pair<int,unsigned char> token;


class RECompiler
{
private:
	unsigned int index;
	string str;
	token nextToken;
	
protected:
	token readNextToken();
public:
	RECompiler(string str);

	virtual ~RECompiler();
	FA toFA();
	FA compileExpr();
	FA compileConjunction();
	FA compileItem();
	FA compileSequence();
	FA compileString();
	FA compileCharacterSet();
	char readChar();
	char peekChar();
	token peekToken();
	token readToken();
	void skipTokens(string bag);
};

#endif
