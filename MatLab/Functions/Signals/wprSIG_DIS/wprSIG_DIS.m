function varargout = wprSIG_DIS(price,N,thresh,scaling,cost,bigPoint,hSub)
%
% Author:           Mark Tompkins
% Revision:			4902.24004
% All rights reserved.

if ~exist('scaling','var'), scaling = 1; end
if ~exist('N','var'), N = 14; end
if ~exist('cost','var'), cost = 0; end
if ~exist('bigPoint','var'), bigPoint = 1; end; % default bigPoint

if ~exist('thresh','var')
    % Overbought | Oversold
    thresh = [-20 -80]; % default threshold
else
    % WPR works with negative values in a range from 0 to -100;
    if numel(thresh) == 1 % scalar value
        thresh = [(100-abs(thresh))*-1, abs(thresh)*-1];
    else
        thresh = abs(thresh)*-1;
    end;
    if thresh(1) < thresh(2)
    	thresh= thresh(2:-1:1);
    end;
end;

if thresh(1) > 0, thresh(1) = thresh(1) * -1; end;
if thresh(2) > 0, thresh(2) = thresh(2) * -1; end;

% Preallocate
% We pass wpr High | Low | Close
bars = zeros(size(price,1),3); %#ok<NASGU>
s = zeros(size(price,1),1);

if size(price,2) == 4
    [fOpen, fHigh, fLow, fClose] = OHLCSplitter(price);
else
    error('wprMETS:InputArg',...
    	'We need as input O | H | L | C.');
end; %if

%% williams %r
w = willpctr([fHigh fLow fClose],N);

%% generate signal
% Crossing the upper threshold (overbought)
indx = w > thresh(1);
s(indx) = -2;

indx = w < thresh(2);
s(indx) = 2;

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
if nargout == 0
    ax(1) = subplot(3,1,1);
    plot([fHigh fLow fClose]), grid on
    axis (ax(1),'tight');
    legend('High','Low','Close')
    title(['W%R Results, Sharpe Ratio = ',num2str(sh,3)])
    
    ax(2) = subplot(3,1,2);
    plot([w,-20*ones(size(w)),-80*ones(size(w))])
    grid on
    legend(['Williams %R ',num2str(N)],'Over bought','Over sold')
    title('W%R')
    
    ax(3) = subplot(3,1,3);
    plot([sigClean,cumsum(r)]), grid on
    legend('Position','Cumulative Return')
    title(['Final Return = ',thousandSepCash(sum(r))])
    linkaxes(ax,'x')
else
    %% Return values
    for i = 1:nargout
        switch i
            case 1
                varargout{1} = s; % signal
            case 2
                varargout{2} = r; % return (pnl)
            case 3
                varargout{3} = sh; % sharpe ratio
            case 4
                varargout{4} = w; % w%r value
            otherwise
                warning('WPR:OutputArg',...
                    'Too many output arguments requested, ignoring last ones');
        end %switch
    end %for
end %if