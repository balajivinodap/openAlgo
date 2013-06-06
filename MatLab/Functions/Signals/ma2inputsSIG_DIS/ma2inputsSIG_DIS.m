function varargout = ma2inputsSIG_DIS(price,F,S,typeMA,scaling,cost,bigPoint,hSub)
%MA2INPUTSSIG_DIS returns a trading signal for a simple lead/lag indicator
%   movAvg2inputs returns a trading signal for a simple lead/lag
%   moving-average technical indicator.
%
%   S = MA2INPUTSSIG_DIS(PRICE) returns a trading signal based upon a 12-period
%   lead and a 26-period lag.  This is the default value used in a MACD
%   indicator.  S is the trading signal of values -1, 0, 1 where -1 denotes
%   a sell (short), 0 is neutral, and 1 is buy (long).
%
%   S = MA2INPUTSSIG_DIS(PRICE,N,M) returns a trading signal for a N-period lead and
%   a M-period lag.
%
%   [S,R,SH,LEAD,LAG] = MA2INPUTSSIG_DIS(...) returns the trading signal S, the
%   absolute return in R, the Sharpe Ratio in SH calcualted using R, and
%   the LEAD or LAG series.
%
%   hSub so that we can return ma2inputs as a subPlot
%
% Author:           Mark Tompkins
% Revision:			4904.27712
% All rights reserved.

if nargin == 0
    error('MA2INPUTSSIG_DIS:NoLagWindowDefined','When defining a LEAD value LAG value must also be given.')
end; %if

%% Process input args
if ~exist('typeMA', 'var')
    typeMA = 0;
end;
if ~exist('scaling','var')
    scaling = 1;
end

if ~exist('cost', 'var')
    cost = 0;
end

if ~exist('bigPoint', 'var')
    bigPoint = 1;
end

if nargin < 2
    % default values - often used in MACD
    S = 26;
    F = 12;
elseif nargin < 3
    error('MA2INPUTSSIG_DIS:NoLagWindowDefined','When defining a LEAD value LAG value must also be given.')
end %if

rows = size(price,1);

%% Input with error check
if (F > S)
	error('MA2INPUTSSIG_DIS:invalidInputs', ...
    	'At a minimum a price vector must be provided.');
end; %if
        
if F > rows || S > rows
	error ('MA2INPUTSSIG_DIS:invalidInputs', ...
    	'Lookback is greater than the number of observations (%d)',rows);
end; %if

[fOpen,fClose] = OHLCSplitter(price);
returns = zeros(rows,1);
SIG = zeros(rows,1);

%% Simple lead/lag calculation
% Get state
[STA, LEAD, LAG] = ma2inputsSTA_mex(fClose,F,S,typeMA);

% Convert state to signal
SIG(STA < 0) = -1.5;
SIG(STA > 0) =  1.5;

% Clear erroneous signals calculated prior to enough data
SIG(1:S-1) = 0;

if(~isempty(find(SIG,1)))
    % Clean up repeating information for PNL
    SIG = remEchos_mex(SIG);

    % Generate PNL
    [~,~,~,returns] = calcProfitLoss([fOpen fClose],SIG,bigPoint,cost);

    % Calculate sharpe ratio
    sharpeRatio=scaling*sharpe(returns,0);
else
    % No signals - no sharpe.
    sharpeRatio= 0;
end; %if

% Correct calculations prior to enough bars for lead & lag
for ii = 1:S-1
    if (ii < F)
        LEAD(ii) = fClose(ii);                 % Reset the moving average calculation to equal the Close
    end;
        LAG(ii) = fClose(ii);                  % Also reset the slower average
end; %for
    
%% If no assignment to variable, show the averages in a chart
    if (nargout == 0) && (~exist('hSub','var'))% Plot
        
    % Plot results
    ax(1) = subplot(2,1,1);
    plot([fClose,LEAD,LAG]); 
    axis (ax(1),'tight');
    grid on
    legend('Close',['Lead ',num2str(F)],['Lag ',num2str(S)],'Location','NorthWest')
    title(['Lead/Lag MA Results, Annual Sharpe Ratio = ',num2str(sharpeRatio,3)])
    
    ax(2) = subplot(2,1,2);
    plot([SIG,cumsum(returns)]); grid on
    legend('Position','Cumulative Return','Location','North')
    title(['Final Return = ',thousandSepCash(sum(returns))])
    linkaxes(ax,'x')
    
    elseif (nargout == 0) && exist('hSub','var')% Plot as subplot
        % We pass hSub as a string so we can have asymmetrical graphs
        % The call to char() parses the passed cell array
        ax(1) = subplot(str2num(char(hSub(1))), str2num(char(hSub(2))), str2num(char(hSub(3)))); %#ok<ST2NM>
        plot([fClose,LEAD,LAG]); 
        axis (ax(1),'tight');
        grid on
        legend('Close',['Lead ',num2str(F)],['Lag ',num2str(S)],'Location','NorthWest')
        title(['Lead/Lag MA Results, Annual Sharpe Ratio = ',num2str(sharpeRatio,3)])
        
        ax(2) = subplot(str2num(char(hSub(1))),str2num(char(hSub(2))), str2num(char(hSub(4)))); %#ok<ST2NM>
        plot([SIG,cumsum(returns)]); grid on
        legend('Position','Cumulative Return','Location','North')
        title(['Final Return = ',thousandSepCash(sum(returns))])
        linkaxes(ax,'x') 
    else
        for i = 1:nargout
            switch i
                case 1
                    varargout{1} = SIG;
                case 2
                    varargout{2} = returns;
                case 3
                    varargout{3} = sharpeRatio;
                case 4
                    varargout{4} = LEAD;
                case 5
                    varargout{5} = LAG;
                otherwise
                    warning('MOVAVG2INPUTS:OutputArg','Too many output arguments requested, ignoring last ones');
            end %switch
        end %for
    end %if
end %if