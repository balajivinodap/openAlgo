// relStrIdx.cpp : Defines the entry point for the console application.
//
// Description available: http://en.wikipedia.org/wiki/Relative_strength_index
//
// Formula: RSI = 100 - 100 / (1 + RS) where
//			
//			RS[<N]	=	NaN
//			RS[N]	=	Avg Gain / Avg Loss
//			RS[>N]	=	(((Avg Gain[-1]*(N-1)) + Avg Gain[0]) / N)
//						------------------------------------------
//						(((Avg Loss[-1]*(N-1)) + Avg Loss[0]) / N)
//
//	NOTE:	The RS value has a weighting effect as it uses historical values in the ratio.
//			As a consequence, "the more data" the more "tuned" an RSI will be.  This is also
//			important to consider when comparing calculated RSI values across datasets.
//
// nlhs Number of output variables nargout 
// plhs Array of mxArray pointers to the output variables varargout
// nrhs Number of input variables nargin
// prhs Array of mxArray pointers to the input variables varargin
//
// Matlab function:
// rsi = relStrIdx_mex(data,N)
// 
// Inputs:
//		data		A 1-D array of prices in the form of PRICE
//		N			A scalar that defines the lookback period
//
// Outputs:
//		rsi			The calculated relative strength index (RSI)
//
// Author:			Mark Tompkins
// Last update:		4907.27867
// All rights reserved.
//

#include "mex.h"
#include <stdio.h>
#include <math.h>
#include <limits>

using namespace std;

// Macros
#define isReal2DfullDouble(P) (!mxIsComplex(P) && mxGetNumberOfDimensions(P) == 2 && !mxIsSparse(P) && mxIsDouble(P))
#define isRealScalar(P) (isReal2DfullDouble(P) && mxGetNumberOfElements(P) == 1)

// Global variables
int obsvIn;
double *barsInPtr;								// Pointer for the price matrix

void mexFunction(int nlhs, mxArray *plhs[], /* Output variables */
				 int nrhs, const mxArray *prhs[]) /* Input variables */
{
	// There are a number of provided functions for interfacing back to Matlab
	// mexFuncion		The gateway to C.  Required in every C & C++ solution to allow Matlab to call it
	// mexEvalString	Execute Matlab command
	// mexCallMatlab	Call Matlab function (.m or .dll) or script
	// mexPrintf		Print to the Matlab command window
	// mexErrMsgTxt		Issue error message and exit returning control to Matlab
	// mexWarnMsgTxt	Issue warning message
	// mexPrintf("Hello, world!"); /* Do something interesting */

	// Check number of inputs
	if (nrhs != 2)
		mexErrMsgIdAndTxt( "MATLAB:relStrIdx:NumInputs",
		"Number of input arguments is not correct. Aborting.");
	
	if (nlhs != 1)
		mexErrMsgIdAndTxt( "MATLAB:relStrIdx:NumOutputs",
		"Number of output assignments is not correct. Aborting.");

	// Define constants (#define assigns a variable as either a constant or a macro)
	// Inputs
	#define bars_IN		prhs[0]
	#define obsv_IN		prhs[1]

	// Outputs
	#define rsi_OUT		plhs[0]

	// Create a NaN value
	// mxArray *nanvalue;
	// nanvalue = mxCreateDoubleScalar(mxGetNaN());
	double m_Nan = std::numeric_limits<double>::quiet_NaN(); 

	// Init variables
	mwSize rowsData, colsData;

	// Assign variables
	rowsData = mxGetM(bars_IN);
	colsData = mxGetN(bars_IN);

	// Check type of supplied inputs
	if (!isReal2DfullDouble(bars_IN)) 
		mexErrMsgIdAndTxt( "MATLAB:relStrIdx:BadInputType",
		"Input 'data' must be a 2 dimensional full double array. Aborting.");

	if (!isRealScalar(obsv_IN)) 
		mexErrMsgIdAndTxt( "MATLAB:relStrIdx:BadInputType",
		"Input 'N' must be a single integer input. Aborting.");

	if (colsData > 1)
		mexErrMsgIdAndTxt( "MATLAB:relStrIdx:BadInputType",
		"We should only be given a 1-D price vector. Aborting.");

	/* Assign pointers to the input array */ 
	barsInPtr =	mxGetPr(bars_IN);

	/* Assign scalar values */
	obsvIn =	int(mxGetScalar(obsv_IN));

	if (obsvIn < 1)
		mexErrMsgIdAndTxt( "MATLAB:relStrIdx:BadInputType",
		"The observation lookback must be a positive integer >= 1. Aborting.");

	/* Create matrices for the return arguments */ 
	// http://www.mathworks.com/help/matlab/matlab_external/c-c-source-mex-files.html
	rsi_OUT = mxCreateDoubleMatrix(rowsData, 1, mxREAL);

	// assign the variables for manipulating the arrays (by pointer reference)
	double *RSI = mxGetPr(rsi_OUT);

	// Create temporary arrays for calculations
	double *advances = new double[rowsData];
	double *declines = new double[rowsData];
	double *avgGain = new double[rowsData];
	double *avgLoss = new double[rowsData];

/////////////
// START
/////////////

	// Calculate advances & declines
	advances[0] = 0;
	declines[0] = 0;

	for (int ii=1;ii != rowsData; ii++)					// starting at one because we are doing a difference of the observation prior
	{
		if (barsInPtr[ii] - barsInPtr[ii-1] > 0)
		{
			advances[ii] = abs(barsInPtr[ii] - barsInPtr[ii-1]);
			declines[ii] = 0;
		}
		else
		{
			advances[ii] = 0;
			declines[ii] = abs(barsInPtr[ii] - barsInPtr[ii-1]);
		}
	}

	// Calculate avgGains & avgLosses
	// Note: these are not averages in the formal sense
	//for (int ii = (obsvIn - 1); ii != rowsData; ii++)
	for (int ii = (obsvIn); ii != rowsData; ii++)
	{
		//if (ii == (obsvIn - 1))
		if (ii == (obsvIn))
		{
			double sumAdv = 0;
			double sumDec = 0;

			for (int jj = 0; jj != obsvIn; jj++)
			{
				sumAdv = sumAdv + advances[ii-jj];
				sumDec = sumDec + declines[ii-jj];
			}

			avgGain[ii] = sumAdv / obsvIn;
			avgLoss[ii] = sumDec / obsvIn;
		}
		else
		{
			avgGain[ii] = ((avgGain[ii-1] * (obsvIn - 1)) + advances[ii]) / obsvIn;
			avgLoss[ii] = ((avgLoss[ii-1] * (obsvIn - 1)) + declines[ii]) / obsvIn;
		}
	}

	// Assign RSI values to output array
	for (int ii = 0; ii != rowsData; ii++)
	{
		if (ii < (obsvIn))
		{
			RSI[ii] = m_Nan;
		}
		else
		{
			if (avgLoss[ii] == 0)
			{
				RSI[ii] = 100;
			}
			else
			{
				RSI[ii] = 100 - (100 / (1 + avgGain[ii] / avgLoss[ii]));
			}
		}
	}

	delete []advances;
	delete []declines;
	delete []avgGain;
	delete []avgLoss;

	advances = NULL;
	declines = NULL;
	avgGain = NULL;
	avgLoss = NULL;

/////////////
// FINISHED
/////////////

	return;
}

