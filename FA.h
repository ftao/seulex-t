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
 * ���Ͷ���
 */
 
/*
 *state_type
 *int 
 */
typedef int state_type;

/*
 *states_type
 *״̬���� 
 */ 
typedef vector<state_type> states_type;
/*
 * transition_type
 * ��Ԫ�� from to label
 */
typedef triple transition_type;

/*
 * transitions_type
 * ת���ļ���
 */
typedef vector<transition_type> transitions_type;

/*
 * epsilonClousres_table_type 
 * һ�� map 
 * key Ϊһ��״̬ (state_type)
 * value Ϊ���Ӧ�Ķ�Ӧ��epsilonClousres (states_type)
 */
typedef map<state_type,states_type> epsilonClousres_table_type;

/*
 * transitions_table_type 
 * һ�� map 
 * key Ϊһ��״̬ (state_type)
 * value Ϊ���Ӧ��ת��·����ŵļ���  (set<int>)
 */
 
typedef map<state_type,vector<int> > transitions_table_type;

/*
 * transitions_table_by_type 
 * һ�� map 
 * key Ϊһ����ǩ (string)
 * value Ϊ���Ӧ��ת��·����ŵļ���  (set<int>)
 */
typedef map<unsigned int,vector<int> > transitions_table_by_type;

/*
 *��NFA -> DFA �㷨��ʹ�õ����ݽṹ
 *һ��NFA״̬���� ��һ�� ��־λ
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
    //�������Ǻ�����ĸ�� (alphabet),��ĸ��Ϊ����char�������ܱ����ַ�
    
    
    transitions_table_type _transitionsFrom; 
    transitions_table_type _transitionsTo;
    transitions_table_by_type _transitionsBy;
     
    epsilonClousres_table_type _epsilonClosures; 
    
    /*
     *����metaData
     *��Ҫ�������״̬��Ӧlex�����ļ�(.l�ļ�) ��������ʽ�ı��.
     *�����ж��(����ifͬʱƥ��ؼ���if �ͱ���if. ���С������.)
     */
    meta_datas_type metaDatas;

    //һЩ��־ 
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
