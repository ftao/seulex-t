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
 
#ifndef CHARACTERSET_H_
#define CHARACTERSET_H_

#include <string>
#include <iostream>
#include <list>
#include <vector>
#include "FA.h"

using namespace std;



typedef pair<unsigned int,unsigned int> CharRange ;
typedef vector<CharRange> CharRanges;
typedef list<CharRange> CharRangesList;

//deal with Re like a-z [^a-z] etc..   
class CharacterSet
{
private:
    
public:
	CharRanges ranges;
    CharacterSet(string str);
    virtual ~CharacterSet();
    CharacterSet(CharRanges ranges);
    CharacterSet(CharRange range);
    void convertString(string str);
    void tidyRanges();
    bool matches(char c);
    CharacterSet complement();
    FA toFA();
    static CharacterSet ANYCHAR;
    static string escape(unsigned int c);
    static unsigned int unescape(unsigned int c);
    friend bool operator == (CharacterSet csa, CharacterSet csb);
	friend ostream& operator << (ostream& out,CharacterSet cs);
};

#endif /*CHARACTERSET_H_*/
