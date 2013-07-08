**This README file describes the SIGnal convention if greater detail.**

There are primarily two types of functions that call elemental function.  These are:
> STAte functions  
> SIGnal function  



**STATE:** A state should reduce to a Boolean true or false condition.  
For example a given 'Price' either is or isn't less than a calculated 'Moving Average'  
> (pseudo e.g.) Observational Price < Moving Average =?= TRUE || FALSE

**SIGNAL:**	A signal is generated from one or more STATE conditions resolving to Boolean true or false.  
A given SIGNAL output has one of three base outputs:

| Signal Output | Action |
| ------------- | ------ |
|SIGNAL < 0|Sell|
|SIGNAL = 0|No action|
|SIGNAL > 0|Buy|

> (pseudo e.g.) BUY Signal IF Price > Moving Average

**Advanced Signals:** By leveraging fractions as additional instructional logic, we are able to construct more meaningful signals beyond the scope of a simple Buy or Sell of quantity 'X'. As you can't buy or sell 1/2 a share or 1/2 a contract, we can quickly check for additional handling of a signal that contains a fractional element for more advanced processing.
		
The convention that is used further refines to the following to effectuate this advanced handling of produced signals. 'NET' is a current net position:

| NET | SIGNAL | ACTION | Description |
| --- | ------ | ------ | ----------- |
|any|0|none|A zero signal is an evaluated false to a possible state trigger and instructs to 'do nothing'.|
|any|X (integer)|Buy or Sell X|An integer instructs to BUY or SELL quantity X.|
|any|+/- 0.5 (fraction)|Close Any Opposite Position|Close out any opposite existing position such that a NET = 0 flat condition exists. Ignore in the case of same direction. If no position exists, no error is thrown.
|||||
|<= 0|X.5 (fraction)|Reverse to position NET = X|Close out any existing short position and buy X longs to create a long position such that NET = X.
|>= 0|-X.5 (fraction)|Reverse to position NET = -X|Close out any existing long position and sell X shorts to create a short position such that NET = -X.



			
			NET < 0		SIGNAL = -X.5 | INT(X)<=-1	ERROR								An error is thrown when we have an existing short position and we are given a reverse to net short signal
			NET > 0		SIGNAL = +X.5 | INT(X)>=1	ERROR								An error is thrown when we have an existing long position and we are given a reverse to net long signal
																						
		NOTE: 	This convention should also work with those who do not want to avail themselves with the fractional logic.
				For example consider the following:
				
		EX 1	Without fractional logic				With fractional logic
				NET		 	= 	-1						NET			=	-1
				SIGNAL		=	 2						SIGNAL		=	1.5
				final NET	=	 1						final NET	=	 1
				
		EX 2	Without fractional logic				With fractional logic
				NET		 	= 	-50						NET			=	-50
				SIGNAL		=	 51						SIGNAL		=	1.5
				final NET	=	 1						final NET	=	 1
				
		EX 3	Without fractional logic				With fractional logic
				NET		 	= 	-50						NET			=	-50
				SIGNAL		=	 55						SIGNAL		=	5.5
				final NET	=	 5						final NET	=	 5
																							
																						
		A SIGNAL is an actionable array that should be able to be passed to the P&L function for analysis.
		
		FUNDAMENTAL CONSIDERATION:
		
		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		!!!! STATES MAY OR MAY NOT REPEAT PER OBSERVATION !!!!
		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

 Author:			Mark Tompkins
 Revision:		4902.24057
 All rights reserved.

		This is a fundamental difference that needs to be considered.  
		A STATE is a condition versus a SIGNAL which is an action.
		
		Naming Convention:
			function				ELEMENTAL function
			functionSIG				SIGNAL function
			functionSTA				STATE function
			functionSIG_DIS			SIGNAL function with graphing
			functionFunctionSIG		SIGNAL AGGREGATOR function
			functionFunctionSIG_DIS	SIGNAL AGGREGATOR with graphing
			functionFunctionSTA		STATE AGGREGATOR function