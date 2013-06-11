// calcProfitLoss.cpp : Defines the entry point for the console application.
// http://www.kobashicomputing.com/node/177 for a reference to x64 bit
//
// nlhs Number of output variables nargout 
// plhs Array of mxArray pointers to the output variables varargout
// nrhs Number of input variables nargin
// prhs Array of mxArray pointers to the input variables varargin
//
// Matlab function:
// [cash,openEQ,netLiq,returns] = calcProfitLoss_mex(data,sig,bigPoint,cost)
// 
// Inputs:
//		data		A 2-D array of prices in the form of Open | Close
//		sig			An array the same length as data, which gives the quantity bought or sold on a given bar.  Consider Matlab remEchosMEX
//		bigPoint	Double representing the full tick dollar value of the contract being P&L'd
//		cost		Double representing the per contract commission
//
// Outputs:
//		cash		A 2D array of cash debts and credits
//		openEQ		A 2D array of bar to bar openEQ values if there is an open position
//		netLiq		A 2D array of aggregated cash transactions plus the current openEQ if any up to a given observation
//		returns		A 2D array of bar to bar returns
//
//	NOTE: This function accepts both advanced (fractional) and standard SIGNAL inputs
//
//		By leveraging fractions as additional logic, we are able to construct more meaningful signals beyond the scope of a simple Buy or Sell of quantity X.
//		As you can't buy or sell 1/2 a share or 1/2 a lot, we can quickly check for additional handling of a signal that contains a fractional element.
//		
//		The convention that is used refines to the following to effectuate some advanced handling of produced signals. 'NET' is a current net position:
//
//			NET = any	SIGNAL = 0					THEN No Action						A zero signal is an evaluated false to a possible state trigger and instructs to 'do nothing'.  
//																						Signal IF conditions have a boolean output of TRUE == 1 or FALSE == 0
//			NET = any	SIGNAL = X		(INTEGER)	THEN Buy or Sell X					An integer instructs to BUY or SELL quantity X. 
//																						This can be additive, reductive or initiating in respect to NET.
//			NET = any	SIGNAL = +/-0.5	(FRACTION)	THEN Close Out Any Position			Close out any existion position such that a NET = 0 flat condition exists.
//																						If no position exists, no error is thrown.
//			
//			NET <= 0	SIGNAL =  X.5	(FRACTION)	THEN Reverse to position NET = X	Close out any existing short position and buy X longs to create a NET long position of quantity X
//			NET >= 0	SIGNAL = -X.5	(FRACTION)	THEN Reverse to position NET = -X	Close out any existing long position and sell X shorts to create a NET short position of quantity X
//			
//			NET < 0		SIGNAL = -X.5 | INT(X)<=-1	ERROR								An error is thrown when we have an existing short position and we are given a reverse to net short signal
//			NET > 0		SIGNAL = +X.5 | INT(X)>= 1	ERROR								An error is thrown when we have an existing long position and we are given a reverse to net long signal
//																						
//		NOTE: 	This convention should also work with those who do not want to avail themselves with the fractional logic.
//				For example consider the following:
//				
//		EX 1	Without fractional logic				With fractional logic
//				NET		 	= 	-1						NET			=	-1
//				SIGNAL		=	 2						SIGNAL		=	1.5
//				final NET	=	 1						final NET	=	 1
//				
//		EX 2	Without fractional logic				With fractional logic
//				NET		 	= 	-50						NET			=	-50
//				SIGNAL		=	 51						SIGNAL		=	1.5
//				final NET	=	 1						final NET	=	 1
//				
//		EX 3	Without fractional logic				With fractional logic
//				NET		 	= 	-50						NET			=	-50
//				SIGNAL		=	 55						SIGNAL		=	5.5
//				final NET	=	 5						final NET	=	 5
//

#include "mex.h"
#include <stdio.h>
#include <deque>

