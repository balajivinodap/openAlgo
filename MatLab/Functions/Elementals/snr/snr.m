function amp = snr(price,iMult,qMult)
%SNR A measurement which attempts to detect a change between trending and ranging states
%
% From the work of John Ehlers.  
% "[...] if we can estimate the signal amplitude relative to the market “noise”, then we have a tool that 
% estimates the quality of our technical analysis.  With the kind of data we have available to us, let us 
% develop a unique definition of noise."
% 
%			-- Edits been made in the balance of the description for clairty --
%
% "Consider a sampled signal as a sinewave with the sampling uncertainty as the high and low of each bar.  
% The high and low is the uncertainty of each of our perfect sinewave sample points.  
% We can make good trades as long as our Signal amplitude is much larger than the average daily range of the bars.  
% When half the average daily range becomes equal to the signal amplitude making money on a trade becomes a crapshoot.  
% Under this condition we could make an entry at the low of the bar containing the signal high and 
% make an exit at the high of the bar containing the signal low for zero profit.  Therefore we will define 
% the case where half the average daily trading range is equal to the signal amplitude as our Zero Decibel Signal 
% to Noise (SNR) condition.  We want the signal amplitude to be at least twice the noise amplitude (6 dB SNR) to have 
% a reasonable chance to make a profit from our analysis"
%
%	INPUTS
%			PRICE	The 'snr.m' function expects O | H | L | C as price input. It must have a minimum of 8 observations.
%			IMULT	Inphase multiplier 		(default 0.635)
%			QMULT	Quadrature multiplier	(default 0.338)
%
%	SNR(PRICE)		Returns a vector of the Signal-to-Noise function with default values.
%	SNR(PRICE,...)	Returns a vector of the Signal-to-Noise function with declared values.
%
%   amp = SNR(PRICE)	Returns a SNR vector with default values:
%									iMult	0.635
%									qMult	0.338
%   amp = SNR(PRICE,...)	Returns a SNR vector with declared values
%
% Author:           Mark Tompkins
% Revision:			4903.17425
% All rights reserved.

%% MEX code to be skipped
coder.extrinsic('OHLCSplitter');

%% Error check
rows = size(price,1);
if rows < 8
    error('snr:dataSizeFailure','snr requires a minimum of 8 observations. Exiting.');
end;

%% Preallocation
inPhase = zeros(rows,1);
quad = zeros(rows,1);
amp = zeros(rows,1);
range = zeros(rows,1);
value1 = zeros(rows,1);
value2 = zeros(rows,1);
fHigh = zeros(rows,1);  %#ok<NASGU>
fLow = zeros(rows,1); %#ok<NASGU>

%% Parse
[~,fHigh,fLow,~] = OHLCSplitter(price);
HighLow = (fHigh+fLow)/2;

%% {Compute "Noise" as the average range}
for ii = 2:rows
    %% {Detrend Price}
    if ii >= 8
        value1(ii) = HighLow(ii) - HighLow(ii-7);
    end;
    range(ii) = .2*(fHigh(ii) - fLow(ii)) + .8*range(ii-1);
end; %for

%% {Compute Hilbert Transform outputs}
for ii = 5:rows
    inPhase(ii) = 1.25*value1(ii-4) - iMult*value1(ii-2) + iMult*inPhase(ii-3);
    quad(ii) = value1(ii-2) - qMult*value1(ii) + qMult*quad(ii-2);
end; %for

%% {Compute smoothed signal amplitude}
for ii = 2:rows
    value2(ii) = .2*(inPhase(ii)*inPhase(ii) + quad(ii)*quad(ii)) + .8*value2(ii-1);
    %% {Compute smoothed SNR in Decibels,...
    %  guarding against a divide by zero error, and compensating for filter loss}
    if value2(ii) < .001, value2(ii) = .001; end;   %prevent div by 0
    if range(ii) > 0
        amp(ii) = .25*(10*log(value2(ii)/(range(ii)*range(ii)))/log(10) + 1.9) + .75*amp(ii-1);
        if amp(ii) < 0, amp(ii) = 0; end;
    end; %if
end; %for

    


