taInvoke.cpp is a wrapper for calling the open source ta-lib library through a compiled mex routine.
mexOpts.txt are the paths for the ta-lib C source files to be passed when mex'ing in MatLab
	e.g.	mex taInvoke.cpp @mexOpts

The ta-library is available from http://www.ta-lib.org

If the size of the compiled taInvoke file is of concern, the individual functions can be mex'ed individually and called.
This may be useful in HPC parametric sweeps to minimize data transfer of unused functions in the larger compilation.