// Declare external reference to undocumented C function
#ifdef __cplusplus
extern "C"
{
#endif

	mxArray *mxCreateSharedDataCopy(const mxArray *pr);
	// and any other prototypes for undocumented API functions you are using

#ifdef __cplusplus
}
#endif

using namespace std;

// Create a struct for convenience
typedef struct tradeEntry
{
	int index;
	int quantity;
	double price;
} tradeEntry;

// Prototypes
tradeEntry createLineEntry(int ID, int qty, double price);
int sumQty(const deque<tradeEntry>& x);
bool fraction(double num);
bool knownAdvSig(double advSig);

// Macros
#define isReal2DfullDouble(P) (!mxIsComplex(P) && mxGetNumberOfDimensions(P) == 2 && !mxIsSparse(P) && mxIsDouble(P))
#define isRealScalar(P) (isReal2DfullDouble(P) && mxGetNumberOfElements(P) == 1)

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
	if (nrhs != 4)
		mexErrMsgIdAndTxt( "MATLAB:calcProfitLoss:NumInputs",
		"Number of input arguments is not correct. Aborting.");

	if (nlhs != 4)
		mexErrMsgIdAndTxt( "MATLAB:calcProfitLoss:NumOutputs",
		"Number of output assignments is not correct. Aborting.");

	// Define constants (#define assigns a variable as either a constant or a macro)
	// Inputs
#define data_IN		prhs[0]
#define sig_IN		prhs[1]
#define bigPoint_IN	prhs[2]
#define cost_IN		prhs[3]
	// Outputs
