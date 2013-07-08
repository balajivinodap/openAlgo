// There are two types of elemental files, those that define a STATE and those that create or modify a SIGNAL.
// This README file describes the STATE convention if greater detail.
//
// STATE	A state should reduce to a boolean true or false condition.  For example a given 'Price' either is or isn't less than a calculated 'Moving Average'
//		e.g. Price < Moving Average == TRUE | FALSE
//		
//		By utilizing one or more conditional evaluations, we are able to create a desired SIGNAL.
//
// SIGNAL	A signal is generated from one or more STATE conditions resolving to boolean true or false.
//
//		FUNDAMENTAL CONSIDERATION:
//
//		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//		!!!! SIGNALS SHOULD BE CLEAN AND NOT REPEAT !!!!
//		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//		
//		This is a fundamental difference that needs to be considered.  
//		A STATE is a condition versus a SIGNAL which is an action.
//		
//		Naming Convention:
//			function				ELEMENTAL function
//			functionSIG				SIGNAL function
//			functionSTA				STATE function
//			functionFunctionSIG		SIGNAL AGGREGATOR function
//			functionFunctionSTA		STATE AGGREGATOR function
//
// Author:			Mark Tompkins
// Revision:		4902.24102
// All rights reserved.