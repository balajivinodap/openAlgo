/*
 * virtualBarsMEX.c
 *
 * Code generation for function 'virtualBarsMEX'
 *
 * C source code generated on: Thu May 30 16:52:20 2013
 *
 */

/* Include files */
#include "rt_nonfinite.h"
#include "virtualBarsMEX.h"
#include "virtualBarsMEX_emxutil.h"
#include "virtualBarsMEX_data.h"

/* Variable Definitions */
static emlrtRSInfo emlrtRSI = { 39, "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m" };

static emlrtRSInfo b_emlrtRSI = { 45, "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m" };

static emlrtRSInfo c_emlrtRSI = { 43, "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m" };

static emlrtRSInfo d_emlrtRSI = { 49, "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m" };

static emlrtRSInfo e_emlrtRSI = { 55, "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m" };

static emlrtRSInfo f_emlrtRSI = { 10, "eml_null_assignment",
  "C:/Program Files/MATLAB/R2013a/toolbox/eml/lib/matlab/eml/eml_null_assignment.m"
};

static emlrtRSInfo g_emlrtRSI = { 11, "eml_null_assignment",
  "C:/Program Files/MATLAB/R2013a/toolbox/eml/lib/matlab/eml/eml_null_assignment.m"
};

static emlrtRSInfo h_emlrtRSI = { 56, "eml_null_assignment",
  "C:/Program Files/MATLAB/R2013a/toolbox/eml/lib/matlab/eml/eml_null_assignment.m"
};

static emlrtRSInfo i_emlrtRSI = { 122, "eml_null_assignment",
  "C:/Program Files/MATLAB/R2013a/toolbox/eml/lib/matlab/eml/eml_null_assignment.m"
};

static emlrtRSInfo j_emlrtRSI = { 125, "eml_null_assignment",
  "C:/Program Files/MATLAB/R2013a/toolbox/eml/lib/matlab/eml/eml_null_assignment.m"
};

static emlrtRSInfo l_emlrtRSI = { 16, "error",
  "C:/Program Files/MATLAB/R2013a/toolbox/eml/lib/matlab/lang/error.m" };

static emlrtMCInfo emlrtMCI = { 39, 21, "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m" };

static emlrtMCInfo b_emlrtMCI = { 45, 21, "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m" };

static emlrtMCInfo c_emlrtMCI = { 57, 9, "eml_null_assignment",
  "C:/Program Files/MATLAB/R2013a/toolbox/eml/lib/matlab/eml/eml_null_assignment.m"
};

static emlrtMCInfo d_emlrtMCI = { 56, 19, "eml_null_assignment",
  "C:/Program Files/MATLAB/R2013a/toolbox/eml/lib/matlab/eml/eml_null_assignment.m"
};

static emlrtMCInfo e_emlrtMCI = { 122, 9, "eml_null_assignment",
  "C:/Program Files/MATLAB/R2013a/toolbox/eml/lib/matlab/eml/eml_null_assignment.m"
};

static emlrtMCInfo f_emlrtMCI = { 125, 9, "eml_null_assignment",
  "C:/Program Files/MATLAB/R2013a/toolbox/eml/lib/matlab/eml/eml_null_assignment.m"
};

static emlrtMCInfo i_emlrtMCI = { 16, 1, "error",
  "C:/Program Files/MATLAB/R2013a/toolbox/eml/lib/matlab/lang/error.m" };

static emlrtRTEInfo emlrtRTEI = { 1, 22, "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m" };

static emlrtRTEInfo b_emlrtRTEI = { 21, 1, "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m" };

static emlrtRTEInfo c_emlrtRTEI = { 40, 13, "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m" };

static emlrtRTEInfo d_emlrtRTEI = { 46, 13, "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m" };

static emlrtRTEInfo e_emlrtRTEI = { 1, 14, "eml_null_assignment",
  "C:/Program Files/MATLAB/R2013a/toolbox/eml/lib/matlab/eml/eml_null_assignment.m"
};

static emlrtECInfo emlrtECI = { 1, 51, 21, "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m" };

static emlrtECInfo b_emlrtECI = { -1, 46, 17, "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m" };

static emlrtDCInfo emlrtDCI = { 46, 17, "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m", 1 };

static emlrtBCInfo emlrtBCI = { -1, -1, 46, 17, "lTemp", "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m", 0 };

static emlrtECInfo c_emlrtECI = { -1, 40, 17, "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m" };

static emlrtDCInfo b_emlrtDCI = { 40, 17, "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m", 1 };

static emlrtBCInfo b_emlrtBCI = { -1, -1, 40, 17, "hTemp", "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m", 0 };

static emlrtECInfo d_emlrtECI = { 1, 36, 17, "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m" };

static emlrtBCInfo c_emlrtBCI = { -1, -1, 35, 49, "data", "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m", 0 };

