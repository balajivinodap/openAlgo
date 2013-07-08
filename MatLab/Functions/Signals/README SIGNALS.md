**This README file describes the SIGnal convention in greater detail.**

There are primarily two types of functions that call elemental function.  These are:
> STAte functions  
> SIGnal function  

**STATE:** A STAte should reduce to a Boolean true or false condition.  
For example a given observational 'Price' either is or isn't less than a calculated 'Moving Average' value:  
> (pseudo e.g.) Observational Price < Moving Average =?= TRUE || FALSE

**SIGNAL:**	A SIGnal is an actionable array that should be able to be passed to the P&L function for analysis.  
A SIGnal is generated from one or more STAte conditions resolving to Boolean true or false.  
> (pseudo e.g.) BUY Signal IF Price > Moving Average

A given SIGNAL output has one of three base outputs:

| Signal Output | Action |
| ------------- | ------ |
|SIGNAL < 0|Sell|
|SIGNAL = 0|No action|
|SIGNAL > 0|Buy|

**Advanced Signals:** By leveraging fractions as additional instructional logic, we are able to construct more meaningful signals beyond the scope of a simple Buy or Sell of quantity 'X'. As you can't buy or sell 1/2 a share or 1/2 a contract, we can quickly check for additional handling of a signal that contains a fractional element for more advanced processing.
> (pseudo e.g.) REVERSE Signal IF Price > Moving Average

The convention that is used further refines to the following to effectuate this advanced handling of produced signals. 'NET' is a current net position:

| NET | SIGNAL | ACTION | Description |
| --- | ------ | ------ | ----------- |
|any|0|none|A zero signal is an evaluated false to a possible state trigger and instructs to 'do nothing'.|
|any|X (integer)|Buy or Sell X|An integer instructs to BUY or SELL quantity X|
|any|+/- 0.5 (fraction)|Close Any Opposite Position|Close out any opposite existing position such that a NET = 0 flat condition exists. Ignore in the case of same direction. If no position exists, no error is thrown|
|||||
|<=0|X.5 (fraction)|Reverse to position NET = X|Close out any existing short position and buy X longs to create a long position such that NET = X|
|>=0|-X.5 (fraction)|Reverse to position NET = -X|Close out any existing long position and sell X shorts to create a short position such that NET = -X|

																						
		NOTE: 	This convention maintains compatibility with those who opt not to avail 
				themselves with the fractional logic.

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
		
**FUNDAMENTAL CONSIDERATIONS:**

		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		!!!! STATES MAY OR MAY NOT REPEAT PER OBSERVATION !!!!
		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		!!!!    SIGNALS SHOULD BE CLEAN AND NOT REPEAT    !!!!
		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

**Function Naming Convention:**  

| Name Style | Description |
| ---------- | ----------- |
|function|ELEMENTAL function|
|functionDIS|DISPLAY function|
|functionPAR|PARAMETRIC function|
|functionSIG|SIGNAL function|
|functionSTA|STATE function|
|functionFunctionSIG|SIGNAL AGGREGATOR function|
|functionFunctionSTA|STATE AGGREGATOR function| 

 Author:		Mark Tompkins  
 Revision:		4937.18292  
 All rights reserved.