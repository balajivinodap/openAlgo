function varargout = rsiSIG_DIS(price,M,thresh,type,scaling,cost,bigPoint,hSub)
%rsiMETS RSI signal generator from rsi.m by The MathWorks, Inc.
%RSISIG
% RSISIG trading strategy.  Note that the trading signal is generated when the
% RSISIG value is above/below the upper/lower threshold.  
% M serves some detrending function which we must investigate

%   NOTE: It is important to consider that an RSI signal generator really has 3 states.
%           Above Threshold is Overbought
%           Below Threshold is Oversold
%           There is also a neutral region between +/- Threshold and 50%
%
%   This should be considered prior to adding or removing any Echos to this signal.
%   We are defining this as an elementary signal and echos that are produced should be
%   passed out to any function call.  For calculating a direct PNL, the signal should first
%   be cleaned with remEchoMEX_mex.
%   
%   Signals with Echos represent a market state, not a signal.  I.e. "we should be ____".
%   For RSI in oversold state, "we should be long".  Removing the Echos produces that buy signal.
%

%% Defaults and parsing
if ~exist('scaling','var'), scaling = 1; end;
if ~exist('type','var'), type=0; end;
if ~exist('cost','var'), cost = 0; end;         % default cost
if ~exist('bigPoint','var'), bigPoint = 1; end; % default bigPoint

if ~exist('M','var')
    M = 0; % no detrending
    N = 14; % default value for rsi calc
else
    % Check if multiple elements are passed.
    % The second element is number of bars to pass to rsindex
    % The default for N (2nd element) is 14
    % If detrender is set to a negative value we will also use the default of 15 * RSIBars
    % This is done so we can test both 0 = No detrending & -1 = Default detrending in a sweep
    % With this adjustment we can sweep [-1:1:14] which will test detrenders 1 through 15 as
    % well as none.
    if numel(M) > 1 
        N = M(1);
        if M(2) < 0
            M = 15 * N;
        else
            M = M(2);
        end; % if
    else
        % M is the detrend average
        % It would appear we are taking a multiple of M below
        % to capture a longer moving average to detrend
        N = M;
        M = 15*M;
    end
end

% We can't exceed the lookback for the RSI Detrender
if M > size(price,1)
    M = size(price,1);
    warning('Detrender reduced to match number of observations.');
end; %if

if ~exist('thresh','var')
    thresh = [30 70]; % default threshold
else
    if numel(thresh) == 1 % scalar value
        thresh = [100-thresh, thresh];
    else
        if thresh(1) > thresh(2)
            thresh= thresh(2:-1:1);
        end
    end
end

[fOpen,fClose] = OHLCSplitter(price);

%% Detrend with a moving average
if M == 0
    ma = zeros(length(fClose),1);
else
    [~,~,~,ma] = ma2inputs_mex(price,M,M,type,scaling,cost,bigPoint);
end

ri = rsindex(fClose - ma, N);

%% Keep on eye on this.  Original errors don't show at the moment
% %% Adjust erronous ri values prior to M value
% for ii = 1:find(ri==0,1)
%     ri(ii)=50;
% end;

%% Generate signal
s = zeros(length(fClose),1);
sigClean = s;

% Crossing the lower threshold (oversold)
indx    = ri < thresh(1);
% Unknown Matlab adjuster
% indx    = [false; indx(1:end-1) & ~indx(2:end)];
s(indx) = 2;

% Crossing the upper threshold (overbought)
indx    = ri > thresh(2);
% Unknown Matlab adjuster
% indx    = [false; indx(1:end-1) & ~indx(2:end)];
s(indx) = -2;

% Set the first position to 1 lot
    % Make sure we have at least one trade first