static emlrtBCInfo d_emlrtBCI = { -1, -1, 35, 18, "data", "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m", 0 };

static emlrtDCInfo c_emlrtDCI = { 35, 18, "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m", 1 };

static emlrtBCInfo e_emlrtBCI = { -1, -1, 34, 18, "data", "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m", 0 };

static emlrtDCInfo d_emlrtDCI = { 34, 18, "virtualBarsMEX",
  "G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m", 1 };

/* Function Declarations */
static void b_emlrt_marshallIn(const mxArray *u, const emlrtMsgIdentifier
  *parentId, emxArray_real_T *y);
static int32_T div_s32_floor(int32_T numerator, int32_T denominator);
static void eml_null_assignment(emxArray_real_T *x, real_T idx);
static void emlrt_marshallIn(const mxArray *b_slidefun, const char_T *identifier,
  emxArray_real_T *y);
static void error(const mxArray *b, emlrtMCInfo *location);
static void g_emlrt_marshallIn(const mxArray *src, const emlrtMsgIdentifier
  *msgId, emxArray_real_T *ret);
static const mxArray *message(const mxArray *b, emlrtMCInfo *location);
static const mxArray *slidefun(const mxArray *b, const mxArray *c, const mxArray
  *d, const mxArray *e, emlrtMCInfo *location);

/* Function Definitions */
static void b_emlrt_marshallIn(const mxArray *u, const emlrtMsgIdentifier
  *parentId, emxArray_real_T *y)
{
  g_emlrt_marshallIn(emlrtAlias(u), parentId, y);
  emlrtDestroyArray(&u);
}

static int32_T div_s32_floor(int32_T numerator, int32_T denominator)
{
  int32_T quotient;
  uint32_T absNumerator;
  uint32_T absDenominator;
  int32_T quotientNeedsNegation;
  uint32_T tempAbsQuotient;
  if (denominator == 0) {
    if (numerator >= 0) {
      quotient = MAX_int32_T;
    } else {
      quotient = MIN_int32_T;
    }

    emlrtDivisionByZeroErrorR2012b(0, emlrtRootTLSGlobal);
  } else {
    if (numerator >= 0) {
      absNumerator = (uint32_T)numerator;
    } else {
      absNumerator = (uint32_T)-numerator;
    }

    if (denominator >= 0) {
      absDenominator = (uint32_T)denominator;
    } else {
      absDenominator = (uint32_T)-denominator;
    }

    quotientNeedsNegation = ((numerator < 0) != (denominator < 0));
    tempAbsQuotient = absNumerator / absDenominator;
    if ((uint32_T)quotientNeedsNegation) {
      absNumerator %= absDenominator;
      if (absNumerator > (uint32_T)0) {
        tempAbsQuotient++;
      }
    }

    if ((uint32_T)quotientNeedsNegation) {
      quotient = -(int32_T)tempAbsQuotient;
    } else {
      quotient = (int32_T)tempAbsQuotient;
    }
  }

  return quotient;
}

