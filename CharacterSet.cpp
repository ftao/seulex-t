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
 
 
#include "CharacterSet.h"
#include <sstream>

CharacterSet::~CharacterSet()
{
}

CharacterSet::CharacterSet(string str)
{
    this->convertString(str);
    this->tidyRanges();
}

CharacterSet::CharacterSet(CharRanges ranges)
{
    this->ranges = ranges;
    this->tidyRanges();
}

CharacterSet::CharacterSet(CharRange range)
{
    this->ranges = CharRanges();
    this->ranges.push_back(range);
    this->tidyRanges();
}

CharRanges AnyCharRanges()
{
	CharRanges ranges = CharRanges();
	ranges.push_back(CharRange(0,255));;
	return ranges;	
}

void CharacterSet::convertString(string str)
{
    this->ranges = CharRanges(); //str.Length
    int i = 0;
    char c0,c1;
    while (i < str.length())
    {
        c0 = c1 = str[i];
        if(str[i] == '\\')
        {
        	if(i+1 >= str.length())
        	{
        		//error;
        		return ;
           	}
           	i++;
        	c0 = c1 = CharacterSet::unescape(str[i]);
        }
        i ++;
        if (i + 1 < str.length() && str[i] == '-')
        {
            c1 = str[i+1];
            i += 2;
        }
        this->ranges.push_back(CharRange(c0,c1));
    }
}

/*
 * Tidy The Ranges (sort and combine, 0-8 2-9 to 0-9  etc)
 *
 * 
*/
void CharacterSet::tidyRanges()
{
	//cout<<ranges.size()<<endl;
	sort(ranges.begin(),ranges.end());
	CharRanges::iterator ite = ranges.begin();
	CharRanges::iterator next = ite;
	next ++;
	while( ite != ranges.end() && next != ranges.end()) 
	{
		if(ite->second >= next->first)
		{
			ite->second = max(ite->second,next->second);
			ranges.erase(next);	
		}
		else
		{
			ite ++;
			next = ite;
			next ++ ;
		}
	}
}

bool CharacterSet::matches( char c)
{
	CharRanges::iterator ite = ranges.begin();
	for(; ite!=ranges.end(); ite++)
	{
		if(c >= ite->first && c <= ite->second)
			return true;
	}
	return false;
}

FA CharacterSet::toFA()
{
	FA::states_type states = FA::states_type();
	states.insert(states.end(),0);
	states.insert(states.end(),1);
	FA::states_type finalStates = FA::states_type();
	finalStates.insert(finalStates.end(),1);
	FA::transitions_type trans = FA::transitions_type();
	
	CharRanges::iterator ite = ranges.begin();
	int c;
	for(; ite!=ranges.end(); ite++)
	{
		c = ite->first;
		for(; c <= ite->second; c++)
		{
			trans.insert(trans.end(),FA::transition_type(0,1,c));
		}
	}
	return FA(states,trans,0,finalStates);
}
 
 
string CharacterSet::escape( unsigned int  c)
{
	ostringstream out; 
	switch(c)
	{
		case '\t':out<<"\\t";break;
		case '\n':out<<"\\n";break;
		case '\r':out<<"\\r";break;
		case '\f':out<<"\\f";break;
		case '\v':out<<"\\v";break;
		case '-':return "\\-";break;
		case '\\':return "\\";break;
		default:
		if(c < 32 || c >=126)	//127 is visible
		{
			out<<"\\"<<int(c);
		}
		else
		{
			out<<c;
		}
	}
	return out.str();
}

unsigned int  CharacterSet::unescape( unsigned int c)
{
	switch(c)
	{
		case 't':return '\t';
		case 'n':return '\n';
		case 'r':return '\r';
		case 'f':return '\f';
		case 'v':return '\v';
		//case '"':return '\"';
		//case '\\':return '\\';
		default: return c;
	}
}
/*
 * ¿ÉÄÜÓÐBUG
 */
CharacterSet CharacterSet::complement()
{
	CharRanges crs = CharRanges();
	CharRanges::iterator ite = ranges.begin();
	CharRanges::iterator next = ite + 1;
	if(ite != ranges.end() && ite->first > 0)
	{
			crs.push_back(CharRange(0,ite->first-1));
	}
	for(; next != ranges.end(); ite ++,next = ite + 1)
	{
		if(ite->second + 1 < next->first)
		{
			crs.push_back(CharRange(ite->second + 1, next->first -1));
		}
	}
	
	if(ite->second < 255)
	{
		crs.push_back(CharRange(ite->second + 1, 255));
	}
	return CharacterSet(crs);
}


CharacterSet CharacterSet::ANYCHAR = CharacterSet(AnyCharRanges());


bool operator == (CharacterSet csa, CharacterSet csb)
{
	return csa.ranges == csb.ranges;
}

/*
 * TODO:
 * many bug
 */
ostream& operator << (ostream& out,CharacterSet cs)
{
	if (cs == CharacterSet::ANYCHAR)
	{
		out<<".";
		return out;
	}
	if (cs.ranges.size() == 0)
	{
		out<<"[^.]";
		return out;
	}
	CharRanges::iterator ite = cs.ranges.begin();
	for(;ite != cs.ranges.end(); ite++)
	{
		if(ite->first == ite->second)
			out<<CharacterSet::escape(ite->first);
		if(ite->first+1 == ite->second 
			&& ite->first != '-'
			&& ite->second != '-')
		{

			out<<CharacterSet::escape(ite->first)
			  <<CharacterSet::escape(ite->second);
		}
		else
		{
			out<<CharacterSet::escape(ite->first)
			   <<'-'
			   <<CharacterSet::escape(ite->second);
		}
	}
	
	return out;
}
