Elementals are functions that are fundamental calculations.  
As a general statement, they should return either a scalar or vector of a mathematical result.

They should NOT return a signal generation directly.
Any SIGNAL generation that is germane to a given elemental function should come from a SIGNAL function specific to said elemental function.

For example, a moving average elemental should calculate a given moving average of a supplied vector or vectors.
A movingAverageSIG should call the elemental function and output a given signal.