static void eml_null_assignment(emxArray_real_T *x, real_T idx)
{
  boolean_T p;
  const mxArray *y;
  static const int32_T iv11[2] = { 1, 31 };

  const mxArray *m5;
  char_T cv7[31];
  int32_T i;
  static const char_T cv8[31] = { 'C', 'o', 'd', 'e', 'r', ':', 'M', 'A', 'T',
    'L', 'A', 'B', ':', 's', 'u', 'b', 's', 'd', 'e', 'l', 'd', 'i', 'm', 'm',
    'i', 's', 'm', 'a', 't', 'c', 'h' };

  int32_T k;
  const mxArray *b_y;
  emxArray_real_T *b_x;
  const mxArray *c_y;
  emxArray_real_T *c_x;
  emlrtHeapReferenceStackEnterFcnR2012b(emlrtRootTLSGlobal);
  emlrtPushRtStackR2012b(&f_emlrtRSI, emlrtRootTLSGlobal);
  p = TRUE;
  if (((int32_T)idx > x->size[0]) || (idx != muDoubleScalarFloor(idx))) {
    p = FALSE;
  }

  if (p) {
  } else {
    emlrtPushRtStackR2012b(&h_emlrtRSI, emlrtRootTLSGlobal);
    y = NULL;
    m5 = mxCreateCharArray(2, iv11);
    for (i = 0; i < 31; i++) {
      cv7[i] = cv8[i];
    }

    emlrtInitCharArrayR2013a(emlrtRootTLSGlobal, 31, m5, cv7);
    emlrtAssign(&y, m5);
    error(message(y, &c_emlrtMCI), &d_emlrtMCI);
    emlrtPopRtStackR2012b(&h_emlrtRSI, emlrtRootTLSGlobal);
  }

  emlrtPopRtStackR2012b(&f_emlrtRSI, emlrtRootTLSGlobal);
  emlrtPushRtStackR2012b(&g_emlrtRSI, emlrtRootTLSGlobal);
  i = x->size[0] - 1;
  for (k = (int32_T)idx; k <= i; k++) {
    x->data[k - 1] = x->data[k];
  }

  if (x->size[0] != 1) {
    if (i <= x->size[0]) {
    } else {
      emlrtPushRtStackR2012b(&i_emlrtRSI, emlrtRootTLSGlobal);
      b_y = NULL;
      m5 = mxCreateString("Assertion failed.");
      emlrtAssign(&b_y, m5);
      error(b_y, &e_emlrtMCI);
      emlrtPopRtStackR2012b(&i_emlrtRSI, emlrtRootTLSGlobal);
    }

    if (1 > i) {
      i = 0;
    }

    emxInit_real_T(&b_x, 1, &e_emlrtRTEI, TRUE);
    k = b_x->size[0];
    b_x->size[0] = i;
    emxEnsureCapacity((emxArray__common *)b_x, k, (int32_T)sizeof(real_T),
                      &e_emlrtRTEI);
    for (k = 0; k < i; k++) {
      b_x->data[k] = x->data[k];
    }

    k = x->size[0];
    x->size[0] = b_x->size[0];
    emxEnsureCapacity((emxArray__common *)x, k, (int32_T)sizeof(real_T),
                      &e_emlrtRTEI);
    i = b_x->size[0];
    for (k = 0; k < i; k++) {
      x->data[k] = b_x->data[k];
    }

    emxFree_real_T(&b_x);
  } else {
    if (i <= i + 1) {
    } else {
      emlrtPushRtStackR2012b(&j_emlrtRSI, emlrtRootTLSGlobal);
      c_y = NULL;
      m5 = mxCreateString("Assertion failed.");
      emlrtAssign(&c_y, m5);
      error(c_y, &f_emlrtMCI);
      emlrtPopRtStackR2012b(&j_emlrtRSI, emlrtRootTLSGlobal);
    }

    if (1 > i) {
      i = -1;
    } else {
      i = 0;
    }

    emxInit_real_T(&c_x, 1, &e_emlrtRTEI, TRUE);
    k = c_x->size[0];
    c_x->size[0] = i + 1;
    emxEnsureCapacity((emxArray__common *)c_x, k, (int32_T)sizeof(real_T),
                      &e_emlrtRTEI);
    k = 0;
    while (k <= i) {
      c_x->data[0] = x->data[0];
      k = 1;
    }

    k = x->size[0];
    x->size[0] = c_x->size[0];
    emxEnsureCapacity((emxArray__common *)x, k, (int32_T)sizeof(real_T),
                      &e_emlrtRTEI);
    i = c_x->size[0];
    for (k = 0; k < i; k++) {
      x->data[k] = c_x->data[k];
    }

    emxFree_real_T(&c_x);
  }

  emlrtPopRtStackR2012b(&g_emlrtRSI, emlrtRootTLSGlobal);
  emlrtHeapReferenceStackLeaveFcnR2012b(emlrtRootTLSGlobal);
}

static void emlrt_marshallIn(const mxArray *b_slidefun, const char_T *identifier,
  emxArray_real_T *y)
{
  emlrtMsgIdentifier thisId;
  thisId.fIdentifier = identifier;
  thisId.fParent = NULL;
  b_emlrt_marshallIn(emlrtAlias(b_slidefun), &thisId, y);
  emlrtDestroyArray(&b_slidefun);
}

static void error(const mxArray *b, emlrtMCInfo *location)
{
  const mxArray *pArray;
  pArray = b;
  emlrtCallMATLABR2012b(emlrtRootTLSGlobal, 0, NULL, 1, &pArray, "error", TRUE,
                        location);
}

static void g_emlrt_marshallIn(const mxArray *src, const emlrtMsgIdentifier
  *msgId, emxArray_real_T *ret)
{
  int32_T iv7[1];
  boolean_T bv0[1];
  int32_T iv8[1];
  int32_T i4;
  iv7[0] = -1;
  bv0[0] = TRUE;
  emlrtCheckVsBuiltInR2012b(emlrtRootTLSGlobal, msgId, src, "double", FALSE, 1U,
    iv7, bv0, iv8);
  i4 = ret->size[0];
  ret->size[0] = iv8[0];
  emxEnsureCapacity((emxArray__common *)ret, i4, (int32_T)sizeof(real_T),
                    (emlrtRTEInfo *)NULL);
  emlrtImportArrayR2011b(src, ret->data, 8, FALSE);
  emlrtDestroyArray(&src);
}

static const mxArray *message(const mxArray *b, emlrtMCInfo *location)
{
  const mxArray *pArray;
  const mxArray *m4;
  pArray = b;
  return emlrtCallMATLABR2012b(emlrtRootTLSGlobal, 1, &m4, 1, &pArray, "message",
    TRUE, location);
}

