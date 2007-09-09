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
 
#include <iostream>
#include "utils.h"
#include "FA.h"
#include "CharacterSet.h"
#include <time.h>
#include <deque>

using namespace std;

/*--------------------------------------------------------------
 * 初始化
 *--------------------------------------------------------------*/

/*
 * 计算一个状态的EpsilonClosure
 * 
 */
FA::states_type FA::computeEpsilonClosure(state_type state)
{
	states_type ec = states_type();
	ec.insert(ec.end(),state);
	for(states_type::size_type index = 0; index <ec.size(); index++)
	{
		vector<int> transFrom = transitionsFrom(ec[index]);
		for(vector<int>::iterator ite = transFrom.begin(); ite != transFrom.end(); ite ++)
		{
			if(transitions[*ite].label == EPSILON && find(ec.begin(),ec.end(),transitions[*ite].to)==ec.end())
			{
				ec.insert(ec.end(),transitions[*ite].to);
			}
		}
	}
	return ec;
}


/*
 *计算所有状态的EpsilonClosure
 */
void FA::computeEpsilonClosures()
{
	_epsilonClosures = epsilonClousres_table_type();
	for(states_type::iterator i = states.begin(); i != states.end(); i++)
	{
		_epsilonClosures[*i] = computeEpsilonClosure(*i);
	}
}


/*
 * 初始化转换表
 */
void FA::initializeTransitionTables()
{
	_transitionsFrom =  transitions_table_type();
	states_type::iterator ite ;
	for(ite = states.begin(); ite != states.end(); ite++)
	{
		_transitionsFrom[*ite] = vector<int>(); 
		_transitionsTo[*ite] = vector<int>(); 
	}
	for(int c = 0; c < 256; c++)
	{
		_transitionsBy[c] = vector<int>();
	}
	transitions_type::size_type pos;
	for(pos = 0; pos < transitions.size(); pos++)
	{
		
		_transitionsFrom[transitions[pos].from].push_back(pos); 
		_transitionsTo[transitions[pos].to].push_back(pos);
		_transitionsBy[transitions[pos].label].push_back(pos);
	}
	_isTransTableIninted = 1;
}



/*--------------------------------------------------------------
 * Access Methods 
 *--------------------------------------------------------------*/

/*
 * 返回状态state 的 epsilonClosure
 */
FA::states_type& FA::epsilonClosure(state_type state)
{
	if(_epsilonClosures.find(state) == _epsilonClosures.end())
		computeEpsilonClosures();
	
	return _epsilonClosures[state];	
}


/*
 * 返回所有以state为开始端的转换
 */
vector<int>& FA::transitionsFrom(state_type state)
{
	if(_isTransTableIninted != 1)
		initializeTransitionTables();
	return _transitionsFrom[state];
}

/*
 * 返回所有以state为结束端的转换编号集合
 */
vector<int>& FA::transitionsTo(state_type state)
{
	if(_isTransTableIninted != 1)
		initializeTransitionTables();
	return _transitionsTo[state];
}

/*
 * 返回所有以label为标签的转换
 */
vector<int>& FA::transitionsBy(unsigned int label)
{
	if(_isTransTableIninted != 1)
		initializeTransitionTables();
	return _transitionsBy[label];
}

/*
 * 返回下一个可用的状态
 */
int FA::nextAvailableState()
{
	return 1 + *(max_element(states.begin(),states.end()));
}


/*
 *返回state 在 input 下 的后续状态集
 *
 */
FA::states_type FA::nextStates(state_type state,unsigned int input)
{
	states_type nss = states_type();
	vector<int>& trans = transitionsFrom(state);
	for(vector<int>::iterator i = trans.begin(); i != trans.end(); i++)
	{
		if(labelMatches(transitions[*i].label, input) && find(nss.begin(),nss.end(),transitions[*i].to)==nss.end())
		{
			states_type epc = epsilonClosure(transitions[*i].to);
			nss.insert(nss.end(),epc.begin(),epc.end());
		}
	}
	//sort(nss.begin(),nss.end());
	return nss;
}


/*
 *返回state 在 input 下 的后续状态
 *only for DFA 
 *not using nextStates instead write this by own
 *TODO:check for no non-DFA to call this method
 */
