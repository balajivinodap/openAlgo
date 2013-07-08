taInvoke.cpp is a wrapper for calling the open source ta-lib library through a compiled mex routine.  
mexOpts.txt contains the paths for the ta-lib C source files to be passed when mex'ing in MatLab
	
	mex taInvoke.cpp @mexOpts

The ta-library is available from [http://www.ta-lib.org](http://www.ta-lib.org "http://www.ta-lib.org")

If the size of the compiled taInvoke file is of concern, the individual functions can be mex'ed individually and called.  This may be useful in not appropriate in HPC parametric sweeps to minimize data transfer of unused functions in the larger compilation.

