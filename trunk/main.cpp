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
#include "tmpl.h"
using namespace std;
vector<FA> fas;
vector<string> actions;
string header;
string footer;
map<string,string> vars;

/*
 * 检测一个字符串是否以另一个字符串结尾. (空格忽略)
 */
bool isEndWith(string str,string delti)
{
	string::size_type pos = str.find_last_of(delti);
	if( pos == string::npos)
		return false;	
	for(string::size_type i = pos + delti.length(); i < str.length(); i++)
	{
		if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r')
			return false;
	}		
	return true;
}


void justCopy(istream& in,string& to,string delit = "")
{
    to = "";
    string temp;
    while(!in.eof())
    {
        getline(in,temp);
        if(in.eof() || (delit != "" && isEndWith(temp, delit)))
        	break;
        to += temp;
        to += "\n";
    }	
}
void parseVar(istream& in)
{
	string temp;
    while(!in.eof())
    {
    	getline(in,temp);
    	#ifdef DEBUG
        cerr<<"Temp :"<<temp<<endl;
        #endif

    	if(isEndWith(temp,"%{") || isEndWith(temp,"%%"))
    		break;
    	string key = "";
    	string value = "";
        if(temp.size() == 0)    //ignore empty line
        	continue;
        unsigned int i = 0;
        while(temp[i] == ' ' || temp[i] == '\t') i++;
        //获得Key
        key += temp[i];
        i++;
        while(temp[i] != '\t' && temp[i] != ' ')
        {
            key += temp[i];
            i++;
        }
        while(temp[i] == ' ' || temp[i] == '\t') i++;
        #ifdef DEBUG
        cerr<<"Key :"<<key<<endl;
        #endif
        //获得Value,如果必要进行替换
        while(i < temp.length())
        {
            if(temp[i] == '{')
            {
                string v = "";
                i++;
                while(temp[i] != '}' && i < temp.length())
                {
                    v += temp[i];
                    i++;
                }
                if(i == temp.length())
                	value += "{" + v;
                else
                {
	                //如果是以前定义过得的变量,进行替换
	                if(vars.find(v) != vars.end())
	                {
	                    value += vars[v];
	                }
	                else
	                {
	                	value += "{" + v + "}";
	                }
                }
            }
            else
            {
                value += temp[i];
            }
            i++;
        }
        #ifdef DEBUG
        cerr<<"value :"<<value<<endl;
        #endif
 		vars[key] = value;
    }	
}
/*
 *解析Lex输入文件
 *为了简化处理,我们约定模式匹配规则部分, 
 *一个规则只能出现在一行.
 *%{ %% %} 等分隔符出现在一行的一开始. 同时该行不能含有其他字符
 */
void parseLFile(istream& in)
{
    //.l 文件第一部分 C 和 Lex 的全局声明
 	//Lex 的变量可能出现在C声明的上方或者下方.
    FA fa = FA::NULL_FA;
	parseVar(in);
	justCopy(in,header,"%}");
	parseVar(in);
    
    //.l 文件第二部分 Lex 的模式匹配规则 
    int regNumber = 0;
    bool isPart2End = false;
    while(!in.eof())
    {
    	if(isPart2End)
    		break;
    	
        int blankPos = 0;       
        string reg = "";
        string action = "";
        string line;
        string temp = "";
        while(!in.eof())
        {
			getline(in,line);
            if(isEndWith(line,"%%"))
            {
            	isPart2End = true;
        		break;
            }
            else
            {
            	if(line.length() == 0)
            		continue;
            	temp += line + "\n";
            	if(isEndWith(line,"}"))
            		break;
            }
        }
        unsigned int i = 0;
        if(temp.size() == 0)
        	continue;
        char c;
        while(temp[i] == ' ' || temp[i] == '\t') i++;
        //now temp[i] is the first non-blank char
        #ifdef DEBUG
        cerr<<"TEMP :"<<temp<<endl;
        #endif
        while(i < temp.length())
        {
            c = temp[i];
            bool normal = true;
            if(c == ' ' && blankPos==0)     //end of reg
            {
               break;
            }
            if(c == '\t' || c== '\n')       //end of reg
            {
               break;
            }
            if(c == '\\')
            {
                if(temp[i+1] != ' ')
                {
	                reg += c;
	                reg += temp[i+1];
	                i++;
	                normal = false;
                }
            }
            if( c == '[' || c == '"')
                blankPos ++ ;
            if( c == ']' || c == '"')
                blankPos -- ; 
            if( c == '{')
            {
                string v = "";
                int j = i+1;
                while(temp[j] != '}')
                {
                    v += temp[j];
                    j ++;
                }
                if(vars.find(v) != vars.end())
                {
                    reg += vars[v];
                    i = j;
                    normal = false;
                }
            }
            if(normal)
                reg += c;
            i++;
        }
        #ifdef DEBUG
        cerr<<"REG:"<<reg<<endl;
        #endif
        while(i < temp.length() && temp[i] != '{') i++;
        //now for action 
        int end = temp.find_last_of("}");
        if(end == string::npos)
        	continue;
        action = temp.substr(i+1,end-i-1);
        #ifdef DEBUG
        cerr<<reg<<'\t'<<action<<endl;
        #endif
        FA fa = RECompiler(reg).toFA();
        fa.addMetaDataForAll(regNumber);
        fas.push_back(fa);
        actions.push_back(action);
        regNumber ++;
    }
    
    //.l 文件第三部分 C 代码
 	justCopy(in,footer);
 	
    #ifdef DEBUG
    cerr<<"----------------------------------"<<endl;
    cerr<<footer<<endl;
    #endif
}

