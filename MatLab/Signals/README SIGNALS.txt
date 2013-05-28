There are two types of elemental files, those that define a STATE and those that create or modify a SIGNAL.
This README file describes the SIGNAL convention if greater detail.

STATE	A state should reduce to a boolean true or false condition.  For example a given 'Price' either is or isn't less than a calculated 'Moving Average'
		E.g. Price < Moving Average == TRUE | FALSE

SIGNAL	A signal is generated from one or more STATE conditions resolving to boolean true or false.  A given SIGNAL output has one of three possibilities:
			SIGNAL < 0 THEN Sell
			SIGNAL = 0 THEN No action
			SIGNAL > 0 THEN Buy
		E.g. BUY Signal IF Price > Moving Average.
		
		The convention that is used further refines to the following:
				  SIGNAL < -1	THEN Sell Reverse		If any long position exists from this or other signals, liquidate all longs and SELL 1 CONTRACT to create net short 1 position.
			-1 >= SIGNAL <  0	THEN Sell Additive		Add a SALE to any current position. This may reduce a long position by one.  It may also create or increase a short position also by one.
			 0 == SIGNAL == 0	THEN No Action			A zero signal is an evaluated false.  Signal IF conditions have a boolean output of TRUE > 0 or FALSE == 0
			 0 >  SIGNAL =< 1	THEN Buy Additive		Add a PURCHASE to any current position.  This may reduce a short position by one.  It may also create or increase a long position also by one.
				  SIGNAL >  1	THEN Buy Reverse		If any short position exists from this or other signals, cover all shorts and BUY 1 CONTRACT to create net long 1 position.
				  
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