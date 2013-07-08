PAR files are wrappers for the parallel processing of vectorized parametric sweeps.  
Most parametric sweeps are coded to sweep for an optimum sharpe ratio.  
**PARMETS** files sweep for an optimum **shMETS** value.

>shMETS = ((shTest * 2) + shVal) / 3

where the input data is bifurcated 80% test set (*shTest*), 20% validation set (*shVal*).
	
**Naming Convention:**  

| Name Style | Description |
| ---------- | ----------- |
|function|ELEMENTAL function|
|functionDIS|DISPLAY function|
|functionPAR|PARAMETRIC function|
|functionSIG|SIGNAL function|
|functionSTA|STATE function|
|functionFunctionSIG|SIGNAL AGGREGATOR function|
|functionFunctionSTA|STATE AGGREGATOR function|  

Author:          Mark Tompkins  
Revision:		 4902.23648