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
 

/*
 * 
Some Utils struct or function
*/

#ifndef UTILS_H
#define UTILS_H
#include <vector>
#include <string>
#include <iostream>
using namespace std;

 
class triple
{
public:
    int from;
    int to;
    unsigned int  label;
    triple(int from,int to,unsigned int label)
    {
    	this->from = from;
    	this->to = to;
    	this->label = label;
    } 
    friend ostream& operator <<(ostream& out, const triple tri)
    {
    	out<<"("<<tri.from<<"-";
    	out<<'"'<<(char)tri.label<<'"';
    	out<<"-"<<tri.to<<")";
    	return out;
    }
    /*
     * 依次比较triple的 from to label 来比较两个 triple
     */
    friend bool operator <(const triple a,const triple b)
    {
    	if(a.from != b.from)
    		return a.from < b.from;
    	if(a.to != b.to)
    		return a.to < b.to;
    	return a.label < b.label;   	
    }
    
    /*
     * 依次比较triple的 from to label 来比较两个 triple
     */
    friend bool operator ==(const triple a,const triple b)
    {
    	if(a.from != b.from)
    		return false;
    	if(a.to != b.to)
    		return false;
    	//return true;
    	return a.label == b.label;   	
    }
    
};


template <class T>
vector<T> makeOneElementVector(T obj)
{
	vector<T> t = vector<T>();
	t.push_back(obj);
	return t;
}

template <class T>
vector<T> makeEmptyVector()
{
	vector<T> t = vector<T>();
	return t;
}

const int debug = 0;

void log(string msg);

string get_file_contents(char* filename);
string dump_string(string name, string content);
#endif
