//
//  main.cpp
//  typingtutor
//
//  Created by Yitping Kok on 25/02/2015.
//  Copyright (c) 2015 Onslow143. All rights reserved.
//

#include <iostream>
#include <string>
#include <ncurses.h>
#include <time.h>

int scrW, scrH;
int buffersize;
int n_char = 0;
char buffer[60] = { 0 };
std::string strTyped, strMaster;

void updateScreen()
{
	int m, n;
	m = scrH*4/10;
	n = (scrW - buffersize)/2;
	mvprintw(m-3, n, "Jiayu's Typing Tutor");
	mvprintw(m-1, n, strMaster.c_str());
	attron(A_BOLD);
	mvprintw(m, n, strTyped.c_str());
	attroff(A_BOLD);
}

void clearBuffer(std::string &a)
{
	a.clear();
	n_char = 0;
	a.assign("");
}

void addChar(std::string &a, char c)
{
	if (n_char > buffersize-1) clearBuffer(a);
	a[n_char]   = c;
	a[n_char+1] = '\0';
	n_char += 1;
}

void delChar(std::string &a)
{
	a[n_char-1] = '\0';
	n_char -= 1;
	if (n_char < 0) n_char = 0;
}

std::string genStr(char *a, int n, int len)
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

int main(int argc, const char * argv[])
{
	int ch;
	
	// initialize screen
	initscr();
	getmaxyx(stdscr, scrH, scrW);
	raw();
	keypad(stdscr, TRUE);
	noecho();
	
	// buffersize is odd if screen width is odd and vice versa
	buffersize  = scrW*3/4;
	buffersize += (scrW % 2) ? ((buffersize % 2) + 1) : (buffersize % 2);

	srand((unsigned int)time(NULL));
	char a[] = {'a', 's', 'd', 'f', ' '};
	strMaster = genStr(a, 5, buffersize);
	strTyped.resize(buffersize, '\0');
	
	updateScreen();
	
	ch = getch();
	while (ch != 3)
	{
		switch (ch)
		{
			case 8:
			case 127:
				delChar(strTyped);
				break;
			case 10:
			case 13:
				clearBuffer(strTyped);
				break;
			case KEY_UP:
			case KEY_DOWN:
			case KEY_RIGHT:
			case KEY_LEFT:
				break;
			default:
				addChar(strTyped, ch);
		}
		updateScreen();
		ch = getch();
	}
	
	refresh();
	endwin();
	
	std::cout << "buffersize = " << buffersize << std::endl;
	printf("Goodbye!\n");
	return 0;
}

