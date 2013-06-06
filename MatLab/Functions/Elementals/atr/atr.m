function atr = atr(price,M)
%ATR Average True Range
%
%	The average true range attempts to modify price data in such a way that it 'corrects'
%	various gaps that may otherwise be evident.  This had a significantly higher degree
%	of relevance when markets were not electronic and had many hours between sessions.
%	The primary modifier of the data is smoothed by an exponential average of elemental
%	bar components High, Low, and Close.
%   
%   atr = ATR(PRICE,M) 		returns the average true range using a a specified
%					 		lookback of 'M' observations when M is an integer > 0
%
% Author:           Mark Tompkins
% Revision:			4903.17425
% All rights reserved.

%% MEX code to be skipped
coder.extrinsic('OHLCSplitter', 'movAvg_mex');

% Parse data
if size(price,2) < 4
    error('ATR:priceDimensions','We need O | H | L | C as an input.  Aborting.');
end; %if

if M < 1
    error('ATR:observations','The lookback must be such that M >= 1.  Aborting.');
end; %if

rows = size(price,1);

%% Preallocation
h_m_l = zeros(rows,1);
h_m_c = zeros(rows,1);
l_m_c = zeros(rows,1);
fHigh = zeros(rows,1);
fLow = zeros(rows,1);
fClose = zeros(rows,1);
tr = zeros(rows,1);

[~,fHigh,fLow,fClose] = OHLCSplitter(price);

%% True range
h_m_l = fHigh-fLow;                                 % high - low
h_m_c = [0;abs(fHigh(2:end)-fClose(1:end-1))];      % abs(high - close)
l_m_c = [0;abs(fLow(2:end)-fClose(1:end-1))];       % abs(low - close)
tr = max([h_m_l,h_m_c,l_m_c],[],2);                 % true range
        
%% Average true range
atr = movAvg_mex(tr,M,M,-1);                        % '-1' calls to exponential calculation
        
end




