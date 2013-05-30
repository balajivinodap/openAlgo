/*
 * virtualBarsMEX_api.c
 *
 * Code generation for function 'virtualBarsMEX_api'
 *
 * C source code generated on: Thu May 30 16:52:21 2013
 *
 */

/* Include files */
#include "rt_nonfinite.h"
#include "virtualBarsMEX.h"
#include "virtualBarsMEX_api.h"
#include "virtualBarsMEX_emxutil.h"

/* Variable Definitions */
static emlrtRTEInfo f_emlrtRTEI = { 1, 1, "virtualBarsMEX_api", "" };

/* Function Declarations */
static void c_emlrt_marshallIn(const mxArray *data, const char_T *identifier,
  emxArray_real_T *y);
static void d_emlrt_marshallIn(const mxArray *u, const emlrtMsgIdentifier
  *parentId, emxArray_real_T *y);
static real_T e_emlrt_marshallIn(const mxArray *inc, const char_T *identifier);
static const mxArray *emlrt_marshallOut(emxArray_real_T *u);
static real_T f_emlrt_marshallIn(const mxArray *u, const emlrtMsgIdentifier
  *parentId);
static void h_emlrt_marshallIn(const mxArray *src, const emlrtMsgIdentifier
  *msgId, emxArray_real_T *ret);
static real_T i_emlrt_marshallIn(const mxArray *src, const emlrtMsgIdentifier
  *msgId);
static void info_helper(ResolvedFunctionInfo info[24]);

/* Function Definitions */
static void c_emlrt_marshallIn(const mxArray *data, const char_T *identifier,
  emxArray_real_T *y)
{
  emlrtMsgIdentifier thisId;
  thisId.fIdentifier = identifier;
  thisId.fParent = NULL;
  d_emlrt_marshallIn(emlrtAlias(data), &thisId, y);
  emlrtDestroyArray(&data);
}

static void d_emlrt_marshallIn(const mxArray *u, const emlrtMsgIdentifier
  *parentId, emxArray_real_T *y)
{
  h_emlrt_marshallIn(emlrtAlias(u), parentId, y);
  emlrtDestroyArray(&u);
}

static real_T e_emlrt_marshallIn(const mxArray *inc, const char_T *identifier)
{
  real_T y;
  emlrtMsgIdentifier thisId;
  thisId.fIdentifier = identifier;
  thisId.fParent = NULL;
  y = f_emlrt_marshallIn(emlrtAlias(inc), &thisId);
  emlrtDestroyArray(&inc);
  return y;
}

static const mxArray *emlrt_marshallOut(emxArray_real_T *u)
{
  const mxArray *y;
  static const int32_T iv6[2] = { 0, 0 };

  const mxArray *m2;
  y = NULL;
  m2 = mxCreateNumericArray(2, (int32_T *)&iv6, mxDOUBLE_CLASS, mxREAL);
  mxSetData((mxArray *)m2, (void *)u->data);
  mxSetDimensions((mxArray *)m2, u->size, 2);
  emlrtAssign(&y, m2);
  return y;
}

static real_T f_emlrt_marshallIn(const mxArray *u, const emlrtMsgIdentifier
  *parentId)
{
  real_T y;
  y = i_emlrt_marshallIn(emlrtAlias(u), parentId);
  emlrtDestroyArray(&u);
  return y;
}

static void h_emlrt_marshallIn(const mxArray *src, const emlrtMsgIdentifier
  *msgId, emxArray_real_T *ret)
{
  int32_T iv9[2];
  boolean_T bv1[2];
  int32_T i;
  int32_T iv10[2];
  for (i = 0; i < 2; i++) {
    iv9[i] = -1;
    bv1[i] = TRUE;
  }

  emlrtCheckVsBuiltInR2012b(emlrtRootTLSGlobal, msgId, src, "double", FALSE, 2U,
    iv9, bv1, iv10);
  ret->size[0] = iv10[0];
  ret->size[1] = iv10[1];
  ret->allocatedSize = ret->size[0] * ret->size[1];
  ret->data = (real_T *)mxGetData(src);
  ret->canFreeData = FALSE;
  emlrtDestroyArray(&src);
}

static real_T i_emlrt_marshallIn(const mxArray *src, const emlrtMsgIdentifier
  *msgId)
{
  real_T ret;
  emlrtCheckBuiltInR2012b(emlrtRootTLSGlobal, msgId, src, "double", FALSE, 0U, 0);
  ret = *(real_T *)mxGetData(src);
  emlrtDestroyArray(&src);
  return ret;
}

