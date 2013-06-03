function out = thousandSepCash(in)
%THOUSANDSEPCASH adds thousands Separators to a 1x1 currency array.
% Example:
% thousandSepCash(1234567)
%
% Author:           Mark Tompkins
% Revision:			4902.23769

import java.text.*
v = DecimalFormat('$###,##0.00');
out = char(v.format(in));