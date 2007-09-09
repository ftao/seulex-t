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
 
#include "RECompiler.h"
#include "FA.h"
#include "CharacterSet.h"
#include "utils.h"
#include <stdlib.h>
#include <iostream>
using namespace std;

RECompiler::RECompiler(string str)
{
	this->str = str;
	this->index = 0;
	this->nextToken = token(NONE,1);
}

RECompiler::~RECompiler()
{
}

/*
 * 查看下一个字符(下一个字符指针移动)
 */
char RECompiler::readChar()
{
	if(index < str.length())
	{
		index ++ ;
		return str[index-1];
	}
	else
		return 0;
}

/*
 * 查看下一个字符(下一个字符指针不移动)
 */
char RECompiler::peekChar()
{
	if(index < str.length())
		return str[index];
	else
		return 0;
}
/*
 * 查看下一Token
 */
token RECompiler::readToken()
{
	token tok;
	tok = (nextToken.first == NONE) ? readNextToken():nextToken;
	nextToken = token(NONE,1);
 	return tok;
}
/*
 * 查看一下token 不改变指针
 */
token RECompiler::peekToken()
{

	if(nextToken.first == NONE)
	{
		nextToken = readNextToken();
	}
 	return nextToken;
}
/*
 * 实际的读token操作 
 * 会分析是否是元字符
 * meta chars 
\ [ ] ^ - ? . * + | ( ) $ / { } % < >
*/
token RECompiler::readNextToken()
{
    char c = readChar();
  
    if (c == 0)
        return token(SEOF,1);
  	if(string("[]|()\"").find(c)!=string::npos)
        return token(META,c);
    if(c == '.')
        return token(META,c);
    if(c == '\\')
    {
    	char cc = readChar();
    	cc = CharacterSet::unescape(cc);
    	return token(cc,cc);
    }
	return token(c,c);	
}

/*
 * 跳过一些字符
 */
void RECompiler::skipTokens(string bag)
{
	 while (peekToken().first != SEOF && bag.find(peekToken().second)!=string::npos)
          readToken();
}



/*
 * 转化称NFA
 */
FA RECompiler::toFA()
{
	this->index = 0;
	this->nextToken = token(NONE,1);
	return this->compileExpr();
}

/*
 * 编译一个表达式
 */
FA RECompiler::compileExpr()
{

	FA fa = FA::NULL_FA;
	while(peekToken().first!=SEOF && (peekToken().first != META || peekToken().second != ')'))
	{
		fa = FAunion(fa, compileSequence());
		log("skipTokens |");
		skipTokens("|");
	}
	return fa;
}

/*
 * 编译若干Item组成的序列
 */
FA RECompiler::compileSequence()
{
	
	FA fa = FA::EMPTY_STRING_FA;
	//循环在下一个token为元字符 ) |  或输入结束时 结束
	while (peekToken().first!=SEOF 
			&& (peekToken().first != META || string(")|").find(peekToken().second)==string::npos))
	{
		fa = concatenation(fa,compileItem());
	}
	return fa;
}

/*
 * 编译一个Item 
 * 
 */
FA RECompiler::compileItem()
{
	//item 的 含义为 一些不可分割的东西加上重复的 次数
	
	FA fa;
	token c = readToken();
	
	if (c.first == META && c.second == '(')	// an item begin with (
	{
		fa = compileExpr();
		token t = readToken();
		if (t.first != META || t.second != ')')
		{
			cout<<"missing ')'";
			return fa;//??
		}	
	}
	else
	{
		
		if(c.first == META && c.second == '"')	// an item begin with "
		{
	
			fa = compileString();
			token t = readToken();
			if (t.first != META || t.second != '"')
			{
				cout<<"missing '\"'";
				return fa;//??
			}				
		}
		else
		{
			if(c.first == META && c.second == '[')	// an item begin with "["
			{
				
				fa = compileCharacterSet();
				token t = readToken();
				if (t.first != META || t.second != ']')
				{
					cout<<"missing ']'";
					return fa;//??
				}				
			}
			else
			{
				if(c.first == META && c.second == '.')
				{
					fa = CharacterSet::ANYCHAR.toFA();	
				}
				else
				{
					fa = singleton(c.second);
				}
			}
		}
	}

	while ((int)peekChar() > 0 && string("?*+{").find(peekChar())!=string::npos)
	{	
		char cc = readChar();
		int min = 0;
		int max = INFINITE;
		string minStr = "";
		string maxStr = "";
		
		switch(cc)
		{
			case '*':fa = closure(fa);break;
			case '?':fa = FAunion(fa, FA::EMPTY_STRING_FA);break;
			case '+':fa = iteration(fa,1);break;
			case '{':
				while(peekChar() != '}' && peekChar() != ',')
				{
					minStr += readChar();
				}
				if ( minStr != "")
					min = atoi(minStr.c_str());
				if( peekChar() == ',' )
				{
					readChar();
					while(peekChar() != '}')
					{
						maxStr += readChar();
					}
					if(maxStr != "")
						max = atoi(maxStr.c_str());
				}
				fa = iteration(fa,min,max);
				readChar();	//should be '}'
				break;
			default:cerr<<"program Error";break;
		}
	}

	return fa;
}

/*
 * 编译一个字符串
 */
FA RECompiler::compileString()
{
	FA fa = FA::EMPTY_STRING_FA;
	char c ;
	while(peekChar() != '"' && peekChar() != 0)
	{
		c = readChar();
		if(c == '\\')	//\t \n \v \\ \" etc...
		{
			c = CharacterSet::unescape(readChar());
		}
		fa = concatenation(fa,singleton(c));
	}
	return fa;
}
/*
 * 编译[]中的内容
 */
FA RECompiler::compileCharacterSet()
{
	FA fa;
	string str = "";
	bool neg = false;
	
	if(peekChar() == '^')
	{
		readChar();
		neg = true;	
	}
	while(peekChar() != ']' && peekChar() != 0)
	{
		str += readChar();
	}
	
	CharacterSet cs = CharacterSet(str);
	
	if(neg)
		cs = cs.complement();
	return cs.toFA();;
}

