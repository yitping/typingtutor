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
int totalLetter, totalError;
float totalTime, letterPerMin, errorPerLetter;
std::time_t ttTimer;
bool isLastLetterAccurate;

int actionLineY, actionLineX1, actionLineX2;
int infoLineY, infoLineX1, infoLineX2;

void initTT()
{
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
	
	resetStatistics();

	srand((unsigned int)time(NULL));
}

void setupScreen()
{
	actionLineY  = screenH*4/10;
	actionLineX1 = (screenW - buffersize)/2;
	actionLineX2 = actionLineX1 + buffersize - 1;
	infoLineY  = screenH-1;
	infoLineX1 = actionLineX1;
	infoLineX2 = actionLineX2;
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
	
	if (err) totalError += 1;
	
	if ((s[i] == ASCII_SPACE) && err)
	{
		mvaddch(actionLineY, actionLineX1+i, ACS_CKBOARD);
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

void updateInfoLine()
{
	char info[100];
	computeStatistics();
	/*
	if (errorPerLetter > 0.99)
	{
		sprintf(info, "L/min = %3.0f | Mistakes/100L = XX", letterPerMin);
	}
	else
	{
		sprintf(info, "L/min = %3.0f | Mistakes/100L = %2.0f%%", letterPerMin, errorPerLetter*100);
	}
	 */
	sprintf(info, "L/min = %3.0f | Mistakes %s %2.0f%%%%",
					letterPerMin,
					(errorPerLetter > 0.99) ? ">" : "=",
					((errorPerLetter > 0.99) ? 0.99 : errorPerLetter)*100);
	//sprintf(info, "tL = %3d | tE = %3d | tT = %4.2f | L/T = %.4f | E/L = %2.0f",
	//				totalLetter, totalError, totalTime, letterPerMin, errorPerLetter*100);
	attron(COLOR_PAIR(COLOR_ID_DEFAULT));
	mvprintw(infoLineY, infoLineX1, info);
}

void updateCursorPosition(std::string &a)
{
	move(actionLineY, actionLineX1+(int)a.size());
}

bool isEOL(std::string &a)
{
	return (a.size() < buffersize) ? FALSE : TRUE;
}

void clearBuffer(std::string &a)
{
	a.clear();
}

void resetStatistics()
{
	totalLetter = 0;
	totalError  = 0;
	totalTime      = 0.0;
	letterPerMin   = 0.0;
	errorPerLetter = 0.0;
}

void computeStatistics()
{
	letterPerMin   = (totalTime > 0) ? (totalLetter-totalError)/(float)totalTime*60 : 0.0;
	errorPerLetter = (totalLetter > 0) ? (float)totalError/totalLetter : 0.0;
}

int addChar(std::string &a, char c)
{
	// a.apacity() includes extra spaces, e.g. for the NULL char
	if (a.size() == buffersize)
	{
		return 0;
	}
	if ((ttTimer == (time_t)-1) || (a.size() == 0)) ttTimer = std::time(NULL);
	std::time_t temp = std::time(NULL);
	totalTime += difftime(temp,ttTimer);
	ttTimer = temp;
	totalLetter += 1;
	a += c;
	return 1;
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
		case ASCII_CTRL_R:
		case ASCII_BACKSPACE:
		case ASCII_DELETE:
		case ASCII_CRETURN:
		case ASCII_NEWLINE:
			return TRUE;
		default:
			return FALSE;
	}
}

void printCLHelp(char *clarg0)
{
	std::cout << "Usage: " << clarg0 << " ";
	std::cout << "-c \"<characters to train in delimited format (default=csv)>\" ";
	std::cout << "[-s <separator>]";
	std::cout << "[-h]";
	std::cout << std::endl;
}

bool hasChar(std::vector<char> &a, char c)
{
	for (int i=0; i<a.size(); i++)
	{
		if (a[i] == c) return TRUE;
	}
	return FALSE;
}

int splitStringToChar(char *in, std::vector<char> &out, char sep)
{
	char *p;
	p = strtok(in, &sep);
	while (p != NULL)
	{
		if (strlen(p) > 1)
		{
			std::cerr << "error: only one character per field" << std::endl;
			return 1;
		}
		if (!hasChar(out,*p))
		{
			out.push_back(*p);
		}
		p = strtok(NULL, &sep);
	}
	std::cout << "size of out = " << out.size() << std::endl;
	std::cout << "out = " << out.data() << std::endl;
	out.push_back(' ');
	return 0;
}

int main(int argc, char * argv[])
{
	int ch, opt;
	int errflag;
	char *optargTrainingSet=NULL, optargSep=',';
	std::vector<char> trainingSet;
	
	while ((opt = getopt(argc, argv, ":hc:s:")) != -1)
	{
		switch (opt)
		{
			case 'h':
				errflag++;
				break;
			case 'c':
				optargTrainingSet = optarg;
				break;
			case 's':
				sscanf(optarg, "%c", &optargSep);
				break;
			case ':':
				std::cerr << "option -" << optopt << "requires an operand" << std::endl;
				errflag++;
				break;
			case '?':
				std::cerr << "unrecognized option:-" << optopt << std::endl;
				errflag++;
				break;
		}
	}
	
	if (errflag || (optargTrainingSet == NULL))
	{
		printCLHelp(argv[0]);
		return EXIT_FAILURE;
	}
	
	if (splitStringToChar(optargTrainingSet, trainingSet, optargSep) != 0)
	{
		return EXIT_FAILURE;
	}
	
	// Initialize app
	initTT();
	
	//char a[] = {'a', 's', 'd', 'f', ' '};
	charsRefer = generateString(trainingSet.data(), (int)trainingSet.size(), buffersize);
	charsTyped.resize(buffersize);
	charsTyped.clear();
	
	setupScreen();
	updateReferLine(charsRefer);
	updateActionLine(charsTyped, charsRefer);
	updateInfoLine();
	updateCursorPosition(charsTyped);
	
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
			case ASCII_CTRL_R:
				resetStatistics();
			case ASCII_NEWLINE:
			case ASCII_CRETURN:
				if (isEOL(charsTyped) || (ch == ASCII_CTRL_R))
				{
					charsRefer = generateString(trainingSet.data(), (int)trainingSet.size(), buffersize);
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
				updateInfoLine();
				updateCursorPosition(charsTyped);
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

