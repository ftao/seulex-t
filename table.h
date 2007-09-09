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
 
#ifndef TABLE_H_
#define TABLE_H_
#include <vector>
int STATE_NUM;
int ACTION_NUM;
vector<vector<int> > automate;
int* yy_ec;//[ACTION_NUM];
int* yy_meta;//[ACTION_NUM];
int actual_action ;//= ACTION_NUM;
int* yy_def;//[STATE_NUM];
int* yy_base;//[STATE_NUM];
int* yy_chk = NULL;
int* yy_nxt = NULL;
int end_comb = 0;
const int HOLE = -1;
const int SPECIAL = -2;
typedef vector<int>							vi;
typedef vector<vector<int> >				vvi;
typedef vector<pair<int, int> >				vpii;
typedef vector<vector<pair<int, int> > >	vvpii;
typedef vector<pair<int, int> >::iterator	vpiitor;
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <utility>
using namespace std;

bool equal(vector<int>& lvalue, vector<int>& rvalue, int ri, int rj)
{
	vector<int>::iterator liter = lvalue.begin();
	vector<int>::iterator riter = rvalue.begin();
	for (; liter!=lvalue.end() && riter!=rvalue.end(); ++liter, ++riter)
	{
		if (automate[*liter][ri] != automate[*riter][rj])
		{
			return false;
		}
	}
	return true;
}

void yy_equ()
{
	vector<vector<int> > equ(ACTION_NUM);
	int i, j;
	for (i = 0 ; i < ACTION_NUM; ++i)
	{
		for (j = 0; j < STATE_NUM; ++j)
		{
			if (automate[j][i] != HOLE)
			{
				equ[i].push_back(j);
			}
		}
	}
	vector<pair<int, int> > vp;
	for (i = 0; i < ACTION_NUM; ++i)
	{
		vp.push_back(make_pair(equ[i].size(), i));
	}
	sort(vp.begin(), vp.end());

	const int INIT = SPECIAL;
	for (i = 0; i < ACTION_NUM; ++i)
	{
		yy_ec[i] = INIT;
	}
	int begin = 0;
	int end = 1;
	while (true)
	{
		while (end < ACTION_NUM)
		{
			if (vp[begin].first == vp[end].first)
			{
				++end;
			}
			else
			{
				break;
			}
		}
		int i = begin;
		yy_ec[vp[begin].second] = vp[begin].second;
		begin = end;
		for(; i < end - 1; ++i)
		{
			int riaction = vp[i].second;
			if (yy_ec[riaction] == riaction)
			{
				bool first_not = true;
				int j = i + 1;
				for (; j < end; ++j)
				{
					int rjaction = vp[j].second;
					if (yy_ec[rjaction] == INIT)
					{
						if (equ[riaction] == equ[rjaction] && equal(equ[riaction],equ[rjaction], riaction, rjaction))
						{
							yy_ec[rjaction] = riaction;
							--actual_action;
						}
						else
						{
							if (first_not)
							{
								yy_ec[rjaction] = rjaction;
								first_not = false;
							}
						}
					}
				}
			}
		}
		end = begin + 1;
		if (end > ACTION_NUM)
		{
			break;
		}
	}
	j = 0;
	for (i = 0; i < ACTION_NUM; i++)
	{
		if (yy_ec[i] != i)
		{
			yy_meta[i] = yy_ec[i];
		}
		else
		{
			yy_meta[i] = j;
			++j;
		}
	}
}
int same_part(vector<vector<int> >& sdef, vector<pair<int, int> >& svp, int vshort, int vlong)
{
	int i;
	int same = 0;
	int rshort = svp[vshort].second;
	int rlong = svp[vlong].second;
	for (i = 0; i < svp[vshort].first; ++i)
	{
		int saction = sdef[rshort][i];
		if (automate[rlong][saction] == HOLE)
		{
			return 0;
		}
		else if (automate[rlong][saction] == automate[rshort][saction])
		{
			++same;
		}
	}
	return same;
}
void yy_default()
{
	vector<vector<int> > def(STATE_NUM);
	int i, j;
	for (i = 0 ; i < STATE_NUM; ++i)
	{
		for (j = 0; j < ACTION_NUM; ++j)
		{
			if (automate[i][j]!=HOLE && yy_ec[j]==j)
			{
				def[i].push_back(j);
			}
		}
	}
	vector<pair<int, int> > vp;
	for (i = 0; i < STATE_NUM; ++i)
	{
		vp.push_back(make_pair(def[i].size(), i));
	}
	sort(vp.begin(), vp.end());
	for (i = 0; i < STATE_NUM; ++i)
	{
		yy_def[i] = i;
	}
	int cur_long = STATE_NUM - 1;
	for (; cur_long > 0; --cur_long)
	{
		int rlong = vp[cur_long].second;
		int current_max_index = rlong;
		int current_max = 0;
		int cur_short = cur_long - 1;
		for (; cur_short >= 0; --cur_short)
		{
			int rshort = vp[cur_short].second;
			int estimate_max = vp[cur_short].first;
			if (estimate_max > current_max)
			{
				int temp = same_part(def, vp, cur_short, cur_long);
				//cout<<"±È½Ï "<<rlong<<" "<<rshort<<" "<<temp<<endl;/////////////////////////////////
				if (temp > current_max)
				{
					current_max = temp;
					current_max_index = rshort;
				}
			}
			else
			{
				break;
			}
		}
		yy_def[rlong] = current_max_index;
	}
}

