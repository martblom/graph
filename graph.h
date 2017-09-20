/************************************************************************************
graph	- A simple terminal data visualizer									   	
Copyright (C) 2017 	Martin Blom
					e-mail: Martin.Blom@kau.se
					
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 	version: 	1.0 															   	
 	date:		September 20, 2017												   	
*************************************************************************************/

/************************************************************************************/
/* constant declaration(s)			 												*/
/************************************************************************************/
#ifndef __GRAPH_H
#define __GRAPH_H
#define DEBUG 0					// global debug flag, set to 1 for debug info
#define MEMMAX 262144			// maximum memory allowed
#define WMAX 1000				// maximum width of graph
#define HMAX 500				// maximum height of graph
#define FBUFMAX 20				// size of single float buffer
/************************************************************************************/
/* library inclusion				 												*/
/************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <ctype.h>
#include <math.h>
#endif
/************************************************************************************/
/* These are the exported functions, i.e. the API for the graph data visualizer		*/
/* #include this file, and link the corresponding object file to use functionality	*/
/************************************************************************************/
int load(char* filename);			// loads file to buffer
void graph_buf();					// draws graph of values in buffer
void graph(float *buf, int size);	// draws graph of size values in buf
void set_style(char style);			// sets the character used to plot data points
void set_width(int s);				// sets maximum width of graph
void set_height(int s);				// sets maximum height of graph
void usage();						// prints how to use the program

/************************************************************************************/
/* error messages, called internally 												*/
/************************************************************************************/
void error();						// prints general error message
void merror();						// prints memory error message
void serror(char* msg, int size);	// prints size error message
void f_error(char* file, char* msg);// prints file error message
void derror();						// prints data error message
/************************************************************************************/