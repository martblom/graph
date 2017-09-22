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
#define KILO 1000
#define MEGA 1000000
static const char LEGAL[] = "0123456789.-";	// legal characters in a float
static int SCREEN_HEIGHT = 17;				// screen height
static int SCREEN_WIDTH = 69;				// screen width
static float maxval=FLT_MIN;				// maximum value, used for y-scaling
static float minval=FLT_MAX;				// minimum value, used for y-scaling
//static float* buffer=NULL;					// pointer to buffer containing data values
//static int buf_size=0;						// size of buffer;
static char stylechar = '*';				// default data point character
static char *unistylechar= "\u0FD5";		// default data point unicode character
static int unicode = 0;						// set to 1 for unicode
static int compression = 2;					// compression scheme, 1=average 2=select
/************************************************************************************/
/* status and error messages														*/
/************************************************************************************/
void usage()
{
	printf("usage:\tgraph [-sstyle] [-xsize] [-ysize] [-l] [file]\n");
	printf("\tstyle - (a)sterisk (d)dash (p)eriod]\n");
	printf("\t0 < xsize < %d, 0 < ysize < %d\n", WMAX, HMAX);
	printf("\t-l prints license\n");
	printf("\tfile should contain float values in plain text\n");
}
static void print_data(float* buf, int size)
{
	int i;
	for(i=0;i<size;i++)
		printf("buf[%d]=%1.5f, ", i,buf[i]);
	printf("\n");
}
void error()
{
	usage();
	exit(-1);
}
void merror(char* filename, int buf_size)
{
	printf("%s is too large, %d floats. Max size is %d\n", filename, buf_size, MEMMAX);
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
void cerror()
{
	printf("Compression scheme unknown, using selection as default\n");
}
/************************************************************************************/
/* load_csv: 	loads values from input file to buffer								*/
/* parameter: 	infil - file pointer to file										*/
/* parameter: 	filename - file name												*/
/************************************************************************************/
static void load_csv(float* out_buf, FILE* infil, char* filename)
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
				out_buf[count++] = atof(buf);
				if(DEBUG) printf("load_csv: out_buf[%d]=%f, ", count-1, out_buf[count-1]);
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
float* load(char* filename, int* buf_size)
{
    FILE* infil;
	float* ret_buf=NULL;
    printf("file: %s ", filename);
    infil = fopen(filename, "r");
    if(!infil)
    {
		f_error(filename, "cannot open file");
    	exit(-1);
    }
    else
    {
		*buf_size = determine_filesize(infil);
		if(*buf_size>MEMMAX)merror(filename, *buf_size);
		if(*buf_size<=0) f_error(filename, "no values found in file");
		//if(ret_buf)free(ret_buf);
		ret_buf = (float*)malloc(*buf_size*sizeof(float));

		load_csv(ret_buf, infil, filename);
		printf("%d values found.\n", *buf_size);
		if(DEBUG)print_data(ret_buf, *buf_size);
		fclose(infil);
    }
    return ret_buf;
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
static float squeeze(float* in_buf, int orig_size, float* out_buf, int desired_size)
{
	float ratio = (float)orig_size / (float)desired_size;
	float average, i;
	int k, m=0;
	if(DEBUG)
		printf("orig_size=%d, des_size=%d, ratio=%f\n", orig_size, desired_size, ratio);
	for(i=0; i<orig_size; i+=ratio)
	{
		average = 0;
		for(k=i;k<i+ratio;k++)
			average += in_buf[k];
		out_buf[m++]=average/ratio;
	}
	if(DEBUG)print_data(out_buf, m);
	return ratio;
}
/************************************************************************************/
/* squeeze2:	compresses buffer to fit within output buffer, using selection		*/
/* parameters: 	out_buf 		output buffer										*/
/* parameters: 	desired_size	size of output buffer								*/
/* returns: 	ratio between # of data points in file and desired_size				*/
/************************************************************************************/
static float squeeze2(float* in_buf, int orig_size, float* out_buf, int desired_size)
{
	float ratio = (float)orig_size / (float)desired_size;
	float i;
	int m=0;
	if(DEBUG)
		printf("buf_size=%d, des_size=%d, ratio=%f\n", orig_size, desired_size, ratio);
	for(i=0; i<orig_size; i+=ratio)
	{
		out_buf[m++]=in_buf[(int)i];
	}
	if(DEBUG)print_data(out_buf, m);
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
	printf("graph \u14B7 Copyright (C) 2017 Martin Blom\n");
	if(size>SCREEN_WIDTH)				// more data points than positions on x-axis?
	{
		if(compression==1)
			xratio = squeeze(buf, size, copy, SCREEN_WIDTH);// squeeze using averages...
		else
			xratio = squeeze2(buf, size, copy, SCREEN_WIDTH);//...or selection
	}
	else
		for(i=0;(i<size)&&(i<SCREEN_WIDTH);i++)				// or simple copy
			copy[i]=buf[i];

	maxval = findmax(buf,size);							// find highest value in set
	minval = findmin(buf,size);							// find lowest value in set
	step = (maxval-(minval>0?0:minval))/((float)SCREEN_HEIGHT);		// compute y-ratio

	printf("%4c\n",'Y');
	for(k=0;k<=SCREEN_HEIGHT;k++)
	{
		kflag=(maxval-step*k)>KILO?1:0;
		mflag=(maxval-step*k)>MEGA?1:0;
		// ------------------------------------------------Y-axis drawing, 3 cases:
		if(origotime&&(maxval-step*k)<=0)				// case 1: origo, i.e. draw '0'
			{printf("%3d |", 0);}						// and a line		
		else if((k%5==0)&&(fabs(maxval-step*k)>0.5)) 	// case 2: draw label + line
			printf("%3.0f%c|", 	mflag?(maxval-step*k)/MEGA: 
								kflag?(maxval-step*k)/KILO:
								maxval-step*k, 
								mflag?'M':
								kflag?'k':
								' ');
		else printf("%5c", '|');						// case 3: draw just the line
		// ------------------------------------------------Plot data points top-down
		for(i=0 ; i<SCREEN_WIDTH&&i<size; i++)
		{
			if(copy[i]>=(maxval-step*k) && copy[i]!=0)	// if value is high enough
			{											// and is not plotted before
				if(unicode) printf("%s", unistylechar);
				else printf("%c", stylechar);			// plot the value using
				copy[i] = 0;							// stylechar
			}
			else										// if no value should be plotted
			{
				if(origotime&&(maxval-step*(k))<=0)printf("_"); // X-axis drawing
				else printf(" ");								// or nothing
			}
		}
		if(origotime&&(maxval-step*(k))<=0)				// Draw rest of X-axis
		{
			for(m=i;m<SCREEN_WIDTH;m++)
				printf("_");
			printf("X (%d)", size<SCREEN_WIDTH?SCREEN_WIDTH:size);
			origotime=0;
		}
		printf("\n");
	}
	print_xscale(xratio);								// Draw the X-scale
	if(DEBUG)printf("maxval=%f, SCREEN_WIDTH=%d, SCREEN_HEIGHT=%d, step=%f, xratio=%f\n", maxval, SCREEN_WIDTH, SCREEN_HEIGHT, step, xratio);
	free(copy);
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
/* parameter: 	the character to be used											*/
/*				if the character is non-standard-ASCII, it is set to '?'			*/
/************************************************************************************/
void set_style(char style)
{
	stylechar = style;
}
void set_unistyle(char* style)
{
	printf("not implemented\n");
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
/************************************************************************************/
/* set_compression:	sets the compression scheme for the data values					*/
/* parameter: 	c - 'a' for average, 's' for select									*/
/*				if c is neither 'a' or 's', select is chosen as default				*/
/************************************************************************************/
void set_compression(char c)
{
	if(c=='a') compression = 1;
	else if(c=='s') compression = 2;
	else cerror();
}