void init_combine(vpiitor first, vpiitor last)
{
	int unexisted_state = SPECIAL;
	pair<int, int> default_data(unexisted_state, HOLE);
	for (; first != last; ++first)
	{
		*first = default_data;
	}
}

bool check(vpii& com, vi& vicol, int offset)
{
	unsigned int i = 0;
	for (; i < vicol.size(); ++i)
	{
		unsigned int icn = vicol[i];
		unsigned int icnao = icn + offset;
		if (icnao < com.size())
		{
			if (com[icnao].second != HOLE)
			{
				return false;
			}
		}
		else
		{
			int oldsize = com.size();
			com.resize(oldsize + actual_action);
			init_combine(com.begin() + oldsize, com.begin() + oldsize + actual_action);
		}
	}
	return true;
}

int percom(vvpii& cnstat, vvi& vps, vpii& for_hash, int i, vpii& com)
{
	int ricol = for_hash[i].second;
	int offset = 0;
	while (true)
	{
		if (check(com, vps[ricol], offset))
		{
			break;
		}
		++offset;
	}
	int j = 0;
	for (; j < vps[ricol].size(); ++j)
	{
		unsigned int icn = vps[ricol][j];
		unsigned int icnao = icn + offset;
		com[icnao] = cnstat[ricol][icn];
	}
	return offset;
}

void comb()
{
	yy_equ();
	yy_default();
	vvpii cur_nxt_stat(STATE_NUM);
	vvi vps(STATE_NUM);
	int i = 0;
	for (; i < STATE_NUM; ++i)
	{
		int j = 0;
		int k = 0;
		for (; (j < ACTION_NUM)&&(k < actual_action); ++j)
		{
			if (yy_ec[j] == j)
			{
				int k_row_nxt = automate[i][j];
				if (k_row_nxt != HOLE)
				{
					if (yy_def[i] != i)
					{
						int main = yy_def[i];
						int m_row_nxt = automate[main][j];
						if (k_row_nxt == m_row_nxt)
						{
							k_row_nxt = HOLE;
						}
						else
						{
							vps[i].push_back(k);
						}
					}
					else
					{
						vps[i].push_back(k);
					}
				}
				cur_nxt_stat[i].push_back(make_pair(i, k_row_nxt));
				++k;
			}
		}
	}
	vpii for_hash(STATE_NUM);
	for (i = 0; i < STATE_NUM; ++i)
	{
		for_hash[i] = make_pair(vps[i].size(), i);
	}
	sort(for_hash.begin(), for_hash.end());
	int combine_size = actual_action;
	vector<pair<int, int> > combination(combine_size);
	init_combine(combination.begin(), combination.end());
	int max_comb = 0;
	int max_comb_index = -1;
	for (i = STATE_NUM - 1; i >= 0; --i)
	{
		int icomb = percom(cur_nxt_stat, vps, for_hash, i, combination);
		yy_base[for_hash[i].second] = icomb;
		if (icomb >= max_comb)
		{
			max_comb = icomb;
			max_comb_index = for_hash[i].second;
		}
	}
	end_comb = max_comb + actual_action;
	yy_chk = new int[end_comb];
	yy_nxt = new int[end_comb];
	int end = end_comb;
	if (end_comb > combination.size())
	{
		end = combination.size();
	}
	for (i = 0; i < end; ++i)
	{
		yy_chk[i] = combination[i].first;
		yy_nxt[i] = combination[i].second;
	}
	for (; i < end_comb; ++i)
	{
		yy_chk[i] = SPECIAL;
		yy_nxt[i] = HOLE;
	}
}





#endif /*TABLE_H_*/