FA::state_type FA::nextState(state_type state,unsigned int  input)
{

	state_type nextS = NON_STATE;
	vector<int>& trans = transitionsFrom(state);
	for(vector<int>::iterator i = trans.begin(); i != trans.end(); i++)
	{
		if(labelMatches(transitions[*i].label, input))
		{
			nextS = transitions[*i].to;
			break;
		}
	}
	return nextS; 
}


/*
 *如果DFA 可以识别 sequence 返回 true
 */
bool FA::accepts(string sequence)
{
	states_type sts = states_type();
	states_type newSts = states_type();
	sts.insert(sts.end(),initialState);
	for(string::iterator i = sequence.begin(); i != sequence.end(); i++)
	{
		newSts.clear();
		for(states_type::iterator pos = sts.begin(); pos != sts.end(); pos++)
		{
			states_type nSts = nextStates(*pos,*i); 
			for(states_type::iterator it = nSts.begin(); it != nSts.end(); it++)
			{
				if(find(newSts.begin(),newSts.end(),*it)==newSts.end())
				{
					newSts.insert(newSts.end(),*it);	
				}
			}
		}
		sts = newSts;
	}
	
	/*
	 * 检查最终状态是否在finalStates中
	 */
	for(states_type::iterator it = sts.begin(); it != sts.end(); it++)
	{
		if(find(finalStates.begin(),finalStates.end(),*it) != finalStates.end())
			return true;
	}
	return false;
}

/*
 * 返回匹配的第一个正则表达式的编号 (没有则返回-1)
 */
int FA::matchWhich(string sequence)
{
	states_type sts = states_type();
	states_type newSts = states_type();
	sts.insert(sts.end(),initialState);
	for(string::iterator i = sequence.begin(); i != sequence.end(); i++)
	{
		newSts.clear();
		for(states_type::iterator pos = sts.begin(); pos != sts.end(); pos++)
		{
			states_type nSts = nextStates(*pos,*i); 
			for(states_type::iterator it = nSts.begin(); it != nSts.end(); it++)
			{
				if(find(newSts.begin(),newSts.end(),*it)==newSts.end())
				{
					newSts.insert(newSts.end(),*it);	
				}
			}
		}
		sts = newSts;
	}
	
	/*
	 * 检查最终状态是否在finalStates中
	 */
	int min = -1;
	for(states_type::iterator it = sts.begin(); it != sts.end(); it++)
	{
		states_type::iterator result = find(finalStates.begin(),finalStates.end(),*it);
		if(result != finalStates.end())
		{
			if(min == -1 ||  metaDatas[*result] < min)
				min = metaDatas[*result];
		}
	}
	if(min == -1)
		return -1;
	else
		return min;	
}

/*--------------------------------------------------------------
 * Reduce Operations 
 * 对FA的一些状态简化操作.
 *--------------------------------------------------------------*/

/*
 *返回一个等价的FA，但是其最小的状态(初始的状态)为initail)
 */
FA FA::sorted(int initial)
{
	if(_isSorted)
		return *this;
	
	map<int,int> mapping = map<int,int>();
	
	FA copy = FA();
	states_type::iterator ite = states.begin();
	for(; ite != states.end(); ite ++)
	{
		copy.states.insert(copy.states.end(),ite - states.begin() + initial);
		mapping[*ite] = ite - states.begin() + initial;
	}
	
	//如果第一个状态不是 初始态,交换
	if(initialState != *states.begin())
	{
		swap(mapping[initialState],mapping[*states.begin()]);
	}
	
	for(ite = finalStates.begin(); ite != finalStates.end(); ite++)
	{
		copy.finalStates.insert(copy.finalStates.end(),mapping[*ite]);
	}
	
	transitions_type::iterator itr = transitions.begin();
	for(; itr != transitions.end(); itr ++)
	{
		copy.transitions.insert(copy.transitions.end(),transition_type(mapping[itr->from],mapping[itr->to],itr->label));
	}
	copy.initialState = mapping[initialState];
	
	meta_datas_type newMeta = meta_datas_type();
	for(meta_datas_type::iterator i = metaDatas.begin(); i != metaDatas.end(); i++)
	{
		newMeta.insert(make_pair(mapping[i->first],i->second));
	}
	copy.metaDatas = newMeta;
	copy._isSorted = 1;
	
	return copy;
}



