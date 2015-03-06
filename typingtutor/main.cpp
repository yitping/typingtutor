//
//  main.cpp
//  typingtutor
//
//  Created by Yitping Kok on 25/02/2015.
//  Copyright (c) 2015 Onslow143. All rights reserved.
//

#include "main.h"

int screenW, screenH;
int buffersize;
std::string charsTyped, charsRefer;
int color1, color2;

int actionLineY, actionLineX1, actionLineX2;

void setupScreen()
{
	actionLineY  = screenH*4/10;
	actionLineX1 = (screenW - buffersize)/2;
	actionLineX2 = actionLineX1 + buffersize - 1;
	attron(A_UNDERLINE);
	mvprintw(actionLineY-3, actionLineX1, TITLE_NAME);
	attroff(A_UNDERLINE);
}

void updateReferLine(std::string &s)
{
	attron(COLOR_PAIR(COLOR_ID_DEFAULT));
	mvprintw(actionLineY-1, actionLineX1, s.c_str());
}

void updateActionLine(std::string &s, std::string &r)
{
	if (s.size() == 0)
	{
		move(actionLineY, actionLineX1);
		return;
	}
	
	int i = (int) s.size() - 1;
	bool err = FALSE;
	if (s[i] != r[i]) err = TRUE;
	// TODO: if last character in buffer is a space, ignore?
	
	attron(A_BOLD);
	if (err) attron(COLOR_PAIR(COLOR_ID_ERROR));
	else attron(COLOR_PAIR(COLOR_ID_DEFAULT));
	
	if ((s[i] == ASCII_SPACE) && err)
	{
		mvaddch(actionLineY, actionLineX1+i, ACS_CKBOARD);
		//mvaddch(actionLineY, actionLineX1+i, s[i] | A_ALTCHARSET);
	}
	else
	{
		mvaddch(actionLineY, actionLineX1+i, s[i]);
	}
	
	attroff(A_BOLD);
}

void clearActionLine()
{
	std::string blank(buffersize, ' ');
	mvprintw(actionLineY, actionLineX1, blank.c_str());
	move(actionLineY, actionLineX1);
}

bool isEOL(std::string &a)
{
	return (a.size() < buffersize) ? FALSE : TRUE;
}

void clearBuffer(std::string &a)
{
	a.clear();
}

int addChar(std::string &a, char c)
{
	// a.apacity() includes space for the NULL char
	if (a.size() == buffersize) return 0;
	a += c;
	return 1;
	// TODO: add timing feature
}

std::string generateString(char *a, int n, int len)
{
	std::string s;
	int i, j;
	
	for (i=0; i<len; i++)
	{
		do
		{
			j = rand() % n;
		} while ((((i == 0) || (i == len-1)) && (a[j] == ' ')) ||
						 ((i != 0) && (a[j] == ' ') && (s[i-1] == ' ')));
		s.push_back(a[j]);
	}
	
	return s;
}

bool isPrintableKey(char c)
{
	if ((c >= 32) && (c <=127)) return TRUE;
	else return FALSE;
}

bool isSpecialKey(char c)
{
	switch (c)
	{
		case ASCII_BACKSPACE:
		case ASCII_DELETE:
		case ASCII_CRETURN:
		case ASCII_NEWLINE:
			return TRUE;
		default:
			return FALSE;
	}
}

int main(int argc, const char * argv[])
{
	int ch;
	
	// initialize screen
	initscr();
	if ((has_colors() == FALSE) && (can_change_color() == FALSE))
	{
		endwin();
		std::cout << "Your terminal does not support color" << std::endl;
		exit(1);
	}
	start_color();
	use_default_colors();
	init_pair(COLOR_ID_ERROR, COLOR_MAGENTA, -1);
	init_pair(COLOR_ID_DEFAULT, -1, -1);
	
	getmaxyx(stdscr, screenH, screenW);
	raw();
	keypad(stdscr, TRUE);
	noecho();
	
	// buffersize is odd if screen width is odd and vice versa
	buffersize  = screenW*3/4;
	buffersize += (screenW % 2) ? ((buffersize % 2) + 1) : (buffersize % 2);

	srand((unsigned int)time(NULL));
	char a[] = {'a', 's', 'd', 'f', ' '};
	charsRefer = generateString(a, 5, buffersize);
	charsTyped.resize(buffersize);
	charsTyped.clear();
	
	setupScreen();
	updateReferLine(charsRefer);
	updateActionLine(charsTyped, charsRefer);
	
	ch = getch();
	while (ch != ASCII_CTRL_C)
	{
		if (!isPrintableKey(ch) && !isSpecialKey(ch))
		{
			ch = getch();
			continue;
		}
		switch (ch)
		{
			case ASCII_BACKSPACE:
			case ASCII_DELETE:
				std::cout << (char) ASCII_CTRL_G; // sound the bell
				refresh();
				break;
			case ASCII_NEWLINE:
			case ASCII_CRETURN:
				if (isEOL(charsTyped))
				{
					charsRefer = generateString(a, 5, buffersize);
					updateReferLine(charsRefer);
					clearActionLine();
					clearBuffer(charsTyped);
				}
				break;
			default:
				if (addChar(charsTyped, ch))
				{
					updateActionLine(charsTyped, charsRefer);
				}
				// if at end of the line
				// - refresh reference string
				// - updateReferLine()
				// - move to actionLineX1
		}
		ch = getch();
	}
	
	refresh();
	endwin();
	
	/*
	std::cout << "buffersize= " << buffersize << std::endl;
	std::cout << "reference = " << charsRefer.size() << std::endl;
	std::cout << "capacity  = " << charsTyped.capacity() << std::endl;
	std::cout << "typed_in  = " << charsTyped.size() << std::endl;
	 */
	printf("Goodbye!\n");
	return 0;
}

