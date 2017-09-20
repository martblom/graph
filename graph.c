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
static const char LEGAL[] = "0123456789.-";	// legal characters in a float
static int SCREEN_HEIGHT = 17;				// screen height
static int SCREEN_WIDTH = 69;				// screen width
static float maxval=FLT_MIN;				// maximum value, used for y-scaling
static float minval=FLT_MAX;				// minimum value, used for y-scaling
static float* buffer=NULL;					// pointer to buffer containing data values
static int buf_size=0;						// size of buffer;
static char stylechar = '*';				// default data point character
static int YMARGIN = 4;						// size of margin left of y-axis
/************************************************************************************/
/* status and error messages														*/
/************************************************************************************/
static void print_data(float* buf)
{
	int i;
	for(i=0;i<buf_size;i++)
		printf("buf[%d]=%f, ", i,buf[i]);
	printf("\n");
}
void error()
{
	usage();
	exit(-1);
}
void merror()
{
	printf("You have tried to input a file with too many values.\n");
	printf("Unless you have an incredibly big terminal window, the precision\n");
	printf("is totally wasted since the values are squeezed together to fit\n");
	printf("the maximum number of characters that can be shown in the terminal.\n");
	printf("Or you might be trying to smash the stack with too large input.\n");
	printf("Either way, it won't work, I quit!\n");
	exit(-1);
}
void serror(char* msg, int size)
{
	printf("Size error (%d): %s\n", size, msg);
	error();
}
void f_error(char* filename, char* msg)
{
	printf("File error in %s: %s\n", filename, msg);
	error();
}
void derror()
{
	printf("Data error in file, not a float\n");
}
/************************************************************************************/
/* load_csv: 	loads values from input file to buffer								*/
/* parameter: 	infil - file pointer to file										*/
/* parameter: 	filename - file name												*/
/************************************************************************************/
static void load_csv(FILE* infil, char* filename)
{
	int i=0, count=0, islegal=0;
	char ctmp, buf[FBUFMAX];
	if(DEBUG)printf("loading data...");
	rewind(infil);
	while((ctmp=getc(infil))!=EOF)
	{
		if(strchr(LEGAL, ctmp))
		{
			if(i>=FBUFMAX){derror();i=0;islegal=0;}
			else
			{
				buf[i++] = ctmp;
				if(DEBUG)printf("[%c]", ctmp);
				islegal=1;
			}
		}
		else
		{
			if(islegal)
			{
				buf[i]='\0';
				buffer[count++] = atof(buf);
				if(DEBUG) printf("load_csv: buffer[%d]=%f, ", count-1, buffer[count-1]);
				islegal=0;
			}
			i=0;
		}
	}
	if(DEBUG)printf("loaded %d values from file %s\n", count, filename);
}
/************************************************************************************/
/* determine_filesize: 	computes # of float values in input file					*/
/* parameter: 			infil - file pointer to data file							*/
/* returns: 			# of float values in infil									*/
/************************************************************************************/
static int determine_filesize(FILE* infil)
{
	int i=0, count=0, islegal=0;
	char ctmp, buf[FBUFMAX];
	if(DEBUG)printf("counting values...");
	while((ctmp=getc(infil))!=EOF)
	{
		if(strchr(LEGAL, ctmp))
		{
			if(i>=FBUFMAX){derror();i=0;islegal=0;}
			else
			{
				buf[i++] = ctmp;
				islegal=1;
			}
		}
		else
		{
			if(islegal)
			{
				count++;
				islegal=0;
			}
			i=0;
		}
	}
	if(DEBUG)printf("found %d...", count);
	return count;
}
/************************************************************************************/
/* load:		loads values from file to local global buffer						*/
/* parameter: 	filename - name of file containing values							*/
/* returns: 	number of values read from file and size of buffer					*/
/************************************************************************************/
int load(char* filename)
{
    FILE* infil;
    int i=0;
    if(DEBUG)printf("opening file %s\n", filename);
    infil = fopen(filename, "r");
    if(!infil)
    {
		f_error(filename, "cannot open file");
    	exit(-1);
    }
    else
    {
		buf_size = determine_filesize(infil);
		if(buf_size>MEMMAX)merror();
		if(buf_size<=0) f_error(filename, "no values found in file");
		buffer = (float*)malloc(buf_size*sizeof(float));

		load_csv(infil, filename);
		if(DEBUG)print_data(buffer);
		fclose(infil);
    }
    return i;
}
/************************************************************************************/
/* findmax: 	finds maximum value in buf											*/
/* parameter: 	buf - the buffer containing values									*/
/* parameter: 	size - the buffer size												*/
/*				returns maxumum value if possible, FLT_MIN otherwise				*/
/************************************************************************************/
static float findmax(float* buf, int size)
{
	int i;
	maxval = FLT_MIN;
	for(i=0;i<size;i++)
		if(maxval<buf[i])
			maxval = buf[i];
	return maxval;
}
/************************************************************************************/
/* findmin: 	finds minimum value in buf											*/
/* parameter: 	buf - the buffer containing values									*/
/* parameter: 	size - the buffer size												*/
/*				returns minimum value if possible, FLT_MAX otherwise				*/
/************************************************************************************/
static float findmin(float* buf, int size)
{
	int i;
	minval = FLT_MAX;
	for(i=0;i<size;i++)
		if(minval>buf[i])
			minval = buf[i];
	return minval;
}
/************************************************************************************/
/* print_xscale: 	draws the x-axis and scale of the graph							*/
/* parameter: 		xratio - the ratio between # of datapoints and graph width		*/
/************************************************************************************/
static void print_xscale(float xratio)
{
	int i;
	printf("%4c", ' ');						// padding before x-scale lines
	for(i=0 ; i<SCREEN_WIDTH; i++)			// print x-scale lines
		if(i%10==0)
			printf("|");
		else
			printf(" ");
	printf("\n%4c", ' ');					// padding before x-scale values
		for(i=0 ; i<SCREEN_WIDTH; i++)		// print x-scale values
			if(i%10==0)
				printf("%-10.0f", i*xratio);
	printf("\n");
}
/************************************************************************************/
/* squeeze:		compresses buffer to fit within output buffer, using averages		*/
/* parameters: 	out_buf 		output buffer										*/
/* parameters: 	desired_size	size of output buffer								*/
/* returns: 	ratio between # of data points in file and desired_size				*/
/************************************************************************************/
static float squeeze(float* out_buf, int desired_size)
{
	float ratio = (float)buf_size / (float)desired_size;
	float average;
	int i, k, m=0;
	if(DEBUG)
		printf("buf_size=%d, des_size=%d, ratio=%f\n", buf_size, desired_size, ratio);
	for(i=0; i<buf_size; i+=ratio)
	{
		average = 0;
		for(k=i;k<i+ratio;k++)
			average += buffer[k];
		out_buf[m++]=average/ratio;
	}
	if(DEBUG)printf("m=%d\n", m);
	return ratio;
}
/************************************************************************************/
/* _graph: 		draws a graph of data values in buf of size size					*/
/* parameter: 	buf - the buffer containing values									*/
/* parameter: 	size - the buffer size												*/
/************************************************************************************/
static void _graph(float *buf, int size)
{
	int i, k, m, origotime=1, kflag=0, mflag=0;
	float step=1.0, xratio=1;
	float *copy = (float*)malloc((size>SCREEN_WIDTH?SCREEN_WIDTH:size)*sizeof(float));
	if(size>SCREEN_WIDTH)				// more data points than positions on x-axis?
		xratio = squeeze(copy, SCREEN_WIDTH);	// squeeze data points to fit
	else
		for(i=0;i<size;i++)				// simple copy
			copy[i]=buf[i];
			
	maxval = findmax(buf,size);		// find highest value in set
	minval = findmin(buf,size);		// find lowest value in set
	step = (maxval-(minval>0?0:minval))/((float)SCREEN_HEIGHT);		// compute y-ratio
	if(step>1000) kflag=1;
	if(step>1000000) mflag=1;
	
	printf("%4c\n",'Y');
	for(k=0;k<=SCREEN_HEIGHT;k++)
	{
		if(origotime&&(maxval-step*(k))<=0)
			{printf("%3d |", 0);}
		else if((k%5==0)&&(fabs(maxval-step*k)>0.5)) printf("%3.0f%c|", mflag?(maxval-step*k)/1000000:kflag?(maxval-step*k)/1000:maxval-step*k, mflag?'M':kflag?'k':' ');
		else printf("%5c", '|');
		for(i=0 ; i<SCREEN_WIDTH&&i<size; i++)
		{
			if(copy[i]>=(maxval-step*k) && copy[i]!=0)
			{
				printf("%c", stylechar);
				copy[i] = 0;
			}
			else
			{
				if(origotime&&(maxval-step*(k))<=0)printf("_");
				else printf(" ");
			}
		}
		if(origotime&&(maxval-step*(k))<=0)
		{
			for(m=i;m<SCREEN_WIDTH;m++)
				printf("_");
			printf("X (%d)", size<SCREEN_WIDTH?SCREEN_WIDTH:size);
			origotime=0;
		}
		printf("\n");
	}
	print_xscale(xratio);
	if(DEBUG)printf("maxval=%f, SCREEN_WIDTH=%d, SCREEN_HEIGHT=%d, step=%f, xratio=%f\n", maxval, SCREEN_WIDTH, SCREEN_HEIGHT, step, xratio);
}
/************************************************************************************/
/* graph_buf: 	draws a graph of data values in (global) buffer of size buf_size	*/
/*				calls _graph for actual work										*/
/* pre: buffer should contain values, load(filename)								*/
/************************************************************************************/
void graph_buf()
{
	if(buf_size<=0)
		printf("No values in buffer, use load <filename> before\n");
	else
		_graph(buffer, buf_size);
}
/************************************************************************************/
/* graph: 		draws a graph of data values in buf of size size					*/
/* parameter: 	buf - the buffer containing values									*/
/* parameter: 	size - the buffer size												*/
/*				calls _graph for actual work										*/
/************************************************************************************/
void graph(float *buf, int size)
{
	_graph(buf, size);
}
/************************************************************************************/
/* set_style:	sets the character used to represent data points					*/
/* parameter: 	the letter input by the user to identify the char.					*/
/*				if the character letter is unknown, the program exits				*/
/************************************************************************************/
void set_style(char style)
{
	stylechar = style;
	/*switch(style[2])
	{
		case 'a': stylechar='*'; break;
		case 'd': stylechar='-'; break;
		case 'p': stylechar='.'; break;
		default : error();
	}*/
}
/************************************************************************************/
/* set_width:	sets the width of the graph	(in characters)							*/
/* parameter: 	s - the width														*/
/*				if size is wrong, the program exits									*/
/************************************************************************************/
void set_width(int s)
{
	if(s > WMAX) serror("width too large", s);
	SCREEN_WIDTH = s;
}
/************************************************************************************/
/* set_height:	sets the height of the graph	(in characters)						*/
/* parameter: 	s - the height														*/
/*				if size is wrong, the program exits									*/
/************************************************************************************/
void set_height(int s)
{
	if(s > HMAX) serror("height too large", s);
	SCREEN_HEIGHT = s;
}
