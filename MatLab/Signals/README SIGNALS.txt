There are two types of elemental files, those that define a STATE and those that create or modify a SIGNAL.
This README file describes the SIGNAL convention if greater detail.

STATE	A state should reduce to a boolean true or false condition.  For example a given 'Price' either is or isn't less than a calculated 'Moving Average'
		E.g. Price < Moving Average == TRUE | FALSE

SIGNAL	A signal is generated from one or more STATE conditions resolving to boolean true or false.  A given SIGNAL output has one of three base outputs:
			SIGNAL < 0 THEN Sell
			SIGNAL = 0 THEN No action
			SIGNAL > 0 THEN Buy
		E.g. BUY Signal IF Price > Moving Average.
		
		By leveraging fractions as additional logic, we are able to construct more meaningful signals beyond the scope of a simple Buy or Sell quantity X.
		As you can't buy or sell 1/2 a share or 1/2 a lot, we can quickly check for additional handling of a signal that contains a fractional element.
		
		The convention that is used further refines to the following to effectuate some advanced handling of produced signals. 'NET' is a current net position:
			NET = any	SIGNAL = 0					THEN No Action						A zero signal is an evaluated false to a possible state trigger and instructs to 'do nothing'.  
																						Signal IF conditions have a boolean output of TRUE == 1 or FALSE == 0
			NET = any	SIGNAL = X		(INTEGER)	THEN Buy or Sell X					An integer instructs to BUY or SELL quantity X. 
																						This can be additive, reductive or initiating in respect to NET.
			NET = any	SIGNAL = +/-0.5	(FRACTION)	THEN Close Out Any Position			Close out any existion position such that a NET = 0 flat condition exists.
																						If no position exists, no error is thrown.
			
			NET <= 0	SIGNAL = X.5	(FRACTION)	THEN Reverse to position NET = X	Close out any existing short position and buy X longs to create a NET long position of quantity X
			NET >= 0	SIGNAL = -X.5	(FRACTION)	THEN Reverse to position NET = -X	Close out any existing long position and sell X shorts to create a NET short position of quantity X
			
			NET < 0		SIGNAL = -X.5 | INT(X)<=-1	ERROR								An error is thrown when we have an existing short position and we are given a reverse to net short signal
			NET > 0		SIGNAL = +X.5 | INT(X)>=1	ERROR								An error is thrown when we have an existing long position and we are given a reverse to net long signal
																						
		NOTE: 	This convention should also work with those who do not want to avail themselves with the fractional logic.
				For example consider the following:
				
		EX 1	Without fractional logic				With fractional logic
				NET		 	= 	-1						NET			=	-1
				SIGNAL		=	2						SIGNAL		=	1.5
				final NET	=	1						final NET	=	1
				
		EX 2	Without fractional logic				With fractional logic
				NET		 	= 	-50						NET			=	-50
				SIGNAL		=	51						SIGNAL		=	1.5
				final NET	=	1						final NET	=	1
				
		X 2		Without fractional logic				With fractional logic
				NET		 	= 	-50						NET			=	-50
				SIGNAL		=	55						SIGNAL		=	5.5
				final NET	=	5						final NET	=	5
																							
																						
		A SIGNAL is an actionable array that should be able to be passed to the P&L function for analysis.
		
		FUNDAMENTAL CONSIDERATION:
		
		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		!!!! STATES MAY OR MAY NOT REPEAT PER OBSERVATION !!!!
		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		This is a fundamental difference that needs to be considered.  
		A STATE is a condition versus a SIGNAL which is an action.
		
		Naming Convention:
			function				ELEMENTAL function
			functionSIG				SIGNAL function
			functionSTA				STATE function
			functionFunctionSIGA	SIGNAL AGGREGATOR function
			functionFunctionSTAA	STATE AGGREGATOR function