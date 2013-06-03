#include <string.h>
#include "mex.h"
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray
*prhs[])
{
    mwSize m, n, i, m1, nbytes;
    double *source, *target;
    
    m = mxGetM(prhs[0]);
    n = mxGetN(prhs[0]);
    m1 = m - 1;
    plhs[0] = mxCreateDoubleMatrix(m1, n, mxREAL);
    target = mxGetPr(plhs[0]);
    source = mxGetPr(prhs[0]);
    nbytes = m1 * sizeof(double);
    for( i=0; i<n; i++ ) {
        memcpy(target, source, nbytes);
        target += m1;
        source += m;
    }
}