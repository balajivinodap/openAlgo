// taInvoke.cpp
// Localized mex'ing: mex taInvoke.cpp @mexOpts.txt
// Matlab function: 
// [varout] = taInvoke(taFunction, varin)
//
// Inputs:
//		taFunction	The name of the TA-Lib function to call
//		varin		The input variable(s) as necessary for the called taFunction
//
// Outputs:
//		varout		The output(s) as produced from the call to the taFunction

#include "mex.h"
#include "ta_libc.h"
#include <map>
#include <algorithm>	// So we can transform the function name string input ...
#include <string>		// from char to string ensuring lowercase
#include "myMath.h"

//#include "matrix.h" <-- May not be necessary

using namespace std;

// Value-Definitions of the different String values
static enum StringValue { taNotDefined, ta_accbands, ta_acos, ta_ad, ta_add, ta_adosc, ta_adx, ta_adxr, ta_apo, ta_aroon, ta_aroonosc, ta_asin, ta_atan, ta_atr, ta_avgdev, ta_avgprice, ta_bbands, 
							ta_beta, ta_bop, ta_cci, 
							// Candlestick section start
							ta_cdl2crows, ta_cdl3blackcrows, ta_cdl3inside, ta_cdl3linestrike, ta_cdl3outside, ta_cdl3starsinsouth, ta_cdl3whitesoldiers, ta_cdlabandonedbaby, 
							ta_cdladvanceblock, ta_cdlbelthold, ta_cdlbreakaway, ta_cdlclosingmarubozu, ta_cdlconcealbabyswall, ta_cdlcounterattack, ta_cdldarkcloudcover, ta_cdldoji, 
							ta_cdldojistar, ta_cdldragonflydoji, ta_cdlengulfing, ta_cdleveningdojistar, ta_cdleveningstar, ta_cdlgapsidesidewhite, ta_cdlgravestonedoji, ta_cdlhammer, 
							ta_cdlhangingman, ta_cdlharami, ta_cdlharamicross, ta_cdlhighwave, ta_cdlhikkake, ta_cdlhikkakemod, ta_cdlhomingpigeon, ta_cdlidentical3crows, ta_cdlinneck, 
							ta_cdlinvertedhammer, ta_cdlkicking, ta_cdlkickingbylength, ta_cdlladderbottom, ta_cdllongleggeddoji, ta_cdllongline, ta_cdlmarubozu, ta_cdlmatchinglow, 
							ta_cdlmathold, ta_cdlmorningdojistar, ta_cdlmorningstar, ta_cdlonneck, ta_cdlpiercing, ta_cdlrickshawman, ta_cdlrisefall3methods, ta_cdlseparatinglines, 
							ta_cdlshootingstar, ta_cdlshortline, ta_cdlspinningtop, ta_cdlstalledpattern, ta_cdlsticksandwich, ta_cdltakuri, ta_cdltasukigap, ta_cdlthrusting, 
							ta_cdltristar, ta_cdlunique3river, ta_cdlupsidegap2crows, ta_cdlxsidegap3methods, 
							// Candlestick section end
							ta_ceil, ta_cmo, ta_correl, ta_cos, ta_cosh, ta_dema, ta_div, ta_dx, ta_ema, ta_exp, ta_floor, ta_ht_dcperiod, ta_ht_dcphase, ta_ht_phasor, ta_ht_sine, 
							ta_ht_trendline, ta_ht_trendmode, ta_kama, ta_linearreg, ta_linearreg_angle, ta_linearreg_intercept, ta_linearreg_slope, ta_ln, ta_log10, ta_ma, ta_macd, 
							ta_macdext, ta_macdfix, ta_mama, ta_mavp, ta_max, ta_maxindex, ta_medprice, ta_mfi, ta_midpoint, ta_midprice, ta_min, ta_minindex, ta_minmax, ta_minmaxindex, 
							ta_minus_di, ta_minus_dm, ta_mom, ta_mult, ta_natr, ta_obv, ta_plus_di, ta_plus_dm, ta_ppo, ta_roc, ta_rocp, ta_rocr, ta_rocr100, ta_rsi, ta_sar, ta_sarext,
							ta_sin, ta_sinh, ta_sma, ta_sqrt, ta_stddev, ta_stoch, ta_stochf, ta_stochrsi, ta_sub, ta_sum, ta_t3, ta_tan, ta_tanh, ta_tema, ta_trange, ta_trima, ta_trix, 
							ta_tsf, ta_typprice, ta_ultosc, ta_var, ta_wclprice, ta_willr, ta_wma};

// Prototypes
// Map to associate the strings with the enum values
static map<string, StringValue> s_mapStringValues;
void chkSingleVec(int colsH, int colsL, int lineNum);
void chkSingleVec(int colsH, int colsL, int colsC, int lineNum);
void chkSingleVec(int colsO, int colsH, int colsL, int colsC, int lineNum);

static void InitSwitchMapping();

// Macros
#define isReal2DfullDouble(P) (!mxIsComplex(P) && mxGetNumberOfDimensions(P) == 2 && !mxIsSparse(P) && mxIsDouble(P))
#define isRealScalar(P) (isReal2DfullDouble(P) && mxGetNumberOfElements(P) == 1)
#define codeLine	__LINE__	// help error trapping

// Global variables
double m_Nan = std::numeric_limits<double>::quiet_NaN(); 