#define cash_OUT	plhs[0]
#define openEQ_OUT	plhs[1]
#define netLiq_OUT	plhs[2]
#define returns_OUT	plhs[3]

	// Init Global variables
	mwSize rowsData, colsData, rowsSig, colsSig;
	double *cashIdx, *openEQIdx, *netLiqIdx, *returnsIdx, *dataInPtr, *sigInPtr; // *bigPointPtr, *costPtr;

	// Check type of supplied inputs
	if (!isReal2DfullDouble(data_IN)) 
		mexErrMsgIdAndTxt( "MATLAB:calcProfitLoss:BadInputType",
		"Input 'data' must be a 2 dimensional full double array. Aborting.");

	if (!isReal2DfullDouble(sig_IN)) 
		mexErrMsgIdAndTxt( "MATLAB:calcProfitLoss:BadInputType",
		"Input 'sig' must be a 2 dimensional full double array. Aborting.");

	if (!isRealScalar(bigPoint_IN)) 
		mexErrMsgIdAndTxt( "MATLAB:calcProfitLoss:BadInputType",
		"Input 'bigPoint' must be a single scalar double. Aborting.");

	if (!isRealScalar(cost_IN)) 
		mexErrMsgIdAndTxt( "MATLAB:calcProfitLoss:BadInputType",
		"Input 'cost' must be a single scalar double. Aborting.");

	// Assign variables
	rowsData = mxGetM(data_IN);
	colsData = mxGetN(data_IN);
	rowsSig = mxGetM(sig_IN);
	colsSig = mxGetN(sig_IN);

	// Primarily for readability
	// Price input must be in the form of O | H | L | C
	const int SHIFT_CLOSE = rowsData;			

	if (rowsData != rowsSig)
		mexErrMsgIdAndTxt( "MATLAB:calcProfitLoss:ArrayMismatch",
		"The number of rows in the data array and the signal array are different. Aborting.");

	if (colsSig > 1)
		mexErrMsgIdAndTxt( "MATLAB:calcProfitLoss:ArrayMismatch",
		"Input 'sig' must be a single column array. Aborting.");

	if (colsData != 2)
		mexErrMsgIdAndTxt( "MATLAB:calcProfitLoss:ArrayMismatch",
		"Input 'data' must be in the form of 'O | C'. Aborting.");

	if (!isRealScalar(bigPoint_IN)) 
		mexErrMsgIdAndTxt( "MATLAB:calcProfitLoss:ScalarMismatch",
		"Input 'bigPoint' must be a double scalar value. Aborting.");

	if (!isRealScalar(cost_IN))  
		mexErrMsgIdAndTxt( "MATLAB:calcProfitLoss:ScalarMismatch",
		"Input 'cost' must be a double scalar value. Aborting.");

	/* Create matrices for the return arguments */ 
	// http://www.mathworks.com/help/matlab/matlab_external/c-c-source-mex-files.html
	cash_OUT = mxCreateDoubleMatrix(rowsData, 1, mxREAL);
	openEQ_OUT = mxCreateDoubleMatrix(rowsData, 1, mxREAL); 
	netLiq_OUT = mxCreateDoubleMatrix(rowsData, 1, mxREAL); 
	returns_OUT = mxCreateDoubleMatrix(rowsData, 1, mxREAL); 

	/* Assign pointers to the arrays */ 
	dataInPtr = mxGetPr(prhs[0]);
	sigInPtr = mxGetPr(prhs[1]);

	// assign values to the two variables passed as arrays
	const double BIG_POINT = mxGetScalar(bigPoint_IN);
	const double COST = mxGetScalar(cost_IN);

	// assign the index variables for manipulating the arrays 
	cashIdx = mxGetPr(cash_OUT);
	openEQIdx = mxGetPr(openEQ_OUT);
	netLiqIdx = mxGetPr(netLiq_OUT);
	returnsIdx = mxGetPr(returns_OUT);

	// START //
	// Initialize variables
	int	sigIdx;							// Iterator that will store the index of the referenced signal
	int barIdx;							// Iterator that will store the index of the referenced bar
	bool anyTrades = false;				// Variable that indicates if we have any trades

	// Check that we have at least one signal (at least one trade)
	for (sigIdx=0; sigIdx < rowsSig; sigIdx++)					// Remember C++ starts counting at '0'
	{
		if (abs(sigInPtr[sigIdx]) >=1)		// See if we have a signal that generates a position
		{
			anyTrades=true;					// Trade found
			break;							// Exit the for loop
		}
	}	

		// We have trades
		// RETURN zeros if the signal is the last bar
		if (anyTrades && sigIdx < rowsSig)
		{
			// Initialize a ledger for open positions
			deque<tradeEntry> openLedger;

			// Put first trade on ledger
			// price is 'sigIdx+1' because execution price lags signal by one observation
			// We only need the integer portion of the first trade
			openLedger.push_back(createLineEntry(sigIdx, int(sigInPtr[sigIdx]), dataInPtr[sigIdx+1]));

			// Initialize position trackers
			int netPO = 0, newPO = 0;

			// ITERATE
			// Start iterating at next observation
			// Finish at observation before last in signal array
			for (int ii = sigIdx+1; ii < rowsSig-1; ii++)
			{
				if (sigInPtr[ii] != 0)
				{
					// Get net open position netPO
					netPO = sumQty(openLedger);

					// Check if additive or reductive
					// Additive
					if ((netPO <= 0 && sigInPtr[ii] <= -1) || (netPO >= 0 && sigInPtr[ii] >= 1))
					{
						// No special handling required for an advanced signal when it is additive
						// We will add a check for the proper advanced fraction value or ease of extending
						// the logic in the future
						double advSig = sigInPtr[ii]-int(sigInPtr[ii]);
						if (knownAdvSig(advSig))
						{
							// Trade is additive. Add or create existing position --> openLedger
							openLedger.push_back(createLineEntry(ii, int(sigInPtr[ii]), dataInPtr[ii+1]));
						}
						else
						{
							// Unknown advanced signal.  Throw an error.
							mexErrMsgIdAndTxt( "calcProfitLoss:AdvancedSignal:fractionUnknown",
								"A signal contained an advanced fractional instruction %f that we could not interpret. Aborting.",sigInPtr[ii]);
						}
					}
					// Reductive
					else if ((netPO < 0 && sigInPtr[ii] > 0) || (netPO > 0 && sigInPtr[ii] < 0))
					{
						// Is this an advanced signal?
						if(fraction(sigInPtr[ii]))
						{
							if(knownAdvSig(sigInPtr[ii]))
							{
								// Liquidate any open position
								while (openLedger.size() !=0)
								{
									// Aggregate cash for corresponding observations (signal + 1)
									cashIdx[ii+1] = cashIdx[ii+1] + ((dataInPtr[ii+1] - openLedger[0].price) * openLedger[0].quantity * BIG_POINT) - (abs(openLedger[0].quantity)* COST);
									openLedger.pop_front();
								}

								// New position should be the integer of the advanced signal
								newPO = int(sigInPtr[ii]);

								if (newPO != 0)
								{
									// Put new openPosition on the ledger if applicable (not 0)
									openLedger.push_back(createLineEntry(ii, newPO, dataInPtr[ii+1]));
								}
							}
							else
							{
								// Unknown advanced signal.  Throw an error.
								mexErrMsgIdAndTxt( "calcProfitLoss:AdvancedSignal:fractionUnknown",
									"A signal contained an advanced fractional instruction %f that we could not interpret. Aborting.",sigInPtr[ii]);
							}
							
						}
						// Simple signal.  It may not fully liquidate any open position and must be handled accordingly.
						// We cannot simply empty the openLedger.
						else
						{
							// Check if new trade is larger than existing position
							if (abs(sigInPtr[ii]) >= abs(netPO))
							{
								// New trade is larger than or equal to existing position. Calculate cash on all ledger lines
								while (openLedger.size() !=0)
								{
									// Aggregate cash for corresponding observations (signal + 1)
									cashIdx[ii+1] = cashIdx[ii+1] + ((dataInPtr[ii+1] - openLedger[0].price) * openLedger[0].quantity * BIG_POINT) - (abs(openLedger[0].quantity)* COST);
									openLedger.pop_front();
								}

								// Reduce new trades by previous position and create new entry if applicable
								newPO = sigInPtr[ii] + netPO;		// variable to calculate new position size
								if (newPO != 0)
								{
									openLedger.push_back(createLineEntry(ii,newPO,dataInPtr[ii+1]));
								}
							}
							else
							{
								// New trade is smaller than the current open position.
								// How many do we need to reduce by?
								int needQty = sigInPtr[ii];
								// Prepare to iterate until we are satisfied
								while (needQty !=0)
								{
									// Is the current line item quantity larger than what we need?
									if (abs(openLedger[0].quantity) > needQty)
									{
										// If so we will P&L the quantity we need and reduce the open position size
										cashIdx[ii+1] = cashIdx[ii+1] + ((dataInPtr[ii+1] - openLedger[0].price) * -needQty * BIG_POINT) - (abs(needQty) * COST);
										// Reduce the position size.  We are aggregating so we add (e.g. 5 Purchases + 4 Sales = 1 Long)
										openLedger[0].quantity = openLedger[0].quantity + needQty;
										// We are satisfied and don't need any more contracts
										needQty = 0;
									}
									// Current line item quantity is equal to or smaller than what we need.  Process P&L and remove.
									else
									{
										// P&L entire quantity
										cashIdx[ii+1] = cashIdx[ii+1] + ((dataInPtr[ii+1] - openLedger[0].price) * -openLedger[0].quantity * BIG_POINT) - (abs(openLedger[0].quantity) * COST);
										// Reduce needed quantity by what we've been provided
										needQty = needQty + openLedger[0].quantity;
										// Remove the line item (FIFO)
										openLedger.pop_front();
									}
								}
							}
						}
					}
				}

				// Calculate current openEQ if there are any positions
				// Make sure we have an open position
				if (openLedger.size() != 0)
				{
					// We will aggregate all line items
					for (int jj = 0; jj < openLedger.size(); jj++)
					{
						openEQIdx[ii+1] = openEQIdx[ii+1] + ((dataInPtr[ii+1+SHIFT_CLOSE] - openLedger[jj].price) * openLedger[jj].quantity * BIG_POINT);
					}
				}
			} // end for

			// destroy the deque
			openLedger.~deque();

			// These are for convenience and could be removed for optimization
			// Calculate a cumulative sum of closed trades and open equity per observation
			double runSum = 0;
			for (int kk=0; kk < rowsData; kk++)
			{
				runSum = runSum + cashIdx[kk];
				netLiqIdx[kk] = runSum + openEQIdx[kk];
			}

			// Calculate a return from day to day based on the change in value observation to observation
			returnsIdx[0] = 0;
			for (int ll=1; ll < rowsData; ll++)
			{
				returnsIdx[ll] = netLiqIdx[ll] - netLiqIdx[ll-1];
			}
		}
		// No trades or signal on the last observation. Return zeros.
		else
		{
			for (int mm=0; mm < rowsData; mm++)
			{
				cashIdx[mm] = 0;
				openEQIdx[mm]=0;
				netLiqIdx[mm]=0;
				returnsIdx[mm]=0;
			}
		}

	return;
}