static const mxArray *slidefun(const mxArray *b, const mxArray *c, const mxArray
  *d, const mxArray *e, emlrtMCInfo *location)
{
  const mxArray *pArrays[4];
  const mxArray *m3;
  pArrays[0] = b;
  pArrays[1] = c;
  pArrays[2] = d;
  pArrays[3] = e;
  return emlrtCallMATLABR2012b(emlrtRootTLSGlobal, 1, &m3, 4, pArrays,
    "slidefun", TRUE, location);
}

void virtualBarsMEX(const emxArray_real_T *data, real_T inc, emxArray_real_T
                    *vBars)
{
  emxArray_real_T *u;
  emxArray_real_T *b_u;
  int32_T i;
  emxArray_real_T *hTemp;
  emxArray_real_T *h;
  emxArray_real_T *l;
  emxArray_real_T *r0;
  emxArray_real_T *b_data;
  emxArray_real_T *c_data;
  emxArray_real_T *d_data;
  emxArray_real_T *e_data;
  emxArray_real_T *f_data;
  real_T x;
  real_T y;
  int32_T loop_ub;
  int32_T i0;
  int32_T i1;
  real_T d0;
  int32_T i2;
  int32_T i3;
  int32_T l_idx_0;
  int32_T unnamed_idx_0;
  int32_T b_unnamed_idx_0;
  const mxArray *b_y;
  static const int32_T iv0[2] = { 1, 3 };

  const mxArray *m0;
  char_T cv0[3];
  static const char_T cv1[3] = { 'm', 'a', 'x' };

  const mxArray *c_y;
  const mxArray *d_y;
  real_T (*pData)[];
  const mxArray *e_y;
  static const int32_T iv1[2] = { 1, 7 };

  char_T cv2[7];
  static const char_T cv3[7] = { 'f', 'o', 'r', 'w', 'a', 'r', 'd' };

  const mxArray *f_y;
  static const int32_T iv2[2] = { 1, 3 };

  static const char_T cv4[3] = { 'm', 'i', 'n' };

  const mxArray *g_y;
  const mxArray *h_y;
  const mxArray *i_y;
  static const int32_T iv3[2] = { 1, 7 };

  const mxArray *j_y;
  static const int32_T iv4[2] = { 1, 61 };

  char_T cv5[61];
  static const char_T cv6[61] = { 'I', 'n', 'p', 'u', 't', ' ', 'n', 'e', 'e',
    'd', 's', ' ', 't', 'o', ' ', 'b', 'e', ' ', 'i', 'n', ' ', 't', 'h', 'e',
    ' ', 'f', 'o', 'r', 'm', 'a', 't', ' ', 'o', 'f', ' ', '\'', 'O', ' ', '|',
    ' ', 'C', '\'', ' ', 'o', 'r', ' ', '\'', 'O', ' ', '|', ' ', 'H', ' ', '|',
    ' ', 'L', ' ', '|', ' ', 'C', '\'' };

  emlrtHeapReferenceStackEnterFcnR2012b(emlrtRootTLSGlobal);
  emxInit_real_T(&u, 1, &emlrtRTEI, TRUE);
  emxInit_real_T(&b_u, 1, &emlrtRTEI, TRUE);

  /* VIRTUALBARSMEX Transforms a dataseries from a lower timeframe to higher (e.g. 1min data to 4min, 5min etc.) */
  /*    This function allows the transformation of lower period observations in time to higher.  In its */
  /*    current form we are only virtualizing Open and Close.  Further coding to handle High and Low should */
  /*    be considered. We are accepting either a dataset or a double array as input. */
  /*    Vars:   'data' is either a 2 or 4 double array which assumes standard column order of O|C or O | H | L | C */
  /*            'inc' is the increment to virtualize the data to.  We must already know the input increment */
  /*            e.g. 1 minute observations dataset to 4 minute virtualized would be inc = 4 */
  /*    The virtualization logic drops all partial bars from the end of the submitted dataset so that we are */
  /*    left with only full virtualized observations.  There may be need to have partial observations formed */
  /*    which we can revisit. */
  /*  */
  /*  Author: Mark Tompkins */
  /*  */
  /* % MEX code to be skipped */
  /*  Preallocate variables so we can MEX */
  /* % Check if we've been passed a dataset or a double array */
  if ((0 == data->size[0]) || (0 == data->size[1])) {
    i = 0;
  } else if (data->size[0] > data->size[1]) {
    i = data->size[0];
  } else {
    i = data->size[1];
  }

  /*  Check if we've been passed 2 or 4 columns. */
  /*  We can accept 2 columns and assume Open | Close */
  /*  or we can accept 4 columns and assume Open | High | Low | Close */
  emxInit_real_T(&hTemp, 1, &b_emlrtRTEI, TRUE);
  emxInit_real_T(&h, 1, &c_emlrtRTEI, TRUE);
  emxInit_real_T(&l, 1, &d_emlrtRTEI, TRUE);
  b_emxInit_real_T(&r0, 2, &emlrtRTEI, TRUE);
  emxInit_real_T(&b_data, 1, &emlrtRTEI, TRUE);
  emxInit_real_T(&c_data, 1, &emlrtRTEI, TRUE);
  emxInit_real_T(&d_data, 1, &emlrtRTEI, TRUE);
  emxInit_real_T(&e_data, 1, &emlrtRTEI, TRUE);
  emxInit_real_T(&f_data, 1, &emlrtRTEI, TRUE);
  if ((data->size[1] == 2) || (data->size[1] == 4)) {
    x = muDoubleScalarFloor((real_T)i / inc);
    y = x * inc;
    loop_ub = (int32_T)muDoubleScalarFloor((x * inc - inc) / inc);
    for (i0 = 0; i0 <= loop_ub; i0++) {
      i1 = data->size[0];
      d0 = 1.0 + inc * (real_T)i0;
      i2 = (int32_T)emlrtIntegerCheckFastR2012b(d0, &d_emlrtDCI,
        emlrtRootTLSGlobal);
      emlrtDynamicBoundsCheckFastR2012b(i2, 1, i1, &e_emlrtBCI,
        emlrtRootTLSGlobal);
    }

    /*  Open */
    d0 = muDoubleScalarFloor((real_T)i / inc) * inc;
    if ((inc == 0.0) || (((inc > 0.0) && (inc > d0)) || ((0.0 > inc) && (d0 >
           inc)))) {
      i0 = 1;
      i1 = 1;
      i2 = 0;
    } else {
      i0 = data->size[0];
      i1 = (int32_T)emlrtIntegerCheckFastR2012b(inc, &c_emlrtDCI,
        emlrtRootTLSGlobal);
      i0 = emlrtDynamicBoundsCheckFastR2012b(i1, 1, i0, &d_emlrtBCI,
        emlrtRootTLSGlobal);
      i1 = (int32_T)inc;
      i2 = data->size[0];
      i3 = (int32_T)emlrtIntegerCheckFastR2012b(d0, &c_emlrtDCI,
        emlrtRootTLSGlobal);
      i2 = emlrtDynamicBoundsCheckFastR2012b(i3, 1, i2, &d_emlrtBCI,
        emlrtRootTLSGlobal);
    }

    i3 = data->size[1];
    l_idx_0 = data->size[1];
    emlrtDynamicBoundsCheckFastR2012b(l_idx_0, 1, i3, &c_emlrtBCI,
      emlrtRootTLSGlobal);

    /*  Close */
    i3 = (int32_T)muDoubleScalarFloor((y - inc) / inc) + 1;
    l_idx_0 = div_s32_floor(i2 - i0, i1) + 1;
    emlrtDimSizeEqCheckFastR2012b(i3, l_idx_0, &d_emlrtECI, emlrtRootTLSGlobal);
    d0 = x * inc - inc;
    unnamed_idx_0 = (int32_T)muDoubleScalarFloor((x * inc - inc) / inc) + 1;
    i3 = b_data->size[0];
    b_data->size[0] = (int32_T)muDoubleScalarFloor(d0 / inc) + 1;
    emxEnsureCapacity((emxArray__common *)b_data, i3, (int32_T)sizeof(real_T),
                      &emlrtRTEI);
    loop_ub = (int32_T)muDoubleScalarFloor(d0 / inc);
    for (i3 = 0; i3 <= loop_ub; i3++) {
      b_data->data[i3] = data->data[(int32_T)(1.0 + inc * (real_T)i3) - 1];
    }

    i = data->size[1];
    b_unnamed_idx_0 = div_s32_floor(i2 - i0, i1) + 1;
    i3 = c_data->size[0];
    c_data->size[0] = div_s32_floor(i2 - i0, i1) + 1;
    emxEnsureCapacity((emxArray__common *)c_data, i3, (int32_T)sizeof(real_T),
                      &emlrtRTEI);
    loop_ub = div_s32_floor(i2 - i0, i1);
    for (i3 = 0; i3 <= loop_ub; i3++) {
      c_data->data[i3] = data->data[((i0 + i1 * i3) + data->size[0] * (i - 1)) -
        1];
    }

    i3 = vBars->size[0] * vBars->size[1];
    vBars->size[0] = unnamed_idx_0;
    vBars->size[1] = 2;
    emxEnsureCapacity((emxArray__common *)vBars, i3, (int32_T)sizeof(real_T),
                      &emlrtRTEI);
    i3 = 0;
    while (i3 <= 0) {
      for (i3 = 0; i3 < unnamed_idx_0; i3++) {
        vBars->data[i3] = b_data->data[i3];
      }

      i3 = 1;
    }

    i3 = 0;
    while (i3 <= 0) {
      for (i3 = 0; i3 < b_unnamed_idx_0; i3++) {
        vBars->data[i3 + vBars->size[0]] = c_data->data[i3];
      }

      i3 = 1;
    }

    if (data->size[1] == 4) {
      /*  Get the Highest high and Lowest low in the given increment */
      emlrtPushRtStackR2012b(&emlrtRSI, emlrtRootTLSGlobal);
      b_y = NULL;
      m0 = mxCreateCharArray(2, iv0);
      for (i = 0; i < 3; i++) {
        cv0[i] = cv1[i];
      }

      emlrtInitCharArrayR2013a(emlrtRootTLSGlobal, 3, m0, cv0);
      emlrtAssign(&b_y, m0);
      c_y = NULL;
      m0 = mxCreateDoubleScalar(inc);
      emlrtAssign(&c_y, m0);
      loop_ub = data->size[0];
      i3 = u->size[0];
      u->size[0] = loop_ub;
      emxEnsureCapacity((emxArray__common *)u, i3, (int32_T)sizeof(real_T),
                        &emlrtRTEI);
      for (i3 = 0; i3 < loop_ub; i3++) {
        u->data[i3] = data->data[i3 + data->size[0]];
      }

      d_y = NULL;
      m0 = mxCreateNumericArray(1, u->size, mxDOUBLE_CLASS, mxREAL);
      pData = (real_T (*)[])mxGetPr(m0);
      i3 = 0;
      for (i = 0; i < u->size[0]; i++) {
        (*pData)[i3] = u->data[i];
        i3++;
      }

      emlrtAssign(&d_y, m0);
      e_y = NULL;
      m0 = mxCreateCharArray(2, iv1);
      for (i = 0; i < 7; i++) {
        cv2[i] = cv3[i];
      }

      emlrtInitCharArrayR2013a(emlrtRootTLSGlobal, 7, m0, cv2);
      emlrtAssign(&e_y, m0);
      emlrt_marshallIn(slidefun(b_y, c_y, d_y, e_y, &emlrtMCI), "slidefun",
                       hTemp);
      emlrtPopRtStackR2012b(&emlrtRSI, emlrtRootTLSGlobal);

      /*  Moving window of Max's */
      if ((inc == 0.0) || (((inc > 0.0) && (1 > hTemp->size[0])) || ((0.0 > inc)
            && (hTemp->size[0] > 1)))) {
        i3 = 1;
        l_idx_0 = -1;
      } else {
        i3 = hTemp->size[0];
        emlrtDynamicBoundsCheckFastR2012b(1, 1, i3, &b_emlrtBCI,
          emlrtRootTLSGlobal);
        i3 = (int32_T)emlrtIntegerCheckFastR2012b(inc, &b_emlrtDCI,
          emlrtRootTLSGlobal);
        l_idx_0 = hTemp->size[0];
        i = hTemp->size[0];
        l_idx_0 = emlrtDynamicBoundsCheckFastR2012b(i, 1, l_idx_0, &b_emlrtBCI,
          emlrtRootTLSGlobal) - 1;
      }

      emlrtVectorVectorIndexCheckR2012b(hTemp->size[0], 1, 1, div_s32_floor
        (l_idx_0, i3) + 1, &c_emlrtECI, emlrtRootTLSGlobal);
      i = h->size[0];
      h->size[0] = div_s32_floor(l_idx_0, i3) + 1;
      emxEnsureCapacity((emxArray__common *)h, i, (int32_T)sizeof(real_T),
                        &emlrtRTEI);
      loop_ub = div_s32_floor(l_idx_0, i3);
      for (l_idx_0 = 0; l_idx_0 <= loop_ub; l_idx_0++) {
        h->data[l_idx_0] = hTemp->data[i3 * l_idx_0];
      }

      /*  High */
      /*  Trim any overhang */
      while (h->size[0] > (int32_T)muDoubleScalarFloor((y - inc) / inc) + 1) {
        emlrtPushRtStackR2012b(&c_emlrtRSI, emlrtRootTLSGlobal);
        i3 = h->size[0];
        eml_null_assignment(h, i3);
        emlrtPopRtStackR2012b(&c_emlrtRSI, emlrtRootTLSGlobal);
        emlrtBreakCheckFastR2012b(emlrtBreakCheckR2012bFlagVar,
          emlrtRootTLSGlobal);
      }

      emlrtPushRtStackR2012b(&b_emlrtRSI, emlrtRootTLSGlobal);
      f_y = NULL;
      m0 = mxCreateCharArray(2, iv2);
      for (i = 0; i < 3; i++) {
        cv0[i] = cv4[i];
      }

      emlrtInitCharArrayR2013a(emlrtRootTLSGlobal, 3, m0, cv0);
      emlrtAssign(&f_y, m0);
      g_y = NULL;
      m0 = mxCreateDoubleScalar(inc);
      emlrtAssign(&g_y, m0);
      loop_ub = data->size[0];
      i3 = b_u->size[0];
      b_u->size[0] = loop_ub;
      emxEnsureCapacity((emxArray__common *)b_u, i3, (int32_T)sizeof(real_T),
                        &emlrtRTEI);
      for (i3 = 0; i3 < loop_ub; i3++) {
        b_u->data[i3] = data->data[i3 + (data->size[0] << 1)];
      }

      h_y = NULL;
      m0 = mxCreateNumericArray(1, b_u->size, mxDOUBLE_CLASS, mxREAL);
      pData = (real_T (*)[])mxGetPr(m0);
      i3 = 0;
      for (i = 0; i < b_u->size[0]; i++) {
        (*pData)[i3] = b_u->data[i];
        i3++;
      }

      emlrtAssign(&h_y, m0);
      i_y = NULL;
      m0 = mxCreateCharArray(2, iv3);
      for (i = 0; i < 7; i++) {
        cv2[i] = cv3[i];
      }

      emlrtInitCharArrayR2013a(emlrtRootTLSGlobal, 7, m0, cv2);
      emlrtAssign(&i_y, m0);
      emlrt_marshallIn(slidefun(f_y, g_y, h_y, i_y, &b_emlrtMCI), "slidefun",
                       hTemp);
      emlrtPopRtStackR2012b(&b_emlrtRSI, emlrtRootTLSGlobal);

      /*  Moving window of Min's */
      if ((inc == 0.0) || (((inc > 0.0) && (1 > hTemp->size[0])) || ((0.0 > inc)
            && (hTemp->size[0] > 1)))) {
        i3 = 1;
        l_idx_0 = -1;
      } else {
        i3 = hTemp->size[0];
        emlrtDynamicBoundsCheckFastR2012b(1, 1, i3, &emlrtBCI,
          emlrtRootTLSGlobal);
        i3 = (int32_T)emlrtIntegerCheckFastR2012b(inc, &emlrtDCI,
          emlrtRootTLSGlobal);
        l_idx_0 = hTemp->size[0];
        i = hTemp->size[0];
        l_idx_0 = emlrtDynamicBoundsCheckFastR2012b(i, 1, l_idx_0, &emlrtBCI,
          emlrtRootTLSGlobal) - 1;
      }

      emlrtVectorVectorIndexCheckR2012b(hTemp->size[0], 1, 1, div_s32_floor
        (l_idx_0, i3) + 1, &b_emlrtECI, emlrtRootTLSGlobal);
      i = l->size[0];
      l->size[0] = div_s32_floor(l_idx_0, i3) + 1;
      emxEnsureCapacity((emxArray__common *)l, i, (int32_T)sizeof(real_T),
                        &emlrtRTEI);
      loop_ub = div_s32_floor(l_idx_0, i3);
      for (l_idx_0 = 0; l_idx_0 <= loop_ub; l_idx_0++) {
        l->data[l_idx_0] = hTemp->data[i3 * l_idx_0];
      }

      /*  Low */
      /*  Trim any overhang */
      while (l->size[0] > (int32_T)muDoubleScalarFloor((y - inc) / inc) + 1) {
        emlrtPushRtStackR2012b(&d_emlrtRSI, emlrtRootTLSGlobal);
        i3 = l->size[0];
        eml_null_assignment(l, i3);
        emlrtPopRtStackR2012b(&d_emlrtRSI, emlrtRootTLSGlobal);
        emlrtBreakCheckFastR2012b(emlrtBreakCheckR2012bFlagVar,
          emlrtRootTLSGlobal);
      }

      d0 = y - inc;
      unnamed_idx_0 = (int32_T)muDoubleScalarFloor((y - inc) / inc) + 1;
      i3 = d_data->size[0];
      d_data->size[0] = (int32_T)muDoubleScalarFloor(d0 / inc) + 1;
      emxEnsureCapacity((emxArray__common *)d_data, i3, (int32_T)sizeof(real_T),
                        &emlrtRTEI);
      loop_ub = (int32_T)muDoubleScalarFloor(d0 / inc);
      for (i3 = 0; i3 <= loop_ub; i3++) {
        d_data->data[i3] = data->data[(int32_T)(1.0 + inc * (real_T)i3) - 1];
      }

      i3 = r0->size[0] * r0->size[1];
      r0->size[0] = unnamed_idx_0;
      r0->size[1] = 1;
      emxEnsureCapacity((emxArray__common *)r0, i3, (int32_T)sizeof(real_T),
                        &emlrtRTEI);
      i3 = 0;
      while (i3 <= 0) {
        for (i3 = 0; i3 < unnamed_idx_0; i3++) {
          r0->data[i3] = d_data->data[i3];
        }

        i3 = 1;
      }

      i = h->size[0];
      i3 = r0->size[0];
      emlrtDimSizeEqCheckFastR2012b(i3, i, &emlrtECI, emlrtRootTLSGlobal);
      l_idx_0 = l->size[0];
      i3 = r0->size[0];
      emlrtDimSizeEqCheckFastR2012b(i3, l_idx_0, &emlrtECI, emlrtRootTLSGlobal);
      i3 = r0->size[0];
      l_idx_0 = div_s32_floor(i2 - i0, i1) + 1;
      emlrtDimSizeEqCheckFastR2012b(i3, l_idx_0, &emlrtECI, emlrtRootTLSGlobal);
      d0 = x * inc - inc;
      unnamed_idx_0 = (int32_T)muDoubleScalarFloor((x * inc - inc) / inc) + 1;
      i3 = e_data->size[0];
      e_data->size[0] = (int32_T)muDoubleScalarFloor(d0 / inc) + 1;
      emxEnsureCapacity((emxArray__common *)e_data, i3, (int32_T)sizeof(real_T),
                        &emlrtRTEI);
      loop_ub = (int32_T)muDoubleScalarFloor(d0 / inc);
      for (i3 = 0; i3 <= loop_ub; i3++) {
        e_data->data[i3] = data->data[(int32_T)(1.0 + inc * (real_T)i3) - 1];
      }

      i = h->size[0];
      l_idx_0 = l->size[0];
      b_unnamed_idx_0 = div_s32_floor(i2 - i0, i1) + 1;
      i3 = f_data->size[0];
      f_data->size[0] = div_s32_floor(i2 - i0, i1) + 1;
      emxEnsureCapacity((emxArray__common *)f_data, i3, (int32_T)sizeof(real_T),
                        &emlrtRTEI);
      loop_ub = div_s32_floor(i2 - i0, i1);
      for (i2 = 0; i2 <= loop_ub; i2++) {
        f_data->data[i2] = data->data[((i0 + i1 * i2) + data->size[0] * 3) - 1];
      }

      i0 = vBars->size[0] * vBars->size[1];
      vBars->size[0] = unnamed_idx_0;
      vBars->size[1] = 4;
      emxEnsureCapacity((emxArray__common *)vBars, i0, (int32_T)sizeof(real_T),
                        &emlrtRTEI);
      i0 = 0;
      while (i0 <= 0) {
        for (i0 = 0; i0 < unnamed_idx_0; i0++) {
          vBars->data[i0] = e_data->data[i0];
        }

        i0 = 1;
      }

      i0 = 0;
      while (i0 <= 0) {
        for (i0 = 0; i0 < i; i0++) {
          vBars->data[i0 + vBars->size[0]] = h->data[i0];
        }

        i0 = 1;
      }

      i0 = 0;
      while (i0 <= 0) {
        for (i0 = 0; i0 < l_idx_0; i0++) {
          vBars->data[i0 + vBars->size[0] * 2] = l->data[i0];
        }

        i0 = 1;
      }

      i0 = 0;
      while (i0 <= 0) {
        for (i0 = 0; i0 < b_unnamed_idx_0; i0++) {
          vBars->data[i0 + vBars->size[0] * 3] = f_data->data[i0];
        }

        i0 = 1;
      }
    }

    /* if */
  } else {
    /*  Seems like we might have non-standard input.  Throw an error */
    emlrtPushRtStackR2012b(&e_emlrtRSI, emlrtRootTLSGlobal);
    emlrtPushRtStackR2012b(&l_emlrtRSI, emlrtRootTLSGlobal);
    j_y = NULL;
    m0 = mxCreateCharArray(2, iv4);
    for (i = 0; i < 61; i++) {
      cv5[i] = cv6[i];
    }

    emlrtInitCharArrayR2013a(emlrtRootTLSGlobal, 61, m0, cv5);
    emlrtAssign(&j_y, m0);
    error(j_y, &i_emlrtMCI);
    emlrtPopRtStackR2012b(&l_emlrtRSI, emlrtRootTLSGlobal);
    emlrtPopRtStackR2012b(&e_emlrtRSI, emlrtRootTLSGlobal);
  }

  emxFree_real_T(&f_data);
  emxFree_real_T(&e_data);
  emxFree_real_T(&d_data);
  emxFree_real_T(&c_data);
  emxFree_real_T(&b_data);
  emxFree_real_T(&r0);
  emxFree_real_T(&l);
  emxFree_real_T(&h);
  emxFree_real_T(&hTemp);

  /* if */
  /* if */
  emxFree_real_T(&b_u);
  emxFree_real_T(&u);
  emlrtHeapReferenceStackLeaveFcnR2012b(emlrtRootTLSGlobal);
}

/* End of code generation (virtualBarsMEX.c) */
