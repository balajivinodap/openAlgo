function out = thousandSep(in)
%THOUSANDSEP adds thousands Separators to a 1x1 array.
% Example:
% thousandSep(1234567)
%
% Author:           Mark Tompkins
% Revision:			4902.23760

import java.text.*
v = DecimalFormat;
out = char(v.format(in));