/*
 *返回等价的 DFA
 *
 *
 */
 
FA FA::determinized()
{
	FA dfa = FA();
	dfa.states.insert(dfa.states.end(),0);
	dfa.initialState = 0;
	
	vector<d_state_pair> statesSets = vector<d_state_pair>();
	states_type ec = epsilonClosure(initialState);
	sort(ec.begin(),ec.end());
	statesSets.insert(statesSets.end(),d_state_pair(ec,false));
	//如果DFA 的初态中含有NFA的终态
	for(states_type::iterator j = ec.begin(); j != ec.end(); j++)
	{
		if(find(finalStates.begin(),finalStates.end(),*j) != finalStates.end())
		{
			dfa.finalStates.insert(dfa.finalStates.end(),0);
			break;
		}
	}

	int num = 0;
	//对所有DFA状态
	for(vector<d_state_pair>::size_type index = 0; index<statesSets.size(); index++)
	{
		
		//当前的DFA 状态 (多个NFA状态)
		d_state_pair* i = &statesSets[index];
		if(i->marked)
			continue;
		i->marked = true;

		//复杂度 256 * 当前DFA状态中NFA状态数 * 该NFA状态的边数 
		for(unsigned int c = 0; c < 256; c++)//所有可能的转换
		{ 
			i = &statesSets[index];
			
			states_type u = states_type();
			vector<int> transBy = transitionsBy(c);
			if(transBy.size() == 0)
				continue;
			sort(transBy.begin(),transBy.end());
			//检查所有NFA状态的是否有对应的边
			for(states_type::iterator it = i->d_state.begin(); it != i->d_state.end(); it++)
			{
				
				vector<int> transFrom = transitionsFrom(*it);
				sort(transFrom.begin(),transFrom.end());
				
				vector<int> tmp = vector<int>(transBy.size());
				
				vector<int>::iterator end = set_intersection(transBy.begin(),transBy.end(),transFrom.begin(),transFrom.end(),tmp.begin());
				vector<int> result = vector<int>(tmp.begin(),end);
				for(vector<int>::iterator pos = result.begin(); pos != result.end(); pos++)
				{
					num ++;
					ec = epsilonClosure(transitions[*pos].to);
					u.insert(u.end(),ec.begin(),ec.end());
				}
				
			}		
			sort(u.begin(),u.end());
			states_type::iterator end = unique(u.begin(),u.end());
			u = states_type(u.begin(),end); 
			if(u.size()==0)
				continue;

			d_state_pair dsp = d_state_pair(u,false);
			vector<d_state_pair>::iterator position = find(statesSets.begin(),statesSets.end(),dsp);
			int to = position - statesSets.begin();
			int from = index;
			if(position == statesSets.end())
			{
				dfa.states.insert(dfa.states.end(),to);	
				statesSets.insert(statesSets.end(),dsp);
			}
			dfa.transitions.insert(dfa.transitions.end(),FA::transition_type(from,to,c));
			
			bool isFinal = false;
			for(states_type::iterator j = dsp.d_state.begin(); j != dsp.d_state.end(); j++)
			{
				num ++;
				bool isThisFinal = find(finalStates.begin(),finalStates.end(),*j) != finalStates.end();
				if(isThisFinal)
				{
					if(!isFinal)
					{
						dfa.metaDatas[to] = metaDatas[*j];
					}
					else
					{
						if(dfa.metaDatas[to] > metaDatas[*j])
							dfa.metaDatas[to] = metaDatas[*j];
					}
					isFinal = true;
				}
			}
			if(isFinal)
			{
				if(find(dfa.finalStates.begin(),dfa.finalStates.end(),to)==dfa.finalStates.end())
					dfa.finalStates.insert(dfa.finalStates.end(),to);
			}
			
		}// end for char from 0 to 256
	}
	
	dfa._isDeterminized = 1;
	return dfa;
}

/*
 *返回最小化后的 DFA
 *对于特别的用于生成词法分析器的最小化算法
 *我们必须将对应不同token的状态区分出来.
 *我们一开始就将它们划分在不同的集合里面.
 */
