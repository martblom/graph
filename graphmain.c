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
#include "graph.h"
/************************************************************************************/
/* set_size:	sets the size of the graph											*/
/* parameter: 	the argument from the command line used for size setting			*/
/*				if the first letter in the argument is x, width is set  			*/
/*				if the first letter in the argument is y, height is set 			*/
/*				if the size is too large or too small, the program exits			*/
/************************************************************************************/
static void set_size(char* arg)
{
	int s=0;
	if(DEBUG)printf("set_size-->arg=%s, &(arg[1])=%s\n", arg, &(arg[1])); 
	if((s=atoi(&(arg[2])))<0) serror("negative size", s);
	if(arg[1]=='x')
		set_width(s);
	else
		set_height(s);
}
void usage()
{
	printf("usage:\tgraph [-sstyle] [-xsize] [-ysize] [-l] [file]\n");
	printf("\tstyle - (a)sterisk (d)dash (p)eriod]\n");
	printf("\t0 < xsize < %d, 0 < ysize < %d\n", WMAX, HMAX);
	printf("\t-l prints license\n");
	printf("\tfile should contain float values in plain text\n");
}
static void print_welcome()
{
	printf("graph Copyright (C) 2017 Martin Blom\n");
	printf("\n");
    printf("This program is free software: you can redistribute it and/or modify\n");
	printf(" it under the terms of the GNU General Public License as published by\n");
    printf("the Free Software Foundation, either version 3 of the License, or\n");
    printf("(at your option) any later version.\n");
	printf("\n");
    printf("This program is distributed in the hope that it will be useful,\n");
    printf("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
    printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
    printf("GNU General Public License for more details.\n");
	printf("\n");
    printf("You should have received a copy of the GNU General Public License\n");
    printf("along with this program.  If not, see <http://www.gnu.org/licenses/>.\n");
}
/************************************************************************************/
/* process_args:	processes the command line arguments							*/
/* parameter: 		argc and argv as provided to main								*/
/*					-s sets the style, -x sets width, -y sets height, -h shows help */
/*					if the argument does not start with a hyphen, the program exits */
/*					if the argument is unknown, the program exits					*/
/************************************************************************************/
static void process_args(int argc, char** argv)
{
	int i;
	for(i=1;i<argc-1;i++)
	{
		if(argv[i][0]!='-') error();
		switch(argv[i][1])
		{
			case 's': set_style(argv[i][2]); 	break;
			case 'x': set_size(argv[i]); 		break;
			case 'y': set_size(argv[i]); 		break;
			case 'l': print_welcome();			break;
			case 'h': usage(); exit(0);			break;
			default: error();
		}
	}
}
/************************************************************************************/
/* main																				*/
/************************************************************************************/
int main(int argc, char** argv)
{
	if(argc<2||argc>5) error();
	printf("graph Copyright (C) 2017 Martin Blom\n");
	process_args(argc, argv);
	load(argv[argc-1]);
	graph_buf();
	return 0;
}