function [SIG,R,SH,LEAD,LAG] = ma2inputsSIG(price,F,S,type,scaling,cost,bigPoint)
%MA2INPUTSSIG returns a trading signal for a simple lead/lag indicator
%   ma2inputsSIG returns a trading signal for a simple lead/lag
%   moving-average technical indicator.
%
%   S = MA2INPUTSSIG(PRICE) returns a trading signal based upon a 12-period
%   lead and a 26-period lag.  This is the default value used in a MACD
%   indicator.  S is the trading signal of values -1, 0, 1 where -1 denotes
%   a sell (short), 0 is neutral, and 1 is buy (long).
%
%   S = MA2INPUTSSIG(PRICE,N,M) returns a trading signal for a N-period lead and
%   a M-period lag.
%
%   [S,R,SH,LEAD,LAG] = MA2INPUTSSIG(...) returns the trading signal SIG, the
%   absolute return in R, the Sharpe Ratio in SH calcualted using R, and
%   the LEAD or LAG series.
%
%   inputvars:  OC is an array of either 'Close' or 'Open' | 'Close'
%               F = fast period
%               S = slow period
%               Available average types are:
%                   -5  Triangle (Double smoothed similar to Hull)
%                   -4  Trimmed
%                   -3  Harmonic
%                   -2  Geometric
%                   -1	Exponential
%                   0  Simple
%                   >0  Weighted e.g. 0.5 Square root weighted, 1 = linear, 2 = square weighted
%               scaling = scaling for sharpe calculation
%               cost = commision cost
%               bigPoint = value of one full tic
%
% Author:           Mark Tompkins
% Revision:			4902.23951
% All rights reserved.
%
% See also movavg, sharpe, macd, tsmovavg

%%
% Derived from Matlab's leadlag function
%

%% MEX code to be skipped
coder.extrinsic('sharpe','calcProfitLoss','remEchos_mex','movAvg_mex','OHLCSplitter')

%% Process input args
% As we cannot use ~exist in a MEX we will assume by number of inputs and assign


%% Assign correct column to close price
% Check to ensure we have both Open and Close.  If not assume Close only.
% Preallocate so we can MEX
rows = size(price,1);
fOpen = zeros(rows,1);                                      %#ok<NASGU>
fClose = zeros(rows,1);                                     %#ok<NASGU>
[fOpen,fClose] = OHLCSplitter(price);

%% Input with error check
if nargin == 0
    error('METS:ma2inputsSIG:invalidInputs', ...
        'For a demo execution use ''movAvg2inputs''. ''movAvg2inputsMEX'' does not provide a demo.');
else
    if nargin < 3
        error('METS:ma2inputsSIG:invalidInputs', ...
            'At a minimum we need the data, lead and lag as inputs.');
    else
        if (F>S)
            error('METS:ma2inputsSIG:invalidInputs', ...
                'Lead value was greater than Lag. Catch this before submitting to ''movAvg2inputsMEX''');
        end;
        if F > rows || S > rows
            error ('METS:ma2inputsSIG:invalidInputs', ...
                'Lookback is greater than the number of observations (%d)',rows);
        end;
        % Defaults
        if nargin < 4
            type = 0; scaling =1; cost = 0; bigPoint = 1;
        elseif nargin < 5
            scaling = 1; cost = 0; bigPoint = 1;
        elseif nargin < 6
            cost = 0; bigPoint = 1;
        elseif nargin < 7
            bigPoint = 1;
        end;
        
%% Calculations
        % Preallocation
        % The following two preallocations allow MEX to compile
        % http://www.mathworks.com/matlabcentral/newsreader/view_thread/306824
        SIG = zeros(rows,1);
        LEAD = zeros(rows,1);             	%#ok<NASGU>
        LAG = zeros(rows,1);                %#ok<NASGU>
        R = zeros(rows,1);                     
        cleanSIG = SIG;                     %#ok<NASGU>
        
        [LEAD,LAG] = movAvg_mex(fClose,F,S,type);

        SIG(LEAD>LAG) = 2;
        SIG(LEAD<LAG) = -2;

        % Clear erroneous signals calculated prior to enough data
        SIG(1:S-1) = 0;

        if(~isempty(find(SIG,1)))
            % Clean up repeating information for PNL
            % We have to remove Echos while they are all 2's
            cleanSIG = remEchos_mex(SIG);
            
            % Set the first position to 1 lot                      
            firstIdx = find(cleanSIG,1);                       	% Index of first trade
            firstPO = cleanSIG(firstIdx);
            
        	% Loop until first position change
            % We have to address firstIdx as a vector even though it is a scalar
            % Notice we have to ensure the row is in range FIRST!!
            while (firstIdx(1) <= length(cleanSIG)) && ...
            	isequal(firstPO,cleanSIG(firstIdx))          % Current signal is same as first signal
            	cleanSIG(firstIdx) = cleanSIG(firstIdx)/2;
            	firstIdx = firstIdx + 1;
            end;

            % Normalize signal for output (+/- 1)
            % Moving Average signal is really more of a state
            SIG = sign(SIG);

            % Generate PNL
            [~,~,~,R] = calcProfitLoss([fOpen fClose],cleanSIG,bigPoint,cost);
            
            % Calculate sharpe ratio
            SH=scaling*sharpe(R,0);
        else
            % No signals - no sharpe.
            SH= 0;
        end; %if
    
        % Correct calculations prior to enough bars for lead & lag
        for ii = 1:S-1
            if (ii < F)
                LEAD(ii) = fClose(ii);                 % Reset the moving average calculation to equal the Close
            end;
                LAG(ii) = fClose(ii);                      % Also reset the slower average
        end; %for
    end; %if
end %if