FA FA::minimized()
{
	if(_isMinimized)
		return *this;
	if(!_isDeterminized)
		return determinized().minimized();

	deque<states_type> sss = deque<states_type>();
	
    //使用一个map 来保存所有状态和集合归属关系
	map<state_type,deque<states_type>::iterator> mapping = map<state_type,deque<states_type>::iterator>();
	
	
	sort(states.begin(),states.end());
	sort(finalStates.begin(),finalStates.end());
	
	
	states_type tmp = states_type(states.size());
	states_type::iterator end_result = set_difference(states.begin(),states.end(),finalStates.begin(),finalStates.end(),tmp.begin());
	states_type nonFinishStates = states_type(tmp.begin(),end_result);

	if(nonFinishStates.size()>0)
		sss.push_back(nonFinishStates);
	//处理终态 匹配不同token的放到不同集合中
	multimap<int,state_type> finals_by_token;
	for(states_type::iterator i = finalStates.begin(); i != finalStates.end(); i++)
	{
		finals_by_token.insert(make_pair(metaDatas[*i],*i));
	}
	for(multimap<int,state_type>::iterator i = finals_by_token.begin(); i != finals_by_token.end();)
	{
		multimap<int,state_type>::iterator lower = finals_by_token.lower_bound(i->first);
		multimap<int,state_type>::iterator upper = finals_by_token.upper_bound(i->first);
		i = upper;
		if(lower == upper)
			continue;
		states_type tmp = states_type();
		for( ;lower != upper; lower++)
		{
			tmp.push_back(lower->second);
		}
		sss.push_back(tmp);
	}

	//initial  mapping 
	for(deque<states_type>::iterator i = sss.begin(); i != sss.end(); i++)
	{
		for(states_type::iterator j = i->begin(); j != i->end(); j++)
		{
			mapping[*j] = i;
		}
	}
	
	//这个循环的复杂度
	//最终min-DFA 状态数 * 256 * 每个DFA状态的non-min-DFA状态数 * 一个常数(4~5)
	//count 的含义
	//1.如果当前集合 没有被分割  cout --
	//2.如果当前集合被分割 count = sss.size() - i -1;
	//当count == 0 时说明不能再分割了。
	int count = sss.size();
	for(deque<states_type>::iterator i = sss.begin(); i != sss.end()&&count> 0 ; i = sss.begin())
	{	
		int allc = sss.size();
	    deque<states_type>::iterator last;
		//对每一个可能的转换, 检查是否可以拆分
		int c;
		for(c = 0; c < 256 ; c++)
		{
			//i 当前要拆分的集合
			if(i->size() <= 1)
			{
				c = 256; //break with  c = 256 , means can't divide 
				break;
			}
			if(transitionsBy(c).size() < 1)
				continue;
			states_type s1 = states_type();
			states_type s2 = states_type();
			
			states_type::iterator first = i->begin();
			s1.push_back(*first);
			state_type ns0 = nextState(*first,c);
			for(states_type::iterator j = i->begin() + 1; j != i->end(); j++)
			{
				state_type ns = nextState(*j,c);
				//如果和第一个元素通过C到了同一个集合 或者和第一个一样不能通过C到达任何集合
				if((ns==ns0 ||(ns0 != NON_STATE && ns != NON_STATE && mapping[ns0] == mapping[ns])))
				{
					s1.push_back(*j);	
				}
				else
				{
					s2.push_back(*j);
				}
			}

			if(s2.size()>0) //分成了两个集合
			{
				last = sss.insert(sss.end(),s1);
				for(states_type::iterator j = s1.begin(); j != s1.end(); j++)
				{
					mapping[*j] = last;
				}	
				last = sss.insert(sss.end(),s2);
				for(states_type::iterator j = s2.begin(); j != s2.end(); j++)
				{
					mapping[*j] = last;
				}
				//cout<<count<<"  "<<allc<<endl;
				count = sss.size() - 1; 
				break;	//break out the chars 			
			}
		}
		
		
		if( c == 256) //如果没有分割，拷贝一下。
		{
			count --;
			last = sss.insert(sss.end(),*i);
			//重新mapping
			for(states_type::iterator j = i->begin(); j != i->end(); j++)
			{
				mapping[*j] = last ;
			}
		}
		sss.pop_front();
	}

	//从mapping 中收集信息 建立最小化后的DFA
	FA mdfa = FA();
	mdfa.initialState = mapping[initialState] - sss.begin();
	
	set<state_type> stas = set<state_type>();
	for(states_type::iterator i =  states.begin(); i != states.end(); i++)
	{
		stas.insert(mapping[*i] - sss.begin());
	}
	mdfa.states = states_type(stas.begin(),stas.end());

	set<transition_type> trans = set<transition_type>();
	for(transitions_type::iterator i =  transitions.begin(); i != transitions.end(); i++)
	{
		trans.insert(transition_type(mapping[i->from]-sss.begin(),mapping[i->to]-sss.begin(),i->label));	
	}
	mdfa.transitions = transitions_type(trans.begin(),trans.end());

	for(states_type::iterator i =  finalStates.begin(); i != finalStates.end(); i++)
	{
        int number = mapping[*i] - sss.begin();
		if(find(mdfa.finalStates.begin(),mdfa.finalStates.end(),number) == mdfa.finalStates.end())
		{	
			mdfa.finalStates.push_back(number);
		}
		if(mdfa.metaDatas.find(number) == mdfa.metaDatas.end())
			mdfa.metaDatas[number] = metaDatas[*i];
		else
		{
			if(mdfa.metaDatas[number] > metaDatas[*i])
				mdfa.metaDatas[number] = metaDatas[*i];
		}
	}
	mdfa._isDeterminized  = 1;
	mdfa._isMinimized = 1;
	return mdfa;	
}




