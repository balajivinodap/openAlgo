function varargout = iTrendMaSIG_DIS(price,M,typeMA,scaling,cost,bigPoint)
%ITRENDMASIG_DIS An indicator based on the work of John Elhers
%   ITRENDMASIG_DIS returns a trading signal for a given iTrend and MA cross as well as a
%   technical indicator.
%
%   S = ITRENDMASIG_DIS(PRICE) returns a trading signal based upon a 14-period
%   iTrend and a Closing price (~ 1 day average).  
%   S is the trading signal of values -2, 0, 2 where -2 denotes
%   a sell (short reverse), 0 is neutral, and 2 is buy (long reverse).
%
%   S = ITRENDMASIG_DIS(PRICE,I,T) returns a trading signal for a I-period iTrend and
%   a T-period simple moving average.
%
%   [S,R,SH,ITREND,MA] = ITRENDMASIG_DIS(...) returns the trading signal S, the
%   absolute return in R, the Sharpe Ratio in SH calcualted using R, and
%   the ITREND or MA series.
%
% Author:           Mark Tompkins
% Revision:			4902.23812

%% Error check
rows = size(price,1);
if rows < 55
    error('iTrendMA:dataSizeFailure','iTrendMA requires a minimum of 55 observations. Exiting.');
end;

%% Defaults
if ~exist('M','var'), M = 14; end;
if ~exist('typeMA','var'), typeMA = 0; end;
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
    
    [tLine] = iTrend_mex(HighLow);
    [ma] = movAvg_mex(fClose,M,M,typeMA);
    
    ma(1:M)=fClose(1:M);
    
    s(ma>tLine) = 2;
    s(ma<tLine) = -2;
    
    % Clear erroneous signals calculated prior to enough data
    s(1:54) = 0;
    sigClean = s;
    
    % Set the first position to 1 lot
    % Make sure we have at least one trade first
    if ~isempty(find(sigClean,1))      
        % We have to remove Echos while they are all 2's
        % Clean up repeating information
        sigClean = remEchosMEX_mex(sigClean);
        
        firstIdx = find(sigClean,1);                           % Index of first trade
        firstPO = sigClean(firstIdx);
        % Current signal is same as first signal
        % ... and we still have additional observations
        % Notice we have to ensure the row is in range FIRST!!
        % Loop until first position change
      	while ((firstIdx <= length(sigClean)) && firstPO == sigClean(firstIdx))	
            sigClean(firstIdx) = sigClean(firstIdx)/2;
            firstIdx = firstIdx + 1;
        end;

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
    plot([fClose,tLine,ma]); 
    axis (ax(1),'tight');
    grid on
    legend('Close','iTrend',['MA ',num2str(M)],'Location','NorthWest')
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
        plot([fClose,tLine,ma]); 
        axis (ax(1),'tight');
        grid on
        legend('Close','iTrend',['MA ',num2str(M)],'Location','NorthWest')
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
                    varargout{5} = ma;
                otherwise
                    warning('ITRENDMA:OutputArg',...
                        'Too many output arguments requested, ignoring last ones');
            end %switch
        end %for
    end %if
    
end; %if

end

