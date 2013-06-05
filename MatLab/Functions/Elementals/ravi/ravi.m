function [ ind ] = ravi(price,lead,lag,D,M)
%RAVI A measurement which attempts to detect a change between trending and ranging states
%
%	From the work of Tushar S. Chande, PhD. and cited in his book "Beyond Technical Analysis"
%	the Ravi calculation is a measurement of current market "trendiness".  A value greater than
%	3% is claimed to be a measurement of when the market is in a trending phase.  Values less
%	than 3% are a ranging phase.
%
%	PRICE:	We call ATR to normalize price data in the ravi function so we need O | H | L | C as price input.
%	LEAD:	Observation period for the fast period harmonic mean used to calculate Ravi measurement
%	LAG:	Observation period for the slow period harmonic mean used to calculate Ravi measurement
%	D:		Detrender option:
%				0	-	Ravi (default)
%				1	-	ATR 
%	M:		Mean ravi shift used to calibrate the returned vector.
%				e.g.	M = 10 	the mean of the RAVI vector is set to 10%
%	hSub:	String used to manipulate the graphing of the Ravi vector
%
%	RAVI(PRICE)				Returns a graph of the 'ravi.m' function with default values.
%	RAVI(PRICE,...)			Returns a graph of the 'ravi.m' function with declared values.
%
%   ind = RAVI(PRICE) 		returns a RAVI vector with default values:
%								Lead	5
%								Lag		65
%								D		0
%								M		20				
%
% Author:           Mark Tompkins
% Revision:			4903.16719
% All rights reserved.

if size(price,2) < 4
    error('RAVI:tooFewInputs', ...
            'We call ATR to normalize price data therefore we need O | H | L | C as price input. Exiting.');
end; %if

[~,~,~,fClose] = OHLCSplitter(price);

if ~exist('lead','var'), lead = 5; end; % Default lead value
if ~exist('lag','var'), lag = 65; end; % Default lag value
if ~exist('D','var'), D = 0; end; % Default divisor selection
if ~exist('M','var'), M = 20; end; % Percentage to shift the mean ravi to.

if M < 1
    error('RAVI:inputArgs','Lookback M must be a postive integer. Aborting');
end; %if

if nargin == 2
    warning('RAVI:numInputs',...
        'Defaulting the Lag lookback period to 65.  Use two inputs to avoid this message.');
end; % Present a warning about the number of inputs

raviF = slidefun('harmmean',lead,fClose,'backward');
raviS = slidefun('harmmean',lag,fClose,'backward');

% Determine divisor for measuring the rate of change
if D == 0
    ind = (abs(raviF-raviS)./raviS);
elseif D == 1
	ind = (abs(raviF-raviS)./atr(price));
else
    error('RAVI:inputArg','Unknown input in value ''D''. Aborting.');
end;

indAvg = mean(ind);

% Normalize data to a range of 0 - 100.
% Normalize using assumption that 20% or lessrepresents the amount of time we are either consolodating or
% can't tell.  This value should be thought about and revisited.  It may also be a good number to submit
% to a parametric sweep for improvement.
norm = M / indAvg;
ind = ind * norm;

end

