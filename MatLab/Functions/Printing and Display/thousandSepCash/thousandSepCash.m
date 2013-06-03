function out = thousandSepCash(in)
%THOUSANDSEPCASH adds thousands Separators to a 1x1 currency array.
% Example:
% thousandSepCash(1234567)
%
% Author: Mark Tompkins
%

import java.text.*
v = DecimalFormat('$###,##0.00');
out = char(v.format(in));