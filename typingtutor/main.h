//
//  main.h
//  typingtutor
//
//  Created by Yitping Kok on 4/03/2015.
//  Copyright (c) 2015 Onslow143. All rights reserved.
//

#ifndef typingtutor_main_h
#define typingtutor_main_h

#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>

#define COLOR_ID_DEFAULT 1
#define COLOR_ID_ERROR 2

#define TITLE_NAME "Jiayu's Typing Tutor"
#define MIN_SCREEN_WIDTH 50

#define ASCII_CTRL_C 3
#define ASCII_CTRL_G 7 // bell
#define ASCII_CTRL_R 18
#define ASCII_BACKSPACE 8
#define ASCII_NEWLINE 10
#define ASCII_CRETURN 13
#define ASCII_SPACE 32
#define ASCII_SOLID 32
#define ASCII_DELETE 127
#define ASCII_BLOCK 219

void resetStatistics();
void computeStatistics();
void printCLHelp(char *clarg0);

#endif
