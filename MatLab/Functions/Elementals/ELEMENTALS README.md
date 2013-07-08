Elementals are functions that are fundamental calculations. 

As a general statement elementals should:


- return either a scalar or vector of a mathematical result    


- not call any other function other than another elemental function

They should NOT return a SIGnal or STAte generation directly.

This atomic approach allows to code correction without need for downstream edits.

For example, to create a moving average crossover system one would:


1. Create a moving average elemental function 
2. Create a moving average STAte funciton incorporating two averages and their corresponding state to each other
3. Create a moving average SIGnal function that interprets the STAte output and creates an actionable SIGnal.

> elemental function --> state function --> signal function  

**Mexing**:   Elemental functions should always be coded in such a fashion as they can be mex'd.

Author:          Mark Tompkins  
Revision:		 4937.16376
