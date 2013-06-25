function [sta,lBand,mAvg,uBand] = bollBandSTA(price,period,maType,devUp,devDwn)
%BOLLBANDSTA returns a logical STATE for from 'bollBand.m'
% BOLLBANDSTA returns a logical STATE for from 'bollBand.m'
% which is a value that is above/below an upper/lower band intended to measure the
%   "highness" or "lowness" of the price relative to previous trades.
%
%   [sta,lBand,mAvg,uBand] = bollBandSTA(price,period,maType,devUp,devDwn)
%
%   Available average types are:
%       -5  Triangle (Double smoothed similar to Hull)
%       -4  Trimmed  (10%)
%       -3  Harmonic
%       -2  Geometric
%       -1	Exponential
%        0  Simple
%       >0  Weighted e.g. 0.5 Square root weighted, 1 = linear, 2 = square weighted
%
%   INPUTS:     period    	Lookback period (default 20)
%               maType
%               devUp       Number of upward standard deviations (default 2)
%               devDwn      Number of downward standard deviations (default -2)
%
%	OUTPUTS		sta         1      Above upper Bollinger Band
%                          -1      Below lower Bollinger Band
%                           0      Between or equal to the pair of bands
%
%               lBand		Lower Bollinger band    (MA - Kstd)
%				mBand		Midline average         (MA)
%               uBand       Upper Bollinger band    (MA + Kstd)

%% MEX code to be skipped
coder.extrinsic('bollBand_mex');

%% Preallocation
rows = size(price,1);
lBand = nan(rows,1);                    %#ok<NASGU>
mAvg = nan(rows,1);                     %#ok<NASGU>
uBand = nan(rows,1);                    %#ok<NASGU>
sta = zeros(rows,1);

%% Bollinger Band
[lBand, mAvg, uBand] = bollBand_mex(price, period, maType, devUp, devDwn);

%% Generate STATE

% Crossing the lower threshold (oversold)
sta(price < lBand) = -1;
sta(price > uBand) = 1;