static void info_helper(ResolvedFunctionInfo info[24])
{
  info[0].context =
    "[E]G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m";
  info[0].name = "length";
  info[0].dominantType = "double";
  info[0].resolved = "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/elmat/length.m";
  info[0].fileTimeLo = 1303167806U;
  info[0].fileTimeHi = 0U;
  info[0].mFileTimeLo = 0U;
  info[0].mFileTimeHi = 0U;
  info[1].context =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/elmat/length.m!intlength";
  info[1].name = "eml_index_class";
  info[1].dominantType = "";
  info[1].resolved =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_index_class.m";
  info[1].fileTimeLo = 1323192178U;
  info[1].fileTimeHi = 0U;
  info[1].mFileTimeLo = 0U;
  info[1].mFileTimeHi = 0U;
  info[2].context =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/elmat/length.m!intlength";
  info[2].name = "coder.internal.indexIntRelop";
  info[2].dominantType = "";
  info[2].resolved =
    "[IXE]$matlabroot$/toolbox/shared/coder/coder/+coder/+internal/indexIntRelop.m";
  info[2].fileTimeLo = 1326749922U;
  info[2].fileTimeHi = 0U;
  info[2].mFileTimeLo = 0U;
  info[2].mFileTimeHi = 0U;
  info[3].context =
    "[IXE]$matlabroot$/toolbox/shared/coder/coder/+coder/+internal/indexIntRelop.m!float_class_contains_indexIntClass";
  info[3].name = "eml_float_model";
  info[3].dominantType = "char";
  info[3].resolved =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_float_model.m";
  info[3].fileTimeLo = 1326749596U;
  info[3].fileTimeHi = 0U;
  info[3].mFileTimeLo = 0U;
  info[3].mFileTimeHi = 0U;
  info[4].context =
    "[IXE]$matlabroot$/toolbox/shared/coder/coder/+coder/+internal/indexIntRelop.m!is_signed_indexIntClass";
  info[4].name = "intmin";
  info[4].dominantType = "char";
  info[4].resolved = "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/elmat/intmin.m";
  info[4].fileTimeLo = 1311276918U;
  info[4].fileTimeHi = 0U;
  info[4].mFileTimeLo = 0U;
  info[4].mFileTimeHi = 0U;
  info[5].context =
    "[E]G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m";
  info[5].name = "mrdivide";
  info[5].dominantType = "double";
  info[5].resolved = "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/ops/mrdivide.p";
  info[5].fileTimeLo = 1357973148U;
  info[5].fileTimeHi = 0U;
  info[5].mFileTimeLo = 1319751566U;
  info[5].mFileTimeHi = 0U;
  info[6].context = "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/ops/mrdivide.p";
  info[6].name = "rdivide";
  info[6].dominantType = "double";
  info[6].resolved = "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/ops/rdivide.m";
  info[6].fileTimeLo = 1346531988U;
  info[6].fileTimeHi = 0U;
  info[6].mFileTimeLo = 0U;
  info[6].mFileTimeHi = 0U;
  info[7].context = "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/ops/rdivide.m";
  info[7].name = "eml_scalexp_compatible";
  info[7].dominantType = "double";
  info[7].resolved =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_scalexp_compatible.m";
  info[7].fileTimeLo = 1286840396U;
  info[7].fileTimeHi = 0U;
  info[7].mFileTimeLo = 0U;
  info[7].mFileTimeHi = 0U;
  info[8].context = "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/ops/rdivide.m";
  info[8].name = "eml_div";
  info[8].dominantType = "double";
  info[8].resolved = "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_div.m";
  info[8].fileTimeLo = 1313369410U;
  info[8].fileTimeHi = 0U;
  info[8].mFileTimeLo = 0U;
  info[8].mFileTimeHi = 0U;
  info[9].context =
    "[E]G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m";
  info[9].name = "floor";
  info[9].dominantType = "double";
  info[9].resolved = "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/elfun/floor.m";
  info[9].fileTimeLo = 1343851980U;
  info[9].fileTimeHi = 0U;
  info[9].mFileTimeLo = 0U;
  info[9].mFileTimeHi = 0U;
  info[10].context = "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/elfun/floor.m";
  info[10].name = "eml_scalar_floor";
  info[10].dominantType = "double";
  info[10].resolved =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/elfun/eml_scalar_floor.m";
  info[10].fileTimeLo = 1286840326U;
  info[10].fileTimeHi = 0U;
  info[10].mFileTimeLo = 0U;
  info[10].mFileTimeHi = 0U;
  info[11].context =
    "[E]G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m";
  info[11].name = "mtimes";
  info[11].dominantType = "double";
  info[11].resolved = "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/ops/mtimes.m";
  info[11].fileTimeLo = 1289541292U;
  info[11].fileTimeHi = 0U;
  info[11].mFileTimeLo = 0U;
  info[11].mFileTimeHi = 0U;
  info[12].context =
    "[E]G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m";
  info[12].name = "eml_null_assignment";
  info[12].dominantType = "";
  info[12].resolved =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_null_assignment.m";
  info[12].fileTimeLo = 1323192180U;
  info[12].fileTimeHi = 0U;
  info[12].mFileTimeLo = 0U;
  info[12].mFileTimeHi = 0U;
  info[13].context =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_null_assignment.m!is_valid_idx";
  info[13].name = "eml_index_class";
  info[13].dominantType = "";
  info[13].resolved =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_index_class.m";
  info[13].fileTimeLo = 1323192178U;
  info[13].fileTimeHi = 0U;
  info[13].mFileTimeLo = 0U;
  info[13].mFileTimeHi = 0U;
  info[14].context =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_null_assignment.m!is_valid_idx";
  info[14].name = "coder.internal.indexIntRelop";
  info[14].dominantType = "";
  info[14].resolved =
    "[IXE]$matlabroot$/toolbox/shared/coder/coder/+coder/+internal/indexIntRelop.m";
  info[14].fileTimeLo = 1326749922U;
  info[14].fileTimeHi = 0U;
  info[14].mFileTimeLo = 0U;
  info[14].mFileTimeHi = 0U;
  info[15].context =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_null_assignment.m!is_valid_idx";
  info[15].name = "floor";
  info[15].dominantType = "double";
  info[15].resolved = "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/elfun/floor.m";
  info[15].fileTimeLo = 1343851980U;
  info[15].fileTimeHi = 0U;
  info[15].mFileTimeLo = 0U;
  info[15].mFileTimeHi = 0U;
  info[16].context =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_null_assignment.m!onearg_null_assignment";
  info[16].name = "eml_index_class";
  info[16].dominantType = "";
  info[16].resolved =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_index_class.m";
  info[16].fileTimeLo = 1323192178U;
  info[16].fileTimeHi = 0U;
  info[16].mFileTimeLo = 0U;
  info[16].mFileTimeHi = 0U;
  info[17].context =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_null_assignment.m!onearg_null_assignment";
  info[17].name = "eml_index_minus";
  info[17].dominantType = "double";
  info[17].resolved =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_index_minus.m";
  info[17].fileTimeLo = 1286840378U;
  info[17].fileTimeHi = 0U;
  info[17].mFileTimeLo = 0U;
  info[17].mFileTimeHi = 0U;
  info[18].context =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_index_minus.m";
  info[18].name = "eml_index_class";
  info[18].dominantType = "";
  info[18].resolved =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_index_class.m";
  info[18].fileTimeLo = 1323192178U;
  info[18].fileTimeHi = 0U;
  info[18].mFileTimeLo = 0U;
  info[18].mFileTimeHi = 0U;
  info[19].context =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_null_assignment.m!onearg_null_assignment";
  info[19].name = "eml_int_forloop_overflow_check";
  info[19].dominantType = "";
  info[19].resolved =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_int_forloop_overflow_check.m";
  info[19].fileTimeLo = 1346531940U;
  info[19].fileTimeHi = 0U;
  info[19].mFileTimeLo = 0U;
  info[19].mFileTimeHi = 0U;
  info[20].context =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_int_forloop_overflow_check.m!eml_int_forloop_overflow_check_helper";
  info[20].name = "intmax";
  info[20].dominantType = "char";
  info[20].resolved = "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/elmat/intmax.m";
  info[20].fileTimeLo = 1311276916U;
  info[20].fileTimeHi = 0U;
  info[20].mFileTimeLo = 0U;
  info[20].mFileTimeHi = 0U;
  info[21].context =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_null_assignment.m!onearg_null_assignment";
  info[21].name = "eml_index_plus";
  info[21].dominantType = "double";
  info[21].resolved =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_index_plus.m";
  info[21].fileTimeLo = 1286840378U;
  info[21].fileTimeHi = 0U;
  info[21].mFileTimeLo = 0U;
  info[21].mFileTimeHi = 0U;
  info[22].context =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_index_plus.m";
  info[22].name = "eml_index_class";
  info[22].dominantType = "";
  info[22].resolved =
    "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/eml/eml_index_class.m";
  info[22].fileTimeLo = 1323192178U;
  info[22].fileTimeHi = 0U;
  info[22].mFileTimeLo = 0U;
  info[22].mFileTimeHi = 0U;
  info[23].context =
    "[E]G:/openAlgo/MatLab/MEX/M/virtualBarsMEX/virtualBarsMEX.m";
  info[23].name = "error";
  info[23].dominantType = "char";
  info[23].resolved = "[ILXE]$matlabroot$/toolbox/eml/lib/matlab/lang/error.m";
  info[23].fileTimeLo = 1319751566U;
  info[23].fileTimeHi = 0U;
  info[23].mFileTimeLo = 0U;
  info[23].mFileTimeHi = 0U;
}

