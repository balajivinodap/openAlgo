// taInvoke.cpp
// Localized mexing: mex taInvoke.cpp @mexOpts.txt
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
void chkSingleVol(int colsH, int colsL, int lineNum);
void chkSingleVol(int colsH, int colsL, int colsC, int lineNum);
void chkSingleVol(int colsO, int colsH, int colsL, int colsC, int lineNum);

static void InitSwitchMapping();

// Macros
#define isReal2DfullDouble(P) (!mxIsComplex(P) && mxGetNumberOfDimensions(P) == 2 && !mxIsSparse(P) && mxIsDouble(P))
#define isRealScalar(P) (isReal2DfullDouble(P) && mxGetNumberOfElements(P) == 1)

// Global variables
double m_Nan = std::numeric_limits<double>::quiet_NaN(); 

void mexFunction(int nlhs, mxArray *plhs[], /* Output variables */
				 int nrhs, const mxArray *prhs[]) /* Input variables */
{
	// Check number of inputs
	if (nrhs < 2)
		mexErrMsgIdAndTxt( "MATLAB:taInvoke:NumInputs",
		"Number of input arguments is not correct. Aborting (59).");

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
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_accbands:NumInputs",
					"Number of input arguments to function 'ta_accbands' is not correct. Price data should be parsed into vectors H | L | C. Aborting (105).");
				if (nlhs != 3)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_accbands:NumOutputs",
					"The function 'ta_accbands' (Acceleration Bands) produces 3 vector outputs that must be assigned. Aborting (108).");

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
				chkSingleVol(colsH, colsL, colsC, 140);

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
						"The ACCBANDS lookback must be a scalar. Aborting (154).");
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
					"Number of input arguments to function 'ta_acos' is not correct. A single vector of values (-1 =< x =< 1) should be provided. Aborting (214).");
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_acos:NumOutputs",
					"The function 'ta_acos' (Vector Trigonometric ACos) produces a single vector output that must be assigned. Aborting (217).");

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
						"Cosine data should be a single vector array. Aborting (134).");
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
					mexErrMsgTxt("Invocation to 'ta_acos' failed. Aborting (156).");
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
					"Number of input arguments to function 'ta_ad' is not correct. Price data should be parsed into vectors H | L | C followed by a volume vector V. Aborting (111).");
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ad:NumOutputs",
						"The function 'ta_ad' (Chaikin A/D Line) produces a single vector output that must be assigned. Aborting (114).");
			
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
				chkSingleVol(colsH, colsL, colsC, 144);

				if (colsV != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_ad:InputErr",
						"Volume data should be a single vector array. Aborting (152).");
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
					mexErrMsgTxt("Invocation to 'ta_ad' failed. Aborting (180).");
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
					"Number of input arguments to function 'ta_add' is not correct. Two vectors for summation should be provided. Aborting (380).");

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
						"Vector addition inputs should be single dimensional vectors.\nThe augend vector had more than 1 column.  Aborting (408).");
				}

				if (colsAdd != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:InputErr",
						"Vector addition inputs should be single dimensional vectors.\nThe addend vector had more than 1 column.  Aborting (414).");
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
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (434).", taFuncNameIn);
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
					"Number of input arguments is not correct. Price data should be parsed into vectors H | L | C followed by a volume vector V.\nOptional inputs are fastMA | slowMA. Aborting (220).");
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_adosc:NumOutputs",
					"The function 'ta_adosc' (Chaikin A/D Oscillator) produces a single vector output that must be assigned. Aborting (223).");

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
				chkSingleVol(colsH, colsL, colsC, 240);

				if (colsV != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_adosc:InputErr",
						"Volume data should be a single vector array. Aborting (268).");
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
						"The fastMA lookback must be a scalar. Aborting (266).");
					else if (!isRealScalar(slowMA_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The slowMA lookback must be a scalar. Aborting (269).");	

					/* Get the scalar inputs */
					// Assign
					fastMA = (int)mxGetScalar(fastMA_IN);
					slowMA = (int)mxGetScalar(slowMA_IN);

					if (fastMA > slowMA)
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The slowMA (%d) must not be less than the fastMA (%d). Aborting (278).", slowMA, fastMA);

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
					mexErrMsgTxt("Invocation to 'ta_adosc' failed. Aborting (294).");
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
					"Number of input arguments to function 'ta_adx' is not correct. Price data should be parsed into vectors H | L | C. Aborting (351).");
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_adx:NumOutputs",
					"The function 'ta_adx' (Average Directional Movement Index) produces a single vector output that must be assigned. Aborting (354).");

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
				chkSingleVol(colsH, colsL, colsC, 487);

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
						"The ADX lookback must be a scalar. Aborting (527).");
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
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (573).", taFuncNameIn);
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
					"Number of input arguments to function 'ta_apo' is not correct. A single vector of price data should be provided.\nOptional scalar arguments are fastMA, slowMA, typeMA. Aborting (111).");
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_apo:NumOutputs",
					"The function 'ta_apo' (Absolute Price Oscillator) produces a single vector output that must be assigned. Aborting (441).");
				
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
						"Price data should be a single vector array. Aborting (465).");
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
						"The fastMA lookback must be a scalar. Aborting (483).");
					else if (!isRealScalar(slowMA_IN))
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The slowMA lookback must be a scalar. Aborting (490).");
					
					/* Get the scalar inputs */
					// Assign
					fastMA = (int)mxGetScalar(fastMA_IN);
					slowMA = (int)mxGetScalar(slowMA_IN);

					if (fastMA > slowMA)
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"The slowMA (%d) must not be less than the fastMA (%d). Aborting (502).", slowMA, fastMA);


					/* Get the scalar inputs */
					// Assign
					fastMA = (int)mxGetScalar(fastMA_IN);
					slowMA = (int)mxGetScalar(slowMA_IN);

					if (nrhs == 5)
					{
						#define typeMA_IN	prhs[4]

						if (!isRealScalar(typeMA_IN))
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The typeMA must be a scalar value of available types. Aborting (512).");

						typeMA = (int)mxGetScalar(typeMA_IN);

						if (typeMA < 0 || typeMA > 8)
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The typeMA must be a scalar value of available types (0 - 8). Aborting (518).");
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
						"Ambiguous inputs. Aborting (529).");
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
					mexErrMsgTxt("Invocation to 'ta_apo' failed. Aborting (620).");
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
					"Number of input arguments to function 'ta_aroon' is not correct. Price data should be parsed into separate vectors H | L. Aborting (578).");
				if (nlhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_aroon:NumOutputs",
					"The function 'ta_aroon' (Aroon Indicator) produces 2 vectors as output that must be assigned. Aborting (581).");

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
				chkSingleVol(colsH, colsL, 611);

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
						"The AROON lookback must be a scalar. Aborting (625).");
					
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
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (649).", taFuncNameIn);
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
					"Number of input arguments to function 'ta_aroon' is not correct. Price data should be parsed into separate vectors H | L. Aborting (578).");
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_aroonosc:NumOutputs",
					"The function 'ta_aroonosc' (Aroon Oscillator) produces a single vector as output that must be assigned. Aborting (684).");

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
				chkSingleVol(colsH, colsL, 710);

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
						"The AROONOSC lookback must be a scalar. Aborting (725).");

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
					"Number of input arguments to function 'ta_asin' is not correct. A single vector of values (-1 =< x =< 1) should be provided. Aborting (1038).");
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_acos:NumOutputs",
					"The function 'ta_asin' (Vector Trigonometric ASin) produces a single vector output that must be assigned. Aborting (1041).");

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
						"Sine data should be a single vector array. Aborting (1066).");
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
					mexErrMsgTxt("Invocation to 'ta_acos' failed. Aborting (156).");
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
					"Number of input arguments to function 'ta_atan' is not correct. A single vector of Real values should be provided. Aborting (1117).");
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_acos:NumOutputs",
					"The function 'ta_atan' (Vector Trigonometric ATan) produces a single vector output that must be assigned. Aborting (1120).");

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
						"Tangent data should be a single vector array. Aborting (1145).");
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
					mexErrMsgTxt("Invocation to 'ta_atan' failed. Aborting (1167).");
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
					"Number of input arguments to function 'ta_atr' is not correct. Price data should be parsed into vectors H | L | C.\nAn optional lookback scalar may also be provided. Aborting (1196).");
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_atr:NumOutputs",
					"The function 'ta_atr' (Average True Range) produces a single vector output that must be assigned. Aborting (1199).");

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
				chkSingleVol(colsH, colsL, colsC, 1278);

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
						"The ATR lookback must be a scalar. Aborting (1243).");

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
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (1315).", taFuncNameIn);
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
				//		Data				a vector of obseration data

				// OPTIONAL INPUTS
				//		Lookback period		(default 14)

				// OUTPUT
				//		AVGDEV				vector of average deviation values

				// Check number of inputs
				if (nrhs != 2)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_avgdev:NumInputs",
					"Number of input arguments to function 'ta_avgdev' is not correct. Observational data should be a single vector.\n An optional lookback scalar may also be provided. Aborting (1293).");
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_avgdev:NumOutputs",
					"The function 'ta_avgdev' (Average Deviation) produces a single vector output that must be assigned. Aborting (1296).");

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
						"Observational data should be a single vector array. Aborting (1321).");
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
						"The ATR lookback must be a scalar. Aborting (1338).");
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
					"Number of input arguments to function 'ta_avgprice' is not correct. Price data should be parsed into vectors O | H | L | C. Aborting (1387).");
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_avgprice:NumOutputs",
					"The function 'ta_avgprice' (Average Price) produces a single vector output that must be assigned. Aborting (1390).");

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
				chkSingleVol(colsO, colsH, colsL, colsC, 1490);

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
					"Number of input arguments to function 'ta_bbands' is incorrect. Observational data should be a single vector.\nOptional inputs are: Lookback period, Upper STD Multiplier, Lower STD Multiplier. Aborting (1475).");
				if (nrhs == 4)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_bbands:NumInputs",
					"Number of input arguments to function 'ta_bbands' is ambiguous.\nWhen providing optional inputs, you may provide just the 'Lookback period, or 'Lookback period, Upper STD Multiplier, Lower STD Multiplier'. Aborting (1478).");
				if (nlhs != 3)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_bbands:NumOutputs",
					"The function 'ta_bbands' (Bollinger Bands) produces 3 vector outputs that must be assigned. Aborting (1481).");

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
						"Observational data should be a single vector array. Aborting (1504).");
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
						"The BBANDS lookback must be a scalar. Aborting (1524).");

					/* Get the scalar input lookback */
					// Assign
					lookback = (int)mxGetScalar(lookback_IN);

					// Check for integer to protect against STD multiples
					if (fraction(lookback))
					{
						mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
						"A fraction was detected in the BBANDS lookback input.  This may have been intended to be an STD band multiplier? Aborting (1534).");
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
							"The BBANDS STD upper multiplier must be a scalar. Aborting (1550).");
						if (!isRealScalar(dnMult_IN))
							mexErrMsgIdAndTxt( "MATLAB:taInvoke:inputErr",
							"The BBANDS STD lower multiplier must be a scalar. Aborting (1553).");

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
								"The BBANDS typeMA must be a scalar. Aborting (1583).");

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
					"Number of input arguments to function 'ta_beta' is not correct. Price data should be parsed into 2 vectors Individual Instrument | Index.\n An optional lookback scalar may also be provided. Aborting (1725).");
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_beta:NumOutputs",
					"The function 'ta_beta' (Beta) produces a single vector output that must be assigned. Aborting (1728).");

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
						"The individual instrument data should be a single vector array. Aborting (1756).");
				}

				if (colsB != 1)
				{
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_beta:InputErr",
						"The base instrument data should be a single vector array. Aborting (1762).");
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
						"The BETA lookback must be a scalar. Aborting (1778).");

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
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (1801).", taFuncNameIn);
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
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_bop:NumInputs",
					"Number of input arguments to function 'ta_bop' is not correct. Price data should be parsed into vectors O | H | L | C. Aborting (1829).");
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_bop:NumOutputs",
					"The function 'ta_bop' (Balance of Power) produces a single vector output that must be assigned. Aborting (1832).");

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
				chkSingleVol(colsO, colsH, colsL, colsC, 1864);

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
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (1883).", taFuncNameIn);
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
					"Number of input arguments to function 'ta_cci' is not correct. Price data should be parsed into vectors H | L | C.\nAn optional lookback scalar may also be provided. Aborting (1911).");
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:ta_cci:NumOutputs",
					"The function 'ta_cci' (Commodity Channel Index) produces a single vector output that must be assigned. Aborting (1914).");

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
				chkSingleVol(colsH, colsL, colsC, 1943);

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
						"The CCI lookback must be a scalar. Aborting (1958).");

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
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (1980).", taFuncNameIn);
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
		// Two Crows
		case ta_cdl2crows:
		// Three Black Crows
		case ta_cdl3blackcrows:
		// Three Inside Up/Down
		case ta_cdl3inside:
		// Three-Line Strike
		case ta_cdl3linestrike:
		// Three Outside Up/Down
		case ta_cdl3outside:
		// Three Stars In The South
		case ta_cdl3starsinsouth: 
		// Three Advancing White Soldiers
		case ta_cdl3whitesoldiers:
		// Abandoned Baby
		case ta_cdlabandonedbaby:
		// Advance Block
		case ta_cdladvanceblock: 
		// Belt-hold
		case ta_cdlbelthold:  
		// Breakaway
		case ta_cdlbreakaway:  
		// Closing Marubozu
		case ta_cdlclosingmarubozu: 
		// Concealing Baby Swallow
		case ta_cdlconcealbabyswall:  
		// Counterattack
		case ta_cdlcounterattack:
			{
				// REQUIRED INPUTS
				//		Price	O | H | L | C	separate vectors

				// OPTIONAL INPUTS
				//		CDLABANDONEDBABY	
				//			pctPen				Percentage of penetration of a candle within another candle (decimal input)

				// OUTPUTS
				//		CDL2CROWS				vector of the instances identified as Two Crows Candlestick formation

				// Check number of inputs
				if (nrhs > 6)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:Candlestick:NumInputs",
					"The number of input arguments to the called candlestick function is not correct. Price data should be parsed into vectors O | H | L | C. Aborting (2033).");
				if (nlhs != 1)
					mexErrMsgIdAndTxt( "MATLAB:taInvoke:Candlestick:NumOutputs",
					"The called candlestick function produces a single vector output that must be assigned. Aborting (2020).");

				// Create constants for readability
				// Inputs
				#define open_IN			prhs[1]
				#define high_IN			prhs[2]
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
				chkSingleVol(colsO, colsH, colsL, colsC, 2052);

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

							retCode = TA_CDLABANDONEDBABY(startIdx, endIdx, openPtr, highPtr, lowPtr, closePtr, pctPen, &cdlIdx, &outElements, outInt);

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
				}

				// Error handling
				if (retCode) 
				{
					mxFree(outInt);
					mexPrintf("%s%i","Return code=",retCode);
					mexErrMsgIdAndTxt("MATLAB:taInvoke","Invocation to '%s' failed. Aborting (2071).", taFuncNameIn);
				}

				// Populate Output
				cdl_OUT = mxCreateNumericMatrix(cdlIdx + outElements,1, mxINT32_CLASS, mxREAL);
				memcpy((int*)(mxGetData(cdl_OUT)) + cdlIdx, outInt, outElements * mxGetElementSize(cdl_OUT));

				// Cleanup
				mxFree(outInt); 

				break;
			}


		// Dark Cloud Cover
		case ta_cdldarkcloudcover:       

			break;
		// Doji
		case ta_cdldoji:       

			break;
		// Doji Star
		case ta_cdldojistar:       

			break;
		// Dragonfly Doji
		case ta_cdldragonflydoji:       

			break;
		// Engulfing Pattern
		case ta_cdlengulfing:       

			break;
		// Evening Doji Star
		case ta_cdleveningdojistar:       

			break;
		// Evening Star
		case ta_cdleveningstar:       

			break;
		// Up/Down-gap side-by-side white lines
		case ta_cdlgapsidesidewhite:       

			break;
		// Gravestone Doji
		case ta_cdlgravestonedoji:       

			break;
		// Hammer
		case ta_cdlhammer:       

			break;
		// Hanging Man
		case ta_cdlhangingman:       

			break;
		// Harami Pattern
		case ta_cdlharami:       

			break;
		// Harami Cross Pattern
		case ta_cdlharamicross:       

			break;
		// High-Wave Candle
		case ta_cdlhighwave:       

			break;
		// Hikkake Pattern
		case ta_cdlhikkake:       

			break;
		// Modified Hikkake Pattern
		case ta_cdlhikkakemod:       

			break;
		// Homing Pigeon
		case ta_cdlhomingpigeon:       

			break;
		// Identical Three Crows
		case ta_cdlidentical3crows:       

			break;
		// In-Neck Pattern
		case ta_cdlinneck:       

			break;
		// Inverted Hammer
		case ta_cdlinvertedhammer:       

			break;
		// Kicking
		case ta_cdlkicking:       

			break;
		// Kicking - bull/bear determined by the longer Marubozu
		case ta_cdlkickingbylength:       

			break;
		// Ladder Bottom
		case ta_cdlladderbottom:       

			break;
		// Long Legged Doji
		case ta_cdllongleggeddoji:       

			break;
		// Long Line Candle
		case ta_cdllongline:       

			break;
		// Marubozu
		case ta_cdlmarubozu:       

			break;
		// Matching Low
		case ta_cdlmatchinglow:       

			break;
		// Mat Hold
		case ta_cdlmathold:       

			break;
		// Morning Doji Star
		case ta_cdlmorningdojistar:       

			break;
		// Morning Star
		case ta_cdlmorningstar:       

			break;
		// On-Neck Pattern
		case ta_cdlonneck:       

			break;
		// Piercing Pattern
		case ta_cdlpiercing:       

			break;
		// Rickshaw Man
		case ta_cdlrickshawman:       

			break;
		// Rising/Falling Three Methods
		case ta_cdlrisefall3methods:       

			break;
		// Separating Lines
		case ta_cdlseparatinglines:       

			break;
		// Shooting Star
		case ta_cdlshootingstar:       

			break;
		// Short Line Candle
		case ta_cdlshortline:       

			break;
		// Spinning Top
		case ta_cdlspinningtop:       

			break;
		// Stalled Pattern
		case ta_cdlstalledpattern:       

			break;
		// Stick Sandwich
		case ta_cdlsticksandwich:       

			break;
		// Takuri (Dragonfly Doji with very long lower shadow)
		case ta_cdltakuri:       

			break;
		// Tasuki Gap
		case ta_cdltasukigap:       

			break;
		// Thrusting Pattern
		case ta_cdlthrusting:       

			break;
		// Tristar Pattern
		case ta_cdltristar:       

			break;
		// Unique 3 River
		case ta_cdlunique3river:       

			break;
		// Upside Gap Two Crows
		case ta_cdlupsidegap2crows:       

			break;
		// Upside/Downside Gap Three Methods
		case ta_cdlxsidegap3methods:       

			break;
		// Vector Ceil
		case ta_ceil:       

			break;
		// Chande Momentum Oscillator
		case ta_cmo:       

			break;
		// Pearson's Correlation Coefficient (r)
		case ta_correl:       

			break;
		// Vector Trigonometric Cos
		case ta_cos:       

			break;
		// Vector Trigonometric Cosh
		case ta_cosh:       

			break;
		// Double Exponential Moving Average
		case ta_dema:       

			break;
		// Vector Arithmetic Div
		case ta_div:       

			break;
		// Directional Movement Index
		case ta_dx:       

			break;
		// Exponential Moving Average
		case ta_ema:       

			break;
		// Vector Arithmetic Exp
		case ta_exp:       

			break;
		// Vector Floor
		case ta_floor:       

			break;
		// Hilbert Transform - Dominant Cycle Period
		case ta_ht_dcperiod:       

			break;
		// Hilbert Transform - Dominant Cycle Phase
		case ta_ht_dcphase:       

			break;
		// Hilbert Transform - Phasor Components
		case ta_ht_phasor:       

			break;
		// Hilbert Transform - SineWave
		case ta_ht_sine:       

			break;
		// Hilbert Transform - Instantaneous Trendline
		case ta_ht_trendline:       

			break;
		// Hilbert Transform - Trend vs Cycle Mode
		case ta_ht_trendmode:       

			break;
		// Kaufman Adaptive Moving Average
		case ta_kama:       

			break;
		// Linear Regression
		case ta_linearreg:       

			break;
		// Linear Regression Angle
		case ta_linearreg_angle:       

			break;
		// Linear Regression Intercept
		case ta_linearreg_intercept:       

			break;
		// Linear Regression Slope
		case ta_linearreg_slope:       

			break;
		// Vector Log Natural
		case ta_ln:       

			break;
		// Vector Log10
		case ta_log10:       

			break;
		// Moving average
		case ta_ma:       

			break;
		// Moving Average Convergence/Divergence
		case ta_macd:       

			break;
		// MACD with controllable MA type
		case ta_macdext:       

			break;
		// Moving Average Convergence/Divergence Fix 12/26
		case ta_macdfix:       

			break;
		// MESA Adaptive Moving Average
		case ta_mama:       

			break;
		// Moving average with variable period
		case ta_mavp:       

			break;
		// Highest value over a specified period
		case ta_max:       

			break;
		// Index of highest value over a specified period
		case ta_maxindex:       

			break;
		// Median Price
		case ta_medprice:       

			break;
		// Money Flow Index
		case ta_mfi:       

			break;
		// MidPoint over period
		case ta_midpoint:       

			break;
		// Midpoint Price over period
		case ta_midprice:       

			break;
		// Lowest value over a specified period
		case ta_min:       

			break;
		// Index of lowest value over a specified period
		case ta_minindex:       

			break;
		// Lowest and highest values over a specified period
		case ta_minmax:       

			break;
		// Indexes of lowest and highest values over a specified period
		case ta_minmaxindex:       

			break;
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

void chkSingleVol( int colsH, int colsL, int lineNum )
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

void chkSingleVol( int colsH, int colsL, int colsC, int lineNum )
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

void chkSingleVol(int colsO, int colsH, int colsL, int colsC, int lineNum )
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
//   Revision:	4933.20689
//   Copyright:	(c)2013
//