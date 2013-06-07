function [ varargout ] = iTrendSIG_DIS(price,scaling,cost,bigPoint,hSub)
%ITRENDSIGDIS An indicator based on the work of John Elhers
%   instantaneousTrend returns a trading signal for a given iTrend and moving average crossover 
%
%   Input 'price' should be of an O | H | L | C form as we use the average of the Open & Close
%   when passed to iTrend.m
%
%   S = ITRENDSIGDIS(PRICE) returns a trading signal based upon a 14-period
%   iTrend and a Closing price (~ 1 day average).  
%
%   S = ITRENDSIGDIS(PRICE,I,T) returns a trading signal for a I-period iTrend and
%   a T-period simple moving average.
%
%   [S,R,SH,ITREND,MA] = ITRENDSIGDIS(...) 
%           S       derived trading signal 
%           R       absolute return in R
%           SH      derived Sharpe based on R
%           ITREND  iTrend as calculated with a call to iTrend.m
%           MA
%
% Author:           Mark Tompkins
% Revision:			4902.23938
% All rights reserved.

%% Error check
rows = size(price,1);
if rows < 55
    error('iTrend2inputs:dataSizeFailure','iTrend2inputs requires a minimum of 55 observations. Exiting.');
end;

%% Defaults
if ~exist('scaling','var'), scaling = 1; end;
if ~exist('cost','var'), cost = 0; end;         % default cost
if ~exist('bigPoint','var'), bigPoint = 1; end; % default bigPoint

%% Parse
[fOpen,fHigh,fLow,fClose] = OHLCSplitter(price);
HighLow = (fHigh+fLow)/2;


%% iTrend signal generation using dominant cycle crossing
if nargin > 0
    %% Preallocate
    returns = zeros(rows,1);                                 %#ok<NASGU>
    s = zeros(rows,1);
    
    [tLine,instT] = iTrend_mex(HighLow);
    
    s(instT>tLine) = 	1.5;
    s(instT<tLine) =   -1.5;
    
    % Clear erroneous signals calculated prior to enough data
    s(1:54) = 0;
    sigClean = s;
    
    % Set the first position to 1 lot
    % Make sure we have at least one trade first
    if ~isempty(find(sigClean,1))      
        % We have to remove Echos while they are all 2's
        % Clean up repeating information
        sigClean = remEchos_mex(sigClean);
        
        [~,~,~,returns] = calcProfitLoss([fOpen fClose],sigClean,bigPoint,cost);
        sharpeRatio=scaling*sharpe(returns,0);
    
	else
    	returns = 0;
    	sharpeRatio= 0;
    end; %if
    
    %% If no assignment to variable, show the averages in a chart
    if (nargout == 0) && (~exist('hSub','var'))% Plot
        
    % Plot results
    ax(1) = subplot(2,1,1);
    plot([fClose,tLine,instT]); 
    axis (ax(1),'tight');
    grid on
    legend('Close','iTrend','instT','Location','NorthWest')
    title(['iTrend Results, Annual Sharpe Ratio = ',num2str(sharpeRatio,3)])
    
    ax(2) = subplot(2,1,2);
    plot([sigClean,cumsum(returns)]); grid on
    legend('Position','Cumulative Return','Location','North')
    title(['Final Return = ',thousandSepCash(sum(returns))])
    linkaxes(ax,'x')
    
    elseif (nargout == 0) && exist('hSub','var')% Plot as subplot
        % We pass hSub as a string so we can have asymmetrical graphs
        % The call to char() parses the passed cell array
        ax(1) = subplot(str2num(char(hSub(1))), str2num(char(hSub(2))), str2num(char(hSub(3)))); %#ok<ST2NM>
        plot([fClose,tLine,instT]); 
        axis (ax(1),'tight');
        grid on
        legend('Close','iTrend','instT','Location','NorthWest')
        title(['iTrend Results, Annual Sharpe Ratio = ',num2str(sharpeRatio,3)])
        
        ax(2) = subplot(str2num(char(hSub(1))),str2num(char(hSub(2))), str2num(char(hSub(4)))); %#ok<ST2NM>
        plot([sigClean,cumsum(returns)]); grid on
        legend('Position','Cumulative Return','Location','North')
        title(['Final Return = ',thousandSepCash(sum(returns))])
        linkaxes(ax,'x') 
    else
        for i = 1:nargout
            switch i
                case 1
                    varargout{1} = s;
                case 2
                    varargout{2} = returns;
                case 3
                    varargout{3} = sharpeRatio;
                case 4
                    varargout{4} = tLine;
                case 5
                    varargout{5} = instT;
                otherwise
                    warning('ITREND2INPUTS:OutputArg',...
                        'Too many output arguments requested, ignoring last ones');
            end %switch
        end %for
    end %if
    
end; %if

end

