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
 
#ifndef FA_H_
#define FA_H_

#include <set>
#include <vector>
#include <map>
#include <string>
#include "utils.h"
using namespace std;

const unsigned int EPSILON = 999;
const int  INFINITE = -1;

class FA
{   
public:
/*
 * 类型定义
 */
 
/*
 *state_type
 *int 
 */
typedef int state_type;

/*
 *states_type
 *状态集合 
 */ 
typedef vector<state_type> states_type;
/*
 * transition_type
 * 三元组 from to label
 */
typedef triple transition_type;

/*
 * transitions_type
 * 转换的集合
 */
typedef vector<transition_type> transitions_type;

/*
 * epsilonClousres_table_type 
 * 一个 map 
 * key 为一个状态 (state_type)
 * value 为其对应的对应的epsilonClousres (states_type)
 */
typedef map<state_type,states_type> epsilonClousres_table_type;

/*
 * transitions_table_type 
 * 一个 map 
 * key 为一个状态 (state_type)
 * value 为其对应的转换路径编号的集合  (set<int>)
 */
 
typedef map<state_type,vector<int> > transitions_table_type;

/*
 * transitions_table_by_type 
 * 一个 map 
 * key 为一个标签 (string)
 * value 为其对应的转换路径编号的集合  (set<int>)
 */
typedef map<unsigned int,vector<int> > transitions_table_by_type;

/*
 *在NFA -> DFA 算法中使用的数据结构
 *一个NFA状态集合 加一个 标志位
 */
 
typedef map<state_type,int > meta_datas_type;

class d_state_pair
{
public:
	states_type d_state;
	bool marked;
	d_state_pair(states_type d_state,bool marked)
	{
		this->d_state = d_state;
		this->marked  = marked;
	}
	bool operator == (d_state_pair d)
	{
		return this->d_state == d.d_state;
		
	} 
};

protected:
    states_type states;
    transitions_type transitions;
    state_type initialState;
    states_type finalStates;
    
    //set<char> alphabet;
    //这里我们忽略字母表 (alphabet),字母表为所有char类型所能表达的字符
    
    
    transitions_table_type _transitionsFrom; 
    transitions_table_type _transitionsTo;
    transitions_table_by_type _transitionsBy;
     
    epsilonClousres_table_type _epsilonClosures; 
    
    /*
     *关于metaData
     *主要保存结束状态对应lex输入文件(.l文件) 中正则表达式的编号.
     *可能有多个(比如if同时匹配关键字if 和变量if. 编号小的优先.)
     */
    meta_datas_type metaDatas;

    //一些标志 
    int _isSorted;
    int _isTrimmed;
    int _isDeterminized;
    int _isMinimized;
    int _isTransTableIninted;
    int _isEpsilonClosuresIninted;
    
    void initializeTransitionTables();
    
    states_type computeEpsilonClosure(state_type state);
    void computeEpsilonClosures();
    
    vector<int>& transitionsFrom(state_type state);
    vector<int>& transitionsTo(state_type state);
    vector<int>& transitionsBy(unsigned int  label);
    
    states_type& epsilonClosure(state_type state);
    
    
public:  
	FA();
    FA(states_type states,transitions_type transitions,state_type initialState,states_type finalStates);
    
    state_type nextAvailableState();
    
    FA sorted(state_type initial = 0);
	FA trimmed();	
	FA withoutEpsilons();
	FA determinized();
	FA minimized();
   	states_type& getFinalStates(){
   		return finalStates;
   	}
    states_type nextStates(state_type state,unsigned int input);
    
    //only for DFA
    state_type nextState(state_type state,unsigned int  input);
    
    inline bool labelMatches(unsigned int label,unsigned int input){return label == input;}
    bool accepts(string sequence);
    int matchWhich(string sequence);
    
    void addMetaData(state_type state,int number);
    void addMetaDataForAll(int number);
    int getMetaData(state_type state);
    void combineMetaData(meta_datas_type moreMeta);
    
    void printEpsilonClosures(ostream& out = cout);
    void printTransitionsFrom(ostream& out = cout);
    void printMetaDatas(ostream& out = cout);
    void print_yy_accept(ostream& out = cout);
    void printTable(ostream& out = cout);
    
    void genTable(int& state_num,int& action_num,vector<vector<int> > & automate);
    
    
    friend FA closure(FA fa);
    friend FA FAunion(FA fa1,FA fa2);
    friend FA iteration(FA fa, int min = 1, int max = INFINITE);
    friend FA concatenation(FA fa1,FA fa2);
    friend FA option(FA fa);
    friend ostream& operator << (ostream& out, FA fa);
    
    //some constant Re
    static FA NULL_FA;
    static FA EMPTY_STRING_FA;
    static state_type NON_STATE;
};

FA singleton(char c);

#endif