/*--------------------------------------------------------------
 * View Methods
 * 一些输出操作
 *--------------------------------------------------------------*/


/*
 *输出每个状态的转换
 */
void FA::printTransitionsFrom(ostream& out)
{
	for(states_type::iterator ite = states.begin(); ite != states.end(); ite++)
	{
		vector<int> trans = transitionsFrom(*ite);
		out<<"transitionsFrom "<<*ite<<endl;
		for(vector<int>::iterator i = trans.begin(); i != trans.end(); i++)
		{
			out<<transitions[*i];
		}
		out<<endl;
	}	
}
/*
 *输出每个状态的EpsilonClosures
 */
void FA::printEpsilonClosures(ostream& out)
{
	for(states_type::iterator i = states.begin(); i!= states.end(); i++)
	{
		out<<"EpsilonClosure for state "<<*i<<endl;
		states_type ec = epsilonClosure(*i);
		for(states_type::iterator it = ec.begin(); it != ec.end(); it++)
		{
			out<<*it<<' ';
		}
		out<<endl;
	} 
}
/*
 * 输出MetaDatas
 */
void FA::printMetaDatas(ostream& out)
{
	for(meta_datas_type::iterator i = metaDatas.begin(); i != metaDatas.end(); i++)
	{
		out<<i->first<<"  "<<i->second<<endl;
	}
}

/*
 * 输出yy_accept
 * 一定要sorted 才行
 */
void FA::print_yy_accept(ostream& out)
{
	if(!_isSorted)
	{
		cerr<<"Error, plase call sorted";
		return ;
	}
	out<<"int yy_accept["<<states.size()<<"]={";
	for(states_type::iterator i = states.begin(); i != states.end(); i++)
	{
		if(find(finalStates.begin(),finalStates.end(),*i) == finalStates.end())
			out<<"0";
		else
			out<<"1";
		if(i != states.end()-1)
			out<<',';		
	}
	out<<"};\n"<<endl;
	
}

void FA::printTable(ostream& out)
{
	
	sort(states.begin(),states.end());
	out<<"int automate ["<<states.size()<<"]"<<"[256]={";
	for(states_type::iterator i = states.begin(); i != states.end(); i++)
	{
		out<<'{';
		for(int c = 0; c < 256; c ++)
		{
			state_type ns = nextState(*i,c); 
			out << ns;
			if( c != 255)
				out<<',';
		}
		out<<'}';
		if(i != states.end()-1)
		{
			out<<","<<endl;
		}
	}
	out<<"};"<<endl;
}

