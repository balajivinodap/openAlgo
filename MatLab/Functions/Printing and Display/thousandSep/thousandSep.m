function out = thousandSep(in)
%THOUSANDSEP adds thousands Separators to a 1x1 array.
% Example:
% thousandSep(1234567)
%
% Author: Mark Tompkins
%
import java.text.*
v = DecimalFormat;
out = char(v.format(in));
