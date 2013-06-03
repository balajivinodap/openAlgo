#include "mex.h"
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	/*
	mwSize is a type that represents size values, such as array dimensions.
	mxGetN Call mxGetN to determine the number of columns in the specified mxArray. 
	plhs <-- "left hand side"
	prhs <-- "right hand side"
	http://cnx.org/content/m12348/latest/ (mex in general)
	http://www.mathworks.com/help/matlab/apiref/mwsize.html (mwSize)
	http://www.mathworks.com/help/matlab/apiref/mxgetn.html?searchHighlight=mxgetn (mxGetN)
	
	The first line assigns n as a variable that will hold array dimensions
	'prhs[0] represents the input array because it is the '0' (first) element input in the function
	*/
    mwSize n;
    if( n = mxGetN(prhs[0]) )
        mxSetN(prhs[0], n - 1);
}