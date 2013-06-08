function varargout = maRsiSIG(price,N,M,typeMA,Mrsi,thresh,typeRSI,isSignal,scaling,cost,bigPoint)
% MA+RSI in function call
%%
%
%   isSignal:   0 - Filter (default)    We can either combine the signals from 
%               1 - Signal              each element, MA + RSI or we can use
%                                       the RSI as a filter condition for MA.
%                  
% Author:           Mark Tompkins
% Revision:			4906.23172
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

if length(Mrsi) == 1
    Mrsi = [15*Mrsi Mrsi];
end

[fOpen,fClose] = OHLCSplitter(price);

[sma] = ma2inputsSTA_mex(price,N,M,typeMA);
% NOTE: rsiSTA returns a 1 when oversold and -1 when overbought
%[srsi] = rsiSTA_mex(price,Mrsi,thresh,typeRSI);
[srsi] = rsiSTA(price,Mrsi,thresh,typeRSI);

%%  The RSI is either used as a signal generator or a filter condition for another signal
%   If we are using it to generate a signal, we should return only an actionable signal with no repeats
%   If we are using it as a filter, we should return the state of Overbought | Oversold including repeats
%   For this specific 'marsiMETS' case, we combine it with a simple moving average

%% Use RSI as FILTER
if isSignal == 0
    % Aggregate the two states 
    s = (sma+srsi);

    % Any instance where the |sum| of the 2 signals is ~= 2 means both conditions are not met
    % Drop those instances
    s(abs(s)~=2) = 0;
    
    % Refine to a signal
    s = sign(s) * 1.5;

%% Use RSI as SIGNAL
elseif isSignal == 1
    % Aggregate the two signals normalizing them to +/- 1.5
    s = sign(sma+srsi) * 1.5;
end; %if

%% Drop any repeats for PNL
sClean = remEchos_mex(s);

%% Make sure we have at least one trade first
if ~isempty(find(sClean,1))  
    [~,~,~,r] = calcProfitLoss([fOpen fClose],sClean,bigPoint,cost);
    sh = scaling*sharpe(r,0);
else
    % No signal so no return or sharpe.
    r = zeros(length(price),1);
    sh = 0;
end; %if


%% Return values
    for i = 1:nargout
        switch i
            case 1
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
                warning('maRsiSIG:OutputArg',...
                    'Too many output arguments requested, ignoring last ones');
        end %switch
    end %for