function [STA,LEAD,LAG] = ma2inputsSTA(price,F,S,type)
%MA2INPUTSSTA returns a logical STATE for a simple lead/lag calculation
%   ma2inputsSTA returns a logical STATE for a simple lead/lag calculation
%   moving-average technical indicator.
%
%   STA = MA2INPUTSSTA(PRICE, F, S, type) returns a STATE based upon provided
%   lead (F) and lag (S) periods. 
%   
%   INPUTS:     price = an array of any [C] or [O | C] or [O | H | L | C]
%               F = fast period
%               S = slow period
%               type = Available average types are:
%                           -5  Triangle (Double smoothed similar to Hull)
%                           -4  Trimmed
%                           -3  Harmonic
%                           -2  Geometric
%                           -1	Exponential
%                            0  Simple
%                          > 0  Weighted e.g. 0.5 Square root weighted, 1 = linear, 2 = square weighted
%
%   OUTPUT:
%       STA values are LEAD referenced where: 
%           1    LEAD > LAG
%           0    LEAD = LAG
%          -1    LEAD < LAG
%
%   NOTE:
%       As this file is designed to be MEX'd all inputs are required.
%
% Author:           Mark Tompkins
% Revision:			4904.27712
% All rights reserved.
%
% See also movavg, sharpe, macd, tsmovavg

%% MEX code to be skipped
coder.extrinsic('movAvg_mex','OHLCSplitter')

%% Assign correct column to close price
% Check to ensure we have both Open and Close.  If not assume Close only.
% Preallocate so we can MEX
rows = size(price,1);
fOpen = zeros(rows,1);                                      %#ok<NASGU>
fClose = zeros(rows,1);                                     %#ok<NASGU>
fClose = OHLCSplitter(price);

%% Input with error check
if (F > S)
	error('METS:ma2inputsSIG:invalidInputs', ...
    	'LEAD input > LAG input. Catch this before submitting to ''ma2inputsSIG''');
end;
        
if F > rows || S > rows
	error ('METS:ma2inputsSIG:invalidInputs', ...
    	'Lookback is greater than the number of observations (%d)',rows);
end;
       
        
%% Calculations
        % Preallocation
        % The following two preallocations allow MEX to compile
        % http://www.mathworks.com/matlabcentral/newsreader/view_thread/306824
        STA = zeros(rows,1);
        LEAD = zeros(rows,1);             	%#ok<NASGU>
        LAG = zeros(rows,1);                %#ok<NASGU>
                  
        [LEAD,LAG] = movAvg_mex(fClose,F,S,type);

        STA(LEAD>LAG) = 1;
        STA(LEAD<LAG) = -1;

        % Clear erroneous states calculated prior to enough data
        STA(1:S-1) = 0;

    
        % Correct calculations prior to enough bars for lead & lag
        for ii = 1:S-1
            if (ii < F)
                LEAD(ii) = fClose(ii);                 % Reset the moving average calculation to equal the Close
            end;
                LAG(ii) = fClose(ii);                      % Also reset the slower average
        end; %for
end %if