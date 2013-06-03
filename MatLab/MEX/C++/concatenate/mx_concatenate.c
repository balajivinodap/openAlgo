#include <string.h>
#include "mex.h"
/* Function to do horizontal concatenation
 The function takes two 2-D input arrays and outputs the concatenated array*/

void
mexFunction(int nlhs,mxArray *plhs[],int nrhs,const mxArray *prhs[])
{
    int m[2],n[2],nbytes,bytes_to_copy,destination_offset;
    double *ptr,*newptr;
    
    if(nrhs != 2)
    {
        mexErrMsgTxt("Two arrays are required to concatenate");
    }
    if(mxGetNumberOfDimensions(prhs[0]) !=2 || mxGetNumberOfDimensions(prhs[0]) != 2)
    {   
        mexErrMsgTxt("This function only concatenates 2D arrays");
    }
    if(mxGetClassID(prhs[0]) != mxDOUBLE_CLASS || mxGetClassID(prhs[1]) != mxDOUBLE_CLASS)
    {
        mexErrMsgTxt("This function only handles double matrices");
    }
    //get dimensions
    m[0] = mxGetM(prhs[0]);
    m[1] = mxGetM(prhs[1]);
    n[0] = mxGetN(prhs[0]);
    n[1] = mxGetN(prhs[1]);
    if(m[0] != m[1])
    {
           mexErrMsgTxt("Dimension mismatch");
    }
    nbytes = (m[0]) * (n[0] + n[1])* sizeof(double);//size of new array
    destination_offset = m[0] * n[0]; //start offset for copying
    bytes_to_copy = m[1] * n[1] * sizeof(double);
    ptr = mxGetPr(prhs[0]);
	newptr = (double *)mxRealloc(ptr, nbytes);//reallocate array
    mxSetPr(prhs[0],newptr);
    ptr = mxGetPr(prhs[1]);
    memcpy(newptr+destination_offset,ptr,bytes_to_copy);//actual copy
    mxSetN(prhs[0],n[0]+n[1]);//fix dimension
    plhs[0] = mxDuplicateArray(prhs[0]); //return as output
}