/*
 * 输出 FA
 */
ostream& operator << (ostream& out, FA fa)
{
	FA::states_type::iterator ite = fa.states.begin();
	out<<"States:";
	for(; ite != fa.states.end(); ite ++)
	{
		out<<(*ite)<<' ';
	}
	out<<endl;
	out<<"Initial State:"<<fa.initialState<<endl;
	out<<"Final States:";
	for(ite = fa.finalStates.begin(); ite!=fa.finalStates.end(); ite++)
	{
		out<<(*ite)<<' ';
	}
	out<<endl;
	out<<"Transitions:\n";
	for(FA::transitions_type::iterator itr = fa.transitions.begin(); itr!=fa.transitions.end(); itr++)
	{
		out<<*itr<<" ";
	}
	out<<endl;
	return out;
} 

/*
 * 产生automate 表
 */
void FA::genTable(int& state_num,int& action_num,vector<vector<int> >& automate)
{
	
	sort(states.begin(),states.end());
	state_num = states.size();
	action_num = 256;
	for(states_type::iterator i = states.begin(); i != states.end(); i++)
	{
		automate.push_back(vector<int>());
		for(int c = 0; c < action_num; c ++)
		{
			state_type ns = nextState(*i,c); 
			automate[*i].push_back(ns);
		}
	}
}

/*-----------------------------------------------
 * Operations on languages (via their recognizers)
 * These generally return nondeterministic FAs. 
 * ----------------------------------------------*/

/*
 * 返回识别a*的FA， fa识别a
 * 
 * 
 */
FA closure(FA fa)
{
	FA::state_type final = fa.nextAvailableState();
	fa.states.insert(fa.states.end(),final);
	for(FA::states_type::iterator ite = fa.finalStates.begin(); ite != fa.finalStates.end(); ite++)
	{
		fa.transitions.insert(fa.transitions.end(),FA::transition_type(*ite,final,EPSILON));
	}
	fa.finalStates.insert(fa.finalStates.end(),final);
	
	fa.transitions.insert(fa.transitions.end(),FA::transition_type(fa.initialState,final,EPSILON));
	fa.transitions.insert(fa.transitions.end(),FA::transition_type(final,fa.initialState,EPSILON));
	return fa;
}


/*
 * 返回识别a?的FA, fa识别a
 * 
 * TODO:add meta datas ?
 */
FA option(FA fa)
{
	return FAunion(fa,FA::EMPTY_STRING_FA);
}

/*
 * 返回识别a|b的FA，fa1 识别 a , fa2 识别 b
 */
FA FAunion(FA fa1,FA fa2)
{
	FA fa = fa1;
	fa._isDeterminized = false;
	fa._isEpsilonClosuresIninted = false;
	fa._isTransTableIninted = false;
	fa2 = fa2.sorted(fa1.nextAvailableState());
	fa.states.insert(fa.states.end(),fa2.states.begin(),fa2.states.end());
	fa.transitions.insert(fa.transitions.end(),fa2.transitions.begin(),fa2.transitions.end());
	fa.finalStates.insert(fa.finalStates.end(),fa2.finalStates.begin(),fa2.finalStates.end());
	fa.transitions.insert(fa.transitions.end(),triple(fa1.initialState,fa2.initialState,EPSILON));
	fa.combineMetaData(fa2.metaDatas);
	return fa;	
}

/*
 * 返回识别a{min,max}的FA, fa 识别 a 
 * min>=0; 
 * max>=0;
 * max==-1表示最多没有限制
 * TODO: add meta datas ??
 */
FA iteration(FA fa,int min, int max)
{
	//if(min > max && max != INFINITE) error?
	
	if(min > 0)
	{
		return concatenation(fa,iteration(fa,min-1,max==INFINITE?max:max-1));
	}
	if(min == 0 && max > 0 )
	{
		return concatenation(option(fa),iteration(fa, min, max-1 ));
	}
	if(min == 0 && max == 0)
	{
		return FA::EMPTY_STRING_FA;
	}
	if(min == 0 && max == INFINITE)
	{
		return closure(fa);
	}
	
}

