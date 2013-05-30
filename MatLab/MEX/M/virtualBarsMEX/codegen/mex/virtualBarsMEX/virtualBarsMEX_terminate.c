/*
 * virtualBarsMEX_terminate.c
 *
 * Code generation for function 'virtualBarsMEX_terminate'
 *
 * C source code generated on: Thu May 30 16:52:20 2013
 *
 */

/* Include files */
#include "rt_nonfinite.h"
#include "virtualBarsMEX.h"
#include "virtualBarsMEX_terminate.h"

/* Function Definitions */
void virtualBarsMEX_atexit(void)
{
  emlrtCreateRootTLS(&emlrtRootTLSGlobal, &emlrtContextGlobal, NULL, 1);
  emlrtEnterRtStackR2012b(emlrtRootTLSGlobal);
  emlrtLeaveRtStackR2012b(emlrtRootTLSGlobal);
  emlrtDestroyRootTLS(&emlrtRootTLSGlobal);
}

void virtualBarsMEX_terminate(void)
{
  emlrtLeaveRtStackR2012b(emlrtRootTLSGlobal);
  emlrtDestroyRootTLS(&emlrtRootTLSGlobal);
}

/* End of code generation (virtualBarsMEX_terminate.c) */
