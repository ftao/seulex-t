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
 
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "utils.h"
#include "FA.h"
#include "RECompiler.h"
#include "CharacterSet.h"
#include "table.h"
using namespace std;

void testParseLFile()
{
	ifstream in("s.l",ios::in);
	ofstream out("out.txt",ios::out);
	parseLFile(in);
	FA fa = FA::NULL_FA;
	for(vector<FA>::iterator i = fas.begin(); i != fas.end(); i++)
	{	
		fa = FAunion(fa,*i);
		//fa = fa.determinized().minimized();	
	}
	cerr<<fa<<endl;
	FA dfa = fa.determinized();
	cerr<<dfa<<endl;
	FA mdfa = dfa.minimized();
	cerr<<mdfa.sorted()<<endl;
	string tmp;
	cin>>tmp;
	while(!cin.eof())
	{
		cout<<tmp<<" ? "<<mdfa.accepts(tmp)<<endl;
		cin>>tmp;
	}

}

void testParseLFile2()
{
	ifstream in("c.l",ios::in);
	ofstream out("out.txt",ios::out);
	parseLFile(in);
	FA fa = FA::NULL_FA;
	for(vector<FA>::iterator i = fas.begin(); i != fas.end(); i++)
	{	
		fa = FAunion(fa,*i);
		//i->printMetaDatas();
		//fa = fa.determinized().minimized();	
	}
	//cerr<<fa<<endl;
	//fa.printMetaDatas();
	FA dfa = fa.determinized();
	//cerr<<dfa<<endl;
	//dfa.printMetaDatas();
	FA mdfa = dfa.minimized();
	//mdfa.printMetaDatas();
	//cerr<<mdfa.sorted()<<endl;
	mdfa = mdfa.sorted();
	//cerr<<mdfa<<endl;
	//mdfa.printMetaDatas();
	mdfa.printTable();
	mdfa.print_yy_accept();
	return;
	string tmp;
	cin>>tmp;
	while(!cin.eof())
	{
		cout<<tmp<<" ? "<<actions[mdfa.matchWhich(tmp)]<<endl;
		cin>>tmp;
	}

}

void outputTable()
{
	ifstream in("s.l",ios::in);
	ofstream out("out.txt",ios::out);
	vector<FA> fas;
	vector<string> actions;
	parseLFile(in);
	FA fa = FA::NULL_FA;
	for(vector<FA>::iterator i = fas.begin(); i != fas.end(); i++)
	{	
		fa = FAunion(fa,*i);
		//i->printMetaDatas();
		//fa = fa.determinized().minimized();	
	}
	//cerr<<fa<<endl;
	//fa.printMetaDatas();
	FA dfa = fa.determinized();
	//cerr<<dfa<<endl;
	//dfa.printMetaDatas();
	FA mdfa = dfa.minimized();
	//mdfa.printMetaDatas();
	//cerr<<mdfa.sorted()<<endl;
	mdfa = mdfa.sorted();
	//cerr<<mdfa<<endl;
	//mdfa.printMetaDatas();

}

