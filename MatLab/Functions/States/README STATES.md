**This README file describes the STAte convention in greater detail.**

There are primarily two types of functions that call elemental function.  These are:
> STAte functions  
> SIGnal function  

**STATE:** A STAte should reduce to a Boolean true or false condition.  
For example a given observational 'Price' either is or isn't less than a calculated 'Moving Average' value:  
> (pseudo e.g.) Observational Price < Moving Average =?= TRUE || FALSE

**SIGNAL:**	A SIGnal is an actionable array that should be able to be passed to the P&L function for analysis.  
A SIGnal is generated from one or more STAte conditions resolving to Boolean true or false.  
> (pseudo e.g.) BUY Signal IF Price > Moving Average

A given STAte output has one of two outputs:

| Signal Output | Boolean |
| ------------- | ------- |
|0|False|
|1|True|

By utilizing one or more conditional evaluations, we are able to create a desired SIGnal.

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
 Revision:		4937.18944
 All rights reserved.