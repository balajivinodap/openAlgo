SIGnal Aggregators take two or more STAte functions and combines them logically to create an aggregated SIGnal output.

**SIGnal aggregation files should take care to call STAte functions and then produce a signal.**

If this step is bypassed and the aggregation functions uses SIGnal functions,  
additional and unnecessary calls to the function *calcProfitLoss* will occur.

>INPUT:	A minimum of two SIGNAL vectors  
>OUTPUT:	A minimum of a single actionable SIGNAL

**Naming Convention:**  

| Name Style | Description |
| ---------- | ----------- |
|function|ELEMENTAL function| 
|functionSIG|SIGNAL function|
|functionSTA|STATE function|
|functionFunctionSIG|SIGNAL AGGREGATOR function|
|functionFunctionSTA|STATE AGGREGATOR function|  

Author:			Mark Tompkins  
Revision:		4902.23918  
All rights reserved.