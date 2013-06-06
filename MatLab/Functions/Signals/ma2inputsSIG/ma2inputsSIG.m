function [SIG,R,SH,LEAD,LAG] = ma2inputsSIG(price,F,S,type,scaling,cost,bigPoint)
%MA2INPUTSSIG returns a trading signal for a simple lead/lag indicator
%   ma2inputsSIG returns a trading signal derived from 'ma2inputsSTA'
%
%   STA = MA2INPUTSSTA(PRICE, F, S, type) returns a STATE based upon provided
%   lead (F) and lag (S) periods. 
%   
%   INPUTS:     price       an array of any [C] or [O | C] or [O | H | L | C]
%               F           fast period
%               S           slow period
%               type        Available average types are:
%                           -5  Triangle (Double smoothed similar to Hull)
%                           -4  Trimmed
%                           -3  Harmonic
%                           -2  Geometric
%                           -1	Exponential
%                            0  Simple
%                          > 0  Weighted e.g. 0.5 Square root weighted, 1 = linear, 2 = square weighted
%               scaling     sharpe ratio adjuster
%               cost        commission cost for P&L calculation per round turn
%               bigPoint    value of a full tick for P&L calculation
%
%   OUTPUTS:
%               SIG        -1.5 LEAD crossed below LAG.  Sell 1 lot short (reverse if necessary)
%                            0  no action
%                           1.5 LEAD crossed above LAG.  Buy 1 lot long (reverse if necessary)
%                 R         Absolute return
%                SH         Sharpe ratio calculated using R
%              LEAD         Lead vector output
%               LAG	        Lag vector output
%
%   NOTE:
%       As this file is designed to be MEX'd all inputs are required.
%       For a graphical result or variable inputs see 'ma2inputsSIG_DIS.m'
%
% Author:           Mark Tompkins
% Revision:			4904.27712
% All rights reserved.
%
% See also movavg, sharpe, macd, tsmovavg, ma2inputsSTA, ma2inputsSIG_DIS

%% MEX code to be skipped
coder.extrinsic('sharpe','calcProfitLoss','remEchos_mex','ma2inputsSTA_mex','OHLCSplitter')

%% Process input args
% Preallocate so we can MEX
rows = size(price,1);
fOpen = zeros(rows,1);                                      %#ok<NASGU>
fClose = zeros(rows,1);                                     %#ok<NASGU>
STA = zeros(rows,1);                                        %#ok<NASGU>
SIG = zeros(rows,1);                                        
LEAD = zeros(rows,1);                                       %#ok<NASGU>
LAG = zeros(rows,1);                                        %#ok<NASGU>
R = zeros(rows,1);

[fOpen,fClose] = OHLCSplitter(price);

%% Input with error check
if (F > S)
	error('METS:ma2inputsSIG:invalidInputs', ...
    	'LEAD input > LAG input. Catch this before submitting to ''ma2inputsSIG''');
end; %if
        
if (F > rows) || (S > rows)
	error ('METS:ma2inputsSIG:invalidInputs', ...
    	'Lookback is greater than the number of observations (%d)',rows);
end; %if
        
%% Calculations
        % Get state
        [STA, LEAD, LAG] = ma2inputsSTA_mex(fClose,F,S,type);
        
        % Convert state to signal
        SIG(STA < 0) = -1.5;
        SIG(STA > 0) =  1.5;

        % Clear erroneous signals calculated prior to enough data
        SIG(1:S-1) = 0;
        
        if(~isempty(find(SIG,1)))
            % Clean up repeating information for PNL
            SIG = remEchos_mex(SIG);
            
            % Generate PNL
            [~,~,~,R] = calcProfitLoss([fOpen fClose],SIG,bigPoint,cost);
            
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
                LAG(ii) = fClose(ii);                  % Also reset the slower average
        end; %for