void mexFunction(int nlhs, mxArray *plhs[], /* Output variables */
				 int nrhs, const mxArray *prhs[]) /* Input variables */
{
	// Check number of inputs
	if (nrhs < 2)
		mexErrMsgIdAndTxt( "MATLAB:taInvoke:NumInputs",
		"Number of input arguments is not correct. Aborting (%d).",codeLine);

	// Define constants (#define assigns a variable as either a constant or a macro)
	// Inputs
	#define taFuncName_IN		prhs[0]

	/* Assign pointers to the function string */ 
	int funcNumChars = (int)mxGetN(taFuncName_IN)+1;	// +1 for the NULL added at the end
	char *funcAsChars = (char*)mxCalloc(funcNumChars, sizeof(char));	// Allocate space on the heap to store the name

	if (funcAsChars == NULL) mexErrMsgTxt("Not enough heap space to hold converted string.");

	int status = mxGetString(taFuncName_IN, funcAsChars, funcNumChars); 
	if (status != 0) mexErrMsgTxt("Could not parse the given function.");

	string taFuncNameIn((funcAsChars));

	transform(taFuncNameIn.begin(), taFuncNameIn.end(), taFuncNameIn.begin(), ::tolower);

	// Quick cleanup
	mxFree(funcAsChars);

	// Init the string mapping
	InitSwitchMapping();

	switch (s_mapStringValues[taFuncNameIn])
	{
		// Acceleration Bands
		case ta_accbands:
			{
				// REQUIRED INPUTS
				//		Price	H | L | C	separate vectors

				// OPTIONAL INPUTS
				//		Lookback period	(default 14)

				// OUTPUTS
				//		upperBand		
				//		midBand
				//		lowerBand

				// Check number of inputs
				if (nrhs < 4 || nrhs > 5)
					mexErrMsgIdAndTxt("MATLAB:taInvoke:ta_accbands:NumInputs",
					"Number of input arguments to function 'ta_accbands' is incorrect. Price data should be parsed into vectors H | L | C. Aborting (%d).", codeLine);
				if (nlhs != 3)
					mexErrMsgIdAndTxt("MATLAB:taInvoke:ta_accbands:NumOutputs",
					"The function 'ta_accbands' (Acceleration Bands) produces 3 vector outputs that must be assigned. Aborting (%d).",codeLine);

				// Create constants for readability
				// Inputs
				#define high_IN		prhs[1]
				#define low_IN		prhs[2]
				#define close_IN	prhs[3]

				// Outputs
				#define accUpper_OUT	plhs[0]
				#define accMid_OUT		plhs[1]
				#define accLower_OUT	plhs[2]

				// Declare variables
				int startIdx, endIdx, rows, colsH, colsL, colsC, lookback;
				double *highPtr, *lowPtr, *closePtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				highPtr		= mxGetPr(high_IN);
				rows		= (int)mxGetM(high_IN);
				colsH		= (int)mxGetN(high_IN);
				lowPtr		= mxGetPr(low_IN);
				colsL		= (int)mxGetN(low_IN);
				closePtr	= mxGetPr(close_IN);
				colsC		= (int)mxGetN(close_IN);

				// Validate
				chkSingleVec(colsH, colsL, colsC, codeLine);

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int accIdx, outElements;
				double *accUpper, *accMid, *accLower;

				// Parse optional inputs if given, else default 
				if (nrhs == 5) 
				{
					#define lookback_IN	prhs[4]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The ACCBANDS lookback must be a scalar. Aborting (%d).",codeLine);
					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);
				}
				else
					// Default lookback period
				{
					lookback = 14;
				}

				// Preallocate heap
				accUpper	= (double*)mxCalloc(rows, sizeof(double));
				accMid		= (double*)mxCalloc(rows, sizeof(double));
				accLower	= (double*)mxCalloc(rows, sizeof(double));

				retCode = TA_ACCBANDS(startIdx, endIdx, highPtr, lowPtr, closePtr, lookback, &accIdx, &outElements, accUpper, accMid, accLower);

				// Error handling
				if (retCode) 
				{
					mxFree(accUpper);
					mxFree(accMid);
					mxFree(accLower);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (180).", taFuncNameIn);
				}

				// Populate Output
				accUpper_OUT = mxCreateDoubleMatrix(accIdx + outElements,1, mxREAL);
				accMid_OUT = mxCreateDoubleMatrix(accIdx + outElements,1, mxREAL);
				accLower_OUT = mxCreateDoubleMatrix(accIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(accUpper_OUT)) + accIdx, accUpper, outElements * mxGetElementSize(accUpper_OUT));
				memcpy(((double *) mxGetData(accMid_OUT)) + accIdx, accMid, outElements * mxGetElementSize(accMid_OUT));
				memcpy(((double *) mxGetData(accLower_OUT)) + accIdx, accLower, outElements * mxGetElementSize(accLower_OUT));

				// Cleanup
				mxFree(accUpper); 
				mxFree(accMid); 
				mxFree(accLower); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *uBandPtr = mxGetPr(accUpper_OUT);
				double *mBandPtr = mxGetPr(accMid_OUT);
				double *lBandPtr = mxGetPr(accLower_OUT);

				for (int iter = 0; iter < lookback; iter++)
				{
					uBandPtr[iter] = m_Nan;
					mBandPtr[iter] = m_Nan;
					lBandPtr[iter] = m_Nan;
				}

				break;
			}
			
		// Vector Trigonometric ACos
		case ta_acos:
			{
				// REQUIRED INPUTS
				//		input	any single column vector of cosine values (-1 to 1)

				// OPTIONAL INPUTS
				//		none

				// OUTPUTS
				//		ACOS	Inverse cosine of input (radians)
				//				e.g. acos(-1) = pi (~3.14) radians = 180 degrees

				// Check number of inputs
				if (nrhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_acos:NumInputs",
					"Number of input arguments to function 'ta_acos' is not correct. A single vector of values (-1 =< x =< 1) should be provided. Aborting (%d).",codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_acos:NumOutputs",
					"The function 'ta_acos' (Vector Trigonometric ACos) produces a single vector output that must be assigned. Aborting (%d).",codeLine);

				// Create constants for readability
				// Inputs
				#define cos_IN			prhs[1]

				// Outputs
				#define acos_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsCos;
				double *cosPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse inputs and error check
				// Assign pointers and get dimensions
				cosPtr		= mxGetPr(cos_IN);
				colsCos		= (int)mxGetN(cos_IN);
				rows		= (int)mxGetM(cos_IN);

				if (colsCos != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_acos:InputErr",
						"Cosine data should be a single vector array. Aborting (%d).",codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int acosIdx, outElements;
				double *outReal;

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				// May have to change typeMA from decimal to name
				retCode = TA_ACOS(startIdx, endIdx, cosPtr, &acosIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgTxt("Invocation to 'ta_acos' failed. Aborting (283).");
				}

				// Populate Output
				acos_OUT = mxCreateDoubleMatrix(acosIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(acos_OUT)) + acosIdx, outReal, outElements * mxGetElementSize(acos_OUT));

				// Cleanup
				mxFree(outReal); 

				break;
			}
			
		// Chaikin A/D Line
		case ta_ad:
			{
				// REQUIRED INPUTS
				//		Price	H | L | C	separate vectors
				//		Volume

				// OPTIONAL INPUTS
				//		none

				// OUTPUT
				//		AD		vector of Chaikin advance / decline line values

				// Check number of inputs
				if (nrhs != 5)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ad:NumInputs",
					"Number of input arguments to function 'ta_ad' is not correct. Price data should be parsed into vectors H | L | C followed by a volume vector V. Aborting (%d).",codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ad:NumOutputs",
						"The function 'ta_ad' (Chaikin A/D Line) produces a single vector output that must be assigned. Aborting (%d).",codeLine);
			
				// Create constants for readability
				// Inputs
				#define high_IN		prhs[1]
				#define low_IN		prhs[2]
				#define close_IN	prhs[3]
				#define vol_IN		prhs[4]

				// Outputs
				#define ad_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsH, colsL, colsC, colsV;
				double *highPtr, *lowPtr, *closePtr, *volPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse inputs and error check
				// Assign pointers and get dimensions
				highPtr		= mxGetPr(high_IN);
				rows		= (int)mxGetM(high_IN);
				colsH		= (int)mxGetN(high_IN);
				lowPtr		= mxGetPr(low_IN);
				colsL		= (int)mxGetN(low_IN);
				closePtr	= mxGetPr(close_IN);
				colsC		= (int)mxGetN(close_IN);
				volPtr		= mxGetPr(vol_IN);
				colsV		= (int)mxGetN(vol_IN);

				// Input validation
				chkSingleVec(colsH, colsL, colsC, codeLine);

				if (colsV != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ad:InputErr",
						"Volume data should be a single vector array. Aborting (%d).",codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int adIdx, outElements;
				double *outReal;

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));			// added cast

				// Invoke with error catch
				retCode = TA_AD(startIdx, endIdx, highPtr, lowPtr, closePtr, volPtr, &adIdx, &outElements, outReal);
		
				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgTxt("Invocation to 'ta_ad' failed. Aborting.");
				}

				// Populate Output
				ad_OUT = mxCreateDoubleMatrix(adIdx + outElements,1, mxREAL);
				memcpy(((double*) mxGetData(ad_OUT)) + adIdx, outReal, outElements * mxGetElementSize(ad_OUT));
		
				// Cleanup
				mxFree(outReal);  
				break;
			}

		// Vector Arithmetic Add
		case ta_add:       
			{
				// REQUIRED INPUTS
				//		Augend
				//		Addend

				// OPTIONAL INPUTS
				//		none

				// OUTPUTS
				//		Sum

				// Check number of inputs
				if (nrhs != 3)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_add:NumInputs",
					"Number of input arguments to function 'ta_add' is not correct. Two vectors for summation should be provided. Aborting (%d).",codeLine);

				// Create constants for readability
				// Inputs
				#define augend_IN		prhs[1]
				#define added_IN		prhs[2]

				// Outputs
				#define sumTotal_OUT	plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsAug, colsAdd;
				double *augendPtr, *addendPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				augendPtr		= mxGetPr(augend_IN);
				rows			= (int)mxGetM(augend_IN);
				colsAug			= (int)mxGetN(augend_IN);
				addendPtr		= mxGetPr(added_IN);
				colsAdd			= (int)mxGetN(added_IN);

				if (colsAug != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:InputErr",
						"Vector addition inputs should be single dimensional vectors.\nThe augend vector had more than 1 column.  Aborting (%d).",codeLine);
				}

				if (colsAdd != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:InputErr",
						"Vector addition inputs should be single dimensional vectors.\nThe addend vector had more than 1 column.  Aborting (%d).",codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int addIdx, outElements;
				double *sumTotal;

				// Preallocate heap
				sumTotal = (double*)mxCalloc(rows, sizeof(double));

				retCode = TA_ADD(startIdx, endIdx, augendPtr, addendPtr, &addIdx, &outElements, sumTotal);

				// Error handling
				if (retCode) 
				{
					mxFree(sumTotal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				sumTotal_OUT = mxCreateDoubleMatrix(addIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(sumTotal_OUT)) + addIdx, sumTotal, outElements * mxGetElementSize(sumTotal_OUT));

				// Cleanup
				mxFree(sumTotal); 

				break;
			}
			
		// Chaikin A/D Oscillator
		case ta_adosc:
			{
				// REQUIRED INPUTS
				//		Price	H | L | C	separate vectors
				//		Volume
				
				// OPTIONAL INPUTS
				//		fastMA		Fast MA lookback	(default 3)
				//		slowMA		Slow MA lookback	(default 10)

				// OUTPUT
				//		ADOSC		vector of Chaikin advance / decline oscillator values

				// Check number of inputs
				if (nrhs < 5 || nrhs > 7)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_adosc:NumInputs",
					"Number of input arguments is not correct. Price data should be parsed into vectors H | L | C followed by a volume vector V.\nOptional inputs are fastMA | slowMA. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_adosc:NumOutputs",
					"The function 'ta_adosc' (Chaikin A/D Oscillator) produces a single vector output that must be assigned. Aborting (%d).",codeLine);

				// Create constants for readability
				// Inputs
				#define high_IN		prhs[1]
				#define low_IN		prhs[2]
				#define close_IN	prhs[3]
				#define vol_IN		prhs[4]

				// Outputs
				#define adosc_OUT	plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsH, colsL, colsC, colsV;
				double *highPtr, *lowPtr, *closePtr, *volPtr;
				int fastMA, slowMA;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				highPtr		= mxGetPr(high_IN);
				rows		= (int)mxGetM(high_IN);
				colsH		= (int)mxGetN(high_IN);
				lowPtr		= mxGetPr(low_IN);
				colsL		= (int)mxGetN(low_IN);
				closePtr	= mxGetPr(close_IN);
				colsC		= (int)mxGetN(close_IN);
				volPtr		= mxGetPr(vol_IN);
				colsV		= (int)mxGetN(vol_IN);

				// Input validation
				chkSingleVec(colsH, colsL, colsC, codeLine);

				if (colsV != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_adosc:InputErr",
						"Volume data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int adoscIdx, outElements;
				double *outReal;
				
				// Parse optional inputs if given, else default 
				if (nrhs == 7) 
				{
					#define fastMA_IN	prhs[5]
					#define slowMA_IN	prhs[6]

					if (!isRealScalar(fastMA_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The fastMA lookback must be a scalar. Aborting (%d).",codeLine);
					else if (!isRealScalar(slowMA_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The slowMA lookback must be a scalar. Aborting (%d).",codeLine);	

					/* Get the scalar inputs */
					// Assign
					fastMA = (int)mxGetScalar(fastMA_IN);
					slowMA = (int)mxGetScalar(slowMA_IN);

					if (fastMA > slowMA)
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The slowMA (%d) must not be less than the fastMA (%d). Aborting (%d).", slowMA, fastMA, codeLine);

				}
				else
				// Default lookback periods
				{
					fastMA = 3;
					slowMA = 10;
				}

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				retCode = TA_ADOSC(startIdx, endIdx, highPtr, lowPtr, closePtr, volPtr, fastMA, slowMA, &adoscIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgTxt("Invocation to 'ta_adosc' failed. Aborting (577).");
				}

				// Populate Output
				adosc_OUT = mxCreateDoubleMatrix(adoscIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(adosc_OUT)) + adoscIdx, outReal, outElements * mxGetElementSize(adosc_OUT));

				// Cleanup
				mxFree(outReal); 
				break;
			}

		// ADX
		case ta_adx:		// Average Directional Movement Index
		case ta_adxr:		// Average Directional Movement Index Rating
			{
				// REQUIRED INPUTS
				//		Price	H | L | C	separate vectors

				// OPTIONAL INPUTS
				//		Lookback period	(default 14)

				// OUTPUT
				//		adx		vector of Average Directional Movement Index

				// Check number of inputs
				if (nrhs < 4 || nrhs > 5)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_adx:NumInputs",
					"Number of input arguments to function 'ta_adx' is not correct. Price data should be parsed into vectors H | L | C. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_adx:NumOutputs",
					"The function 'ta_adx' (Average Directional Movement Index) produces a single vector output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define high_IN		prhs[1]
				#define low_IN		prhs[2]
				#define close_IN	prhs[3]

				// Outputs
				#define adx_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsH, colsL, colsC, lookback;
				double *highPtr, *lowPtr, *closePtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				highPtr		= mxGetPr(high_IN);
				rows		= (int)mxGetM(high_IN);
				colsH		= (int)mxGetN(high_IN);
				lowPtr		= mxGetPr(low_IN);
				colsL		= (int)mxGetN(low_IN);
				closePtr	= mxGetPr(close_IN);
				colsC		= (int)mxGetN(close_IN);

				// Validate
				chkSingleVec(colsH, colsL, colsC, codeLine);

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int adxIdx, outElements;
				double *outReal;

				// Parse optional inputs if given, else default 
				if (nrhs == 5) 
				{
					#define lookback_IN	prhs[4]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The ADX lookback must be a scalar. Aborting (652).");
					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);
				}
				else
				// Default lookback period
				{
					lookback = 14;
				}

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				if (taFuncNameIn.compare("ta_adx") == 0)
				{
					// Invoke with error catch
					retCode = TA_ADX(startIdx, endIdx, highPtr, lowPtr, closePtr, lookback, &adxIdx, &outElements, outReal);
				}
				else
				{
					// Invoke with error catch
					retCode = TA_ADXR(startIdx, endIdx, highPtr, lowPtr, closePtr, lookback, &adxIdx, &outElements, outReal);
				}
				

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				adx_OUT = mxCreateDoubleMatrix(adxIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(adx_OUT)) + adxIdx, outReal, outElements * mxGetElementSize(adx_OUT));

				// Cleanup
				mxFree(outReal); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *outPtr = mxGetPr(adx_OUT);

				for (int iter = 0; iter < lookback; iter++)
				{
					outPtr[iter] = m_Nan;
				}

				break;
			}

		// Absolute Price Oscillator
		case ta_apo:       
			{
				// REQUIRED INPUTS
				//		Price	any single column vector of price observations

				// OPTIONAL INPUTS
				//		fastMA		Fast MA look back	(default 12)
				//		slowMA		Slow MA look back	(default 26)
				//		typeMA		Moving average calculation type:
				//						0	-	Simple Moving Average				SMA	(default)
				//						1	-	Exponential Moving Average			EMA
				//						2	-	Weighted Moving Average				WMA
				//						3	-	Double Exponential Moving Average	DEMA
				//						4	-	Triple Exponential Moving Average	TEMA
				//						5	-	Triangular Moving Average			TRIMA
				//						6	-	Kaufman Adaptive Moving Average		KAMA
				//						7	-	MESA Adaptive Moving Average		MAMA
				//						8	-	Triple Exponential Moving Average	T3

				// OUTPUT
				//		APO			vector of Chaikin advance / decline line values

				// Check number of inputs
				if (nrhs < 2 || nrhs > 5)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_apo:NumInputs",
					"Number of input arguments to function 'ta_apo' is not correct. A single vector of price data should be provided.\nOptional scalar arguments are fastMA, slowMA, typeMA. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_apo:NumOutputs",
					"The function 'ta_apo' (Absolute Price Oscillator) produces a single vector output that must be assigned. Aborting (%d).", codeLine);
				
				// Create constants for readability
				// Inputs
				#define price_IN		prhs[1]

				// Outputs
				#define apo_OUT			plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsP;
				double *pricePtr;
				int fastMA, slowMA, typeMA;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse inputs and error check
				// Assign pointers and get dimensions
				pricePtr		= mxGetPr(price_IN);
				colsP		= (int)mxGetN(price_IN);
				rows		= (int)mxGetM(price_IN);

				if (colsP != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_apo:InputErr",
						"Price data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int apoIdx, outElements;
				double *outReal;

				// Parse optional inputs if given, else default 
				if (nrhs >= 4) 
				{
					#define fastMA_IN	prhs[2]
					#define slowMA_IN	prhs[3]

					if (!isRealScalar(fastMA_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The fastMA lookback must be a scalar. Aborting (%d).", codeLine);
					else if (!isRealScalar(slowMA_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The slowMA lookback must be a scalar. Aborting (%d).", codeLine);
					
					/* Get the scalar inputs */
					// Assign
					fastMA = (int)mxGetScalar(fastMA_IN);
					slowMA = (int)mxGetScalar(slowMA_IN);

					if (fastMA > slowMA)
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The slowMA (%d) must not be less than the fastMA (%d). Aborting (%d).", slowMA, fastMA, codeLine);


					/* Get the scalar inputs */
					// Assign
					fastMA = (int)mxGetScalar(fastMA_IN);
					slowMA = (int)mxGetScalar(slowMA_IN);

					if (nrhs == 5)
					{
						#define typeMA_IN	prhs[4]

						if (!isRealScalar(typeMA_IN))
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The typeMA must be a scalar value of available types. Aborting (%d).", codeLine);

						typeMA = (int)mxGetScalar(typeMA_IN);

						if (typeMA < 0 || typeMA > 8)
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The typeMA must be a scalar value of available types (0 - 8). Aborting (%d).", codeLine);
					}
					else
					// No typeMA provided
					{
						typeMA = 0;
					}
				}
				else if (nrhs == 3)
				{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"Ambiguous inputs. Aborting (%d).", codeLine);
				}
				else
				// No optional inputs.  Supply defaults
				{
					fastMA = 12;
					slowMA = 26;
					typeMA = 0;
				}

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));			// added cast

				// Invoke with error catch
				// May have to change typeMA from decimal to name
				retCode = TA_APO(startIdx, endIdx, pricePtr, fastMA, slowMA, (TA_MAType)typeMA, &apoIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgTxt("Invocation to 'ta_apo' failed. Aborting (843).");
				}

				// Populate Output
				apo_OUT = mxCreateDoubleMatrix(apoIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(apo_OUT)) + apoIdx, outReal, outElements * mxGetElementSize(apo_OUT));

				// Cleanup
				mxFree(outReal); 
				break;
			}

		// Aroon
		case ta_aroon:       
			{
				// REQUIRED INPUTS
				//		Price	H | L	separate vectors

				// OPTIONAL INPUTS
				//		Lookback period	(default 14)

				// OUTPUT
				//		aroonUp		vector of Aroon Up indicator values
				//		aroonDn		vector of Aroon Down indicator values

				// Check number of inputs
				if (nrhs < 3 || nrhs > 4)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_aroon:NumInputs",
					"Number of input arguments to function 'ta_aroon' is not correct. Price data should be parsed into separate vectors H | L. Aborting (%d).", codeLine);
				if (nlhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_aroon:NumOutputs",
					"The function 'ta_aroon' (Aroon Indicator) produces 2 vectors as output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define high_IN			prhs[1]
				#define low_IN			prhs[2]

				// Outputs
				#define aroonUp_OUT		plhs[0]
				#define aroonDn_OUT		plhs[1]

				// Declare variables
				int startIdx, endIdx, rows, colsH, colsL, lookback;
				double *highPtr, *lowPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				highPtr		= mxGetPr(high_IN);
				rows		= (int)mxGetM(high_IN);
				colsH		= (int)mxGetN(high_IN);
				lowPtr		= mxGetPr(low_IN);
				colsL		= (int)mxGetN(low_IN);

				// Validate
				chkSingleVec(colsH, colsL, codeLine);

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int aroonIdx, outElements;
				double *aroonUp, *aroonDn;

				// Get optional input or assign default
				if (nrhs == 4) 
				{
					#define lookback_IN	prhs[3]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The AROON lookback must be a scalar. Aborting (%d).", codeLine);
					
					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);
				}
				else
				// Default lookback period
				{
					lookback = 14;
				}

				// Preallocate heap
				aroonUp = (double*)mxCalloc(rows, sizeof(double));
				aroonDn = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				retCode = TA_AROON(startIdx, endIdx, highPtr, lowPtr, lookback, &aroonIdx, &outElements, aroonDn, aroonUp);

				// Error handling
				if (retCode) 
				{
					mxFree(aroonUp);
					mxFree(aroonDn);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				aroonDn_OUT = mxCreateDoubleMatrix(aroonIdx + outElements,1, mxREAL);
				aroonUp_OUT = mxCreateDoubleMatrix(aroonIdx + outElements,1, mxREAL);
				
				memcpy(((double *) mxGetData(aroonDn_OUT)) + aroonIdx, aroonDn, outElements * mxGetElementSize(aroonDn_OUT));
				memcpy(((double *) mxGetData(aroonUp_OUT)) + aroonIdx, aroonUp, outElements * mxGetElementSize(aroonUp_OUT));

				// Cleanup
				mxFree(aroonDn); 
				mxFree(aroonUp);

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *upPtr = mxGetPr(aroonUp_OUT);
				double *dnPtr = mxGetPr(aroonDn_OUT);

				for (int iter = 0; iter < lookback; iter++)
				{
					upPtr[iter] = m_Nan;
					dnPtr[iter] = m_Nan;
				}

				break;
			}
			
		// Aroon Oscillator
		case ta_aroonosc:       
			{
				// REQUIRED INPUTS
				//		Price	H | L	separate vectors

				// OPTIONAL INPUTS
				//		Lookback period	(default 14)

				// OUTPUT
				//		aroonOsc	vector of Aroon Oscillator values

				// Check number of inputs
				if (nrhs < 3 || nrhs > 4)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_aroonosc:NumInputs",
					"Number of input arguments to function 'ta_aroon' is not correct. Price data should be parsed into separate vectors H | L. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_aroonosc:NumOutputs",
					"The function 'ta_aroonosc' (Aroon Oscillator) produces a single vector as output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define high_IN			prhs[1]
				#define low_IN			prhs[2]

				// Outputs
				#define aroonOsc_OUT	plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsH, colsL, lookback;
				double *highPtr, *lowPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				highPtr		= mxGetPr(high_IN);
				rows		= (int)mxGetM(high_IN);
				colsH		= (int)mxGetN(high_IN);
				lowPtr		= mxGetPr(low_IN);
				colsL		= (int)mxGetN(low_IN);

				// Validate
				chkSingleVec(colsH, colsL, codeLine);

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int aroonoscIdx, outElements;
				double *aroonOsc;

				// Get optional input or assign default
				if (nrhs == 4) 
				{
					#define lookback_IN	prhs[3]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The AROONOSC lookback must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);
				}
				else
					// Default lookback period
				{
					lookback = 14;
				}

				// Preallocate heap
				aroonOsc = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				retCode = TA_AROONOSC(startIdx, endIdx, highPtr, lowPtr, lookback, &aroonoscIdx, &outElements, aroonOsc);

				// Error handling
				if (retCode) 
				{
					mxFree(aroonOsc);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (749).", taFuncNameIn);
				}

				// Populate Output
				aroonOsc_OUT = mxCreateDoubleMatrix(aroonoscIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(aroonOsc_OUT)) + aroonoscIdx, aroonOsc, outElements * mxGetElementSize(aroonOsc_OUT));

				// Cleanup
				mxFree(aroonOsc); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *outPtr = mxGetPr(aroonOsc_OUT);

				for (int iter = 0; iter < lookback; iter++)
				{
					outPtr[iter] = m_Nan;
				}

				break;
			}

		// Vector Trigonometric ASin
		case ta_asin:       
			{
				// REQUIRED INPUTS
				//		input	any single column vector of sine values (-1 to 1)
				//				

				// OPTIONAL INPUTS
				//		none

				// OUTPUTS
				//		ASIN	Inverse sine of input (radians)
				//				e.g. asin(1) = pi/2 (~1.57) radians = 90 degrees

				// Check number of inputs
				if (nrhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_asin:NumInputs",
					"Number of input arguments to function 'ta_asin' is not correct. A single vector of values (-1 =< x =< 1) should be provided. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_acos:NumOutputs",
					"The function 'ta_asin' (Vector Trigonometric ASin) produces a single vector output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define sin_IN			prhs[1]

				// Outputs
				#define asin_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsSin;
				double *sinPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse inputs and error check
				// Assign pointers and get dimensions
				sinPtr		= mxGetPr(sin_IN);
				colsSin		= (int)mxGetN(sin_IN);
				rows		= (int)mxGetM(sin_IN);

				if (colsSin != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_asin:InputErr",
						"Sine data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int asinIdx, outElements;
				double *outReal;

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				// May have to change typeMA from decimal to name
				retCode = TA_ASIN(startIdx, endIdx, sinPtr, &asinIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgTxt("Invocation to 'ta_acos' failed. Aborting (1140).");
				}

				// Populate Output
				asin_OUT = mxCreateDoubleMatrix(asinIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(asin_OUT)) + asinIdx, outReal, outElements * mxGetElementSize(asin_OUT));

				// Cleanup
				mxFree(outReal);

				break;
			}
			
		// Vector Trigonometric ATan
		case ta_atan: 
			{
				// REQUIRED INPUTS
				//		input	any single column vector of tan values (all Reals)

				// OPTIONAL INPUTS
				//		none

				// OUTPUTS
				//		ATAN	Inverse tangent of input (radians)
				//				e.g. atan(0) = 0 radians = 0 degrees

				// Check number of inputs
				if (nrhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_atan:NumInputs",
					"Number of input arguments to function 'ta_atan' is not correct. A single vector of Real values should be provided. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_acos:NumOutputs",
					"The function 'ta_atan' (Vector Trigonometric ATan) produces a single vector output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define tan_IN			prhs[1]

				// Outputs
				#define atan_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsTan;
				double *tanPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse inputs and error check
				// Assign pointers and get dimensions
				tanPtr		= mxGetPr(tan_IN);
				colsTan		= (int)mxGetN(tan_IN);
				rows		= (int)mxGetM(tan_IN);

				if (colsTan != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_atan:InputErr",
						"Tangent data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int atanIdx, outElements;
				double *outReal;

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				// May have to change typeMA from decimal to name
				retCode = TA_ATAN(startIdx, endIdx, tanPtr, &atanIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgTxt("Invocation to 'ta_atan' failed. Aborting (1219).");
				}

				// Populate Output
				atan_OUT = mxCreateDoubleMatrix(atanIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(atan_OUT)) + atanIdx, outReal, outElements * mxGetElementSize(atan_OUT));

				// Cleanup
				mxFree(outReal); 

				break;
			}

		// Average True Range (Uses TRANGE for smoothing.)
		case ta_atr:       
			{
				// REQUIRED INPUTS
				//		Price	H | L | C	separate vectors

				// OPTIONAL INPUTS
				//		Lookback period		(default 14)

				// OUTPUT
				//		ATR					vector of average true range values

				// Check number of inputs
				if (nrhs < 4 || nrhs > 5)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_atr:NumInputs",
					"Number of input arguments to function 'ta_atr' is not correct. Price data should be parsed into vectors H | L | C.\nAn optional lookback scalar may also be provided. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_atr:NumOutputs",
					"The function 'ta_atr' (Average True Range) produces a single vector output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define high_IN		prhs[1]
				#define low_IN		prhs[2]
				#define close_IN	prhs[3]

				// Outputs
				#define atr_OUT	plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsH, colsL, colsC, lookback;
				double *highPtr, *lowPtr, *closePtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				highPtr		= mxGetPr(high_IN);
				rows		= (int)mxGetM(high_IN);
				colsH		= (int)mxGetN(high_IN);
				lowPtr		= mxGetPr(low_IN);
				colsL		= (int)mxGetN(low_IN);
				closePtr	= mxGetPr(close_IN);
				colsC		= (int)mxGetN(close_IN);

				// Validate
				chkSingleVec(colsH, colsL, colsC, codeLine);

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int atrIdx, outElements;
				double *outReal;

				// Parse optional inputs if given, else default 
				if (nrhs == 5) 
				{
					#define lookback_IN	prhs[4]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The ATR lookback must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);
				}
				else
				// Default lookback period
				{
					lookback = 14;
				}

				// Preallocate heap
				outReal	= (double*)mxCalloc(rows, sizeof(double));

				retCode = TA_ATR(startIdx, endIdx, highPtr, lowPtr, closePtr, lookback, &atrIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				atr_OUT = mxCreateDoubleMatrix(atrIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(atr_OUT)) + atrIdx, outReal, outElements * mxGetElementSize(atr_OUT));

				// Cleanup
				mxFree(outReal);

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *outPtr = mxGetPr(atr_OUT);

				for (int iter = 0; iter < lookback; iter++)
				{
					outPtr[iter] = m_Nan;
				}

				break;
			}

		// Average Deviation
		case ta_avgdev:       
			{
				// REQUIRED INPUTS
				//		Data				a vector of observation data

				// OPTIONAL INPUTS
				//		Lookback period		(default 14)

				// OUTPUT
				//		AVGDEV				vector of average deviation values

				// Check number of inputs
				if (nrhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_avgdev:NumInputs",
					"Number of input arguments to function 'ta_avgdev' is not correct. Observational data should be a single vector.\n An optional lookback scalar may also be provided. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_avgdev:NumOutputs",
					"The function 'ta_avgdev' (Average Deviation) produces a single vector output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN		prhs[1]

				// Outputs
				#define avgdev_OUT	plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsD, lookback;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				rows		= (int)mxGetM(data_IN);
				colsD		= (int)mxGetN(data_IN);

				// Validate
				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_avgdev:InputErr",
						"Observational data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int avgdevIdx, outElements;
				double *outReal;

				// Parse optional inputs if given, else default 
				if (nrhs == 3) 
				{
					#define lookback_IN	prhs[2]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The ATR lookback must be a scalar. Aborting (%d).", codeLine);
					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);
				}
				else
					// Default lookback period
				{
					lookback = 14;
				}

				// Preallocate heap
				outReal	= (double*)mxCalloc(rows, sizeof(double));

				retCode = TA_AVGDEV(startIdx, endIdx, dataPtr, lookback, &avgdevIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (1359).", taFuncNameIn);
				}

				// Populate Output
				avgdev_OUT = mxCreateDoubleMatrix(avgdevIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(avgdev_OUT)) + avgdevIdx, outReal, outElements * mxGetElementSize(avgdev_OUT));

				// Cleanup
				mxFree(outReal);

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *outPtr = mxGetPr(avgdev_OUT);

				for (int iter = 0; iter < lookback; iter++)
				{
					outPtr[iter] = m_Nan;
				}

				break;
			}

			// Average Price (O + H + L + C / 4)
		case ta_avgprice:       
			{
				// REQUIRED INPUTS
				//		Price	O | H | L | C	separate vectors

				// OPTIONAL INPUTS
				//		none

				// OUTPUTS
				//		AVGPRICE				vector of the Averages of O H L C  per observation

				// Check number of inputs
				if (nrhs != 5)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_avgprice:NumInputs",
					"Number of input arguments to function 'ta_avgprice' is not correct. Price data should be parsed into vectors O | H | L | C. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_avgprice:NumOutputs",
					"The function 'ta_avgprice' (Average Price) produces a single vector output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define open_IN			prhs[1]
				#define high_IN			prhs[2]
				#define low_IN			prhs[3]
				#define close_IN		prhs[4]

				// Outputs
				#define avgPrice_OUT	plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsO, colsH, colsL, colsC, lookback;
				double *openPtr, *highPtr, *lowPtr, *closePtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				openPtr		= mxGetPr(open_IN);
				colsO		= (int)mxGetN(open_IN);
				rows		= (int)mxGetM(open_IN);
				highPtr		= mxGetPr(high_IN);
				colsH		= (int)mxGetN(high_IN);
				lowPtr		= mxGetPr(low_IN);
				colsL		= (int)mxGetN(low_IN);
				closePtr	= mxGetPr(close_IN);
				colsC		= (int)mxGetN(close_IN);

				// Validate
				chkSingleVec(colsO, colsH, colsL, colsC, codeLine);

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int avgpriceIdx, outElements;
				double *outReal;

				// Preallocate heap
				outReal	= (double*)mxCalloc(rows, sizeof(double));

				retCode = TA_AVGPRICE(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &avgpriceIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (1442).", taFuncNameIn);
				}

				// Populate Output
				avgPrice_OUT = mxCreateDoubleMatrix(avgpriceIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(avgPrice_OUT)) + avgpriceIdx, outReal, outElements * mxGetElementSize(avgPrice_OUT));

				// Cleanup
				mxFree(outReal); 

				break;
			}

		// Bollinger Bands
		case ta_bbands:       
			{
				// REQUIRED INPUTS
				//		Data		a single vector of observational data

				// OPTIONAL INPUTS
				//		Lookback	lookback period (default 5)
				//		DevUp		multiplier for upper band (default 2)
				//		DevDn		multiplier for lower band (default 2)
				//		typeMA		Moving average calculation type:
				//						0	-	Simple Moving Average				SMA	(default)
				//						1	-	Exponential Moving Average			EMA
				//						2	-	Weighted Moving Average				WMA
				//						3	-	Double Exponential Moving Average	DEMA
				//						4	-	Triple Exponential Moving Average	TEMA
				//						5	-	Triangular Moving Average			TRIMA
				//						6	-	Kaufman Adaptive Moving Average		KAMA
				//						7	-	MESA Adaptive Moving Average		MAMA
				//						8	-	Triple Exponential Moving Average	T3	

				// OUTPUTS
				//		upperBand		
				//		midBand
				//		lowerBand

				// Check number of inputs
				if (nrhs < 2 || nrhs > 6)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_bbands:NumInputs",
					"Number of input arguments to function 'ta_bbands' is incorrect. Observational data should be a single vector.\nOptional inputs are: Lookback period, Upper STD Multiplier, Lower STD Multiplier. Aborting (%d).", codeLine);
				if (nrhs == 4)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_bbands:NumInputs",
					"Number of input arguments to function 'ta_bbands' is ambiguous.\nWhen providing optional inputs, you may provide just the 'Lookback period, or 'Lookback period, Upper STD Multiplier, Lower STD Multiplier'. Aborting (%d).", codeLine);
				if (nlhs != 3)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_bbands:NumOutputs",
					"The function 'ta_bbands' (Bollinger Bands) produces 3 vector outputs that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define bbUpper_OUT		plhs[0]
				#define bbMid_OUT		plhs[1]
				#define bbLower_OUT		plhs[2]

				// Declare variables
				int startIdx, endIdx, rows, colsD, lookback;
				double *dataPtr, upMult, dnMult;
				int typeMA;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				rows		= (int)mxGetM(data_IN);
				colsD		= (int)mxGetN(data_IN);

				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_bbands:InputErr",
						"Observational data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int bbandsIdx, outElements;
				double *bbUpper, *bbMid, *bbLower;

				// Parse optional inputs if given, else default 
				if (nrhs > 2) 
				{
					#define lookback_IN	prhs[2]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The BBANDS lookback must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);

					// Check for integer to protect against STD multiples
					if (fraction(lookback))
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"A fraction was detected in the BBANDS lookback input.  This may have been intended to be an STD band multiplier? Aborting (%d).", codeLine);
					}

					if (nrhs == 3)
					{
						upMult = 2;
						dnMult = 2;
						typeMA = 0;
					}
					else
					{
						#define upMult_IN	prhs[3]
						#define dnMult_IN	prhs[4]

						// Validate
						if (!isRealScalar(upMult_IN))
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The BBANDS STD upper multiplier must be a scalar. Aborting (%d).", codeLine);
						if (!isRealScalar(dnMult_IN))
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The BBANDS STD lower multiplier must be a scalar. Aborting (%d).", codeLine);

						/* Get the scalar input multipliers */
						// Assign
						upMult = (double)mxGetScalar(upMult_IN);
						dnMult = (double)mxGetScalar(dnMult_IN);

						if (nrhs == 5)
						{
							typeMA = 0;
						}
						else
						{
							#define typeMA_IN	prhs[5]

							// Validate
							if (!isRealScalar(typeMA_IN))
								mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
								"The BBANDS typeMA must be a scalar. Aborting (%d).", codeLine);

							typeMA = (int)mxGetScalar(typeMA_IN);
						}
					}
				}
				// No optional inputs given.  Default all values
				else
				{
					lookback = 5;
					upMult = 2;
					dnMult = 2;
					typeMA = 0;
				}

				// Preallocate heap
				bbUpper	= (double*)mxCalloc(rows, sizeof(double));
				bbMid	= (double*)mxCalloc(rows, sizeof(double));
				bbLower	= (double*)mxCalloc(rows, sizeof(double));

				retCode = TA_BBANDS(startIdx, endIdx, dataPtr, lookback, upMult,dnMult, (TA_MAType)typeMA, &bbandsIdx, &outElements, bbUpper, bbMid, bbLower);

				// Error handling
				if (retCode) 
				{
					mxFree(bbUpper);
					mxFree(bbMid);
					mxFree(bbLower);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgTxt("Invocation to 'ta_bbands' failed. Aborting (1604).");
				}

				// Populate Output
				bbUpper_OUT = mxCreateDoubleMatrix(bbandsIdx + outElements,1, mxREAL);
				bbMid_OUT = mxCreateDoubleMatrix(bbandsIdx + outElements,1, mxREAL);
				bbLower_OUT = mxCreateDoubleMatrix(bbandsIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(bbUpper_OUT)) + bbandsIdx, bbUpper, outElements * mxGetElementSize(bbUpper_OUT));
				memcpy(((double *) mxGetData(bbMid_OUT)) + bbandsIdx, bbMid, outElements * mxGetElementSize(bbMid_OUT));
				memcpy(((double *) mxGetData(bbLower_OUT)) + bbandsIdx, bbLower, outElements * mxGetElementSize(bbLower_OUT));

				// Cleanup
				mxFree(bbUpper); 
				mxFree(bbMid); 
				mxFree(bbLower); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *uBandPtr = mxGetPr(bbUpper_OUT);
				double *mBandPtr = mxGetPr(bbMid_OUT);
				double *lBandPtr = mxGetPr(bbLower_OUT);

				for (int iter = 0; iter < lookback; iter++)
				{
					uBandPtr[iter] = m_Nan;
					mBandPtr[iter] = m_Nan;
					lBandPtr[iter] = m_Nan;
				}

				break;
			}

		// Beta - correlated volatility (http://en.wikipedia.org/wiki/Beta_coefficient) 
		case ta_beta:       
			{
				// REQUIRED INPUTS
				//		Data		a single vector of observational data for a stock or future
				//		Base		the base to create the comparison (e.g. S&P 500)

				// OPTIONAL INPUTS
				//		Lookback period		(default 5)

				// OUTPUT
				//		BETA				vector of correlated volatility measurements

				// Check number of inputs
				if (nrhs < 3 || nrhs > 4)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_beta:NumInputs",
					"Number of input arguments to function 'ta_beta' is not correct. Price data should be parsed into 2 vectors Individual Instrument | Index.\nAn optional lookback scalar may also be provided. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_beta:NumOutputs",
					"The function 'ta_beta' (Beta) produces a single vector output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define ind_IN		prhs[1]
				#define base_IN		prhs[2]

				// Outputs
				#define beta_OUT	plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsI, colsB, lookback;
				double *indPtr, *basePtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				indPtr		= mxGetPr(ind_IN);
				rows		= (int)mxGetM(ind_IN);
				colsI		= (int)mxGetN(ind_IN);
				basePtr		= mxGetPr(base_IN);
				colsB		= (int)mxGetN(ind_IN);

				if (colsI != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_beta:InputErr",
						"The individual instrument data should be a single vector array. Aborting (%d).", codeLine);
				}

				if (colsB != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_beta:InputErr",
						"The base instrument data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int betaIdx, outElements;
				double *outReal;

				// Parse optional inputs if given, else default 
				if (nrhs == 4) 
				{
				#define lookback_IN	prhs[3]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The BETA lookback must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);
				}
				else
					// Default lookback period
				{
					lookback = 5;
				}

				// Preallocate heap
				outReal	= (double*)mxCalloc(rows, sizeof(double));

				retCode = TA_BETA(startIdx, endIdx, indPtr, basePtr, lookback, &betaIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				beta_OUT = mxCreateDoubleMatrix(betaIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(beta_OUT)) + betaIdx, outReal, outElements * mxGetElementSize(beta_OUT));

				// Cleanup
				mxFree(outReal);

				break;
			}

		// Balance Of Power
		case ta_bop:       
			{
				// REQUIRED INPUTS
				//		Price			O | H | L | C	separate vectors

				// OPTIONAL INPUTS
				//		none

				// OUTPUTS
				//		BOP				vector of the Balance of Power calculations

				// Check number of inputs
				if (nrhs != 5)
					mexErrMsgIdAndTxt("MATLAB:taInvoke:ta_bop:NumInputs",
					"Number of input arguments to function 'ta_bop' is not correct. Price data should be parsed into vectors O | H | L | C. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt("MATLAB:taInvoke:ta_bop:NumOutputs",
					"The function 'ta_bop' (Balance of Power) produces a single vector output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define open_IN			prhs[1]
				#define high_IN			prhs[2]
				#define low_IN			prhs[3]
				#define close_IN		prhs[4]

				// Outputs
				#define bop_OUT			plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsO, colsH, colsL, colsC, lookback;
				double *openPtr, *highPtr, *lowPtr, *closePtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				openPtr		= mxGetPr(open_IN);
				colsO		= (int)mxGetN(open_IN);
				rows		= (int)mxGetM(open_IN);
				highPtr		= mxGetPr(high_IN);
				colsH		= (int)mxGetN(high_IN);
				lowPtr		= mxGetPr(low_IN);
				colsL		= (int)mxGetN(low_IN);
				closePtr	= mxGetPr(close_IN);
				colsC		= (int)mxGetN(close_IN);

				// Validate
				chkSingleVec(colsO, colsH, colsL, colsC, codeLine);

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int bopIdx, outElements;
				double *outReal;

				// Preallocate heap
				outReal	= (double*)mxCalloc(rows, sizeof(double));

				retCode = TA_BOP(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &bopIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				bop_OUT = mxCreateDoubleMatrix(bopIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(bop_OUT)) + bopIdx, outReal, outElements * mxGetElementSize(bop_OUT));

				// Cleanup
				mxFree(outReal); 

				break;
			}

		// Commodity Channel Index
		case ta_cci:       
			{
				// REQUIRED INPUTS
				//		Price	H | L | C	separate vectors

				// OPTIONAL INPUTS
				//		Lookback period		(default 14)

				// OUTPUT
				//		CCI					vector of average true range values

				// Check number of inputs
				if (nrhs < 4 || nrhs > 5)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_cci:NumInputs",
					"Number of input arguments to function 'ta_cci' is not correct. Price data should be parsed into vectors H | L | C.\nAn optional lookback scalar may also be provided. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_cci:NumOutputs",
					"The function 'ta_cci' (Commodity Channel Index) produces a single vector output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define high_IN		prhs[1]
				#define low_IN		prhs[2]
				#define close_IN	prhs[3]

				// Outputs
				#define cci_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsH, colsL, colsC, lookback;
				double *highPtr, *lowPtr, *closePtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				highPtr		= mxGetPr(high_IN);
				rows		= (int)mxGetM(high_IN);
				colsH		= (int)mxGetN(high_IN);
				lowPtr		= mxGetPr(low_IN);
				colsL		= (int)mxGetN(low_IN);
				closePtr	= mxGetPr(close_IN);
				colsC		= (int)mxGetN(close_IN);

				// Validate
				chkSingleVec(colsH, colsL, colsC, codeLine);

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int cciIdx, outElements;
				double *outReal;

				// Parse optional inputs if given, else default 
				if (nrhs == 5) 
				{
					#define lookback_IN	prhs[4]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The CCI lookback must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);
				}
				else
					// Default lookback period
				{
					lookback = 14;
				}

				// Preallocate heap
				outReal	= (double*)mxCalloc(rows, sizeof(double));

				retCode = TA_CCI(startIdx, endIdx, highPtr, lowPtr, closePtr, lookback, &cciIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				cci_OUT = mxCreateDoubleMatrix(cciIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(cci_OUT)) + cciIdx, outReal, outElements * mxGetElementSize(cci_OUT));

				// Cleanup
				mxFree(outReal);

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *outPtr = mxGetPr(cci_OUT);

				for (int iter = 0; iter < lookback; iter++)
				{
					outPtr[iter] = m_Nan;
				}

				break;
			}

		// CANDLESTICK DETECTIONS
		case ta_cdl2crows:								// Two Crows
		case ta_cdl3blackcrows:							// Three Black Crows
		case ta_cdl3inside:								// Three Inside Up/Down
		case ta_cdl3linestrike:							// Three-Line Strike
		case ta_cdl3outside:							// Three Outside Up/Down
		case ta_cdl3starsinsouth:						// Three Stars In The South
		case ta_cdl3whitesoldiers:						// Three Advancing White Soldiers
		case ta_cdlabandonedbaby:						// Abandoned Baby
		case ta_cdladvanceblock:						// Advance Block
		case ta_cdlbelthold:							// Belt-hold
		case ta_cdlbreakaway:							// Breakaway 
		case ta_cdlclosingmarubozu:						// Closing Marubozu
		case ta_cdlconcealbabyswall:					// Concealing Baby Swallow
		case ta_cdlcounterattack:						// Counterattack
		case ta_cdldarkcloudcover:						// Dark Cloud Cover
		case ta_cdldoji:								// Doji
		case ta_cdldojistar:							// Doji Star
		case ta_cdldragonflydoji:						// Dragonfly Doji
		case ta_cdlengulfing:							// Engulfing Pattern
		case ta_cdleveningdojistar:						// Evening Doji Star
		case ta_cdleveningstar:							// Evening Star
		case ta_cdlgapsidesidewhite:					// Up | Down-gap side-by-side white lines
		case ta_cdlgravestonedoji:						// Gravestone Doji
		case ta_cdlhammer:								// Hammer 
		case ta_cdlhangingman:							// Hanging Man
		case ta_cdlharami:								// Harami Pattern
		case ta_cdlharamicross:							// Harami Cross Pattern
		case ta_cdlhighwave:							// High-Wave Candle
		case ta_cdlhikkake:								// Hikkake Pattern
		case ta_cdlhikkakemod:							// Modified Hikkake Pattern 
		case ta_cdlhomingpigeon:						// Homing Pigeon
		case ta_cdlidentical3crows:						// Identical Three Crows
		case ta_cdlinneck:								// In-Neck Pattern
		case ta_cdlinvertedhammer:						// Inverted Hammer
		case ta_cdlkicking:								// Kicking
		case ta_cdlkickingbylength:						// Kicking - bull/bear determined by the longer Marubozu
		case ta_cdlladderbottom:						// Ladder Bottom
		case ta_cdllongleggeddoji:						// Long Legged Doji
		case ta_cdllongline:							// Long Line Candle
		case ta_cdlmarubozu:							// Marubozu
		case ta_cdlmatchinglow:							// Matching Low
		case ta_cdlmathold:								// Mat Hold
		case ta_cdlmorningdojistar:						// Morning Doji Star
		case ta_cdlmorningstar:							// Morning Star
		case ta_cdlonneck:								// On-Neck Pattern
		case ta_cdlpiercing:							// Piercing Pattern
		case ta_cdlrickshawman:							// Rickshaw Man
		case ta_cdlrisefall3methods:					// Rising/Falling Three Methods
		case ta_cdlseparatinglines:						// Separating Lines
		case ta_cdlshootingstar:						// Shooting Star
		case ta_cdlshortline:							// Short Line Candle
		case ta_cdlspinningtop:							// Spinning Top
		case ta_cdlstalledpattern:						// Stalled Pattern  
		case ta_cdlsticksandwich:						// Stick Sandwich
		case ta_cdltakuri:								// Takuri (Dragonfly Doji with very long lower shadow)
		case ta_cdltasukigap:							// Tasuki Gap
		case ta_cdlthrusting:							// Thrusting Pattern
		case ta_cdltristar:								// Tristar Pattern
		case ta_cdlunique3river:						// Unique 3 River
		case ta_cdlupsidegap2crows:						// Upside Gap Two Crows
		case ta_cdlxsidegap3methods:					// Upside/Downside Gap Three Methods
			{
				// REQUIRED INPUTS
				//		Price	O | H | L | C	separate vectors

				// OPTIONAL INPUT
				//		CDLABANDONEDBABY
				//		CDLDARKCLOUDCOVER
				//		CDLEVENINGDOJISTAR
				//			pctPen				Percentage of penetration of a candle within another candle (decimal input)

				// OUTPUTS
				//		CDL2CROWS				vector of the instances identified as Two Crows Candlestick formation

				// Check number of inputs
				if (nrhs > 6)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:Candlestick:NumInputs",
					"The number of input arguments to the called candlestick function is not correct. Price data should be parsed into vectors O | H | L | C. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:Candlestick:NumOutputs",
					"The called candlestick function produces a single vector output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define open_IN			prhs[1]
				#define data_IN			prhs[2]
				#define low_IN			prhs[3]
				#define close_IN		prhs[4]

				// Outputs
				#define cdl_OUT			plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsO, colsH, colsL, colsC, lookback;
				double *openPtr, *highPtr, *lowPtr, *closePtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				openPtr		= mxGetPr(open_IN);
				colsO		= (int)mxGetN(open_IN);
				rows		= (int)mxGetM(open_IN);
				highPtr		= mxGetPr(high_IN);
				colsH		= (int)mxGetN(high_IN);
				lowPtr		= mxGetPr(low_IN);
				colsL		= (int)mxGetN(low_IN);
				closePtr	= mxGetPr(close_IN);
				colsC		= (int)mxGetN(close_IN);

				// Validate
				chkSingleVec(colsO, colsH, colsL, colsC, codeLine);

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int cdlIdx, outElements;
				int *outInt;

				// Preallocate heap
				outInt	= (int*)mxCalloc(rows, sizeof(int));

				// Candlestick Pattern Switch
				switch (s_mapStringValues[taFuncNameIn])
				{
					case ta_cdl2crows:
						{
							retCode = TA_CDL2CROWS(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdl3blackcrows:   
						{
							retCode = TA_CDL3BLACKCROWS(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdl3inside:  
						{
							retCode = TA_CDL3INSIDE(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdl3linestrike: 
						{
							retCode = TA_CDL3LINESTRIKE(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdl3outside:
						{
							retCode = TA_CDL3OUTSIDE(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdl3starsinsouth:  
						{
							retCode = TA_CDL3STARSINSOUTH(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdl3whitesoldiers:   
						{
							retCode = TA_CDL3WHITESOLDIERS(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}			
					case ta_cdlabandonedbaby:
					case ta_cdldarkcloudcover:
					case ta_cdleveningdojistar:
					case ta_cdleveningstar:
					case ta_cdlmathold:
					case ta_cdlmorningdojistar:
					case ta_cdlmorningstar:
						{
							double pctPen;
							// Parse optional inputs if given, else default 
							if (nrhs == 6) 
							{
								#define pctPen_IN	prhs[5]
								if (!isRealScalar(pctPen_IN))
									mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
									"The CCI lookback must be a scalar. Aborting (2132).");

								/* Get the scalar input lookback */
								// Assign
								pctPen = (double)mxGetScalar(pctPen_IN);

								// Validate
								if (pctPen < 0 || pctPen > 1)
								{
									mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
										"The penetration percent for CANDLEAVGPERIOD is not in a decimal format. Aborting (2143).");
								}
							}
							else
								// Default penetration percentage period
							{
								pctPen = .3;
							}

							switch (s_mapStringValues[taFuncNameIn])
							{
								case ta_cdlabandonedbaby:
									{
										retCode = TA_CDLABANDONEDBABY(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, pctPen, &cdlIdx, &outElements, outInt);
										break;
									}

								case ta_cdldarkcloudcover:
									{
										retCode = TA_CDLDARKCLOUDCOVER(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, pctPen, &cdlIdx, &outElements, outInt);
										break;
									}
								case ta_cdleveningdojistar:  
									{
										retCode = TA_CDLEVENINGDOJISTAR(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, pctPen, &cdlIdx, &outElements, outInt);
										break;
									}
								case ta_cdleveningstar:  
									{
										retCode = TA_CDLEVENINGSTAR(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, pctPen, &cdlIdx, &outElements, outInt);
										break;
									}
								case ta_cdlmathold:  
									{
										retCode = TA_CDLMATHOLD(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr,  pctPen, &cdlIdx, &outElements, outInt);
										break;
									}
								case ta_cdlmorningdojistar:  
									{
										retCode = TA_CDLMORNINGDOJISTAR(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, pctPen, &cdlIdx, &outElements, outInt);
										break;
									}
								case ta_cdlmorningstar:  
									{
										retCode = TA_CDLMORNINGSTAR(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, pctPen, &cdlIdx, &outElements, outInt);
										break;
									}
							}

							break;
						}
					case ta_cdladvanceblock:  
						{
							retCode = TA_CDLADVANCEBLOCK(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlbelthold:  
						{
							retCode = TA_CDLBELTHOLD(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlbreakaway:  
						{
							retCode = TA_CDLBREAKAWAY(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlclosingmarubozu:  
						{
							retCode = TA_CDLCLOSINGMARUBOZU(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlconcealbabyswall:  
						{
							retCode = TA_CDLCONCEALBABYSWALL(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlcounterattack:  
						{
							retCode = TA_CDLCOUNTERATTACK(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdldoji:  
						{
							retCode = TA_CDLDOJI(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdldojistar:  
						{
							retCode = TA_CDLDOJISTAR(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdldragonflydoji:  
						{
							retCode = TA_CDLDRAGONFLYDOJI(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlengulfing:  
						{
							retCode = TA_CDLENGULFING(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlgapsidesidewhite:  
						{
							retCode = TA_CDLGAPSIDESIDEWHITE(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlgravestonedoji:  
						{
							retCode = TA_CDLGRAVESTONEDOJI(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlhammer:  
						{
							retCode = TA_CDLHAMMER(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlhangingman:  
						{
							retCode = TA_CDLHANGINGMAN(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlharami:  
						{
							retCode = TA_CDLHARAMI(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlharamicross:  
						{
							retCode = TA_CDLHARAMICROSS(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlhighwave:  
						{
							retCode = TA_CDLHIGHWAVE(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlhikkake:  
						{
							retCode = TA_CDLHIKKAKE(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlhikkakemod:  
						{
							retCode = TA_CDLHIKKAKEMOD(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlhomingpigeon:  
						{
							retCode = TA_CDLHIKKAKEMOD(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlidentical3crows:  
						{
							retCode = TA_CDLIDENTICAL3CROWS(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlinneck:  
						{
							retCode = TA_CDLINNECK(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlinvertedhammer:  
						{
							retCode = TA_CDLINVERTEDHAMMER(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlkicking:  
						{
							retCode = TA_CDLKICKING(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlkickingbylength:  
						{
							retCode = TA_CDLKICKINGBYLENGTH(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlladderbottom:  
						{
							retCode = TA_CDLLADDERBOTTOM(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdllongleggeddoji:  
						{
							retCode = TA_CDLLONGLEGGEDDOJI(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdllongline:  
						{
							retCode = TA_CDLLONGLINE(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlmarubozu:  
						{
							retCode = TA_CDLMARUBOZU(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlmatchinglow:  
						{
							retCode = TA_CDLMATCHINGLOW(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlonneck:  
						{
							retCode = TA_CDLONNECK(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlpiercing:  
						{
							retCode = TA_CDLPIERCING(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlrickshawman:  
						{
							retCode = TA_CDLRICKSHAWMAN(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlrisefall3methods:  
						{
							retCode = TA_CDLRISEFALL3METHODS(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlseparatinglines:  
						{
							retCode = TA_CDLSEPARATINGLINES(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlshootingstar:  
						{
							retCode = TA_CDLSHOOTINGSTAR(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlshortline:  
						{
							retCode = TA_CDLSHORTLINE(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlspinningtop:  
						{
							retCode = TA_CDLSPINNINGTOP(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlstalledpattern:  
						{
							retCode = TA_CDLSTALLEDPATTERN(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlsticksandwich:  
						{
							retCode = TA_CDLSTICKSANDWICH(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdltakuri:  
						{
							retCode = TA_CDLTAKURI(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdltasukigap:  
						{
							retCode = TA_CDLTASUKIGAP(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlthrusting:  
						{
							retCode = TA_CDLTHRUSTING(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdltristar:  
						{
							retCode = TA_CDLTRISTAR(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlunique3river:  
						{
							retCode = TA_CDLUNIQUE3RIVER(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlupsidegap2crows:  
						{
							retCode = TA_CDLUPSIDEGAP2CROWS(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}
					case ta_cdlxsidegap3methods:  
						{
							retCode = TA_CDLXSIDEGAP3METHODS(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, &cdlIdx, &outElements, outInt);
							break;
						}					
				}

				// Error handling
				if (retCode) 
				{
					mxFree(outInt);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				cdl_OUT = mxCreateNumericMatrix(cdlIdx + outElements,1, mxINT32_CLASS, mxREAL);
				memcpy((int*)(mxGetData(cdl_OUT)) + cdlIdx, outInt, outElements * mxGetElementSize(cdl_OUT));

				// Cleanup
				mxFree(outInt); 

				break;
			}

		// Vector Ceil
		case ta_ceil:       
			{
				// REQUIRED INPUTS
				//		data	any single column vector of observations

				// OPTIONAL INPUTS
				//		none

				// OUTPUTS
				//		CEIL	A vector output of CEIL values

				// Check number of inputs
				if (nrhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ceil:NumInputs",
					"Number of input arguments to function 'ta_ceil' is not correct. A single vector of values should be provided. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ceil:NumOutputs",
					"The function 'ta_ceil' (Ceil - round up) produces a single vector output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define ceil_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsD;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				colsD		= (int)mxGetN(data_IN);
				rows		= (int)mxGetM(data_IN);

				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ceil:InputErr",
						"Observation data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *outReal;

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				// May have to change typeMA from decimal to name
				retCode = TA_CEIL(startIdx, endIdx, dataPtr, &dataIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgTxt("Invocation to 'ta_ceil' failed. Aborting (2562).");
				}

				// Populate Output
				ceil_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(ceil_OUT)) + dataIdx, outReal, outElements * mxGetElementSize(ceil_OUT));

				// Cleanup
				mxFree(outReal); 

				break;
			}
			
		// Chande Momentum Oscillator
		case ta_cmo:       
			{
				// REQUIRED INPUTS
				//		data		any single column vector of observations

				// OPTIONAL INPUTS
				//		lookback	Lookback period	(default 14)

				// OUTPUT
				//		CMO			vector of Chande Momentum Oscillator values

				// Check number of inputs
				if (nrhs < 2 || nrhs > 3)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_cmo:NumInputs",
					"The number of input arguments to function 'ta_cmo' is incorrect. A single vector of observations should be provided.\nAn optional lookback period may be specified. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_cmo:NumOutputs",
					"The function 'ta_cmo' (Chande Momentum Oscillator) produces a single vector as output that must be assigned. Aborting (%).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define cmo_OUT			plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsD, lookback;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				rows		= (int)mxGetM(data_IN);
				colsD		= (int)mxGetN(data_IN);

				// Validation
				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_cmo:InputErr",
						"Observation data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int cmoIdx, outElements;
				double *outReal;

				// Get optional input or assign default
				if (nrhs == 3) 
				{
				#define lookback_IN	prhs[2]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The CMO lookback must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);
				}
				else
					// Default lookback period
				{
					lookback = 14;
				}

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				retCode = TA_CMO(startIdx, endIdx, dataPtr, lookback, &cmoIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				cmo_OUT = mxCreateDoubleMatrix(cmoIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(cmo_OUT)) + cmoIdx, outReal, outElements * mxGetElementSize(cmo_OUT));

				// Cleanup
				mxFree(outReal); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *outPtr = mxGetPr(cmo_OUT);

				for (int iter = 0; iter < lookback; iter++)
				{
					outPtr[iter] = m_Nan;
				}

				break;
			}
			
		// Pearson's Correlation Coefficient (r)
		case ta_correl:       
			{

				// REQUIRED INPUTS
				//		data		Two vectors of observations to produce a correlation analysis

				// OPTIONAL INPUTS
				//		Lookback	lookback period	(default 30)

				// OUTPUT
				//		corr		vector of Correlation Coefficient values


				// Check number of inputs
				if (nrhs < 3 || nrhs > 4)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_correl:NumInputs",
					"Number of input arguments to function 'ta_correl' is incorrect. Observation data should be parsed into two separate vectors for comparison. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_correl:NumOutputs",
					"The function 'ta_correl' (Pearson's Correlation Coefficient) produces a single vector as output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define obvA_IN			prhs[1]
				#define obvB_IN			prhs[2]

				// Outputs
				#define corr_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsA, colsB, lookback;
				double *obsAPtr, *obsBPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				obsAPtr		= mxGetPr(obvA_IN);
				rows		= (int)mxGetM(obvA_IN);
				colsA		= (int)mxGetN(obvA_IN);
				obsBPtr		= mxGetPr(obvB_IN);
				colsB		= (int)mxGetN(obvB_IN);

				// Validate
				if (colsA != 1 || colsB != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_correl:InputErr",
						"Observational data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int corrIdx, outElements;
				double *outReal;

				// Get optional input or assign default
				if (nrhs == 4) 
				{
					#define lookback_IN	prhs[3]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The CORREL lookback must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);
				}
				else
					// Default lookback period
				{
					lookback = 30;
				}

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				retCode = TA_CORREL(startIdx, endIdx, obsAPtr, obsBPtr, lookback, &corrIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				corr_OUT = mxCreateDoubleMatrix(corrIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(corr_OUT)) + corrIdx, outReal, outElements * mxGetElementSize(corr_OUT));

				// Cleanup
				mxFree(outReal); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *outPtr = mxGetPr(corr_OUT);

				for (int iter = 0; iter < lookback; iter++)
				{
					outPtr[iter] = m_Nan;
				}

				break;
			}

		// Vector Trigonometric Cos
		case ta_cos:       
			{
				// REQUIRED INPUTS
				//		input	any single column vector of angle values (radians)

				// OPTIONAL INPUTS
				//		none

				// OUTPUTS
				//		COS		cosine of input (-1 =< x =< 1)
				//				e.g. cos(3.14) = cos(pi) = -1

				// Check number of inputs
				if (nrhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_cos:NumInputs",
					"Number of input arguments to function 'ta_cos' is incorrect. A single vector of angle values (in radians) should be provided. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_cos:NumOutputs",
					"The function 'ta_cos' (Vector Trigonometric Cos) produces a single vector output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define theta_IN		prhs[1]

				// Outputs
				#define cos_OUT			plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsT;
				double *thetaPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse inputs and error check
				// Assign pointers and get dimensions
				thetaPtr	= mxGetPr(theta_IN);
				colsT		= (int)mxGetN(theta_IN);
				rows		= (int)mxGetM(theta_IN);

				if (colsT != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_cos:InputErr",
						"Angle observations should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int cosIdx, outElements;
				double *outReal;

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				// May have to change typeMA from decimal to name
				retCode = TA_COS(startIdx, endIdx, thetaPtr, &cosIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgTxt("Invocation to 'ta_cos' failed. Aborting (2856).");
				}

				// Populate Output
				cos_OUT = mxCreateDoubleMatrix(cosIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(cos_OUT)) + cosIdx, outReal, outElements * mxGetElementSize(cos_OUT));

				// Cleanup
				mxFree(outReal); 

				break;
			}

		// Vector Trigonometric Hyperbolic Cos
		case ta_cosh:       
			{
				// REQUIRED INPUTS
				//		input	any single column vector of angle values (radians)

				// OPTIONAL INPUTS
				//		none

				// OUTPUTS
				//		COSH	hyperbolic cosine of input 
				//				e.g. cosh(3.14) = cosh(pi) = ~11.57

				// Check number of inputs
				if (nrhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_cosh:NumInputs",
					"Number of input arguments to function 'ta_cosh' is incorrect. A single vector of angle values (in radians) should be provided. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_cosh:NumOutputs",
					"The function 'ta_cosh' (Vector Trigonometric Hyperbolic Cos) produces a single vector output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define theta_IN		prhs[1]

				// Outputs
				#define cosh_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsT;
				double *thetaPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse inputs and error check
				// Assign pointers and get dimensions
				thetaPtr	= mxGetPr(theta_IN);
				colsT		= (int)mxGetN(theta_IN);
				rows		= (int)mxGetM(theta_IN);

				if (colsT != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_cosh:InputErr",
						"Angle observations should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int coshIdx, outElements;
				double *outReal;

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				// May have to change typeMA from decimal to name
				retCode = TA_COSH(startIdx, endIdx, thetaPtr, &coshIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgTxt("Invocation to 'ta_cosh' failed. Aborting (2935).");
				}

				// Populate Output
				cosh_OUT = mxCreateDoubleMatrix(coshIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(cos_OUT)) + coshIdx, outReal, outElements * mxGetElementSize(cosh_OUT));

				// Cleanup
				mxFree(outReal); 

				break;
			}
		// Double Exponential Moving Average
		case ta_dema:       
			{
				{

					// REQUIRED INPUTS
					//		data		A single vector of observations

					// OPTIONAL INPUTS
					//		Lookback	lookback period	(default 30)

					// OUTPUT
					//		DEMA		A single vector of double exponential moving average values


					// Check number of inputs
					if (nrhs < 2 || nrhs > 3)
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_dema:NumInputs",
						"Number of input arguments to function 'ta_dema' is incorrect. Observation data should be parsed into a single input vector. Aborting (%d).", codeLine);
					if (nlhs != 1)
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_dema:NumOutputs",
						"The function 'ta_dema' (Double Exponential Moving Average) produces a single vector as output that must be assigned. Aborting (%d).", codeLine);

					// Create constants for readability
					// Inputs
					#define data_IN			prhs[1]

					// Outputs
					#define dema_OUT		plhs[0]

					// Declare variables
					int startIdx, endIdx, rows, colsD, lookback;
					double *dataPtr;

					// Initialize error handling 
					TA_RetCode retCode;

					// Parse required inputs and error check
					// Assign pointers and get dimensions
					dataPtr		= mxGetPr(data_IN);
					rows		= (int)mxGetM(data_IN);
					colsD		= (int)mxGetN(data_IN);

					// Validate
					if (colsD != 1)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_dema:InputErr",
							"Observational data should be a single vector array. Aborting (%d).", codeLine);
					}

					endIdx = rows - 1;  // Adjust for C++ starting at '0'
					startIdx = 0;

					// Output variables
					int dataIdx, outElements;
					double *outReal;

					// Get optional input or assign default
					if (nrhs == 3) 
					{
						#define lookback_IN	prhs[2]
						if (!isRealScalar(lookback_IN))
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The DEMA lookback must be a scalar. Aborting (%d).", codeLine);

						/* Get the scalar input lookback */
						// Assign
						lookback = (int)mxGetScalar(lookback_IN);

						if(lookback < 2)
						{
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
								"The DEMA lookback must be a scalar with a value of 3 or greater. Aborting (%d).", codeLine);
						}
					}
					else
						// Default lookback period
					{
						lookback = 30;
					}

					// Preallocate heap
					outReal = (double*)mxCalloc(rows, sizeof(double));

					// Invoke with error catch
					retCode = TA_DEMA(startIdx, endIdx, dataPtr, lookback, &dataIdx, &outElements, outReal);

					// Error handling
					if (retCode) 
					{
						mxFree(outReal);
						mexPrintf("%s%i","Return code=",retCode);
						mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
					}

					// Populate Output
					dema_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
					memcpy(((double *) mxGetData(dema_OUT)) + dataIdx, outReal, outElements * mxGetElementSize(dema_OUT));

					// Cleanup
					mxFree(outReal); 

					// NaN data before lookback
					// assign the variables for manipulating the arrays (by pointer reference)
					double *outPtr = mxGetPr(dema_OUT);

					for (int iter = (lookback * 2) - 1; iter < lookback; iter++)
					{
						outPtr[iter] = m_Nan;
					}

					break;
				}
			}
			
		// Vector Arithmetic Div
		case ta_div:       
			{
				// REQUIRED INPUTS
				//		Dividend
				//		Divisor

				// OPTIONAL INPUTS
				//		none

				// OUTPUTS
				//		DIV			A single vector of quotient values

				// Check number of inputs
				if (nrhs != 3)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_div:NumInputs",
					"Number of input arguments to function 'ta_div' is incorrect. Two vectors for division should be provided. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define divid_IN		prhs[1]
				#define divis_IN		prhs[2]

				// Outputs
				#define quot_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsDD, colsDV;
				double *dividPtr, *divisPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				dividPtr		= mxGetPr(divid_IN);
				rows			= (int)mxGetM(divid_IN);
				colsDD			= (int)mxGetN(divid_IN);
				divisPtr		= mxGetPr(divis_IN);
				colsDV			= (int)mxGetN(divis_IN);

				if (colsDD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:InputErr",
						"Vector division inputs should be single dimensional vectors.\nThe dividend vector has more than 1 column.  Aborting (%d).", codeLine);
				}

				if (colsDV != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:InputErr",
						"Vector division inputs should be single dimensional vectors.\nThe divisor vector had more than 1 column.  Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int quotIdx, outElements;
				double *quotient;

				// Preallocate heap
				quotient = (double*)mxCalloc(rows, sizeof(double));

				retCode = TA_DIV(startIdx, endIdx, dividPtr, divisPtr, &quotIdx, &outElements, quotient);

				// Error handling
				if (retCode) 
				{
					mxFree(quotient);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (3132).", taFuncNameIn);
				}

				// Populate Output
				quot_OUT = mxCreateDoubleMatrix(quotIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(quot_OUT)) + quotIdx, quotient, outElements * mxGetElementSize(quot_OUT));

				// Cleanup
				mxFree(quotient); 

				break;
			}

		// Directional Movement Index
		case ta_dx:       
			{
				// REQUIRED INPUTS
				//		Price	H | L | C	separate vectors

				// OPTIONAL INPUTS
				//		Lookback period	(default 14)

				// OUTPUTS
				//		DX		A single vector of directional movement index values		

				// Check number of inputs
				if (nrhs < 4 || nrhs > 5)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_dx:NumInputs",
					"Number of input arguments to function 'ta_dx' is incorrect. Price data should be parsed into vectors H | L | C. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_dx:NumOutputs",
					"The function 'ta_dx' (Directional Movement Index) produces a single vector output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define high_IN		prhs[1]
				#define low_IN		prhs[2]
				#define close_IN	prhs[3]

				// Outputs
				#define dx_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsH, colsL, colsC, lookback;
				double *highPtr, *lowPtr, *closePtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				highPtr		= mxGetPr(high_IN);
				rows		= (int)mxGetM(high_IN);
				colsH		= (int)mxGetN(high_IN);
				lowPtr		= mxGetPr(low_IN);
				colsL		= (int)mxGetN(low_IN);
				closePtr	= mxGetPr(close_IN);
				colsC		= (int)mxGetN(close_IN);

				// Validate
				chkSingleVec(colsH, colsL, colsC, codeLine);

				break;
			}
			
		// Exponential Moving Average
		case ta_ema:       
			{
				{

					// REQUIRED INPUTS
					//		data		A single vector of observations

					// OPTIONAL INPUTS
					//		Lookback	lookback period	(default 30)

					// OUTPUT
					//		EMA		vector of exponential moving average values


					// Check number of inputs
					if (nrhs < 2 || nrhs > 3)
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ema:NumInputs",
						"Number of input arguments to function 'ta_ema' is incorrect. Observation data should be parsed into a single input vector. Aborting (%d).", codeLine);
					if (nlhs != 1)
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ema:NumOutputs",
						"The function 'ta_ema' (Double Exponential Moving Average) produces a single vector as output that must be assigned. Aborting (%d).", codeLine);

					// Create constants for readability
					// Inputs
					#define data_IN			prhs[1]

					// Outputs
					#define ema_OUT		plhs[0]

					// Declare variables
					int startIdx, endIdx, rows, colsD, lookback;
					double *dataPtr;

					// Initialize error handling 
					TA_RetCode retCode;

					// Parse required inputs and error check
					// Assign pointers and get dimensions
					dataPtr		= mxGetPr(data_IN);
					rows		= (int)mxGetM(data_IN);
					colsD		= (int)mxGetN(data_IN);

					// Validate
					if (colsD != 1)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ema:InputErr",
							"Observational data should be a single vector array. Aborting (%d).", codeLine);
					}

					endIdx = rows - 1;  // Adjust for C++ starting at '0'
					startIdx = 0;

					// Output variables
					int dataIdx, outElements;
					double *outReal;

					// Get optional input or assign default
					if (nrhs == 3) 
					{
						#define lookback_IN	prhs[2]
						if (!isRealScalar(lookback_IN))
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The EMA lookback must be a scalar. Aborting (%d).", codeLine);

						/* Get the scalar input lookback */
						// Assign
						lookback = (int)mxGetScalar(lookback_IN);

						if(lookback < 2)
						{
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
								"The EMA lookback must be a scalar with a value of 3 or greater. Aborting (%d).", codeLine);
						}
					}
					else
						// Default lookback period
					{
						lookback = 30;
					}

					// Preallocate heap
					outReal = (double*)mxCalloc(rows, sizeof(double));

					// Invoke with error catch
					retCode = TA_EMA(startIdx, endIdx, dataPtr, lookback, &dataIdx, &outElements, outReal);

					// Error handling
					if (retCode) 
					{
						mxFree(outReal);
						mexPrintf("%s%i","Return code=",retCode);
						mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
					}

					// Populate Output
					ema_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
					memcpy(((double *) mxGetData(ema_OUT)) + dataIdx, outReal, outElements * mxGetElementSize(ema_OUT));

					// Cleanup
					mxFree(outReal); 

					// NaN data before lookback
					// assign the variables for manipulating the arrays (by pointer reference)
					double *outPtr = mxGetPr(ema_OUT);

					for (int iter = 0; iter < lookback; iter++)
					{
						outPtr[iter] = m_Nan;
					}

					break;
				}
			}

		// Vector Arithmetic Exp (e ^ observation)
		case ta_exp:       
				{

					// REQUIRED INPUTS
					//		data	A single vector of observations

					// OPTIONAL INPUTS
					//		none

					// OUTPUT
					//		EXP		vector of e ^ observation values
					//				e.g. e ^ 1 = ~2.718


					// Check number of inputs
					if (nrhs != 2)
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_exp:NumInputs",
						"Number of input arguments to function 'ta_exp' is incorrect. Observation data should be parsed into a single input vector. Aborting (%d).", codeLine);
					if (nlhs != 1)
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_exp:NumOutputs",
						"The function 'ta_exp' (Vector Arithmetic Exp) produces a single vector as output that must be assigned. Aborting (%d).", codeLine);

					// Create constants for readability
					// Inputs
					#define data_IN			prhs[1]

					// Outputs
					#define e_OUT		plhs[0]

					// Declare variables
					int startIdx, endIdx, rows, colsD, lookback;
					double *dataPtr;

					// Initialize error handling 
					TA_RetCode retCode;

					// Parse required inputs and error check
					// Assign pointers and get dimensions
					dataPtr		= mxGetPr(data_IN);
					rows		= (int)mxGetM(data_IN);
					colsD		= (int)mxGetN(data_IN);

					// Validate
					if (colsD != 1)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_exp:InputErr",
							"Observational data should be a single vector array. Aborting (%d).", codeLine);
					}

					endIdx = rows - 1;  // Adjust for C++ starting at '0'
					startIdx = 0;

					// Output variables
					int dataIdx, outElements;
					double *outReal;

					// Preallocate heap
					outReal = (double*)mxCalloc(rows, sizeof(double));

					// Invoke with error catch
					retCode = TA_EXP(startIdx, endIdx, dataPtr, &dataIdx, &outElements, outReal);

					// Error handling
					if (retCode) 
					{
						mxFree(outReal);
						mexPrintf("%s%i","Return code=",retCode);
						mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
					}

					// Populate Output
					e_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
					memcpy(((double *) mxGetData(e_OUT)) + dataIdx, outReal, outElements * mxGetElementSize(e_OUT));

					// Cleanup
					mxFree(outReal); 

					break;
				}

		// Vector Floor
		case ta_floor:       
			{
				// REQUIRED INPUTS
				//		data	any single column vector of observations

				// OPTIONAL INPUTS
				//		none

				// OUTPUTS
				//		FLOOR	A vector output of FLOOR values

				// Check number of inputs
				if (nrhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_floor:NumInputs",
					"Number of input arguments to function 'ta_floor' is not correct. A single vector of values should be provided. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_floor:NumOutputs",
					"The function 'ta_floor' (Floor - round down) produces a single vector output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define floor_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsD;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				colsD		= (int)mxGetN(data_IN);
				rows		= (int)mxGetM(data_IN);

				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_floor:InputErr",
						"Observation data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *outReal;

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				// May have to change typeMA from decimal to name
				retCode = TA_FLOOR(startIdx, endIdx, dataPtr, &dataIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgTxt("Invocation to 'ta_floor' failed. Aborting (3458).");
				}

				// Populate Output
				ceil_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(ceil_OUT)) + dataIdx, outReal, outElements * mxGetElementSize(ceil_OUT));

				// Cleanup
				mxFree(outReal); 

				break;
			}

		// Hilbert Transform - Dominant Cycle Period
		case ta_ht_dcperiod:       
			{
				// REQUIRED INPUTS
				//		data	any single column vector of observations

				// OPTIONAL INPUTS
				//		none

				// OUTPUTS
				//		DCPERIOD	A vector output of Hilbert Transform - dominant cycle period values

				// Check number of inputs
				if (nrhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_dcperiod:NumInputs",
					"Number of input arguments to function 'ta_ht_dcperiod' is not correct. A single vector of values should be provided. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_dcperiod:NumOutputs",
					"The function 'ta_ht_dcperiod' (Hilbert Transform - Dominant Cycle Period) produces a single vector output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define period_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsD;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				colsD		= (int)mxGetN(data_IN);
				rows		= (int)mxGetM(data_IN);

				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_dcperiod:InputErr",
						"Observation data should be a single vector array. Aborting (%d).", codeLine);
				}

				if (rows < 33)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_dcperiod:InputErr",
						"The function 'ta_ht_dcperiod' needs a minimum of 33 observations. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *outReal;

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				// May have to change typeMA from decimal to name
				retCode = TA_HT_DCPERIOD(startIdx, endIdx, dataPtr, &dataIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgTxt("Invocation to 'ta_ht_dcperiod' failed. Aborting (3536).");
				}

				// Populate Output
				period_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(period_OUT)) + dataIdx, outReal, outElements * mxGetElementSize(period_OUT));

				// Cleanup
				mxFree(outReal); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *outPtr = mxGetPr(period_OUT);

				for (int iter = 0; iter < 32; iter++)
				{
					outPtr[iter] = m_Nan;
				}

				break;
			}

		// Hilbert Transform - Dominant Cycle Phase
		case ta_ht_dcphase:       
			{
				// REQUIRED INPUTS
				//		data	any single column vector of observations

				// OPTIONAL INPUTS
				//		none

				// OUTPUTS
				//		DCPHASE	A vector output of Hilbert Transform - dominant cycle phase values

				// Check number of inputs
				if (nrhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_dcphase:NumInputs",
					"Number of input arguments to function 'ta_ht_dcphase' is incorrect. A single vector of values should be provided. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_dcphase:NumOutputs",
					"The function 'ta_ht_dcphase' (Hilbert Transform - Dominant Cycle Phase) produces a single vector output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define phase_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsD;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				colsD		= (int)mxGetN(data_IN);
				rows		= (int)mxGetM(data_IN);

				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_dcphase:InputErr",
						"Observation data should be a single vector array. Aborting (%d).", codeLine);
				}

				if (rows < 64)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_dcphase:InputErr",
						"The function 'ta_ht_dcphase' needs a minimum of 64 observations. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *outReal;

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				// May have to change typeMA from decimal to name
				retCode = TA_HT_DCPHASE(startIdx, endIdx, dataPtr, &dataIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgTxt("Invocation to 'ta_ht_dcperiod' failed. Aborting (3536).");
				}

				// Populate Output
				phase_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(phase_OUT)) + dataIdx, outReal, outElements * mxGetElementSize(phase_OUT));

				// Cleanup
				mxFree(outReal); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *outPtr = mxGetPr(phase_OUT);

				for (int iter = 0; iter < 63; iter++)
				{
					outPtr[iter] = m_Nan;
				}

				break;
			}
		// Hilbert Transform - Phasor Components
		case ta_ht_phasor:       
			{
				// REQUIRED INPUTS
				//		data	any single column vector of observations

				// OPTIONAL INPUTS
				//		none

				// OUTPUTS
				//		inPhase
				//		Quadrature

				// Description: The Hilbert Transform is a technique used to generate inPhase and quadrature components 
				//	of a de-trended real-valued "analytic-like" signal (such as a Price Series) in order to analyze 
				//	variations of the instantaneous phase and amplitude. 'ta_ht_phasor' returns the Hilbert Transform 
				//	generated inPhase and Quadrature components of the given data vector.

				// Check number of inputs
				if (nrhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_phasor:NumInputs",
					"Number of input arguments to function 'ta_ht_dcphase' is incorrect. A single vector of values should be provided. Aborting (%d).", codeLine);
				if (nlhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_phasor:NumOutputs",
					"The function 'ta_ht_dcphase' (Hilbert Transform - Phasor Components) produces a two output vectors (inPhase & Quadrature) that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define inPhase_OUT		plhs[0]
				#define quad_OUT		plhs[1]

				// Declare variables
				int startIdx, endIdx, rows, colsD;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				colsD		= (int)mxGetN(data_IN);
				rows		= (int)mxGetM(data_IN);

				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_phasor:InputErr",
						"Observation data should be a single vector array. Aborting (%d).", codeLine);
				}

				if (rows < 33)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_phasor:InputErr",
						"The function 'ta_ht_dcphase' needs a minimum of 33 observations. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *inPhase, *quad;

				// Preallocate heap
				inPhase = (double*)mxCalloc(rows, sizeof(double));
				quad = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				// May have to change typeMA from decimal to name
				retCode = TA_HT_PHASOR(startIdx, endIdx, dataPtr, &dataIdx, &outElements, inPhase, quad);

				// Error handling
				if (retCode) 
				{
					mxFree(inPhase);
					mxFree(quad);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgTxt("Invocation to 'ta_ht_dcperiod' failed. Aborting (3736).");
				}

				// Populate Output
				inPhase_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				quad_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(inPhase_OUT)) + dataIdx, inPhase, outElements * mxGetElementSize(inPhase_OUT));
				memcpy(((double *) mxGetData(quad_OUT)) + dataIdx, quad, outElements * mxGetElementSize(quad_OUT));

				// Cleanup
				mxFree(inPhase); 
				mxFree(quad); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *inPhasePtr = mxGetPr(inPhase_OUT);
				double *quadPtr = mxGetPr(quad_OUT);

				for (int iter = 0; iter < 32; iter++)
				{
					inPhasePtr[iter] = m_Nan;
					quadPtr[iter] = m_Nan;
				}

				break;
			}

		// Hilbert Transform - SineWave
		case ta_ht_sine:       
			{
				// REQUIRED INPUTS
				//		data	any single column vector of observations

				// OPTIONAL INPUTS
				//		none

				// OUTPUTS
				//		sine			sine of the DC Phase
				//		leadSine

				// Description: The Hilbert Transform is a technique used to generate inphase and quadrature components of 
				//	a de-trended real-valued "analytic-like" signal (such as a Price Series) in order to analyze variations 
				//	of the instantaneous phase and amplitude. 'ta_ht_phase' returns the Hilbert Transform Phase of the Dominant Cycle. 
				//	The Dominant Cycle Phase lies in the range of 0 to 360 degrees. 
				//	The Hilbert Transform Sine is just the sine of the DC Phase.
				//	The DC Phase at a specific bar gives the phase position from 0 to 360 degrees within the current 
				//	Hilbert Transform Period instantaneously measured at that bar. The 'ta_ht_sine:sine' is the sine of 
				//	the DC Phase at a specific bar. It is most often used in conjunction with the 'ta_ht_sine:leadsine' values as an
				//	indicator to identify cyclic turning points.
				//	
				//	Quoting from Market Mode Strategies.doc by John Ehlers from MESA Software: 
				//		"A clear, unequivocal cycle mode indicator can be generated by plotting the Sine of the measured phase 
				//		angle advanced by 45 degrees. This leading signal crosses the sinewave 1/8th of a cycle BEFORE the 
				//		peaks and valleys of the cyclic turning points, enabling you to make your trading decision in time 
				//		to profit from the entire amplitude swing of the cycle. A significant additional advantage is that 
				//		the two indicator lines don't cross except at cyclic turning points, avoiding the false whipsaw signals
				//		of most "oscillators" when the market is in a Trend Mode. 
				//		The two lines don't cross because the phase rate of change is nearly zero in a trend mode. 
				//		Since the phase is not changing, the two lines separated by 45 degrees in phase never get the 
				//		opportunity to cross."
				//
				//	Read more: http://www.quantshare.com/index.php?option=manual&dir=/QuantShare%20Language/Indicators/Ht_Sine%201.html#ixzz2YJ0p2KxV 

				// Check number of inputs
				if (nrhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_sine:NumInputs",
					"Number of input arguments to function 'ta_ht_sine' is incorrect. A single vector of values should be provided. Aborting (%d).", codeLine);
				if (nlhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_sine:NumOutputs",
					"The function 'ta_ht_sine' (Hilbert Transform - SineWave) produces a two output vectors (sine & leadSine) that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define sine_OUT		plhs[0]
				#define leadSine_OUT	plhs[1]

				// Declare variables
				int startIdx, endIdx, rows, colsD;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				colsD		= (int)mxGetN(data_IN);
				rows		= (int)mxGetM(data_IN);

				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_phasor:InputErr",
						"Observation data should be a single vector array. Aborting (%d).", codeLine);
				}

				if (rows < 64)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_sine:InputErr",
						"The function 'ta_ht_sine' needs a minimum of 64 observations. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *sine, *leadSine;

				// Preallocate heap
				sine = (double*)mxCalloc(rows, sizeof(double));
				leadSine = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				// May have to change typeMA from decimal to name
				retCode = TA_HT_SINE(startIdx, endIdx, dataPtr, &dataIdx, &outElements, sine, leadSine);

				// Error handling
				if (retCode) 
				{
					mxFree(sine);
					mxFree(leadSine);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgTxt("Invocation to 'ta_ht_sine' failed. Aborting (3861).");
				}

				// Populate Output
				sine_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				leadSine_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(sine_OUT)) + dataIdx, sine, outElements * mxGetElementSize(sine_OUT));
				memcpy(((double *) mxGetData(leadSine_OUT)) + dataIdx, leadSine, outElements * mxGetElementSize(leadSine_OUT));

				// Cleanup
				mxFree(sine); 
				mxFree(leadSine); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *sinePtr = mxGetPr(sine_OUT);
				double *leadSinePtr = mxGetPr(leadSine_OUT);

				for (int iter = 0; iter < 63; iter++)
				{
					sinePtr[iter] = m_Nan;
					leadSinePtr[iter] = m_Nan;
				}

				break;
			}

		// Hilbert Transform - Instantaneous Trendline
		case ta_ht_trendline:       
			{
				// REQUIRED INPUTS
				//		data	any single column vector of observations

				// OPTIONAL INPUTS
				//		none

				// OUTPUT
				//		TRENDLINE	A vector output of Hilbert Transform trendline values

				// Check number of inputs
				if (nrhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_trendline:NumInputs",
					"Number of input arguments to function 'ta_ht_trendlineta_ht_trendline is incorrect. A single vector of values should be provided. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_trendline:NumOutputs",
					"The function 'ta_ht_trendline' (Hilbert Transform - Instantaneous Trendline) produces a single vector output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define trend_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsD;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				colsD		= (int)mxGetN(data_IN);
				rows		= (int)mxGetM(data_IN);

				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_trendline:InputErr",
						"Observation data should be a single vector array. Aborting (%d).", codeLine);
				}

				if (rows < 64)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_trendline:InputErr",
						"The function 'ta_ht_trendline' needs a minimum of 64 observations. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *outReal;

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				// May have to change typeMA from decimal to name
				retCode = TA_HT_TRENDLINE(startIdx, endIdx, dataPtr, &dataIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgTxt("Invocation to 'ta_ht_trendline' failed. Aborting (3959).");
				}

				// Populate Output
				trend_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(trend_OUT)) + dataIdx, outReal, outElements * mxGetElementSize(trend_OUT));

				// Cleanup
				mxFree(outReal); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *outPtr = mxGetPr(trend_OUT);

				for (int iter = 0; iter < 63; iter++)
				{
					outPtr[iter] = m_Nan;
				}

				break;
			}

		// Hilbert Transform - Trend vs Cycle Mode
		case ta_ht_trendmode:       
			{
				// REQUIRED INPUTS
				//		data	any single column vector of observations

				// OPTIONAL INPUTS
				//		none

				// OUTPUT
				//		TRENDMODE	A vector output of Hilbert Transform trend mode (cycle = 0 or trend = 1)

				// Check number of inputs
				if (nrhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_trendmode:NumInputs",
					"Number of input arguments to function 'ta_ht_trendmode' is incorrect. A single vector of values should be provided. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_trendmode:NumOutputs",
					"The function 'ta_ht_trendmode' (Hilbert Transform - Trend vs Cycle Mode) produces a single vector output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define mode_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsD;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				colsD		= (int)mxGetN(data_IN);
				rows		= (int)mxGetM(data_IN);

				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_trendmode:InputErr",
						"Observation data should be a single vector array. Aborting (%d).", codeLine);
				}

				if (rows < 64)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ht_trendline:InputErr",
						"The function 'ta_ht_trendline' needs a minimum of 64 observations. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				int *outInt;

				// Preallocate heap
				outInt = (int*)mxCalloc(rows, sizeof(int));

				// Invoke with error catch
				// May have to change typeMA from decimal to name
				retCode = TA_HT_TRENDMODE(startIdx, endIdx, dataPtr, &dataIdx, &outElements, outInt);

				// Error handling
				if (retCode) 
				{
					mxFree(outInt);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgTxt("Invocation to 'ta_ht_trendmode' failed. Aborting (4046).");
				}

				// Populate Output
				mode_OUT = mxCreateNumericMatrix(dataIdx + outElements,1, mxINT32_CLASS, mxREAL);
				memcpy(((int *) mxGetData(mode_OUT)) + dataIdx, outInt, outElements * mxGetElementSize(mode_OUT));

				// Cleanup
				mxFree(outInt); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *outPtr = mxGetPr(trend_OUT);

				for (int iter = 0; iter < 63; iter++)
				{
					outPtr[iter] = m_Nan;
				}

				break;
			}

		// Kaufman Adaptive Moving Average
		case ta_kama:       
				{
					// REQUIRED INPUTS
					//		data		A single vector of observations

					// OPTIONAL INPUTS
					//		Lookback	lookback period	(default 30)

					// OUTPUT
					//		KAMA		vector of kaufman adaptive moving average values


					// Check number of inputs
					if (nrhs < 2 || nrhs > 3)
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_kama:NumInputs",
						"Number of input arguments to function 'ta_kama' is incorrect. Observation data should be parsed into a single input vector. Aborting (%d).", codeLine);
					if (nlhs != 1)
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_kama:NumOutputs",
						"The function 'ta_kama' (Kaufman Adaptive Moving Average) produces a single vector as output that must be assigned. Aborting (%d).", codeLine);

					// Create constants for readability
					// Inputs
					#define data_IN			prhs[1]

					// Outputs
					#define kama_OUT		plhs[0]

					// Declare variables
					int startIdx, endIdx, rows, colsD, lookback;
					double *dataPtr;

					// Initialize error handling 
					TA_RetCode retCode;

					// Parse required inputs and error check
					// Assign pointers and get dimensions
					dataPtr		= mxGetPr(data_IN);
					rows		= (int)mxGetM(data_IN);
					colsD		= (int)mxGetN(data_IN);

					// Validate
					if (colsD != 1)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_kama:InputErr",
							"Observational data should be a single vector array. Aborting (%d).", codeLine);
					}

					endIdx = rows - 1;  // Adjust for C++ starting at '0'
					startIdx = 0;

					// Output variables
					int dataIdx, outElements;
					double *outReal;

					// Get optional input or assign default
					if (nrhs == 3) 
					{
						#define lookback_IN	prhs[2]
						if (!isRealScalar(lookback_IN))
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The KAMA lookback must be a scalar. Aborting (%d).", codeLine);

						/* Get the scalar input lookback */
						// Assign
						lookback = (int)mxGetScalar(lookback_IN);

						if(lookback < 2)
						{
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
								"The KAMA lookback must be a scalar with a value of 2 or greater. Aborting (%d).", codeLine);
						}
					}
					else
						// Default lookback period
					{
						lookback = 30;
					}

					// Preallocate heap
					outReal = (double*)mxCalloc(rows, sizeof(double));

					// Invoke with error catch
					retCode = TA_KAMA(startIdx, endIdx, dataPtr, lookback, &dataIdx, &outElements, outReal);

					// Error handling
					if (retCode) 
					{
						mxFree(outReal);
						mexPrintf("%s%i","Return code=",retCode);
						mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
					}

					// Populate Output
					kama_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
					memcpy(((double *) mxGetData(kama_OUT)) + dataIdx, outReal, outElements * mxGetElementSize(kama_OUT));

					// Cleanup
					mxFree(outReal); 

					// NaN data before lookback
					// assign the variables for manipulating the arrays (by pointer reference)
					double *outPtr = mxGetPr(kama_OUT);

					for (int iter = 0; iter < lookback; iter++)
					{
						outPtr[iter] = m_Nan;
					}

					break;
				}

		// Linear Regression
		case ta_linearreg:       
				{
					// REQUIRED INPUTS
					//		data		A single vector of observations

					// OPTIONAL INPUTS
					//		Lookback	lookback period	(default 14)

					// OUTPUT
					//		LINREG		vector of linear regression values


					// Check number of inputs
					if (nrhs < 2 || nrhs > 3)
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_linearreg:NumInputs",
						"Number of input arguments to function 'ta_linearreg' is incorrect. Observation data should be parsed into a single input vector. Aborting (%d).", codeLine);
					if (nlhs != 1)
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_linearreg:NumOutputs",
						"The function 'ta_linearreg' (Linear Regression) produces a single vector as output that must be assigned. Aborting (%d).", codeLine);

					// Create constants for readability
					// Inputs
					#define data_IN			prhs[1]

					// Outputs
					#define linreg_OUT		plhs[0]

					// Declare variables
					int startIdx, endIdx, rows, colsD, lookback;
					double *dataPtr;

					// Initialize error handling 
					TA_RetCode retCode;

					// Parse required inputs and error check
					// Assign pointers and get dimensions
					dataPtr		= mxGetPr(data_IN);
					rows		= (int)mxGetM(data_IN);
					colsD		= (int)mxGetN(data_IN);

					// Validate
					if (colsD != 1)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_linearreg:InputErr",
							"Observational data should be a single vector array. Aborting (%d).", codeLine);
					}

					endIdx = rows - 1;  // Adjust for C++ starting at '0'
					startIdx = 0;

					// Output variables
					int dataIdx, outElements;
					double *outReal;

					// Get optional input or assign default
					if (nrhs == 3) 
					{
						#define lookback_IN	prhs[2]
						if (!isRealScalar(lookback_IN))
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The LINEAR REGRESSION lookback must be a scalar. Aborting (%d).", codeLine);

						/* Get the scalar input lookback */
						// Assign
						lookback = (int)mxGetScalar(lookback_IN);

						if(lookback < 2)
						{
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
								"The LINEAR REGRESSION lookback must be a scalar with a value of 2 or greater. Aborting (%d).", codeLine);
						}
					}
					else
						// Default lookback period
					{
						lookback = 14;
					}

					// Preallocate heap
					outReal = (double*)mxCalloc(rows, sizeof(double));

					// Invoke with error catch
					retCode = TA_LINEARREG(startIdx, endIdx, dataPtr, lookback, &dataIdx, &outElements, outReal);

					// Error handling
					if (retCode) 
					{
						mxFree(outReal);
						mexPrintf("%s%i","Return code=",retCode);
						mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
					}

					// Populate Output
					linreg_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
					memcpy(((double *) mxGetData(linreg_OUT)) + dataIdx, outReal, outElements * mxGetElementSize(linreg_OUT));

					// Cleanup
					mxFree(outReal); 

					// NaN data before lookback
					// assign the variables for manipulating the arrays (by pointer reference)
					double *outPtr = mxGetPr(linreg_OUT);

					for (int iter = 0; iter < lookback; iter++)
					{
						outPtr[iter] = m_Nan;
					}

					break;
				}

		// Linear Regression Angle
		case ta_linearreg_angle:       
			{
				// REQUIRED INPUTS
				//		data		A single vector of observations

				// OPTIONAL INPUTS
				//		Lookback	lookback period	(default 14)

				// OUTPUT
				//		LINREGA		vector of linear regression angle values in degrees

				// Description: Linear Regression is a concept also known as the "least squares method" or "best fit." 
				//	Linear Regression attempts to fit a straight line between several data points in such a way that 
				//	distance between each data point and the line is minimized.
				//  For each point, a straight line over the specified previous bar period is determined in terms of 
				//	y = mx + b
				//
				//	TA_LINEARREG				Returns b+m*(period-1)
				//	TA_LINEARREG_SLOPE			Returns 'm'
				//	TA_LINEARREG_ANGLE			Returns 'm' in degrees
				//	TA_LINEARREG_INTERCEPT		Returns 'b'
				//	TA_TSF						Returns b+m*(period)


				// Check number of inputs
				if (nrhs < 2 || nrhs > 3)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_linearreg_angle:NumInputs",
					"Number of input arguments to function 'ta_linearreg_angle' is incorrect. Observation data should be parsed into a single input vector. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_linearreg_angle:NumOutputs",
					"The function 'ta_linearreg_angle' (Linear Regression Angle) produces a single vector as output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define linrega_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsD, lookback;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				rows		= (int)mxGetM(data_IN);
				colsD		= (int)mxGetN(data_IN);

				// Validate
				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_linearreg_angle:InputErr",
						"Observational data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *outReal;

				// Get optional input or assign default
				if (nrhs == 3) 
				{
					#define lookback_IN	prhs[2]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The LINEAR REGRESSION ANGLE lookback must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);

					if(lookback < 2)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The LINEAR REGRESSION ANGLE lookback must be a scalar with a value of 2 or greater. Aborting (%d).", codeLine);
					}
				}
				else
					// Default lookback period
				{
					lookback = 14;
				}

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				retCode = TA_LINEARREG_ANGLE(startIdx, endIdx, dataPtr, lookback, &dataIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				linrega_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(linrega_OUT)) + dataIdx, outReal, outElements * mxGetElementSize(linrega_OUT));

				// Cleanup
				mxFree(outReal); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *outPtr = mxGetPr(linrega_OUT);

				for (int iter = 0; iter < lookback; iter++)
				{
					outPtr[iter] = m_Nan;
				}

				break;
			}

		// Linear Regression Intercept
		case ta_linearreg_intercept:       
			{
				// REQUIRED INPUTS
				//		data		A single vector of observations

				// OPTIONAL INPUTS
				//		Lookback	lookback period	(default 14)

				// OUTPUT
				//		LINREGA		vector of linear regression intercept values in degrees

				// Description: Linear Regression is a concept also known as the "least squares method" or "best fit." 
				//	Linear Regression attempts to fit a straight line between several data points in such a way that 
				//	distance between each data point and the line is minimized.
				//  For each point, a straight line over the specified previous bar period is determined in terms of 
				//	y = mx + b
				//
				//	TA_LINEARREG				Returns b+m*(period-1)
				//	TA_LINEARREG_SLOPE			Returns 'm'
				//	TA_LINEARREG_ANGLE			Returns 'm' in degrees
				//	TA_LINEARREG_INTERCEPT		Returns 'b'
				//	TA_TSF						Returns b+m*(period)


				// Check number of inputs
				if (nrhs < 2 || nrhs > 3)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_linearreg_intercept:NumInputs",
					"Number of input arguments to function 'ta_linearreg_intercept' is incorrect. Observation data should be parsed into a single input vector. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_linearreg_intercept:NumOutputs",
					"The function 'ta_linearreg_intercept' (Linear Regression Intercept) produces a single vector as output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define linregi_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsD, lookback;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				rows		= (int)mxGetM(data_IN);
				colsD		= (int)mxGetN(data_IN);

				// Validate
				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_linearreg_intercept:InputErr",
						"Observational data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *outReal;

				// Get optional input or assign default
				if (nrhs == 3) 
				{
					#define lookback_IN	prhs[2]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The LINEAR REGRESSION ANGLE lookback must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);

					if(lookback < 2)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The LINEAR REGRESSION INTERCEPT lookback must be a scalar with a value of 2 or greater. Aborting (%d).", codeLine);
					}
				}
				else
					// Default lookback period
				{
					lookback = 14;
				}

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				retCode = TA_LINEARREG_INTERCEPT(startIdx, endIdx, dataPtr, lookback, &dataIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				linregi_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(linregi_OUT)) + dataIdx, outReal, outElements * mxGetElementSize(linregi_OUT));

				// Cleanup
				mxFree(outReal); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *outPtr = mxGetPr(linregi_OUT);

				for (int iter = 0; iter < lookback; iter++)
				{
					outPtr[iter] = m_Nan;
				}

				break;
			}

		// Linear Regression Slope
		case ta_linearreg_slope:       
			{
				// REQUIRED INPUTS
				//		data		A single vector of observations

				// OPTIONAL INPUTS
				//		Lookback	lookback period	(default 14)

				// OUTPUT
				//		LINREGS		vector of linear regression slope values in degrees

				// Description: Linear Regression is a concept also known as the "least squares method" or "best fit." 
				//	Linear Regression attempts to fit a straight line between several data points in such a way that 
				//	distance between each data point and the line is minimized.
				//  For each point, a straight line over the specified previous bar period is determined in terms of 
				//	y = mx + b
				//
				//	TA_LINEARREG				Returns b+m*(period-1)
				//	TA_LINEARREG_SLOPE			Returns 'm'
				//	TA_LINEARREG_ANGLE			Returns 'm' in degrees
				//	TA_LINEARREG_INTERCEPT		Returns 'b'
				//	TA_TSF						Returns b+m*(period)


				// Check number of inputs
				if (nrhs < 2 || nrhs > 3)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_linearreg_slope:NumInputs",
					"Number of input arguments to function 'ta_linearreg_slope' is incorrect. Observation data should be parsed into a single input vector. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_linearreg_slope:NumOutputs",
					"The function 'ta_linearreg_slope' (Linear Regression Slope) produces a single vector as output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define linregs_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsD, lookback;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				rows		= (int)mxGetM(data_IN);
				colsD		= (int)mxGetN(data_IN);

				// Validate
				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_linearreg_slope:InputErr",
						"Observational data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *outReal;

				// Get optional input or assign default
				if (nrhs == 3) 
				{
					#define lookback_IN	prhs[2]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The LINEAR REGRESSION SLOPE lookback must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);

					if(lookback < 2)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The LINEAR REGRESSION SLOPE lookback must be a scalar with a value of 2 or greater. Aborting (%d).", codeLine);
					}
				}
				else
					// Default lookback period
				{
					lookback = 14;
				}

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				retCode = TA_LINEARREG_SLOPE(startIdx, endIdx, dataPtr, lookback, &dataIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				linregs_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(linregs_OUT)) + dataIdx, outReal, outElements * mxGetElementSize(linregs_OUT));

				// Cleanup
				mxFree(outReal); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *outPtr = mxGetPr(linregs_OUT);

				for (int iter = 0; iter < lookback; iter++)
				{
					outPtr[iter] = m_Nan;
				}

				break;
			}

		// Vector Log Natural
		case ta_ln:       
			{
				// REQUIRED INPUTS
				//		input	any single column vector of Real values

				// OPTIONAL INPUTS
				//		none

				// OUTPUTS
				//		LN		Natural logarithm of input vector

				// Check number of inputs
				if (nrhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ln:NumInputs",
					"Number of input arguments to function 'ta_ln' is not correct. A single vector of values should be provided. Aborting (%d).",codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ln:NumOutputs",
					"The function 'ta_ln' (Vector Natural Logarithm) produces a single vector output that must be assigned. Aborting (%d).",codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN		prhs[1]

				// Outputs
				#define ln_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsD;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				colsD		= (int)mxGetN(data_IN);
				rows		= (int)mxGetM(data_IN);

				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_acos:InputErr",
						"Cosine data should be a single vector array. Aborting (%d).",codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int lnIdx, outElements;
				double *outReal;

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				// May have to change typeMA from decimal to name
				retCode = TA_LN(startIdx, endIdx, dataPtr, &lnIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgTxt("Invocation to 'ta_ln' failed. Aborting (4735).");
				}

				// Populate Output
				ln_OUT = mxCreateDoubleMatrix(lnIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(ln_OUT)) + lnIdx, outReal, outElements * mxGetElementSize(ln_OUT));

				// Cleanup
				mxFree(outReal); 

				break;
			}

		// Vector Base 10 Logarithm
		case ta_log10:       
			{
				// REQUIRED INPUTS
				//		input	any single column vector of Real values

				// OPTIONAL INPUTS
				//		none

				// OUTPUTS
				//		LOG10	Base 10 logarithm of input vector

				// Check number of inputs
				if (nrhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_log10:NumInputs",
					"Number of input arguments to function 'ta_log10' is not correct. A single vector of values should be provided. Aborting (%d).",codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ln:NumOutputs",
					"The function 'ta_log10' (Vector Base 10 Logarithm) produces a single vector output that must be assigned. Aborting (%d).",codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN		prhs[1]

				// Outputs
				#define log10_OUT	plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsD;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				colsD		= (int)mxGetN(data_IN);
				rows		= (int)mxGetM(data_IN);

				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_log10:InputErr",
						"Input data should be a single vector array. Aborting (%d).",codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int log10Idx, outElements;
				double *outReal;

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				// May have to change typeMA from decimal to name
				retCode = TA_LOG10(startIdx, endIdx, dataPtr, &log10Idx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgTxt("Invocation to 'ta_log10' failed. Aborting (4813).");
				}

				// Populate Output
				log10_OUT = mxCreateDoubleMatrix(log10Idx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(log10_OUT)) + log10Idx, outReal, outElements * mxGetElementSize(log10_OUT));

				// Cleanup
				mxFree(outReal); 

				break;
			}

		// Moving average
		case ta_ma:       
			{
				// REQUIRED INPUTS
				//		data		A single vector of observations

				// OPTIONAL INPUTS
				//		Lookback	lookback period	(default 14)
				//		typeMA		Moving average calculation type:
				//						0	-	Simple Moving Average				SMA	(default)
				//						1	-	Exponential Moving Average			EMA
				//						2	-	Weighted Moving Average				WMA
				//						3	-	Double Exponential Moving Average	DEMA
				//						4	-	Triple Exponential Moving Average	TEMA
				//						5	-	Triangular Moving Average			TRIMA
				//						6	-	Kaufman Adaptive Moving Average		KAMA
				//						7	-	MESA Adaptive Moving Average		MAMA
				//						8	-	Triple Exponential Moving Average	T3	

				// OUTPUT
				//		MA			vector of simple moving average values

				// Check number of inputs
				if (nrhs < 2 || nrhs > 4)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ma:NumInputs",
					"Number of input arguments to function 'ta_ma' is incorrect. Observation data should be parsed into a single input vector.\nOptional inputs are lookback, typeMA. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ma:NumOutputs",
					"The function 'ta_ma' (Moving Average) produces a single vector as output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN		prhs[1]

				// Outputs
				#define ma_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsD, lookback, typeMA;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				rows		= (int)mxGetM(data_IN);
				colsD		= (int)mxGetN(data_IN);

				// Validate
				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ma:InputErr",
						"Observational data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *outReal;

				// Get optional input or assign default
				if (nrhs > 2) 
				{
					#define lookback_IN	prhs[2]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The MOVING AVERAGE lookback must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);

					if(lookback < 1)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The MOVING AVERAGE lookback must be a scalar with a value of 1 or greater. Aborting (%d).", codeLine);
					}

					if(nrhs == 4)
					{
						#define typeMA_IN	prhs[3]
						if (!isRealScalar(typeMA_IN))
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The MOVING AVERAGE typeMA must be a scalar. Aborting (%d).", codeLine);

						/* Get the scalar input typeMA */
						// Assign
						typeMA = (int)mxGetScalar(typeMA_IN);

						// Validate
						if(typeMA < 0 || typeMA > 8)
						{
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
								"The MOVING AVERAGE typeMA must be a scalar with a value between 0 - 8. Aborting (%d).", codeLine);
						}
					}
					else
					{
						typeMA = 0;
					}
				}
				else
				{
					typeMA = 0;
					lookback = 14;
				}

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				retCode = TA_MA(startIdx, endIdx, dataPtr, lookback, (TA_MAType)typeMA, &dataIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				ma_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(ma_OUT)) + dataIdx, outReal, outElements * mxGetElementSize(ma_OUT));

				// Cleanup
				mxFree(outReal); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *outPtr = mxGetPr(ma_OUT);

				for (int iter = 0; iter < lookback; iter++)
				{
					outPtr[iter] = m_Nan;
				}

				break;
			}

		// Moving Average Convergence/Divergence
		case ta_macd:       
			{
				// REQUIRED INPUTS
				//		data		A single vector of observations

				// OPTIONAL INPUTS
				//		fastMA		Fast MA lookback	(default 12)
				//		slowMA		Slow MA lookback	(default 26)
				//		smoothP		Smoothing period	(default 9)

				// OUTPUTS
				//		MACD		Moving Average Convergence/Divergence
				//		MACD		SigMoving Average Convergence/Divergence Signal
				//		MACDHist	Moving Average Convergence/Divergence History

				// Check number of inputs
				if (nrhs < 2 || nrhs > 5)
					mexErrMsgIdAndTxt("MATLAB:taInvoke:ta_macd:NumInputs",
					"Number of input arguments to function 'ta_macd' is incorrect. Observation data should be parsed into a single input vector.\nOptional inputs are fastMA, slowMA, smoothP. Aborting (%d).", codeLine);
				if (nrhs == 3)
					mexErrMsgIdAndTxt("MATLAB:taInvoke:ta_macd:NumOutputs",
					"The function 'ta_macd' (Moving Average Convergence/Divergence) cannot interpret the ambiguous input provided. Aborting (%d).",codeLine);

				if (nlhs != 3)
					mexErrMsgIdAndTxt("MATLAB:taInvoke:ta_macd:NumOutputs",
					"The function 'ta_macd' (Moving Average Convergence/Divergence) produces 3 vector outputs that must be assigned. Aborting (%d).",codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define macd_OUT		plhs[0]
				#define macdSig_OUT		plhs[1]
				#define macdHist_OUT	plhs[2]

				// Declare variables
				int startIdx, endIdx, rows, colsD, lookback, fastMA, slowMA, smoothP;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				rows		= (int)mxGetM(data_IN);
				colsD		= (int)mxGetN(data_IN);

				// Validate
				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_macd:InputErr",
						"Observational data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *macd, *macdSig, *macdHist;

				// Parse optional inputs if given, else default 
				if (nrhs > 2) 
				{
					#define fastMA_IN	prhs[2]
					#define slowMA_IN	prhs[3]

					// Validate
					if (!isRealScalar(fastMA_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The MACD fastMA input must be a scalar. Aborting (%d).",codeLine);
					if (!isRealScalar(slowMA_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The MACD slowMA input must be a scalar. Aborting (%d).",codeLine);

					/* Get the scalar input lookback */
					// Assign
					fastMA = (int)mxGetScalar(fastMA_IN);
					slowMA = (int)mxGetScalar(slowMA_IN);

					// Validate
					if (fastMA > slowMA)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The MACD fastMA must be less than or equal to slowMA input. Aborting (%d).",codeLine);
					}

					if (fastMA < 2 || slowMA < 2)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The MACD average lookback values must be greater than or equal to 2. Aborting (%d).",codeLine);
					}

					if(nrhs == 5)
					{
						#define smoothP_IN	prhs[4]

						if (!isRealScalar(smoothP_IN))
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The MACD smoothing lookback input must be a scalar. Aborting (%d).",codeLine);

						smoothP = (int)mxGetScalar(smoothP_IN);

						// Validate
						if (smoothP < 1)
						{
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
								"The MACD smoothing lookback value must be greater than or equal to 1. Aborting (%d).",codeLine);
						}

					}
					else
					{
						smoothP = 9;
					}
				}
				else
				// Assign defaults
				{
					fastMA = 12;
					slowMA = 26;
					smoothP = 9;

					if (rows < slowMA || rows < fastMA)
					{
						fastMA = int(rows / 2);
						slowMA = rows;
					}

					if (rows < 9)
					{
						smoothP = rows;
					}
				}

				// Preallocate heap
				macd		= (double*)mxCalloc(rows, sizeof(double));
				macdSig		= (double*)mxCalloc(rows, sizeof(double));
				macdHist	= (double*)mxCalloc(rows, sizeof(double));

				retCode = TA_MACD(startIdx, endIdx, dataPtr, fastMA, slowMA, smoothP, &dataIdx, &outElements, macd, macdSig, macdHist);

				// Error handling
				if (retCode) 
				{
					mxFree(macd);
					mxFree(macdSig);
					mxFree(macdHist);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (5121).", taFuncNameIn);
				}

				// Populate Output
				macd_OUT		= mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				macdSig_OUT		= mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				macdHist_OUT	= mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(macd_OUT)) + dataIdx, macd, outElements * mxGetElementSize(macd_OUT));
				memcpy(((double *) mxGetData(macdSig_OUT)) + dataIdx, macdSig, outElements * mxGetElementSize(macdSig_OUT));
				memcpy(((double *) mxGetData(macdHist_OUT)) + dataIdx, macdHist, outElements * mxGetElementSize(macdHist_OUT));

				// Cleanup
				mxFree(macd); 
				mxFree(macdSig); 
				mxFree(macdHist); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *macdPtr = mxGetPr(macd_OUT);
				double *macdSigPtr = mxGetPr(macdSig_OUT);
				double *macdHistPtr = mxGetPr(macdHist_OUT);

				for (int iter = 0; iter < slowMA; iter++)
				{
					macdPtr[iter] = m_Nan;
					macdSigPtr[iter] = m_Nan;
				}

				for (int iter = 0; iter < smoothP; iter++)
				{
					macdHistPtr[iter] = m_Nan;
				}
									
				break;
			}

		// MACD with controllable MA type
		case ta_macdext:       
			{
				// REQUIRED INPUTS
				//		data		A single vector of observations

				// OPTIONAL INPUTS
				//		fastMA		Fast MA lookback		(default 12)
				//		fastType	Fast MA type			(default 0)
				//		slowMA		Slow MA lookback		(default 26)
				//		slowType	Slow MA type			(default 0)
				//		smoothP		Smoothing period		(default 9)
				//		smoothType	Smoothing average type	(default 0)

				// OUTPUTS
				//		MACD		Moving Average Convergence/Divergence
				//		MACD		SigMoving Average Convergence/Divergence Signal
				//		MACDHist	Moving Average Convergence/Divergence History

				// Check number of inputs
				if (nrhs < 2 || nrhs > 8)
					mexErrMsgIdAndTxt("MATLAB:taInvoke:ta_macdext:NumInputs",
					"Number of input arguments to function 'ta_macdext' is incorrect. Observation data should be parsed into a single input vector.\nOptional inputs are fastMA, fastType, slowMA, slowType, smoothP, smoothType./nAdditional arguments must be added in pairs to prevent ambiguity. Aborting (%d).", codeLine);
				if (nrhs >= 3 && nrhs <= 5)
					mexErrMsgIdAndTxt("MATLAB:taInvoke:ta_macdext:NumOutputs",
					"The function 'ta_macdext' (Moving Average Convergence/Divergence) cannot interpret the ambiguous input provided.\nIf specifying fastMA you must also specify slowMA including average types. Aborting (%d).",codeLine);
				if (nrhs == 7)
					mexErrMsgIdAndTxt("MATLAB:taInvoke:ta_macdext:NumOutputs",
					"The function 'ta_macdext' (Moving Average Convergence/Divergence) cannot interpret the ambiguous input provided.\nIf specifying smoothP lookback you must also specify the smoothP average type. Aborting (%d).",codeLine);
				if (nlhs != 3)
					mexErrMsgIdAndTxt("MATLAB:taInvoke:ta_macdext:NumOutputs",
					"The function 'ta_macdext' (Moving Average Convergence/Divergence) produces 3 vector outputs that must be assigned. Aborting (%d).",codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define macd_OUT		plhs[0]
				#define macdSig_OUT		plhs[1]
				#define macdHist_OUT	plhs[2]

				// Declare variables
				int startIdx, endIdx, rows, colsD, lookback, fastMA, fastType, slowMA, slowType, smoothP, smoothType;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				rows		= (int)mxGetM(data_IN);
				colsD		= (int)mxGetN(data_IN);

				// Validate
				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_macdext:InputErr",
						"Observational data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *macd, *macdSig, *macdHist;

				// Parse optional inputs if given, else default 
				if (nrhs > 2) 
				{
					#define fastMA_IN	prhs[2]
					#define fastType_IN	prhs[3]
					#define slowMA_IN	prhs[4]
					#define slowType_IN	prhs[5]

					// Validate
					if (!isRealScalar(fastMA_IN))
						mexErrMsgIdAndTxt("MATLAB:taInvoke:inputErr",
						"The MACDEXT fastMA input must be a scalar. Aborting (%d).",codeLine);
					if (!isRealScalar(fastType_IN))
						mexErrMsgIdAndTxt("MATLAB:taInvoke:inputErr",
						"The MACDEXT fastType input must be a scalar. Aborting (%d).",codeLine);
					if (!isRealScalar(slowMA_IN))
						mexErrMsgIdAndTxt("MATLAB:taInvoke:inputErr",
						"The MACDEXT slowMA input must be a scalar. Aborting (%d).",codeLine);
					if (!isRealScalar(slowType_IN))
						mexErrMsgIdAndTxt("MATLAB:taInvoke:inputErr",
						"The MACDEXT slowType input must be a scalar. Aborting (%d).",codeLine);

					/* Get the scalar input lookback */
					// Assign
					fastMA = (int)mxGetScalar(fastMA_IN);
					fastType = (int)mxGetScalar(fastType_IN);
					slowMA = (int)mxGetScalar(slowMA_IN);
					slowType = (int)mxGetScalar(slowType_IN);

					// Validate
					if (fastMA > slowMA)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The MACDEXT fastMA must be less than or equal to slowMA input. Aborting (%d).",codeLine);
					}

					if (fastMA < 2 || slowMA < 2)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The MACDEXt average lookback values must be greater than or equal to 2. Aborting (%d).",codeLine);
					}

					if (slowType < 0 || slowType > 8 || fastType < 0 || fastType > 8)
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The MACDEXT average types values must be a value between 0 - 8. Aborting (%d).",codeLine);

					if(nrhs == 8)
					{
						#define smoothP_IN		prhs[6]
						#define smoothType_IN	prhs[7]

						if (!isRealScalar(smoothP_IN))
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The MACDEXT smoothing lookback input must be a scalar. Aborting (%d).",codeLine);
						if (!isRealScalar(smoothType_IN))
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The MACDEXT smoothing average type must be a scalar. Aborting (%d).",codeLine);

						smoothP = (int)mxGetScalar(smoothP_IN);
						smoothType = (int)mxGetScalar(smoothType_IN);

						// Validate
						if (smoothP < 1)
						{
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
								"The MACDEXT smoothing lookback value must be greater than or equal to 1. Aborting (%d).",codeLine);
						}
						if (smoothType < 0 || smoothType > 8)
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The MACDEXT smoothing average type must be a value between 0 - 8. Aborting (%d).",codeLine);
					}
					else
					{
						smoothP = 9;
						smoothType = 0;
					}
				}
				else
					// Assign defaults
				{
					fastMA = 12;
					fastType = 0;
					slowMA = 26;
					slowType = 0;
					smoothP = 9;
					smoothType = 0;

					if (rows < slowMA || rows < fastMA)
					{
						fastMA = int(rows / 2);
						slowMA = rows;
					}

					if (rows < 9)
					{
						smoothP = rows;
					}
				}

				// Preallocate heap
				macd		= (double*)mxCalloc(rows, sizeof(double));
				macdSig		= (double*)mxCalloc(rows, sizeof(double));
				macdHist	= (double*)mxCalloc(rows, sizeof(double));

				retCode = TA_MACDEXT(startIdx, endIdx, dataPtr, fastMA, (TA_MAType)fastType, slowMA, (TA_MAType)slowType, smoothP, (TA_MAType)smoothType, &dataIdx, &outElements, macd, macdSig, macdHist);

				// Error handling
				if (retCode) 
				{
					mxFree(macd);
					mxFree(macdSig);
					mxFree(macdHist);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (5121).", taFuncNameIn);
				}

				// Populate Output
				macd_OUT		= mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				macdSig_OUT		= mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				macdHist_OUT	= mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(macd_OUT)) + dataIdx, macd, outElements * mxGetElementSize(macd_OUT));
				memcpy(((double *) mxGetData(macdSig_OUT)) + dataIdx, macdSig, outElements * mxGetElementSize(macdSig_OUT));
				memcpy(((double *) mxGetData(macdHist_OUT)) + dataIdx, macdHist, outElements * mxGetElementSize(macdHist_OUT));

				// Cleanup
				mxFree(macd); 
				mxFree(macdSig); 
				mxFree(macdHist); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *macdPtr = mxGetPr(macd_OUT);
				double *macdSigPtr = mxGetPr(macdSig_OUT);
				double *macdHistPtr = mxGetPr(macdHist_OUT);

				for (int iter = 0; iter < slowMA; iter++)
				{
					macdPtr[iter] = m_Nan;
					macdSigPtr[iter] = m_Nan;
				}

				for (int iter = 0; iter < smoothP; iter++)
				{
					macdHistPtr[iter] = m_Nan;
				}

				break;
			}

		// Moving Average Convergence/Divergence Fixed 12|26)
		case ta_macdfix:       
			{
				// REQUIRED INPUTS
				//		data		A single vector of observations

				// OPTIONAL INPUTS
				//		smoothP		Smoothing period lookback	(default 9)

				// OUTPUTS
				//		MACD		Moving Average Convergence/Divergence
				//		MACD		SigMoving Average Convergence/Divergence Signal
				//		MACDHist	Moving Average Convergence/Divergence History

				// Check number of inputs
				if (nrhs < 2 || nrhs > 3)
					mexErrMsgIdAndTxt("MATLAB:taInvoke:ta_macdfix:NumInputs",
					"Number of input arguments to function 'ta_macdfix' is incorrect. Observation data should be parsed into a single input vector.\nOptional input is smoothing period. Aborting (%d).", codeLine);
				if (nlhs != 3)
					mexErrMsgIdAndTxt("MATLAB:taInvoke:ta_macdfix:NumOutputs",
					"The function 'ta_macdfix' (Moving Average Convergence/Divergence Fixed 12|26) produces 3 vector outputs that must be assigned. Aborting (%d).",codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define macd_OUT		plhs[0]
				#define macdSig_OUT		plhs[1]
				#define macdHist_OUT	plhs[2]

				// Declare variables
				int startIdx, endIdx, rows, colsD, smoothP;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				rows		= (int)mxGetM(data_IN);
				colsD		= (int)mxGetN(data_IN);

				// Validate
				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_macdfix:InputErr",
						"Observational data should be a single vector array. Aborting (%d).", codeLine);
				}

				if (rows < 26)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_macdfix:InputErr",
						"'ta_macdfix' requires a minimum of 26 observations. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *macd, *macdSig, *macdHist;

				// Parse optional inputs if given, else default 
				if (nrhs > 2) 
				{
					#define smoothP_IN	prhs[2]

					// Validate
					if (!isRealScalar(smoothP_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The MACDFIX smoothing lookback input must be a scalar. Aborting (%d).",codeLine);

					/* Get the scalar input lookback */
					// Assign
					smoothP = (int)mxGetScalar(smoothP_IN);

					// Validate
					if (smoothP < 1)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The MACDFEX smoothing lookback must be greater than or equal to 1. Aborting (%d).",codeLine);
					}
				}
				else
					// Assign defaults
				{
					smoothP = 9;
				}

				// Preallocate heap
				macd		= (double*)mxCalloc(rows, sizeof(double));
				macdSig		= (double*)mxCalloc(rows, sizeof(double));
				macdHist	= (double*)mxCalloc(rows, sizeof(double));

				retCode = TA_MACDFIX(startIdx, endIdx, dataPtr, smoothP, &dataIdx, &outElements, macd, macdSig, macdHist);

				// Error handling
				if (retCode) 
				{
					mxFree(macd);
					mxFree(macdSig);
					mxFree(macdHist);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (5121).", taFuncNameIn);
				}

				// Populate Output
				macd_OUT		= mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				macdSig_OUT		= mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				macdHist_OUT	= mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(macd_OUT)) + dataIdx, macd, outElements * mxGetElementSize(macd_OUT));
				memcpy(((double *) mxGetData(macdSig_OUT)) + dataIdx, macdSig, outElements * mxGetElementSize(macdSig_OUT));
				memcpy(((double *) mxGetData(macdHist_OUT)) + dataIdx, macdHist, outElements * mxGetElementSize(macdHist_OUT));

				// Cleanup
				mxFree(macd); 
				mxFree(macdSig); 
				mxFree(macdHist); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *macdPtr = mxGetPr(macd_OUT);
				double *macdSigPtr = mxGetPr(macdSig_OUT);
				double *macdHistPtr = mxGetPr(macdHist_OUT);

				for (int iter = 0; iter < 26; iter++)
				{
					macdPtr[iter] = m_Nan;
					macdSigPtr[iter] = m_Nan;
				}

				for (int iter = 0; iter < smoothP; iter++)
				{
					macdHistPtr[iter] = m_Nan;
				}

				break;
			}

		// MESA Adaptive Moving Average
		case ta_mama:       
			{
				// REQUIRED INPUTS
				//		data		A single vector of observations

				// OPTIONAL INPUTS
				//		fastLmt		Upper limit used in the adaptive algorithm (0.01 - 0.99, default: 0.5)
				//		slowLmt		Lower limit used in the adaptive algorithm (0.01 - 0.99, default: 0.05)

				// OUTPUT
				//		MAMA		vector of MESA Adaptive Moving Average values
				//		FAMA

				// Description: “The MESA Adaptive Moving Average (MAMA) adapts to price movement based on the 
				//	rate of change of phase as measured by the Hilbert Transform Discriminator 
				//	(Technical Analysis of Stocks and Commodities magazine, December 2000). 
				//	This method features a fast attack average and a slow decay average so that composite average 
				//	rapidly ratchets behind price changes and holds the average value until the next ratchet occurs.” 
				//
				//	MAMA and FAMA lines can be used to generate Buy and Sell signals. 
				//	When the MAMA crosses above the FAMA a buy signal is given. 
				//	Alternatively, when the MAMA crosses below the FAMA a sell signal is given.


				// Check number of inputs
				if (nrhs < 2 || nrhs > 4)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_mama:NumInputs",
					"Number of input arguments to function 'ta_mama' is incorrect. Observation data should be parsed into a single input vector.\nOptional inputs are fastLmt and slowLmt. Aborting (%d).", codeLine);
				if (nlhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_mama:NumOutputs",
					"The function 'ta_mama' (MESA Adaptive Moving Average) produces two vectors as output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define mama_OUT		plhs[0]
				#define fama_OUT		plhs[1]

				// Declare variables
				int startIdx, endIdx, rows, colsD, lookback;
				double *dataPtr, fastLmt, slowLmt;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				rows		= (int)mxGetM(data_IN);
				colsD		= (int)mxGetN(data_IN);

				// Validate
				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_mama:InputErr",
						"Observational data should be a single vector array. Aborting (%d).", codeLine);
				}

				if (rows < 32)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_mama:InputErr",
						"'ta_mama' requires a minimum of 32 observations. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *mama, *fama;

				// Get optional input or assign default
				if (nrhs >= 3) 
				{
					#define fastLmt_IN	prhs[2]
					if (!isRealScalar(fastLmt_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The MESA ADAPTIVE MOVING AVERAGE fastLmt must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					fastLmt = (double)mxGetScalar(fastLmt_IN);

					// Validate
					if(fastLmt < .01 || fastLmt > .99)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The MESA ADAPTIVE MOVING AVERAGE fastLmt must be a scalar with a value between 0.01 - 0.99. Aborting (%d).", codeLine);
					}

					slowLmt = 0.05;

					if (nrhs == 4) 
					{
						#define slowLmt_IN	prhs[3]
						if (!isRealScalar(slowLmt_IN))
							mexErrMsgIdAndTxt("MATLAB:taInvoke:inputErr",
							"The MESA ADAPTIVE MOVING AVERAGE slowLmt must be a scalar. Aborting (%d).", codeLine);

						/* Get the scalar input lookback */
						// Assign
						slowLmt = (double)mxGetScalar(slowLmt_IN);

						// Validate
						if(slowLmt < .01 || slowLmt > .99)
						{
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
								"The MESA ADAPTIVE MOVING AVERAGE slowLmt must be a scalar with a value between 0.01 - 0.99. Aborting (%d).", codeLine);
						}
					}
					else
					{
						fastLmt = 0.5;
					}
				}
				else
				// Defaults
				{
					fastLmt = 0.5;
					slowLmt = 0.05;
				}

				if (slowLmt > fastLmt)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The MESA ADAPTIVE MOVING AVERAGE slowLmt must be less than or equal to the fastLmt. Aborting (%d).", codeLine);
				}

				// Preallocate heap
				mama = (double*)mxCalloc(rows, sizeof(double));
				fama = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				retCode = TA_MAMA(startIdx, endIdx, dataPtr, fastLmt, slowLmt, &dataIdx, &outElements, mama, fama);

				// Error handling
				if (retCode) 
				{
					mxFree(mama);
					mxFree(fama);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				mama_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				fama_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(mama_OUT)) + dataIdx, mama, outElements * mxGetElementSize(mama_OUT));
				memcpy(((double *) mxGetData(fama_OUT)) + dataIdx, fama, outElements * mxGetElementSize(fama_OUT));

				// Cleanup
				mxFree(mama); 
				mxFree(fama); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *mamaPtr = mxGetPr(mama_OUT);
				double *famaPtr = mxGetPr(fama_OUT);

				for (int iter = 0; iter < 32; iter++)
				{
					mamaPtr[iter] = m_Nan;
					famaPtr[iter] = m_Nan;
				}

				break;
			}

		// Moving average with variable period
		case ta_mavp:       
			{
				// REQUIRED INPUTS
				//		data		A single vector of observations
				//		period		lookback

				// OPTIONAL INPUTS
				//		minPeriod	Minimum lookback period threshold	(default: 2)
				//					Period values less than minPeriod will be changed to minPeriod
				//		maxPeriod	Maximum lookback period threshold	(default: 30)
				//					Period values greater than maxPeriod will be changed to minPeriod
				//		typeMA		Moving average calculation type:
				//						0	-	Simple Moving Average				SMA	(default)
				//						1	-	Exponential Moving Average			EMA
				//						2	-	Weighted Moving Average				WMA
				//						3	-	Double Exponential Moving Average	DEMA
				//						4	-	Triple Exponential Moving Average	TEMA
				//						5	-	Triangular Moving Average			TRIMA
				//						6	-	Kaufman Adaptive Moving Average		KAMA
				//						7	-	MESA Adaptive Moving Average		MAMA
				//						8	-	Triple Exponential Moving Average	T3	

				// OUTPUT
				//		MAVP		vector of moving average variable period values

				// Check number of inputs
				if (nrhs < 3 || nrhs > 6)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_mavp:NumInputs",
					"Number of input arguments to function 'ta_mavp' is incorrect. Observation data should be parsed into a single input vector and an initial lookback period provided.\nOptional inputs are minPeriod, maxPeriod, typeMA. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_mavp:NumOutputs",
					"The function 'ta_mavp' (Moving average with variable period) produces a single vector as output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]
				#define period_IN		prhs[2]

				// Outputs
				#define mavp_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsD, minPeriod, maxPeriod, typeMA;
				double *dataPtr, *periodPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				rows		= (int)mxGetM(data_IN);
				colsD		= (int)mxGetN(data_IN);
				

				// Validate
				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_mavp:InputErr",
						"Observational data should be a single vector array. Aborting (%d).", codeLine);
				}

				if (!isRealScalar(period_IN))
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
					"The MOVING AVERAGE VARIABLE PERIOD period must be a scalar. Aborting (%d).", codeLine);

				periodPtr		= mxGetPr(period_IN);

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *mavp;

				// Get optional input or assign default
				if (nrhs >= 4) 
				{
					#define minPeriod_IN	prhs[3]
					if (!isRealScalar(minPeriod_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The MOVING AVERAGE VARIABLE PERIOD minPeriod must be a scalar. Aborting (%d).", codeLine);

					// Assign
					minPeriod = (int)mxGetScalar(minPeriod_IN);

					// Validate
					if(minPeriod < 2)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The MOVING AVERAGE VARIABLE PERIOD minPeriod must be a scalar with a value greater than or equal to 2. Aborting (%d).", codeLine);
					}

					if (nrhs >= 5) 
					{
						#define maxPeriod_IN	prhs[4]
						if (!isRealScalar(maxPeriod_IN))
							mexErrMsgIdAndTxt("MATLAB:taInvoke:inputErr",
							"The MOVING AVERAGE VARIABLE PERIOD maxPeriod must be a scalar with a value greater than or equal to 2. Aborting (%d).", codeLine);

						// Assign
						maxPeriod = (int)mxGetScalar(maxPeriod_IN);

						// Validate
						if(maxPeriod < 2)
						{
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
								"The MOVING AVERAGE VARIABLE PERIOD maxPeriod must be a scalar with a value greater than or equal to 2. Aborting (%d).", codeLine);
						}

						if (nrhs == 6)
						{
							#define typeMA_IN	prhs[5]
							if (!isRealScalar(typeMA_IN))
								mexErrMsgIdAndTxt("MATLAB:taInvoke:inputErr",
								"The MOVING AVERAGE VARIABLE PERIOD typeMA must be a scalar with a value between 0 - 8. Aborting (%d).", codeLine);

							// Assign
							typeMA = (int)mxGetScalar(typeMA_IN);

							// Validate
							if(typeMA < 0 || typeMA > 8)
							{
								mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
									"The MOVING AVERAGE VARIABLE PERIOD typeMA must be a scalar with a value between 0 - 8. Aborting (%d).", codeLine);
							}
						}
						else
						{
							typeMA = 0;
						}
					}
					else
					{
						maxPeriod = 30;
						typeMA = 0;
					}
				}
				else
					// Defaults
				{
					minPeriod = 2;
					maxPeriod = 30;
					typeMA = 0;
				}

				if (minPeriod > maxPeriod)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The MESA ADAPTIVE MOVING AVERAGE minPeriod must be less than or equal to the maxPeriod. Aborting (%d).", codeLine);
				}

				// Preallocate heap
				mavp = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				retCode = TA_MAVP(startIdx, endIdx, dataPtr, periodPtr, minPeriod, maxPeriod, (TA_MAType)typeMA, &dataIdx, &outElements, mavp);

				// Error handling
				if (retCode) 
				{
					mxFree(mavp);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				mavp_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(mavp_OUT)) + dataIdx, mavp, outElements * mxGetElementSize(mavp_OUT));

				// Cleanup
				mxFree(mavp); 

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *mavpPtr = mxGetPr(mavp_OUT);

				// Get the larger of the minPeriod or the original lookback period
				int isGtr = max((int)*periodPtr, minPeriod);

				for (int iter = 0; iter < isGtr; iter++)
				{
					mavpPtr[iter] = m_Nan;
				}

				break;
			}

		// Maximum value over a specified period
		case ta_max:       
			{
				// REQUIRED INPUTS
				//		data		A single vector of observations

				// OPTIONAL INPUTS
				//		Lookback	lookback period	(default 30)

				// OUTPUT
				//		MAX			Vector of maximum value within lookback period


				// Check number of inputs
				if (nrhs < 2 || nrhs > 3)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_max:NumInputs",
					"Number of input arguments to function 'ta_max' is incorrect. Observation data should be parsed into a single input vector. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_max:NumOutputs",
					"The function 'ta_max' (Maximum value over a specified period) produces a single vector as output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN		prhs[1]

				// Outputs
				#define max_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsD, lookback;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				rows		= (int)mxGetM(data_IN);
				colsD		= (int)mxGetN(data_IN);

				// Validate
				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_max:InputErr",
						"Observational data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *outReal;

				// Get optional input or assign default
				if (nrhs == 3) 
				{
					#define lookback_IN	prhs[2]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The MAXIMUM VALUE OVER A SPECIFIED PERIOD lookback must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);

					if(lookback < 2)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The MAXIMUM VALUE OVER A SPECIFIED PERIOD lookback must be a scalar with a value of 2 or greater. Aborting (%d).", codeLine);
					}
				}
				else
					// Default lookback period
				{
					lookback = 30;
				}

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				retCode = TA_MAX(startIdx, endIdx, dataPtr, lookback, &dataIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				max_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(max_OUT)) + dataIdx, outReal, outElements * mxGetElementSize(max_OUT));

				// Cleanup
				mxFree(outReal); 

				break;
			}

		// Index of maximum value over a specified period
		case ta_maxindex:       
			{
				// REQUIRED INPUTS
				//		data		A single vector of observations

				// OPTIONAL INPUTS
				//		Lookback	lookback period	(default 30)

				// OUTPUT
				//		MAXIDX		Vector of index locations of maximum value within lookback period


				// Check number of inputs
				if (nrhs < 2 || nrhs > 3)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_maxindex:NumInputs",
					"Number of input arguments to function 'ta_maxindex' is incorrect. Observation data should be parsed into a single input vector. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_maxindex:NumOutputs",
					"The function 'ta_maxindex' (Index of maximum value over a specified period) produces a single vector as output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define maxidx_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsD, lookback;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				rows		= (int)mxGetM(data_IN);
				colsD		= (int)mxGetN(data_IN);

				// Validate
				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_maxindex:InputErr",
						"Observational data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				int *outInt;

				// Get optional input or assign default
				if (nrhs == 3) 
				{
					#define lookback_IN	prhs[2]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The INDEX OF MAXIMUM VALUE OVER A SPECIFIED PERIOD lookback must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);

					if(lookback < 2)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The INDEX OF MAXIMUM VALUE OVER A SPECIFIED PERIOD lookback must be a scalar with a value of 2 or greater. Aborting (%d).", codeLine);
					}
				}
				else
					// Default lookback period
				{
					lookback = 30;
				}

				// Preallocate heap
				outInt = (int*)mxCalloc(rows, sizeof(int));

				// Invoke with error catch
				retCode = TA_MAXINDEX(startIdx, endIdx, dataPtr, lookback, &dataIdx, &outElements, outInt);

				// Error handling
				if (retCode) 
				{
					mxFree(outInt);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				maxidx_OUT = mxCreateNumericMatrix(dataIdx + outElements,1, mxINT32_CLASS, mxREAL);
				memcpy(((int *) mxGetData(maxidx_OUT)) + dataIdx, outInt, outElements * mxGetElementSize(maxidx_OUT));

				// Cleanup
				mxFree(outInt); 

				break;
			}

		// Median Price
		case ta_medprice:       
			{
				// REQUIRED INPUTS
				//		High		A single vector of HIGH price observations
				//		Low			A single vector of LOW price observations

				// OPTIONAL INPUTS
				//		none

				// OUTPUT
				//		MED			Vector of median prices derived from provided inputs


				// Check number of inputs
				if (nrhs != 3)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_medprice:NumInputs",
					"Number of input arguments to function 'ta_maxindex' is incorrect. Observation data should be parsed into two input vectors (e.g. High | Low). Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_medprice:NumOutputs",
					"The function 'ta_medprice' (Median Price) produces a single vector as output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define high_IN			prhs[1]
				#define low_IN			prhs[2]

				// Outputs
				#define med_OUT			plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsH, colsL;
				double *highPtr, *lowPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				highPtr		= mxGetPr(high_IN);
				lowPtr		= mxGetPr(low_IN);
				rows		= (int)mxGetM(high_IN);
				colsH		= (int)mxGetN(high_IN);
				colsL		= (int)mxGetN(low_IN);

				// Validate
				if (colsH != 1 || colsL != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_medprice:InputErr",
						"Observational data should be provided as single vector arrays. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *outReal;

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				retCode = TA_MEDPRICE(startIdx, endIdx, highPtr, lowPtr, &dataIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				med_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(med_OUT)) + dataIdx, outReal, outElements * mxGetElementSize(med_OUT));

				// Cleanup
				mxFree(outReal); 

				break;
			}

		// Money Flow Index
		case ta_mfi:       
			{
				// REQUIRED INPUTS
				//		Price		H | L | C	separate vectors
				//		Volume

				// OPTIONAL INPUTS
				//		Lookback	lookback period	(default 14)	

				// OUTPUT
				//		MFI			vector of Chaikin advance / decline line values

				// Check number of inputs
				if (nrhs < 5 || nrhs > 6)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_mfi:NumInputs",
					"Number of input arguments to function 'ta_mfi' is incorrect. Price data should be parsed into vectors H | L | C followed by a volume vector V.\nOptional input of lookback may be provided. Aborting (%d).",codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_mfi:NumOutputs",
					"The function 'ta_mfi' (Money Flow Index) produces a single vector output that must be assigned. Aborting (%d).",codeLine);

				// Create constants for readability
				// Inputs
				#define high_IN		prhs[1]
				#define low_IN		prhs[2]
				#define close_IN	prhs[3]
				#define vol_IN		prhs[4]

				// Outputs
				#define mfi_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsH, colsL, colsC, colsV, lookback;
				double *highPtr, *lowPtr, *closePtr, *volPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse inputs and error check
				// Assign pointers and get dimensions
				highPtr		= mxGetPr(high_IN);
				rows		= (int)mxGetM(high_IN);
				colsH		= (int)mxGetN(high_IN);
				lowPtr		= mxGetPr(low_IN);
				colsL		= (int)mxGetN(low_IN);
				closePtr	= mxGetPr(close_IN);
				colsC		= (int)mxGetN(close_IN);
				volPtr		= mxGetPr(vol_IN);
				colsV		= (int)mxGetN(vol_IN);

				// Input validation
				chkSingleVec(colsH, colsL, colsC, codeLine);

				if (colsV != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_mfi:InputErr",
						"Volume data should be a single vector array. Aborting (%d).",codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int adIdx, outElements;
				double *outReal;

				// Get optional input or assign default
				if (nrhs == 6) 
				{
					#define lookback_IN	prhs[5]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The MONEY FLOW INDEX lookback must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);

					if(lookback < 2)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The MONEY FLOW INDEX lookback must be a scalar with a value of 2 or greater. Aborting (%d).", codeLine);
					}
				}
				else
				// Default lookback period
				{
					lookback = 14;
				}

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));			// added cast

				// Invoke with error catch
				retCode = TA_MFI(startIdx, endIdx, highPtr, lowPtr, closePtr, volPtr, lookback, &adIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgTxt("Invocation to 'ta_ad' failed. Aborting.");
				}

				// Populate Output
				mfi_OUT = mxCreateDoubleMatrix(adIdx + outElements,1, mxREAL);
				memcpy(((double*) mxGetData(mfi_OUT)) + adIdx, outReal, outElements * mxGetElementSize(mfi_OUT));

				// Cleanup
				mxFree(outReal);  

				// NaN data before lookback
				// assign the variables for manipulating the arrays (by pointer reference)
				double *outPtr = mxGetPr(ma_OUT);

				for (int iter = 0; iter < lookback; iter++)
				{
					outPtr[iter] = m_Nan;
				}

				break;
			}

		// MidPoint over period
		case ta_midpoint:       
			{
				// REQUIRED INPUTS
				//		data		A single vector of observations

				// OPTIONAL INPUTS
				//		Lookback	lookback period	(default 14)

				// OUTPUT
				//		MID		Vector of midpoint values within lookback period


				// Check number of inputs
				if (nrhs < 2 || nrhs > 3)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_midpoint:NumInputs",
					"Number of input arguments to function 'ta_midpoint' is incorrect. Observation data should be parsed into a single input vector. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_midpoint:NumOutputs",
					"The function 'ta_midpoint' (MidPoint over period) produces a single vector as output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define midpt_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsD, lookback;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				rows		= (int)mxGetM(data_IN);
				colsD		= (int)mxGetN(data_IN);

				// Validate
				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_midpoint:InputErr",
						"Observational data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *outReal;

				// Get optional input or assign default
				if (nrhs == 3) 
				{
					#define lookback_IN	prhs[2]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The MIDPOINT OVER PERIOD lookback must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);

					if(lookback < 2)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The MIDPOINT OVER PERIOD lookback must be a scalar with a value of 2 or greater. Aborting (%d).", codeLine);
					}
				}
				else
					// Default lookback period
				{
					lookback = 14;
				}

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				retCode = TA_MIDPOINT(startIdx, endIdx, dataPtr, lookback, &dataIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				midpt_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(midpt_OUT)) + dataIdx, outReal, outElements * mxGetElementSize(midpt_OUT));

				// Cleanup
				mxFree(outReal); 

				break;
			}

		// Midpoint Price over period
		case ta_midprice:       
			{
				// REQUIRED INPUTS
				//		High		A single vector of HIGH price observations
				//		Low			A single vector of LOW price observations

				// OPTIONAL INPUTS
				//		Lookback	lookback period	(default 14)

				// OUTPUT
				//		MID		Vector of midprice values within lookback period


				// Check number of inputs
				if (nrhs < 3 || nrhs > 4)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_midprice:NumInputs",
					"Number of input arguments to function 'ta_midprice' is incorrect. Observation data should be parsed into two input vector (e.g. High | Low). Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_midprice:NumOutputs",
					"The function 'ta_midprice' (Midpoint Price over period) produces a single vector as output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define high_IN			prhs[1]
				#define low_IN			prhs[2]

				// Outputs
				#define midpr_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsH, colsL, lookback;
				double *highPtr, *lowPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				highPtr		= mxGetPr(high_IN);
				lowPtr		= mxGetPr(low_IN);
				rows		= (int)mxGetM(high_IN);
				colsH		= (int)mxGetN(high_IN);
				colsL		= (int)mxGetN(low_IN);

				// Validate
				if (colsH != 1 || colsL != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_midprice:InputErr",
						"Observational data should be provided as single vector arrays. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *outReal;

				// Get optional input or assign default
				if (nrhs == 4) 
				{
					#define lookback_IN	prhs[3]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The MIDPOINT PRICE OVER PERIOD lookback must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);

					if(lookback < 2)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The MIDPOINT PRICE OVER PERIOD lookback must be a scalar with a value of 2 or greater. Aborting (%d).", codeLine);
					}
				}
				else
					// Default lookback period
				{
					lookback = 14;
				}

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				retCode = TA_MIDPRICE(startIdx, endIdx, highPtr, lowPtr, lookback, &dataIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				midpr_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(midpr_OUT)) + dataIdx, outReal, outElements * mxGetElementSize(midpr_OUT));

				// Cleanup
				mxFree(outReal); 

				break;
			}
		
		// Minimum value over a specified period
		case ta_min:       
			{
				// REQUIRED INPUTS
				//		data		A single vector of observations

				// OPTIONAL INPUTS
				//		Lookback	lookback period	(default 30)

				// OUTPUT
				//		MIN			Vector of minimum values within lookback period


				// Check number of inputs
				if (nrhs < 2 || nrhs > 3)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_min:NumInputs",
					"Number of input arguments to function 'ta_min is incorrect. Observation data should be parsed into a single input vector. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_max:NumOutputs",
					"The function 'ta_min' (Minimum value over a specified period) produces a single vector as output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN		prhs[1]

				// Outputs
				#define min_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsD, lookback;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				rows		= (int)mxGetM(data_IN);
				colsD		= (int)mxGetN(data_IN);

				// Validate
				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_min:InputErr",
						"Observational data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *outReal;

				// Get optional input or assign default
				if (nrhs == 3) 
				{
					#define lookback_IN	prhs[2]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The MINIMUM VALUE OVER A SPECIFIED PERIOD lookback must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);

					if(lookback < 2)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The MINIMUM VALUE OVER A SPECIFIED PERIOD lookback must be a scalar with a value of 2 or greater. Aborting (%d).", codeLine);
					}
				}
				else
					// Default lookback period
				{
					lookback = 30;
				}

				// Preallocate heap
				outReal = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				retCode = TA_MIN(startIdx, endIdx, dataPtr, lookback, &dataIdx, &outElements, outReal);

				// Error handling
				if (retCode) 
				{
					mxFree(outReal);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				min_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(min_OUT)) + dataIdx, outReal, outElements * mxGetElementSize(min_OUT));

				// Cleanup
				mxFree(outReal); 

				break;
			}

		// Index of minimum values over a specified period
		case ta_minindex:       
			{
				// REQUIRED INPUTS
				//		data		A single vector of observations

				// OPTIONAL INPUTS
				//		Lookback	lookback period	(default 30)

				// OUTPUT
				//		MINIDX		Vector of index locations of minimum value within lookback period


				// Check number of inputs
				if (nrhs < 2 || nrhs > 3)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_minindex:NumInputs",
					"Number of input arguments to function 'ta_minindex' is incorrect. Observation data should be parsed into a single input vector. Aborting (%d).", codeLine);
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_minindex:NumOutputs",
					"The function 'ta_minindex' (Index of minimum values over a specified period) produces a single vector as output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define minidx_OUT		plhs[0]

				// Declare variables
				int startIdx, endIdx, rows, colsD, lookback;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				rows		= (int)mxGetM(data_IN);
				colsD		= (int)mxGetN(data_IN);

				// Validate
				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_minindex:InputErr",
						"Observational data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				int *outInt;

				// Get optional input or assign default
				if (nrhs == 3) 
				{
					#define lookback_IN	prhs[2]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The INDEX OF MINIMUM VALUE OVER A SPECIFIED PERIOD lookback must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);

					if(lookback < 2)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The INDEX OF MINIMUM VALUE OVER A SPECIFIED PERIOD lookback must be a scalar with a value of 2 or greater. Aborting (%d).", codeLine);
					}
				}
				else
				// Default lookback period
				{
					lookback = 30;
				}

				// Preallocate heap
				outInt = (int*)mxCalloc(rows, sizeof(int));

				// Invoke with error catch
				retCode = TA_MININDEX(startIdx, endIdx, dataPtr, lookback, &dataIdx, &outElements, outInt);

				// Error handling
				if (retCode) 
				{
					mxFree(outInt);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				minidx_OUT = mxCreateNumericMatrix(dataIdx + outElements,1, mxINT32_CLASS, mxREAL);
				memcpy(((int*) mxGetData(minidx_OUT)) + dataIdx, outInt, outElements * mxGetElementSize(minidx_OUT));

				// Cleanup
				mxFree(outInt); 

				break;
			}

		// Minimum and Maximum values over a specified period
		case ta_minmax:       
			{
				// REQUIRED INPUTS
				//		data		A single vector of observations

				// OPTIONAL INPUTS
				//		Lookback	lookback period	(default 30)

				// OUTPUT
				//		MIN			Vector of minimum values within the lookback period
				//		MAX			Vector of maximum values within the lookback period


				// Check number of inputs
				if (nrhs < 2 || nrhs > 3)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_max:NumInputs",
					"Number of input arguments to function 'ta_minmax' is incorrect. Observation data should be parsed into a single input vector. Aborting (%d).", codeLine);
				if (nlhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_max:NumOutputs",
					"The function 'ta_max' (Minimum and Maximum values over a specified period) produces two vector as output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN		prhs[1]

				// Outputs
				#define min_OUT		plhs[0]
				#define max_OUT		plhs[1]

				// Declare variables
				int startIdx, endIdx, rows, colsD, lookback;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				rows		= (int)mxGetM(data_IN);
				colsD		= (int)mxGetN(data_IN);

				// Validate
				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_max:InputErr",
						"Observational data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				double *outMin, *outMax;

				// Get optional input or assign default
				if (nrhs == 3) 
				{
					#define lookback_IN	prhs[2]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The MINIMUM AND MAXIMUM VALUES OVER A SPECIFIED PERIOD lookback must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);

					if(lookback < 2)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The MINIMUM AND MAXIMUM VALUE OVER A SPECIFIED PERIOD lookback must be a scalar with a value of 2 or greater. Aborting (%d).", codeLine);
					}
				}
				else
					// Default lookback period
				{
					lookback = 30;
				}

				// Preallocate heap
				outMin = (double*)mxCalloc(rows, sizeof(double));
				outMax = (double*)mxCalloc(rows, sizeof(double));

				// Invoke with error catch
				retCode = TA_MINMAX(startIdx, endIdx, dataPtr, lookback, &dataIdx, &outElements, outMin, outMax);

				// Error handling
				if (retCode) 
				{
					mxFree(outMin);
					mxFree(outMax);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				max_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				min_OUT = mxCreateDoubleMatrix(dataIdx + outElements,1, mxREAL);
				memcpy(((double *) mxGetData(min_OUT)) + dataIdx, outMin, outElements * mxGetElementSize(min_OUT));
				memcpy(((double *) mxGetData(max_OUT)) + dataIdx, outMax, outElements * mxGetElementSize(max_OUT));

				// Cleanup
				mxFree(outMin); 
				mxFree(outMax); 

				// NaN those values less than the lookback period
				double *minPtr = mxGetPr(min_OUT);
				double *maxPtr = mxGetPr(max_OUT);
				for (int iter = 0; iter < lookback - 1; iter++)
				{
					minPtr[iter] = m_Nan;
					maxPtr[iter] = m_Nan;
				}


				break;
				break;
			}

		// Indeces of minimum and maximum values over a specified period
		case ta_minmaxindex:       
			{
				// REQUIRED INPUTS
				//		data		A single vector of observations

				// OPTIONAL INPUTS
				//		Lookback	lookback period	(default 30)

				// OUTPUT
				//		MINIDX		Vector of minimum values index locations within the lookback period
				//		MAXIDX		Vector of maximum values index locations within the lookback period


				// Check number of inputs
				if (nrhs < 2 || nrhs > 3)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_max:NumInputs",
					"Number of input arguments to function 'ta_minmax' is incorrect. Observation data should be parsed into a single input vector. Aborting (%d).", codeLine);
				if (nlhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_max:NumOutputs",
					"The function 'ta_max' (Minimum and Maximum values over a specified period) produces two vector as output that must be assigned. Aborting (%d).", codeLine);

				// Create constants for readability
				// Inputs
				#define data_IN			prhs[1]

				// Outputs
				#define minIdx_OUT		plhs[0]
				#define maxIdx_OUT		plhs[1]

				// Declare variables
				int startIdx, endIdx, rows, colsD, lookback;
				double *dataPtr;

				// Initialize error handling 
				TA_RetCode retCode;

				// Parse required inputs and error check
				// Assign pointers and get dimensions
				dataPtr		= mxGetPr(data_IN);
				rows		= (int)mxGetM(data_IN);
				colsD		= (int)mxGetN(data_IN);

				// Validate
				if (colsD != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_max:InputErr",
						"Observational data should be a single vector array. Aborting (%d).", codeLine);
				}

				endIdx = rows - 1;  // Adjust for C++ starting at '0'
				startIdx = 0;

				// Output variables
				int dataIdx, outElements;
				int *outMinIdx, *outMaxIdx;

				// Get optional input or assign default
				if (nrhs == 3) 
				{
					#define lookback_IN	prhs[2]
					if (!isRealScalar(lookback_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The INDICES OF MINIMUM AND MAXIMUM VALUES OVER A SPECIFIED PERIOD lookback must be a scalar. Aborting (%d).", codeLine);

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);

					if(lookback < 2)
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The INDICES OF MINIMUM AND MAXIMUM VALUE OVER A SPECIFIED PERIOD lookback must be a scalar with a value of 2 or greater. Aborting (%d).", codeLine);
					}
				}
				else
				// Default lookback period
				{
					lookback = 30;
				}

				// Preallocate heap
				outMinIdx = (int*)mxCalloc(rows, sizeof(int));
				outMaxIdx = (int*)mxCalloc(rows, sizeof(int));

				// Invoke with error catch
				retCode = TA_MINMAXINDEX(startIdx, endIdx, dataPtr, lookback, &dataIdx, &outElements, outMinIdx, outMaxIdx);

				// Error handling
				if (retCode) 
				{
					mxFree(outMinIdx);
					mxFree(outMaxIdx);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (%d).", taFuncNameIn, codeLine);
				}

				// Populate Output
				maxIdx_OUT = mxCreateNumericMatrix(dataIdx + outElements,1, mxINT32_CLASS, mxREAL);
				minIdx_OUT = mxCreateNumericMatrix(dataIdx + outElements,1, mxINT32_CLASS, mxREAL);
				memcpy(((int *) mxGetData(minIdx_OUT)) + dataIdx, outMinIdx, outElements * mxGetElementSize(minIdx_OUT));
				memcpy(((int *) mxGetData(maxIdx_OUT)) + dataIdx, outMaxIdx, outElements * mxGetElementSize(maxIdx_OUT));

				// Cleanup
				mxFree(outMinIdx); 
				mxFree(outMaxIdx); 

				break;
			}

		// Minus Directional Indicator
		case ta_minus_di:       

			break;
		// Minus Directional Movement
		case ta_minus_dm:       

			break;
		// Momentum
		case ta_mom:       

			break;
		// Vector Arithmetic Mult
		case ta_mult:       

			break;
		// Normalized Average True Range
		case ta_natr:       

			break;
		// On Balance Volume
		case ta_obv:       

			break;
		// Plus Directional Indicator
		case ta_plus_di:       

			break;
		// Plus Directional Movement
		case ta_plus_dm:       

			break;
		// Percentage Price Oscillator
		case ta_ppo:       

			break;
		// Rate of change : ((price/prevPrice)-1)*100
		case ta_roc:       

			break;
		// Rate of change Percentage: (price-prevPrice)/prevPrice
		case ta_rocp:       

			break;
		// Rate of change ratio: (price/prevPrice)
		case ta_rocr:       

			break;
		// Rate of change ratio 100 scale: (price/prevPrice)*100
		case ta_rocr100:       

			break;
		// Relative Strength Index
		case ta_rsi:       

			break;
		// Parabolic SAR
		case ta_sar:       

			break;
		// Parabolic SAR - Extended
		case ta_sarext:       

			break;
		// Vector Trigonometric Sin
		case ta_sin:       

			break;
		// Vector Trigonometric Sinh
		case ta_sinh:       

			break;
		// Simple Moving Average
		case ta_sma:       

			break;
		// Vector Square Root
		case ta_sqrt:       

			break;
		// Standard Deviation
		case ta_stddev:       

			break;
		// Stochastic
		case ta_stoch:       

			break;
		// Stochastic Fast
		case ta_stochf:       

			break;
		// Stochastic Relative Strength Index
		case ta_stochrsi:       

			break;
		// Vector Arithmetic Subtraction
		case ta_sub:       

			break;
		// Summation
		case ta_sum:       

			break;
		// Triple Exponential Moving Average (T3)
		case ta_t3:       

			break;
		// Vector Trigonometric Tan
		case ta_tan:       

			break;
		// Vector Trigonometric Tanh
		case ta_tanh:       
	
			break;
		// Triple Exponential Moving Average
		case ta_tema:       

			break;
		// True Range
		case ta_trange:       

			break;
		// Triangular Moving Average
		case ta_trima:       

			break;
		// 1-day Rate-Of-Change (ROC) of a Triple Smooth EMA
		case ta_trix:       

			break;
		// Time Series Forecast
		case ta_tsf:       

			break;
		// Typical Price
		case ta_typprice:       

			break;
		// Ultimate Oscillator
		case ta_ultosc:       

			break;
		// Variance
		case ta_var:       

			break;
		// Weighted Close Price
		case ta_wclprice:       

			break;
		// Williams' %R
		case ta_willr:       

			break;
		// Weighted Moving Average
		case ta_wma:       

			break;

		// Unknown function given as input
		default:
			mexErrMsgIdAndTxt( "MATLAB:taInvoke:UnknownFunction",
				"Unable to find a matching function to: '%s'. Aborting (2714).",taFuncNameIn);
			break;
	}

	return;
}

void InitSwitchMapping()
{
	s_mapStringValues["ta_accbands"]			= ta_accbands;
	s_mapStringValues["ta_acos"]				= ta_acos;
	s_mapStringValues["ta_ad"]					= ta_ad;
	s_mapStringValues["ta_add"]					= ta_add;
	s_mapStringValues["ta_adosc"]				= ta_adosc;
	s_mapStringValues["ta_adx"]					= ta_adx;
	s_mapStringValues["ta_adxr"]				= ta_adxr;
	s_mapStringValues["ta_apo"]					= ta_apo;
	s_mapStringValues["ta_aroon"]				= ta_aroon;
	s_mapStringValues["ta_aroonosc"]			= ta_aroonosc;
	s_mapStringValues["ta_asin"]				= ta_asin;
	s_mapStringValues["ta_atan"]				= ta_atan;
	s_mapStringValues["ta_atr"]					= ta_atr;
	s_mapStringValues["ta_avgdev"]				= ta_avgdev;
	s_mapStringValues["ta_avgprice"]			= ta_avgprice;
	s_mapStringValues["ta_bbands"]				= ta_bbands;
	s_mapStringValues["ta_beta"]				= ta_beta;
	s_mapStringValues["ta_bop"]					= ta_bop;
	s_mapStringValues["ta_cci"]					= ta_cci;
	s_mapStringValues["ta_cdl2crows"]			= ta_cdl2crows;
	s_mapStringValues["ta_cdl3blackcrows"]		= ta_cdl3blackcrows;
	s_mapStringValues["ta_cdl3inside"]			= ta_cdl3inside;
	s_mapStringValues["ta_cdl3linestrike"]		= ta_cdl3linestrike;
	s_mapStringValues["ta_cdl3outside"]			= ta_cdl3outside;
	s_mapStringValues["ta_cdl3starsinsouth"]	= ta_cdl3starsinsouth;
	s_mapStringValues["ta_cdl3whitesoldiers"]	= ta_cdl3whitesoldiers;
	s_mapStringValues["ta_cdlabandonedbaby"]	= ta_cdlabandonedbaby;
	s_mapStringValues["ta_cdladvanceblock"]		= ta_cdladvanceblock;
	s_mapStringValues["ta_cdlbelthold"]			= ta_cdlbelthold;
	s_mapStringValues["ta_cdlbreakaway"]		= ta_cdlbreakaway;
	s_mapStringValues["ta_cdlclosingmarubozu"]	= ta_cdlclosingmarubozu;
	s_mapStringValues["ta_cdlconcealbabyswall"]	= ta_cdlconcealbabyswall;
	s_mapStringValues["ta_cdlcounterattack"]	= ta_cdlcounterattack;
	s_mapStringValues["ta_cdldarkcloudcover"]	= ta_cdldarkcloudcover;
	s_mapStringValues["ta_cdldoji"]				= ta_cdldoji;
	s_mapStringValues["ta_cdldojistar"]			= ta_cdldojistar;
	s_mapStringValues["ta_cdldragonflydoji"]	= ta_cdldragonflydoji;
	s_mapStringValues["ta_cdlengulfing"]		= ta_cdlengulfing;
	s_mapStringValues["ta_cdleveningdojistar"]	= ta_cdleveningdojistar;
	s_mapStringValues["ta_cdleveningstar"]		= ta_cdleveningstar;
	s_mapStringValues["ta_cdlgapsidesidewhite"]	= ta_cdlgapsidesidewhite;
	s_mapStringValues["ta_cdlgravestonedoji"]	= ta_cdlgravestonedoji;
	s_mapStringValues["ta_cdlhammer"]			= ta_cdlhammer;
	s_mapStringValues["ta_cdlhangingman"]		= ta_cdlhangingman;
	s_mapStringValues["ta_cdlharami"]			= ta_cdlharami;
	s_mapStringValues["ta_cdlharamicross"]		= ta_cdlharamicross;
	s_mapStringValues["ta_cdlhighwave"]			= ta_cdlhighwave;
	s_mapStringValues["ta_cdlhikkake"]			= ta_cdlhikkake;
	s_mapStringValues["ta_cdlhikkakemod"]		= ta_cdlhikkakemod;
	s_mapStringValues["ta_cdlhomingpigeon"]		= ta_cdlhomingpigeon;
	s_mapStringValues["ta_cdlidentical3crows"]	= ta_cdlidentical3crows;
	s_mapStringValues["ta_cdlinneck"]			= ta_cdlinneck;
	s_mapStringValues["ta_cdlinvertedhammer"]	= ta_cdlinvertedhammer;
	s_mapStringValues["ta_cdlkicking"]			= ta_cdlkicking;
	s_mapStringValues["ta_cdlkickingbylength"]	= ta_cdlkickingbylength;
	s_mapStringValues["ta_cdlladderbottom"]		= ta_cdlladderbottom;
	s_mapStringValues["ta_cdllongleggeddoji"]	= ta_cdllongleggeddoji;
	s_mapStringValues["ta_cdllongline"]			= ta_cdllongline;
	s_mapStringValues["ta_cdlmarubozu"]			= ta_cdlmarubozu;
	s_mapStringValues["ta_cdlmatchinglow"]		= ta_cdlmatchinglow;
	s_mapStringValues["ta_cdlmathold"]			= ta_cdlmathold;
	s_mapStringValues["ta_cdlmorningdojistar"]	= ta_cdlmorningdojistar;
	s_mapStringValues["ta_cdlmorningstar"]		= ta_cdlmorningstar;
	s_mapStringValues["ta_cdlonneck"]			= ta_cdlonneck;
	s_mapStringValues["ta_cdlpiercing"]			= ta_cdlpiercing;
	s_mapStringValues["ta_cdlrickshawman"]		= ta_cdlrickshawman;
	s_mapStringValues["ta_cdlrisefall3methods"]	= ta_cdlrisefall3methods;
	s_mapStringValues["ta_cdlseparatinglines"]	= ta_cdlseparatinglines;
	s_mapStringValues["ta_cdlshootingstar"]		= ta_cdlshootingstar;
	s_mapStringValues["ta_cdlshortline"]		= ta_cdlshortline;
	s_mapStringValues["ta_cdlspinningtop"]		= ta_cdlspinningtop;
	s_mapStringValues["ta_cdlstalledpattern"]	= ta_cdlstalledpattern;
	s_mapStringValues["ta_cdlsticksandwich"]	= ta_cdlsticksandwich;
	s_mapStringValues["ta_cdltakuri"]			= ta_cdltakuri;
	s_mapStringValues["ta_cdltasukigap"]		= ta_cdltasukigap;
	s_mapStringValues["ta_cdlthrusting"]		= ta_cdlthrusting;
	s_mapStringValues["ta_cdltristar"]			= ta_cdltristar;
	s_mapStringValues["ta_cdlunique3river"]		= ta_cdlunique3river;
	s_mapStringValues["ta_cdlupsidegap2crows"]	= ta_cdlupsidegap2crows;
	s_mapStringValues["ta_cdlxsidegap3methods"]	= ta_cdlxsidegap3methods;
	s_mapStringValues["ta_ceil"]				= ta_ceil;
	s_mapStringValues["ta_cmo"]					= ta_cmo;
	s_mapStringValues["ta_correl"]				= ta_correl;
	s_mapStringValues["ta_cos"]					= ta_cos;
	s_mapStringValues["ta_cosh"]				= ta_cosh;
	s_mapStringValues["ta_dema"]				= ta_dema;
	s_mapStringValues["ta_div"]					= ta_div;
	s_mapStringValues["ta_dx"]					= ta_dx;
	s_mapStringValues["ta_ema"]					= ta_ema;
	s_mapStringValues["ta_exp"]					= ta_exp;
	s_mapStringValues["ta_floor"]				= ta_floor;
	s_mapStringValues["ta_ht_dcperiod"]			= ta_ht_dcperiod;
	s_mapStringValues["ta_ht_dcphase"]			= ta_ht_dcphase;
	s_mapStringValues["ta_ht_phasor"]			= ta_ht_phasor;
	s_mapStringValues["ta_ht_sine"]				= ta_ht_sine;
	s_mapStringValues["ta_ht_trendline"]		= ta_ht_trendline;
	s_mapStringValues["ta_ht_trendmode"]		= ta_ht_trendmode;
	s_mapStringValues["ta_kama"]				= ta_kama;
	s_mapStringValues["ta_linearreg"]			= ta_linearreg;
	s_mapStringValues["ta_linearreg_angle"]		= ta_linearreg_angle;
	s_mapStringValues["ta_linearreg_intercept"]	= ta_linearreg_intercept;
	s_mapStringValues["ta_linearreg_slope"]		= ta_linearreg_slope;
	s_mapStringValues["ta_ln"]					= ta_ln;
	s_mapStringValues["ta_log10"]				= ta_log10;
	s_mapStringValues["ta_ma"]					= ta_ma;
	s_mapStringValues["ta_macd"]				= ta_macd;
	s_mapStringValues["ta_macdext"]				= ta_macdext;
	s_mapStringValues["ta_macdfix"]				= ta_macdfix;
	s_mapStringValues["ta_mama"]				= ta_mama;
	s_mapStringValues["ta_mavp"]				= ta_mavp;
	s_mapStringValues["ta_max"]					= ta_max;
	s_mapStringValues["ta_maxindex"]			= ta_maxindex;
	s_mapStringValues["ta_medprice"]			= ta_medprice;
	s_mapStringValues["ta_mfi"]					= ta_mfi;
	s_mapStringValues["ta_midpoint"]			= ta_midpoint;
	s_mapStringValues["ta_midprice"]			= ta_midprice;
	s_mapStringValues["ta_min"]					= ta_min;
	s_mapStringValues["ta_minindex"]			= ta_minindex;
	s_mapStringValues["ta_minmax"]				= ta_minmax;
	s_mapStringValues["ta_minmaxindex"]			= ta_minmaxindex;
	s_mapStringValues["ta_minus_di"]			= ta_minus_di;
	s_mapStringValues["ta_minus_dm"]			= ta_minus_dm;
	s_mapStringValues["ta_mom"]					= ta_mom;
	s_mapStringValues["ta_mult"]				= ta_mult;
	s_mapStringValues["ta_natr"]				= ta_natr;
	s_mapStringValues["ta_obv"]					= ta_obv;
	s_mapStringValues["ta_plus_di"]				= ta_plus_di;
	s_mapStringValues["ta_plus_dm"]				= ta_plus_dm;
	s_mapStringValues["ta_ppo"]					= ta_ppo;
	s_mapStringValues["ta_roc"]					= ta_roc;
	s_mapStringValues["ta_rocp"]				= ta_rocp;
	s_mapStringValues["ta_rocr"]				= ta_rocr;
	s_mapStringValues["ta_rocr100"]				= ta_rocr100;
	s_mapStringValues["ta_rsi"]					= ta_rsi;
	s_mapStringValues["ta_sar"]					= ta_sar;
	s_mapStringValues["ta_sarext"]				= ta_sarext;
	s_mapStringValues["ta_sin"]					= ta_sin;
	s_mapStringValues["ta_sinh"]				= ta_sinh;
	s_mapStringValues["ta_sma"]					= ta_sma;
	s_mapStringValues["ta_sqrt"]				= ta_sqrt;
	s_mapStringValues["ta_stddev"]				= ta_stddev;
	s_mapStringValues["ta_stoch"]				= ta_stoch;
	s_mapStringValues["ta_stochf"]				= ta_stochf;
	s_mapStringValues["ta_stochrsi"]			= ta_stochrsi;
	s_mapStringValues["ta_sub"]					= ta_sub;
	s_mapStringValues["ta_sum"]					= ta_sum;
	s_mapStringValues["ta_t3"]					= ta_t3;
	s_mapStringValues["ta_tan"]					= ta_tan;
	s_mapStringValues["ta_tanh"]				= ta_tanh;
	s_mapStringValues["ta_tema"]				= ta_tema;
	s_mapStringValues["ta_trange"]				= ta_trange;
	s_mapStringValues["ta_trima"]				= ta_trima;
	s_mapStringValues["ta_trix"]				= ta_trix;
	s_mapStringValues["ta_tsf"]					= ta_tsf;
	s_mapStringValues["ta_typprice"]			= ta_typprice;
	s_mapStringValues["ta_ultosc"]				= ta_ultosc;
	s_mapStringValues["ta_var"]					= ta_var;
	s_mapStringValues["ta_wclprice"]			= ta_wclprice;
	s_mapStringValues["ta_willr"]				= ta_willr;
	s_mapStringValues["ta_wma"]					= ta_wma;

}

void chkSingleVec( int colsH, int colsL, int lineNum )
{
	if (colsH != 1)
	{
		mexErrMsgIdAndTxt( "MATLAB:taInvoke:InputErr",
			"Price data should be passed to the function already parsed into H | L vectors.\nThe 'High' vector had more than 1 column.  Aborting (%i).", lineNum);
	}

	if (colsL != 1)
	{
		mexErrMsgIdAndTxt( "MATLAB:taInvoke:InputErr",
			"Price data should be passed to the function already parsed into H | L vectors.\nThe 'Low' vector had more than 1 column.  Aborting (%i).", lineNum);
	}
}

void chkSingleVec( int colsH, int colsL, int colsC, int lineNum )
{
	if (colsH != 1)
	{
		mexErrMsgIdAndTxt( "MATLAB:taInvoke:InputErr",
			"Price data should be passed to the function already parsed into H | L | C vectors.\nThe 'High' vector had more than 1 column.  Aborting (%i).", lineNum);
	}

	if (colsL != 1)
	{
		mexErrMsgIdAndTxt( "MATLAB:taInvoke:InputErr",
			"Price data should be passed to the function already parsed into H | L | C vectors.\nThe 'Low' vector had more than 1 column.  Aborting (%i).", lineNum);
	}

	if (colsC != 1)
	{
		mexErrMsgIdAndTxt( "MATLAB:taInvoke:InputErr",
			"Price data should be passed to the function already parsed into H | L | C vectors.\nThe 'Close' vector had more than 1 column.  Aborting (%i).", lineNum);
	}
}

void chkSingleVec(int colsO, int colsH, int colsL, int colsC, int lineNum )
{
	if (colsO != 1)
	{
		mexErrMsgIdAndTxt( "MATLAB:taInvoke:InputErr",
			"Price data should be passed to the function already parsed into O | H | L | C vectors.\nThe 'Open' vector had more than 1 column.  Aborting (%i).", lineNum);
	}

	if (colsH != 1)
	{
		mexErrMsgIdAndTxt( "MATLAB:taInvoke:InputErr",
			"Price data should be passed to the function already parsed into O | H | L | C vectors.\nThe 'High' vector had more than 1 column.  Aborting (%i).", lineNum);
	}

	if (colsL != 1)
	{
		mexErrMsgIdAndTxt( "MATLAB:taInvoke:InputErr",
			"Price data should be passed to the function already parsed into O | H | L | C vectors.\nThe 'Low' vector had more than 1 column.  Aborting (%i).", lineNum);
	}

	if (colsC != 1)
	{
		mexErrMsgIdAndTxt( "MATLAB:taInvoke:InputErr",
			"Price data should be passed to the function already parsed into O | H | L | C vectors.\nThe 'Close' vector had more than 1 column.  Aborting (%i).", lineNum);
	}
}


//
//  -------------------------------------------------------------------------
//                                  _    _ 
//         ___  _ __   ___ _ __    / \  | | __ _  ___   ___  _ __ __ _ 
//        / _ \| '_ \ / _ \ '_ \  / _ \ | |/ _` |/ _ \ / _ \| '__/ _` |
//       | (_) | |_) |  __/ | | |/ ___ \| | (_| | (_) | (_) | | | (_| |
//        \___/| .__/ \___|_| |_/_/   \_\_|\__, |\___(_)___/|_|  \__, |
//             |_|                         |___/                 |___/
//  -------------------------------------------------------------------------
//        This code is distributed in the hope that it will be useful,
//
//                         WITHOUT ANY WARRANTY AND
//
//                  WITHOUT CLAIM AS TO MERCHANTABILITY
//
//                  OR FITNESS FOR A PARTICULAR PURPOSE
//
//                           EXPRESSED OR IMPLIED.
//
//   Use of this code, pseudocode, algorithmic or trading logic contained
//   herein, whether sound or faulty for any purpose is the sole
//   responsibility of the USER. Any such use of these algorithms, coding
//   logic or concepts in whole or in part carry no covenant of correctness
//   or recommended usage from the AUTHOR or any of the possible
//   contributors listed or unlisted, known or unknown.
//
//	 Redistribution and use in source and binary forms, with or without
//	 modification, are permitted provided that the following conditions are met: 
//
//	 1. Redistributions of source code must retain the below copyright notice, 
//	 this list of conditions and the following disclaimer. 
//	 2. Redistributions in binary form must reproduce the below copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution. 
//
//   The public sharing of this code does not relinquish, reduce, restrict or
//   encumber any rights the AUTHOR has in respect to claims of intellectual
//   property.
//
//	 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
//	 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//	 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
//	 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//	 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//	 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//	 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//	 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//	 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//	 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
//	 POSSIBILITY OF SUCH DAMAGE.
//
//   -------------------------------------------------------------------------
//
//                             ALL RIGHTS RESERVED
//
//   -------------------------------------------------------------------------
//
//   Author:	Mark Tompkins
//   Revision:	4937.32889
//   Copyright:	(c)2013
//