void printHelp()
{
cout<<
"slex - A Lexical Analyzer Generator.\n\
useage: slex [-st] [--help|--copyright] [file]\n\
	-s  don't compress the transition table\n\
	-t  write to stdout instead of lex.yy.c\n\
	--help     produce this help message\n\
	--copyright show copyright infomation\n\
	file  the input file , if not given, stdin will be used.\n\
\n\
This program is created by Tao Fei, Tang Junjie and Song Mingzhou \n\
It is released under GNU General Public License version 2.\n\
Using slex --copyright for more information.\n\
If you have any question,please send a email to Filia.Tao@gmail.com.\n\
";
}

void printCopyright()
{
 cout<<"\n\
 /***************************************************************************\n\
 *   Copyright (C) 2007 by                                                 *\n\
 *   Tao Fei (Filia.Tao@gmail.com)                                         *\n\
 *   Tang Junjie (junjietang.jie@gmail.com)                                *\n\
 *   Song Mingzhou (songmingzhou@hotmail.com)                              *\n\
 *                                                                         *\n\
 *   This program is free software; you can redistribute it and/or modify  *\n\
 *   it under the terms of the GNU General Public License as published by  *\n\
 *   the Free Software Foundation; either version 2 of the License, or     *\n\
 *   (at your option) any later version.                                   *\n\
 *                                                                         *\n\
 *   This program is distributed in the hope that it will be useful,       *\n\
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *\n\
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *\n\
 *   GNU General Public License for more details.                          *\n\
 *                                                                         *\n\
 *   You should have received a copy of the GNU General Public License     *\n\
 *   along with this program; if not, write to the                         *\n\
 *   Free Software Foundation, Inc.,                                       *\n\
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *\n\
 ***************************************************************************/\n\
";
}
void dump_int_array(string name, int size, int* arr,ostream& out)
{
    out<<"int "<<name<<"["<<size<<"] = {";
    for(int i = 0; i < size; i++)
    {
            out<<arr[i];
            if(i < size -1)
                    out<<',';
    }
    out<<"};"<<endl;
}

void go(ostream& out,bool isS = false)
{
    FA fa = FA::NULL_FA;
    
    for(vector<FA>::iterator i = fas.begin(); i != fas.end(); i++)
    {       
        fa = FAunion(fa,*i);
    }
    

    FA dfa = fa.determinized();

    FA mdfa = dfa.minimized();

    mdfa = mdfa.sorted();

    mdfa.genTable(STATE_NUM,ACTION_NUM,automate);
    yy_ec = new int[ACTION_NUM];
    yy_meta = new int[ACTION_NUM];
    yy_def = new int[STATE_NUM];
    yy_base = new int[STATE_NUM];
    actual_action = ACTION_NUM;
    out<<header<<endl;
    //copy head.tmp to outputfile        
    out<<headerTMPL;
    
    if(!isS)
    {
        out<<"#define PACK\n";
        comb();
        out<<"#define ACTION_NUM "<<ACTION_NUM<<endl;
        out<<"#define STATE_NUM "<<STATE_NUM<<endl;
        out<<"#define END_COMB "<<end_comb<<endl;
        dump_int_array("yy_base",STATE_NUM,yy_base,out);
        dump_int_array("yy_ec",ACTION_NUM,yy_ec,out);
        dump_int_array("yy_meta",ACTION_NUM,yy_meta,out);
        dump_int_array("yy_def",STATE_NUM,yy_def,out);
        dump_int_array("yy_chk",end_comb,yy_chk,out);
        dump_int_array("yy_nxt",end_comb,yy_nxt,out);
        mdfa.print_yy_accept(out);
    }
    else
    {
        mdfa.printTable(out);
        mdfa.print_yy_accept(out);
    }
    
    out<<unpackTMPL;
    //here print the switch's
    out<<"\n\t\t\tswitch(current_state)\n\t\t\t{\n";
    FA::states_type& finalStates = mdfa.getFinalStates();
    sort(finalStates.begin(),finalStates.end());
    for(FA::states_type::iterator i = finalStates.begin(); i != finalStates.end(); i++)
    {
            out<<"\t\t\tcase "<<*i<<":"<<actions[mdfa.getMetaData(*i)]<<endl;
            out<<"\t\t\tbreak;"<<endl;
    }
    out<<"\t\t\tdefault:ECHO;\n";
    out<<"\t\t\t}\n";
    out<<footerTMPL<<endl;
    out<<endl;
    out<<footer<<endl;
        
}

int main(int argc, char *argv[])
{
    /*
     *分析命令行参数
     *OPTIONS
     *-t Write the resulting program to standard output instead of lex.yy.c.
     *-s 不压缩转换表
     *OPERANDS 
     *file 输入文件 否则使用stdin;
     */
    bool isT = false;
    bool isS = false;
    vector<string> files;
    for(int i = 1;i< argc; i++)
    {
    		if(strcmp(argv[i],"-t")==0)
    		{
    			isT = true;
    			continue;
    		}
    		if(strcmp(argv[i],"-s")==0)
    		{
    			isS = true;
    			continue;
    		}
     		if(strcmp(argv[i],"--help")==0)
    		{
    			printHelp();
    			return 0;
    		}
      		if(strcmp(argv[i],"--copyright")==0)
    		{
    			printCopyright();
    			return 0;
    		}	    
        	files.push_back(string(argv[i]));
    } 
    if(files.size() > 0)
    {
        ifstream in(files[0].c_str(),ios::in);
        parseLFile(in);
    }
    else
    {
    	//ifstream in("s.l",ios::in);
    	//parseLFile(in);
      	parseLFile(cin);
        
    }
    if(isT)
    {
        go(cout,isS);
    }
    else
    {
        ofstream out("lex.yy.c",ios::out);
        go(out,isS);
            
    }
    
    return 0;
}