/////////////
//
// METHODS
//
/////////////

// Constructor for ledger line item creation
tradeEntry createLineEntry(int ID, int qty, double price)
{
	tradeEntry lineEntry;
	lineEntry.index = ID;
	lineEntry.quantity = qty;
	lineEntry.price = price;

	return lineEntry;
}

// Method to sum the quantity values in any struct of type tradeEntry
int sumQty(const deque<tradeEntry>& x)
{
	int sumOfQty = 0;  // the sum is accumulated here
	// for (int i=0; i<x.size(); i++)
	for (deque<tradeEntry>::const_iterator it=x.begin();it!=x.end();it++)
	{
		//sumOfQty += x[i].price;
		sumOfQty += it->quantity;
	}

	return sumOfQty;
}

bool fraction(double num)
{
	if (int(num) == num)
	{
		return false;
	}
	return true;
}

bool knownAdvSig(double advSig)
{
	// We can check for known advanced signals to help in debugging
	// by registering them here.  This can be a searchable array when
	// more than one advanced signal exists.
	// For now we only need to check for |0.5|

	if (abs(advSig - int(advSig)) == 0.5)
	{
		return true;
	}
	return false;
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
//                      	 WITHOUT ANY WARRANTY
//
//                  WITHOUT CLAIM AS TO MERCHANTABILITY
//
//                  OR FITNESS FOR A PARTICULAR PURPOSE
//
//                           expressed or implied.
//
//   Use of this code, pseudocode, algorithmic or trading logic contained
//   herein, whether sound or faulty for any purpose is the sole
//   responsibility of the USER. Any such use of these algorithms, coding
//   logic or concepts in whole or in part carry no covenant of correctness
//   or recommended usage from the AUTHOR or any of the possible
//   contributors listed or unlisted, known or unknown.
//
//   Any reference of this code or to this code including any variants from
//   this code, or any other credits due this AUTHOR from this code shall be
//   clearly and unambiguously cited and evident during any use, whether in
//   whole or in part.
//
//   The public sharing of this code does not relinquish, reduce, restrict or
//   encumber any rights the AUTHOR has in respect to claims of intellectual
//   property.
//
//   IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
//   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
//   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
//   STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//   ANY WAY OUT OF THE USE OF THIS SOFTWARE, CODE, OR CODE FRAGMENT(S), EVEN
//   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//   -------------------------------------------------------------------------
//
//                             ALL RIGHTS RESERVED
//
//   -------------------------------------------------------------------------
//
//   Author:	Mark Tompkins
//   Revision:	4910.25042
//   Copyright:	(c)2013
//