const mxArray *emlrtMexFcnResolvedFunctionsInfo(void)
{
  const mxArray *nameCaptureInfo;
  ResolvedFunctionInfo info[24];
  ResolvedFunctionInfo u[24];
  int32_T i;
  const mxArray *y;
  int32_T iv5[1];
  ResolvedFunctionInfo *r1;
  const char * b_u;
  const mxArray *b_y;
  const mxArray *m1;
  const mxArray *c_y;
  const mxArray *d_y;
  const mxArray *e_y;
  uint32_T c_u;
  const mxArray *f_y;
  const mxArray *g_y;
  const mxArray *h_y;
  const mxArray *i_y;
  nameCaptureInfo = NULL;
  info_helper(info);
  for (i = 0; i < 24; i++) {
    u[i] = info[i];
  }

  y = NULL;
  iv5[0] = 24;
  emlrtAssign(&y, mxCreateStructArray(1, iv5, 0, NULL));
  for (i = 0; i < 24; i++) {
    r1 = &u[i];
    b_u = r1->context;
    b_y = NULL;
    m1 = mxCreateString(b_u);
    emlrtAssign(&b_y, m1);
    emlrtAddField(y, b_y, "context", i);
    b_u = r1->name;
    c_y = NULL;
    m1 = mxCreateString(b_u);
    emlrtAssign(&c_y, m1);
    emlrtAddField(y, c_y, "name", i);
    b_u = r1->dominantType;
    d_y = NULL;
    m1 = mxCreateString(b_u);
    emlrtAssign(&d_y, m1);
    emlrtAddField(y, d_y, "dominantType", i);
    b_u = r1->resolved;
    e_y = NULL;
    m1 = mxCreateString(b_u);
    emlrtAssign(&e_y, m1);
    emlrtAddField(y, e_y, "resolved", i);
    c_u = r1->fileTimeLo;
    f_y = NULL;
    m1 = mxCreateNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
    *(uint32_T *)mxGetData(m1) = c_u;
    emlrtAssign(&f_y, m1);
    emlrtAddField(y, f_y, "fileTimeLo", i);
    c_u = r1->fileTimeHi;
    g_y = NULL;
    m1 = mxCreateNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
    *(uint32_T *)mxGetData(m1) = c_u;
    emlrtAssign(&g_y, m1);
    emlrtAddField(y, g_y, "fileTimeHi", i);
    c_u = r1->mFileTimeLo;
    h_y = NULL;
    m1 = mxCreateNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
    *(uint32_T *)mxGetData(m1) = c_u;
    emlrtAssign(&h_y, m1);
    emlrtAddField(y, h_y, "mFileTimeLo", i);
    c_u = r1->mFileTimeHi;
    i_y = NULL;
    m1 = mxCreateNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
    *(uint32_T *)mxGetData(m1) = c_u;
    emlrtAssign(&i_y, m1);
    emlrtAddField(y, i_y, "mFileTimeHi", i);
  }

  emlrtAssign(&nameCaptureInfo, y);
  emlrtNameCapturePostProcessR2012a(emlrtAlias(nameCaptureInfo));
  return nameCaptureInfo;
}

void virtualBarsMEX_api(const mxArray * const prhs[2], const mxArray *plhs[1])
{
  emxArray_real_T *data;
  emxArray_real_T *vBars;
  real_T inc;
  emlrtHeapReferenceStackEnterFcnR2012b(emlrtRootTLSGlobal);
  b_emxInit_real_T(&data, 2, &f_emlrtRTEI, TRUE);
  b_emxInit_real_T(&vBars, 2, &f_emlrtRTEI, TRUE);

  /* Marshall function inputs */
  c_emlrt_marshallIn(emlrtAlias(prhs[0]), "data", data);
  inc = e_emlrt_marshallIn(emlrtAliasP(prhs[1]), "inc");

  /* Invoke the target function */
  virtualBarsMEX(data, inc, vBars);

  /* Marshall function outputs */
  plhs[0] = emlrt_marshallOut(vBars);
  vBars->canFreeData = FALSE;
  emxFree_real_T(&vBars);
  data->canFreeData = FALSE;
  emxFree_real_T(&data);
  emlrtHeapReferenceStackLeaveFcnR2012b(emlrtRootTLSGlobal);
}

/* End of code generation (virtualBarsMEX_api.c) */
