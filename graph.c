/************************************************************************************/
/************************************************************************************/
/** graph.c																		   **/
/** A simple terminal graph drawer												   **/
/** author: 	Martin Blom														   **/
/** 			Martin.Blom@kau.se												   **/
/** version: 	1.0 															   **/
/** date:		September 20, 2017												   **/
/************************************************************************************/
/************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <ctype.h>
#include <math.h>
#define DEBUG 0								// debug flag
#define MEMMAX 262144						// maximum memory allowed
#define WMAX 1000							// maximum width of graph
#define HMAX 500							// maximum height of graph
#define FBUFMAX 20							// size of single float buffer
static const char LEGAL[] = "0123456789.-";	// legal characters in a float
static int SCREEN_HEIGHT = 24;				// screen height
static int SCREEN_WIDTH = 105;				// screen width
static float maxval=FLT_MIN;				// maximum value, used for y-scaling
static float minval=FLT_MAX;				// minimum value, used for y-scaling
static float* buffer=NULL;					// pointer to buffer containing data values
static int buf_size=0;						// size of buffer;
static char stylechar = '*';				// default data point character

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
static void usage()
{
	printf("usage:\tgraph [-sstyle] [-xsize] [-ysize] file\n");
	printf("\tstyle - (a)sterisk (d)dash (p)eriod]\n");
	printf("\t0 < xsize < %d, 0 < ysize < %d\n", WMAX, HMAX);
	printf("\tfile should contain whitespace-separated float values\n");
}
static void error()
{
	usage();
	exit(-1);
}
static void merror()
{
	printf("You have tried to input a file with too many values.\n");
	printf("Unless you have an incredibly big terminal window, the precision\n");
	printf("is totally wasted since the values are squeezed together to fit\n");
	printf("the maximum number of characters that can be shown in the terminal.\n");
	printf("Or you might be trying to smash the stack with too large input.\n");
	printf("Either way, it won't work, I quit!\n");
	exit(-1);
}
static void serror(char* msg)
{
	printf("Size error: %s\n", msg);
	error();
}
static void f_error(char* filename, char* msg)
{
	printf("File error in %s: %s\n", filename, msg);
	error();
}
static void derror()
{
	printf("Data error in file, not a float\n");
}
/************************************************************************************/
/* squeeze:		compresses buffer to fit within output buffer, using averages		*/
/* parameters: 	out_buf 		output buffer										*/
/* parameters: 	desired_size	size of output buffer								*/
/* returns: 	ratio between # of data points in file and desired_size				*/
/************************************************************************************/
static float squeeze(float* out_buf, int desired_size)
{
	int ratio = buf_size / desired_size;
	float average;
	int i, k, m=0;
	if(DEBUG)
		printf("buf_size=%d, des_size=%d, ratio=%d\n", buf_size, desired_size, ratio);
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
/* load_csv: 	loads values from input file to buffer								*/
/* parameter: 	infil - file pointer to file										*/
/* parameter: 	filename - file name												*/
/************************************************************************************/
void load_csv(FILE* infil, char* filename)
{
	int i=0, count=0, islegal=0;
	char ctmp, buf[FBUFMAX];
	printf("loading data...");
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
	printf("loaded %d values from file %s\n", count, filename);
}
/************************************************************************************/
/* determine_filesize: 	computes # of float values in input file					*/
/* parameter: 			infil - file pointer to data file							*/
/* returns: 			# of float values in infil									*/
/************************************************************************************/
int determine_filesize(FILE* infil)
{
	int i=0, count=0, islegal=0;
	char ctmp, buf[FBUFMAX];
	printf("counting values...");
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
	printf("found %d...", count);
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
    printf("opening file %s\n", filename);
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
void print_xscale(float xratio)
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
		if(origotime&&(maxval-step*(k))<=0){printf("%3d |", 0);} // origo indication
		else if((k%5==0)&&(fabs(maxval-step*k)>0.5))printf("%3.0f%c|", mflag?(maxval-step*(k))/1000000:kflag?(maxval-step*(k))/1000:maxval-step*(k), mflag?'M':kflag?'k':' ');
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
	if(DEBUG)printf("maxval=%f, SCREEN_HEIGHT=%d, step=%f, xratio=%f\n", maxval, SCREEN_HEIGHT, step, xratio);
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
void set_style(char* style)
{
	switch(style[2])
	{
		case 'a': stylechar='*'; break;
		case 'd': stylechar='-'; break;
		case 'p': stylechar='.'; break;
		/* parameter: 	buf - the buffer containing values									*/
		default : error();
	}
}
/************************************************************************************/
/* set_size:	sets the size of the graph											*/
/* parameter: 	the argument from the command line used for size setting			*/
/*				if the first letter in the argument is x, width is set  			*/
/*				if the first letter in the argument is y, height is set 			*/
/*				if the size is too large or too small, the program exits			*/
/************************************************************************************/
void set_size(char* arg)
{
	int s=0;
	if(DEBUG)printf("set_size-->arg=%s, &(arg[1])=%s\n", arg, &(arg[1])); 
	if((s=atoi(&(arg[2])))<0) serror("negative size");
	if(arg[1]=='x')
	{
		if(s > WMAX) serror("size too large");
		SCREEN_WIDTH = s;		
	}
	else
	{
		if(s > HMAX) serror("size too large");
		SCREEN_HEIGHT = s;
	}
}
/************************************************************************************/
/* process_args:	processes the command line arguments							*/
/* parameter: 		argc and argv as provided to main								*/
/*					-s sets the style, -x sets width, -y sets height, -h shows help */
/*					if the argument does not start with a hyphen, the program exits */
/*					if the argument is unknown, the program exits					*/
/************************************************************************************/
void process_args(int argc, char** argv)
{
	int i;
	for(i=1;i<argc-1;i++)
	{
		if(argv[i][0]!='-') error();
		switch(argv[i][1])
		{
			case 's': set_style(argv[i]); 	break;
			case 'x': set_size(argv[i]); 	break;
			case 'y': set_size(argv[i]); 	break;
			case 'h': usage(); exit(0);		break;
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
	process_args(argc, argv);
	load(argv[argc-1]);
	graph_buf();
	return 0;
}