if ~isempty(find(s,1))
    % We have to remove Echos while they are all 2's
    % Clean up repeating information so we can calculate a PNL
	sigClean = remEchos_mex(s);
    
	firstIdx = find(sigClean,1);                           % Index of first trade
	firstPO = sigClean(firstIdx);
    % Notice we have to ensure the row is in range FIRST!!
    % Loop until first position change
	while ((firstIdx <= length(sigClean)) && firstPO == sigClean(firstIdx))
        % Changes first signal from +/-2 to +/-1
    	sigClean(firstIdx) = sigClean(firstIdx)/2;                
    	firstIdx = firstIdx + 1;
	end;
    

    
    %% PNL Caclulation
	[~,~,~,r] = calcProfitLoss([fOpen fClose],sigClean,bigPoint,cost);
	sh = scaling*sharpe(r,0);
else
    % No signal - no return or sharpe
    r = zeros(length(fClose),1);
	sh = 0;
end; %if

%% Plot if requested
if nargout == 0 && (~exist('hSub','var'))% Plot
    ax(1) = subplot(3,1,1);
    if M == 0
        ma = price;
    end; %if
    plot([fClose,ma]);
    axis (ax(1),'tight');
    grid on
    legend('Price',['Detrender ',num2str(M)],'Location', 'NorthWest')
    title(['RSI Results, Sharpe Ratio = ',num2str(sh,3)])
    
    ax(2) = subplot(3,1,2);
    ylim([0 100])
    axis manual;
    hold on;
    plot([ri,thresh(2)*ones(size(ri)),thresh(1)*ones(size(ri))])
    grid on
    legend(['RSI Bars ',num2str(N)],['RSI Upper ',num2str(thresh(2)),'%'],...
            ['RSI Lower ',num2str(thresh(1)),'%'],'Location', 'North')
    title('RSI')
    
    ax(3) = subplot(3,1,3);
    plot([sigClean,cumsum(r)]), grid on
    legend('Position','Cumulative Return','Location','North')
    title(['Final Return = ',thousandSepCash(sum(r))])
    linkaxes(ax,'x')
    
elseif (nargout == 0) && exist('hSub','var')% Plot as subplot
    % We pass hSub as a string so we can have asymmetrical graphs
	% The call to char() parses the passed cell array
    ax(1) = subplot(str2num(char(hSub(1))),str2num(char(hSub(2))),str2num(char(hSub(3)))); %#ok<ST2NM>
    if M == 0
        ma = price;
    end; %if
    plot([fClose,ma]);
    axis (ax(1),'tight');
    grid on
    legend('Price',['Detrender ',num2str(M)],'Location', 'NorthWest')
    title(['RSI Results, Sharpe Ratio = ',num2str(sh,3)])
    
    ax(2) = subplot(str2num(char(hSub(1))),str2num(char(hSub(2))),str2num(char(hSub(4)))); %#ok<ST2NM>
    ylim([0 100])
    axis manual;
    hold on;
    plot([ri,thresh(2)*ones(size(ri)),thresh(1)*ones(size(ri))])
    grid on
    legend(['RSI Bars ',num2str(N)],['RSI Upper ',num2str(thresh(2)),'%'],...
        ['RSI Lower ',num2str(thresh(1)),'%'],'Location', 'North')
    title('RSI')
    
    ax(3) = subplot(str2num(char(hSub(1))),str2num(char(hSub(2))),str2num(char(hSub(5)))); %#ok<ST2NM>
    plot([sigClean,cumsum(r)]), grid on
    legend('Position','Cumulative Return','Location','North')
    title(['Final Return = ',thousandSepCash(sum(r))])
    linkaxes(ax,'x')
else
    %% Return values
    for ii = 1:nargout
        switch ii
            case 1
                varargout{1} = sign(s); % signal (contains Echos)
            case 2
                varargout{2} = r; % return (pnl)
            case 3
                varargout{3} = sh; % sharpe ratio
            case 4
                varargout{4} = ri; % rsi signal
            case 5
                varargout{5} = ma; % moving average
            case 6
                varargout{6} = thresh; % threshold
            otherwise
                warning('RSI:OutputArg',...
                    'Too many output arguments requested, ignoring last ones');
        end %switch
    end %for
end