void testRecomlier()
{
	//CharacterSet cs = CharacterSet(".");
	//cout<<cs;
	
	cout<<"ab|a\n"<<RECompiler("ab|a").toFA()<<endl;
	cout<<"[a-c0-4]\n"<<RECompiler("[a-c0-4]").toFA()<<endl;
	cout<<"[^a-z0-9A-Z]\n"<<RECompiler("[^a-z0-9A-Z]").toFA()<<endl;
	cout<<"a*\n"<<RECompiler("a*").toFA()<<endl;
	cout<<"a+\n"<<RECompiler("a+").toFA()<<endl;
	cout<<"a?\n"<<RECompiler("a?").toFA()<<endl;
	cout<<"a{1,2}\n"<<RECompiler("a{1,2}").toFA()<<endl;
	cout<<"a{1,}\n"<<RECompiler("a{1,}").toFA()<<endl;
	cout<<"a{0,}\n"<<RECompiler("a{0,}").toFA()<<endl;
	cout<<"(a)\n"<<RECompiler("(a)").toFA()<<endl;
	cout<<"((a-b))\n"<<RECompiler("((a-b))").toFA()<<endl;
	cout<<"\"[a](b)\"\n"<<RECompiler("\"[a](b)\"").toFA()<<endl;
	cout<<"\\\"[a](b)\n"<<RECompiler("\\\"[a](b)").toFA()<<endl;

}
void testCharacterSet()
{
	cout<<"a-zb-c\n"<<CharacterSet("a-zb-c")<<endl;
	cout<<"a-zb-c\n"<<CharacterSet("a-zb-c").toFA()<<endl;
	cout<<"^0-9b-c\n"<<CharacterSet("0-9b-c").complement()<<endl;
	cout<<"^0-9b-c\n"<<CharacterSet("0-9b-c").complement().toFA()<<endl;
}
void testReCompilerFromCin()
{
	string tmp;
	cin>>tmp;
	//FA fa = CharacterSet(tmp).complement().toFA();
	FA fa = RECompiler(tmp).toFA().determinized();
	cout<<fa;
	cin>>tmp;
	cout<<fa.accepts(tmp)<<endl;
}
void testFAOP()
{
	FA fa1 = singleton('a');
	cout<<"fa1(a sigle 'a'):\n"<<fa1<<endl;
	//cout<<fa1.nextAvailableState()<<endl;
	//cout<<fa1.sorted(2)<<endl;
	cout<<"option fa1:\n"<<option(fa1)<<endl;
	cout<<"concatenation a with a (that is 'aa'\n"<<concatenation(fa1,fa1)<<endl;
	cout<<"closure fa1 (that is a*):\n"<<closure(fa1)<<endl;
	cout<<"iterator  a{1,2}:\n"<<iteration(fa1,1,2)<<endl;
	cout<<"iterator  a{0,2}:\n"<<iteration(fa1,0,2)<<endl;;

	FA fa2 = singleton('b');
	cout<<"fa2 ( a sigle 'b':\n"<<fa2<<endl;
	cout<<"concatenation fa1 with fa2 (that is 'ab'\n"<<concatenation(fa1,fa2)<<endl;
	cout<<"union fa1 fa2 that is a|b:\n"<<FAunion(fa1,fa2)<<endl;
	
	cout<<"FA::EMPTY_STRING_FA:\n"<<FA::EMPTY_STRING_FA<<endl;
	cout<<"FA::NULL_FA:\n"<<FA::NULL_FA<<endl;
	cout<<"union fa1 with NULL_FA\n"<<FAunion(FA::NULL_FA,fa1)<<endl;
}

void testFA()
{
	FA fa = RECompiler("abc|ab|a").toFA();
	FA fa2 = fa.determinized();
	cout<<"abc|ab|a\n"<<fa<<endl;
	cout<<"determinized():\n"<<fa2<<endl;
	cout<<"Match a ?"<<fa2.accepts("a")<<endl;
	cout<<"Match ab ?"<<fa2.accepts("ab")<<endl;
	cout<<"Match abc ?"<<fa2.accepts("abc")<<endl;
	cout<<"Match b ?"<<fa2.accepts("b")<<endl;
	cout<<"Match c ?"<<fa2.accepts("c")<<endl;
	
}

void testDFA()
{
	FA fa = RECompiler("\"abc\"").toFA();
	cout<<fa<<endl;
	FA fa2 = fa.determinized();
	cout<<fa2<<endl;
	FA fa3 = fa2.minimized();
	cout<<fa3<<endl;
	cout<<fa3.sorted()<<endl;
//	cout<<"abc|ab|a\n"<<fa<<endl;
//	cout<<"determinized():\n"<<fa2<<endl;
//	cout<<"Match a ?"<<fa2.accepts("a")<<endl;
//	cout<<"Match ab ?"<<fa2.accepts("ab")<<endl;
//	cout<<"Match abc ?"<<fa2.accepts("abc")<<endl;
//	cout<<"Match b ?"<<fa2.accepts("b")<<endl;
//	cout<<"Match c ?"<<fa2.accepts("c")<<endl;
	
}

