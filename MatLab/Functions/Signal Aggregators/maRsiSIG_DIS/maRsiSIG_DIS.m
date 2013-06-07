function varargout = maRsiSIG_DIS(price,N,M,typeMA,Mrsi,thresh,typeRSI,isSignal,scaling,cost,bigPoint)
% MA+RSI in function call
%%
%
%   isSignal:   0 - Filter (default), 1 - Signal
%               We can either combine the signals from each element, MA + RSi or we can use the RSI as a
%               filter condition for MA.  
%
% Author:           Mark Tompkins
% Revision:			4902.23857
% All rights reserved.

%% NEED TO ADD ERROR CHECKING OF INPUTS
%% Defaults
if ~exist('N','var'), N = 12; end;
if ~exist('M','var'), M = 26; end;
if ~exist('typeMA','var'), typeMA=0; end;
if ~exist('Mrsi','var'), Mrsi = [14 0]; end;
if ~exist('thresh','var'), thresh = 65; end;
if ~exist('typeRSI','var'), typeRSI = 0; end;
if ~exist('isSignal','var'), isSignal = 0; end;
if ~exist('scaling','var'), scaling = 1; end;
if ~exist('cost','var'), cost = 0; end;         % default cost
if ~exist('bigPoint','var'), bigPoint = 1; end; % default bigPoint

if numel(thresh) == 1 % scalar value
	thresh = [100-thresh, thresh];
else
    if thresh(1) > thresh(2)
        thresh = thresh(2:-1:1);
    end %if	
end %if

if length(Mrsi) == 1
    Mrsi = [15*Mrsi Mrsi];
end

[fOpen,fClose] = OHLCSplitter(price);

%[sma,~,~,lead,lag] = ma2inputsSIG_mex(price,N,M,typeMA,scaling,cost,bigPoint);
%[srsi,~,~,ri,ma,thresh] = rsiSIG(price,Mrsi,thresh,typeRSI,scaling,cost,bigPoint);

[sma,lead,lag] = ma2inputsSTA_mex(price,N,M,typeMA);
[srsi,ri,ma] = rsiSTA_mex(price,Mrsi,thresh,typeRSI);

%%  The RSI is either used as a signal generator or a filter condition for another signal
%   If we are using it to generate a signal, we should return only an actionable signal with no repeats
%   If we are using it as a filter, we should return the state of Overbought | Oversold including repeats
%   For this specific 'marsiMETS' case, we combine it with a simple moving average
if isSignal == 0
    %% FILTER
    % Aggregate the two states 
    s = (sma+srsi);

    % Any instance where the |sum| of the 2 signals is ~= 2 means both conditions are not met
    % Drop those instances
    s(abs(s)~=2) = 0;
    
    % Refine to a signal
    s = sign(s) * 1.5;

elseif isSignal == 1
    %% SIGNAL
    % Aggregate the two signals normalizing them to +/- 1.5
    s = sign(sma+srsi) * 1.5;
end; %if

% Drop any repeats for PNL
sClean = remEchos_mex(s);

% Make sure we have at least one trade first
if ~isempty(find(sClean,1))
    
    % Set the first position to +/- 1 lot
    firstIdx = find(sClean,1);                           % Index of first trade
    firstPO = sClean(firstIdx);

    % Notice we have to ensure the row is in range FIRST!!
    % Loop until first position change
    while ((firstIdx <= length(sClean)) && firstPO == sClean(firstIdx))
        % Changes first signal from +/-2 to +/-1
        sClean(firstIdx) = sClean(firstIdx)/2;                
        firstIdx = firstIdx + 1;
    end; %while
    
    [~,~,~,r] = calcProfitLoss([fOpen fClose],sClean,bigPoint,cost);

    sh = scaling*sharpe(r,0);
else
    % No signal so no return or sharpe.
    r = zeros(length(price),1);
    sh = 0;
end; %if

if nargout == 0
   
    figure()
    % Not using MEX so we get a graphical response
    % Each element must be the same length - nonsense - thanks MatLab
    % http://www.mathworks.com/help/matlab/matlab_prog/cell-arrays-of-strings.html
    layout = ['6         ';'3         ';'[1 4 7]   ';'[10 13 16]'];
    hSub = cellstr(layout);
    ma2inputsSIG_DIS(price,N,M,typeMA,scaling,cost,bigPoint,hSub);
    
    layout = ['6      ';'3      ';'[2 5]  ';'[8 11] ';'[14 17]'];
    hSub = cellstr(layout);
    rsiSIG_DIS(price,Mrsi,thresh,typeRSI,scaling,cost,bigPoint,hSub);
    
    % If we were given a negative value for the detrender change the information to something 
    % intelligent, otherwise give the value.
    if(Mrsi(2)<0)
        detrendStr = num2str(Mrsi(1)*15);
    else
        detrendStr = num2str(Mrsi(2));
    end;

    ax(1) = subplot(6,3,[3 6]);
    if ma == 0 
        ma = price;
    end; %if
    plot([price,lead,lag,ma]);
    axis (ax(1),'tight');
    grid on
    legend('Price',['Lead ',num2str(N)],['Lag ',num2str(M)],...
    ['RSI Detrend ',detrendStr],'Location', 'NorthWest')
    title(['MA+RSI Results, Sharpe Ratio = ',num2str(sh,3)])
    
    ax(2) = subplot(6,3,[9 12]);
    ylim([0 100])
    hold on
    plot([ri,thresh(2)*ones(size(ri)),thresh(1)*ones(size(ri))])
    grid on
    legend(['RSI Bars ',num2str(Mrsi(1))],['RSI Upper ',num2str(thresh(2)),'%'],...
        ['RSI Lower ',num2str(thresh(1)),'%'],'Location', 'North')
    title('RSI')
    
    ax(3) = subplot(6,3,[15 18]);
    plot([sClean,cumsum(r)]), grid on
    legend('Position','Cumulative Return','Location', 'North')
    title(['Final Return = ',thousandSepCash(sum(r))])
    
    linkaxes(ax,'x')

else
    %% Return values
    for i = 1:nargout
        switch i
            case 1
                if isSignal == 1
                    warning('We do not expect to use marsiMETS to generate signals when isSignal == 1');
                    warning('It is already the aggregatioin of two elemental signals.');
                end;
                varargout{1} = sign(s); % signal (with repeats because it contains an MA signal)
            case 2
                varargout{2} = r; % return (pnl)
            case 3
                varargout{3} = sh; % sharpe ratio
            case 4
                varargout{4} = ri; % rsi signal
            case 5
                varargout{5} = ma; % moving average
            otherwise
                warning('RSI:OutputArg',...
                    'Too many output arguments requested, ignoring last ones');
        end %switch
    end %for
end
