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
 
#ifndef TMPL_H_
#define TMPL_H_
char * headerTMPL = "\
#include <stdio.h>\n\
\n\
#ifndef NULL\n\
	#define NULL 0\n\
#endif\n\
#define YY_SC_TO_UI(c) ((unsigned int) (unsigned char) c) \n\
#ifndef YYEOF\n\
#define YYEOF 0\n\
#endif\n\
\n\
#ifndef bool\n\
	#define bool int\n\
	#define true 1\n\
	#define false 0\n\
#endif\n\
\n\
bool  yymore = false;\n\
#define HOLE -1\n\
#define MAX_TOK 4096000\n\
#define ECHO fprintf(yyout, \"%s\", yytext)\n\
int back_stack[MAX_TOK];\n\
\n\
extern char  yytext[MAX_TOK];\n\
extern int   yyleng;\n\
extern FILE* yyin;\n\
extern FILE* yyout;\n\
char         yytext[MAX_TOK] = \"\\0\";\n\
int          yyleng = 0;\n\
FILE*        yyin   = NULL;\n\
FILE*        yyout  = NULL;\n\
\n\
#define output(c)   fputc(c, yyout)\n\
#define yymore()    yymore = 1\n\
int yywrap();\n\
#define BUF_SIZE MAX_TOK\n\
char buffer[BUF_SIZE];\n\
int buf_pos = 0;\n\
int buf_using  = 0;\n\
char input()\n\
{\n\
	if (buf_using > 0)\n\
	{\n\
		--buf_using;\n\
		return buffer[(buf_pos++) % BUF_SIZE];\n\
	}\n\
	else\n\
	{\n\
		char temp = getc(yyin);\n\
		buffer[(buf_pos++) % BUF_SIZE] = temp;\n\
		return temp;\n\
	}\n\
}\n\
void unput(char c)\n\
{\n\
	++buf_using;\n\
	--buf_pos;\n\
	buf_pos += BUF_SIZE;\n\
	buf_pos %= BUF_SIZE;\n\
	buffer[buf_pos] = c;\n\
}\n\
void yyless(int n)\n\
{\n\
	buf_using += n;\n\
	buf_pos -= n;\n\
	buf_pos += BUF_SIZE;\n\
	buf_pos %= BUF_SIZE;\n\
}\n\
";
char * unpackTMPL = "\
#ifdef PACK\n\
int unpack(int state, int action)\n\
{\n\
	int raction1 = yy_ec[action];\n\
	int raction = yy_meta[raction1];\n\
	while (yy_def[state] != state)\n\
	{\n\
		if (yy_nxt[yy_base[state] + raction] == HOLE)\n\
		{\n\
			state = yy_def[state];\n\
		}\n\
		else\n\
		{\n\
			if (yy_chk[yy_base[state] + raction] == state)\n\
			{\n\
				return yy_nxt[yy_base[state] + raction];\n\
			}\n\
			else\n\
			{\n\
				state = yy_def[state];\n\
			}\n\
		}\n\
	}\n\
	if (yy_chk[yy_base[state] + raction] == state)\n\
	{\n\
		return yy_nxt[yy_base[state] + raction];\n\
	}\n\
	else\n\
	{\n\
		return HOLE;\n\
	}\n\
}\n\
#else\n\
#define unpack(state,action) automate[state][YY_SC_TO_UI(action)]\n\
#endif\n\
\n\
int yylex()\n\
{\n\
	if (!yyin)\n\
		yyin = stdin;\n\
	if (!yyout)\n\
		yyout = stdout;\n\
	char action = '\\0';\n\
	do\n\
	{\n\
		while ((action = input()) != EOF)\n\
		{\n\
			int current_state = 0;\n\
			if (!yymore)\n\
			{\n\
				yyleng = 0;\n\
			}\n\
			else\n\
			{\n\
				yymore = false;\n\
			}\n\
			back_stack[yyleng] = current_state;\n\
			yytext[yyleng++] = action;\n\
			int temp = 0;\n\
			while ((temp = unpack(current_state, YY_SC_TO_UI(action))) != HOLE)\n\
			{\n\
				current_state = temp;\n\
				back_stack[yyleng] = current_state;\n\
				if ((action = input()) != EOF)\n\
				{\n\
					if (unpack(current_state, YY_SC_TO_UI(action)) == HOLE)\n\
					{\n\
						unput(action);\n\
					}\n\
					else\n\
					{\n\
						yytext[yyleng++] = action;\n\
					}\n\
				}\n\
				else\n\
				{\n\
					break;\n\
				}\n\
			}\n\
			while (yy_accept[current_state] != 1)\n\
			{\n\
				if (yyleng == 1)\n\
				{\n\
					break;\n\
				}\n\
				current_state = back_stack[--yyleng];\n\
				unput(yytext[yyleng]);\n\
			}\n\
			yytext[yyleng] = \'\\0\';\n\
";




char * footerTMPL = "\
		}\n\
        fclose(yyin);\n\
	} while (!yywrap());\n\
	return YYEOF;\n\
}\n\
\n\
";
#endif /*TMPL_H_*/
