function varargout = atr(price,M,hSub)
%ATR Average True Range
%
%	The average true range attempts to modify price data in such a way that it 'corrects'
%	various gaps that may otherwise be evident.  This had a significantly higher degree
%	of relevance when markets were not electronic and had many hours between sessions.
%	The primary modifier of the data is smoothed by an exponential average of elemental
%	bar components High, Low, and Close.
%
%   atr = ATR(PRICE) 		returns the average true range using a default lookback
% 							of 20 observations
%   
%   atr = ATR(PRICE,M) 		returns the average true range using a a specified
%					 		lookback of 'M' observations when M is an integer > 0
%   
%	atr = ATR(PRICE,M,hSub) returns the average true range using a a specified
%					 		lookback of 'M' observations while allowing the parameters
%							of hSub to be passed for graphing
%
% Author:           Mark Tompkins
% Revision:			4903.17425
% All rights reserved.

%% Error check

%% Validate inputs
if ~exist('M','var')
    M = 20;                   % default ema lookback
elseif M < 1
    warning('Invalid input %d for ATR lookback. Value should be M > 0. Using default of M = 20');
    M = 20;
end; %if

% Parse data
if size(price,2) < 4
    error('ATR:priceDimensions','We need O | H | L | C as a minimum input.  Aborting.');
else
    [~,fHigh,fLow,fClose] = OHLCSplitter(price);
end; %if

%% True range
h_m_l = fHigh-fLow;                                 % high - low
h_m_c = [0;abs(fHigh(2:end)-fClose(1:end-1))];      % abs(high - close)
l_m_c = [0;abs(fLow(2:end)-fClose(1:end-1))];       % abs(low - close)
tr = max([h_m_l,h_m_c,l_m_c],[],2);                 % true range
        
%% Average true range
atr = movAvg_mex(tr,M,M,-1);                     % '-1' calls to exponential calculation
        
%% Plot if requested
if nargout == 0 && (~exist('hSub','var'))% Plot
    ax(1) = subplot(2,1,1);
    plot(fClose);
    axis (ax(1),'tight');
    grid on
    legend('Price','Location', 'NorthWest')
    title('Price')
    
    ax(2) = subplot(2,1,2);
    plot(atr)
    grid on
    legend('ATR Bars ',num2str(M),'Location', 'Best')
    title('AVerage True Range')
    linkaxes(ax,'x')
    
elseif (nargout == 0) && exist('hSub','var')% Plot as subplot
    % We pass hSub as a string so we can have asymmetrical graphs
	% The call to char() parses the passed cell array
    ax(1) = subplot(str2num(char(hSub(1))),str2num(char(hSub(2))),str2num(char(hSub(3)))); %#ok<ST2NM>
    plot(fClose);
    axis (ax(1),'tight');
    grid on
    legend('Price','Location', 'NorthWest')
    title('Price')
    
    ax(2) = subplot(str2num(char(hSub(1))),str2num(char(hSub(2))),str2num(char(hSub(4)))); %#ok<ST2NM>
    plot(atr)
    grid on
    legend('ATR Bars ',num2str(M),'Location', 'Best')
    title('AVerage True Range')
    linkaxes(ax,'x')
else
    %% Return values
    for ii = 1:nargout
        switch ii
            case 1
                varargout{1} = atr; % atr
            otherwise
                warning('ATR:OutputArg',...
                    'Too many output arguments requested, ignoring last ones');
        end %switch
    end %for
end