void testFA2()
{
	FA fa = RECompiler("a*").toFA();
	FA fa2 = fa.determinized();
	//cout<<"a\n"<<RECompiler("a").toFA().determinized()<<endl;
	cout<<"a*\n"<<fa<<endl;
	cout<<"determinized():\n"<<fa2<<endl;
	cout<<"Match \"\" ?"<<fa2.accepts("")<<endl;
	cout<<"Match a ?"<<fa2.accepts("a")<<endl;
	cout<<"Match aa ?"<<fa2.accepts("aa")<<endl;
	cout<<"Match aaa ?"<<fa2.accepts("aaa")<<endl;
	cout<<"Match b ?"<<fa2.accepts("b")<<endl;
}
void testFA3()
{
	FA fa = RECompiler("[0-9]").toFA();
	FA fa2 = fa.determinized();
	cout<<"[0-9]\n"<<fa<<endl;
	cout<<"determinized():\n"<<fa2<<endl;
	cout<<"Match \"\" ?"<<fa2.accepts("")<<endl;
	cout<<"Match 0 ?"<<fa2.accepts("0")<<endl;
	cout<<"Match 9 ?"<<fa2.accepts("1")<<endl;
	cout<<"Match a ?"<<fa2.accepts("a")<<endl;
	cout<<"Match 11 ?"<<fa2.accepts("11")<<endl;
	cout<<"Match \\t ?"<<fa2.accepts("\t")<<endl;
}
void testFA4()
{
	FA fa = RECompiler("[^0-9a-zA-Z]").toFA();
	FA fa2 = fa.determinized();
	cout<<"[^0-9a-zA-Z]\n"<<fa<<endl;
	cout<<"determinized():\n"<<fa2<<endl;
	cout<<"Match \"\" ?"<<fa2.accepts("")<<endl;
	cout<<"Match 1 ?"<<fa2.accepts("1")<<endl;
	cout<<"Match a ?"<<fa2.accepts("a")<<endl;
	cout<<"Match - ?"<<fa2.accepts("-")<<endl;
	cout<<"Match ## ?"<<fa2.accepts("##")<<endl;
	cout<<"Match \\t ?"<<fa2.accepts("\t")<<endl;
}

void testFA5()
{
	FA fa1 = RECompiler("unsigned").toFA();
	cout<<fa1<<endl;
	//cout<<"unsigned"<<fa1.accepts("unsigned")<<endl;
	fa1.addMetaDataForAll(1);
	fa1 = fa1.determinized();
	cout<<"unsigned"<<fa1.matchWhich("unsigned")<<endl;

	FA fa2 = RECompiler("signed").toFA();
	cout<<fa2<<endl;
	//cout<<"signed"<<fa2.accepts("signed")<<endl;
	fa2.addMetaDataForAll(2);
	fa2 = fa2.determinized();
	cout<<"signed"<<fa2.matchWhich("signed")<<endl;
	
	FA fa = FAunion(fa1,fa2);
	cout<<fa;
	fa.printEpsilonClosures();
	fa.printTransitionsFrom();
	fa = fa.determinized();
	cout<<fa;

	cout<<"unsigned"<<fa.accepts("unsigned")<<endl;
	cout<<"signed"<<fa.accepts("signed")<<endl;
	
	cout<<"unsigned"<<fa.matchWhich("unsigned")<<endl;
	cout<<"signed"<<fa.matchWhich("signed")<<endl;	
}

void testFA6()
{
	FA fa1 = RECompiler("unsigned").toFA();
	cout<<fa1<<endl;
	//cout<<"unsigned"<<fa1.accepts("unsigned")<<endl;
	fa1.addMetaDataForAll(1);
	//fa1 = fa1.determinized();
	//cout<<"unsigned"<<fa1.matchWhich("unsigned")<<endl;

	FA fa2 = RECompiler("signed").toFA();
	cout<<fa2<<endl;
	//cout<<"signed"<<fa2.accepts("signed")<<endl;
	fa2.addMetaDataForAll(2);
	//fa2 = fa2.determinized();
	//cout<<"signed"<<fa2.matchWhich("signed")<<endl;
	
	FA fa = FAunion(fa1,fa2);
	//cout<<fa;
	//fa.printEpsilonClosures();
	//fa.printTransitionsFrom();
	fa = fa.determinized();
	cout<<fa;

	//cout<<"unsigned"<<fa.accepts("unsigned")<<endl;
	//cout<<"signed"<<fa.accepts("signed")<<endl;
	
	cout<<"unsigned"<<fa.matchWhich("unsigned")<<endl;
	cout<<"signed"<<fa.matchWhich("signed")<<endl;	
	fa = fa.minimized();
	cout<<fa;
	cout<<"unsigned"<<fa.matchWhich("unsigned")<<endl;
	cout<<"signed"<<fa.matchWhich("signed")<<endl;		
}