/*
 * virtualBarsMEX_initialize.c
 *
 * Code generation for function 'virtualBarsMEX_initialize'
 *
 * C source code generated on: Thu May 30 16:52:20 2013
 *
 */

/* Include files */
#include "rt_nonfinite.h"
#include "virtualBarsMEX.h"
#include "virtualBarsMEX_initialize.h"
#include "virtualBarsMEX_data.h"

/* Function Definitions */
void virtualBarsMEX_initialize(emlrtContext *aContext)
{
  emlrtBreakCheckR2012bFlagVar = emlrtGetBreakCheckFlagAddressR2012b();
  emlrtCreateRootTLS(&emlrtRootTLSGlobal, aContext, NULL, 1);
  emlrtClearAllocCountR2012b(emlrtRootTLSGlobal, FALSE, 0U, 0);
  emlrtEnterRtStackR2012b(emlrtRootTLSGlobal);
  emlrtFirstTimeR2012b(emlrtRootTLSGlobal);
}

/* End of code generation (virtualBarsMEX_initialize.c) */
