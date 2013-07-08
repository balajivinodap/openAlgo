// Signal Aggregators take two or more STATE functions and combines them logically to create an aggregated SIGNAL output.
//
//	Signal aggregator files should take care to call STA functions and then produce a signal (SIG).  If this step is bypassed
//	and the aggregator uses SIG functions, additional and unnecessary calls to calcProfitLoss will occur.
//
// INPUT:	A minimum of two SIGNAL vectors
// OUTPUT:	A minimum of a single actionable SIGNAL
//
// Naming Convention:
//			function				ELEMENTAL function
//			functionSIG				SIGNAL function
//			functionSTA				STATE function
//			functionFunctionSIG		SIGNAL AGGREGATOR function
//			functionFunctionSTA		STATE AGGREGATOR function
//
// Author:			Mark Tompkins
// Revision:		4902.23918
// All rights reserved.