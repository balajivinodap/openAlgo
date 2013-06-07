function state = wprSTA(price,N,thresh)
%WPRSTA returns a logical STATE for from the 'willpctr.m' function by The MathWorks, Inc.
% WPRSTA returns a logical STATE for from the 'willpctr.m' function by The MathWorks, Inc.
% which is a value that is above/below an upper/lower threshold intended to locate
% overbought and oversold conditions.
% N serves as an optional lookback period (default 14 observations)
%   NOTES: It is important to consider that an RSI STATE really has 3 states.
%            1      Above upper threshold is overbought
%           -1      Below lower threshold is oversold
%            0      There is also a neutral region between thresholds and 50%
% 
%           WPR is normally used with stocks where there is a natural upward price bias
%
% Author:           Mark Tompkins
% Revision:			4906.29345
% All rights reserved.

%% MEX code to be skipped
coder.extrinsic('OHLCSplitter','willpctr')

% WPR works with negative values in a range from 0 to -100;
if numel(thresh) == 1 % scalar value
    thresh = [(100-abs(thresh))*-1, abs(thresh)*-1];
else
    thresh = abs(thresh)*-1;
end;
if thresh(1) < thresh(2)
    thresh= thresh(2:-1:1);
end;

if thresh(1) > 0, thresh(1) = thresh(1) * -1; end;
if thresh(2) > 0, thresh(2) = thresh(2) * -1; end;

% Preallocate so we can MEX
rows = size(price,1);
fClose = zeros(rows,1);                                     %#ok<NASGU>
fHigh = zeros(rows,1);                                      %#ok<NASGU>
fLow = zeros(rows,1);                                       %#ok<NASGU>
state = zeros(rows,1);
w = zeros(rows,1);                                          %#ok<NASGU>

if size(price,2) == 4
    [~, fHigh, fLow, fClose] = OHLCSplitter(price);
else
    error('wprMETS:InputArg',...
    	'We need as input O | H | L | C.');
end; %if

%% williams %r
w = willpctr([fHigh fLow fClose],N);

%% generate signal
% Crossing the upper threshold (overbought)
indx = w > thresh(1);
state(indx) = -1;

indx = w < thresh(2);
state(indx) = 1;