/*
 *返回识别ab的FA， fa1 识别 a,fa2 识别 b
 *TODO: add meta data ??
 */
FA concatenation(FA fa1,FA fa2)
{
	fa2 = fa2.sorted(fa1.nextAvailableState());
	FA fa = fa1;
	fa.states.insert(fa.states.end(),fa2.states.begin(),fa2.states.end());
	fa.transitions.insert(fa.transitions.end(),fa2.transitions.begin(),fa2.transitions.end());
	fa.finalStates = fa2.finalStates;
	
	FA::states_type::iterator ite = fa1.finalStates.begin();
	for(; ite != fa1.finalStates.end(); ite++)
	{
		fa.transitions.insert(fa.transitions.end(),FA::transition_type(*ite, fa2.initialState,EPSILON));
	}
	//fa.metaDatas = fa2.metaDatas;
	return fa;
}



/*--------------------------------------------------------------
 * Meta Data 操作
 *--------------------------------------------------------------*/

/*
 *add meta data for one final state 
 */
void FA::addMetaData(state_type state,int number)
{
	metaDatas[state] = number;	
}

/*
 *add meta data for all final state 
 */
void FA::addMetaDataForAll(int number)
{
	for(states_type::iterator i = finalStates.begin(); i != finalStates.end(); i++)
	{
		metaDatas[*i] = number;
	}
}

/*
 * get meta data for the state
 */
int FA::getMetaData(state_type state)
{
	if(metaDatas.find(state) == metaDatas.end())
		return -1;
	return metaDatas[state];
	
} 

/*
 * combine current meta data with argument metadata
 */ 
void FA::combineMetaData(meta_datas_type moreMeta)
{
	
	for(meta_datas_type::iterator i = moreMeta.begin(); i != moreMeta.end(); i++)
	{
		
		if(metaDatas.find(i->first) == metaDatas.end())
			metaDatas.insert(*i);
		else
		{
			if(metaDatas[i->first] > i->second)
				metaDatas[i->first] = i->second;
		}
	}
} 
 

/*--------------------------------------------------------------
 * 构造函数和其他构造简单FA的函数
 * 以及FA常量
 *--------------------------------------------------------------*/

/*
 *构造函数
 *
 */
FA::FA(states_type states,transitions_type transitions,state_type initialState,states_type finalStates)
{
    this->states = states;
    this->transitions = transitions;
    this->initialState = initialState;
    this->finalStates = finalStates;
    this->metaDatas = meta_datas_type();
    this->_isSorted = 0;
    this->_isTrimmed = 0;
   	this->_isDeterminized = 0;
    this->_isMinimized = 0;
    this->_isTransTableIninted = 0;
    this->_isEpsilonClosuresIninted = 0;
}
/*
 *默认构造函数
 */
FA::FA()
{
    this->states = states_type();
    this->transitions = transitions_type();
    //this->initialState = 0;
    this->finalStates = states_type();
    
	this->metaDatas = meta_datas_type();
    this->_isSorted = 0;
    this->_isTrimmed = 0;
   	this->_isDeterminized = 0;
    this->_isMinimized = 0;
    this->_isTransTableIninted = 0;
    this->_isEpsilonClosuresIninted = 0;
}


/*
 *返回识别一个字符的FA
 */
FA singleton(char c)
{
    FA::states_type states = FA::states_type();
    states.insert(states.end(),0);
    states.insert(states.end(),1);   
    FA::states_type finalStates = FA::states_type();
    finalStates.insert(finalStates.end(),1);
    FA::transitions_type trans = FA::transitions_type();
    FA::transition_type tri = FA::transition_type(0,1,c);
    trans.insert(trans.end(),tri);
    return FA(states,trans,0,finalStates); 
}



FA::states_type makeOneStateSet()
{
	FA::states_type oneStateSet = FA::states_type();
	oneStateSet.insert(oneStateSet.end(),0);
	return oneStateSet;
}


FA::state_type FA::NON_STATE = -1; 
FA FA::NULL_FA = FA(makeOneStateSet(),transitions_type(),0,states_type());
FA FA::EMPTY_STRING_FA = FA(makeOneStateSet(),transitions_type(), 0, makeOneStateSet());
