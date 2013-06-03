function varargout = maRaviSIG_DIS(price,maF,maS,typeMA,raviF,raviS,raviD,raviM,raviE,raviThresh,scaling,cost,bigPoint)
%MARAVISIG_DIS 2 inputs MA signal generation with a RAVI based transformer
%   MA is a classic 2 input crossover signal generator
%   RAVI is an indicator that indicates whether a market is in a ranging or a trending phase.
%   
%   By using a RAVI transformer on an RSI signal, we should have a noticable improvement on the
%   resulting performance.
%
%   This produces a logically valid signal output
%
%   maF:            MA lead lookback period
%   maS:            MA lag lookback period
%   typeMA:         MA type from the following selection
%                       -4  Trimmed
%                       -3  Harmonic
%                       -2  Geometric
%                       -1	Exponential
%                        0  Simple  (default)
%                       >0  Weighted e.g. 0.5 Square root weighted, 1 = linear, 2 = square weighted
%   raviF:          RAVI lead moving average lookback period (default = 5)
%   raviS:          RAVI lag moving average lookback period  (default = 65)
%   raviD:          RAVI denominator (0: MA (default)   1: ATR)
%   raviM:          RAVI mean shift (default = 20)
%   raviE:          RAVI effects
%                       Effect 0: Remove the signal in trending markets (default = 0)
%                       Effect 1: Remove the signal in ranging markets
%                       Effect 2: Reverse the signal in trending markets
%                       Effect 3: Reverse the signal in ranging markets
%   raviThresh:     RAVI threshold where the market changes from Ranging to Trending (default UNKNOWN, using 20)
%                   We are uncertain of a good raviThresh reading.  We need to sweep for this and update
%                   with our findings.  Recommended sweep is similar to RSI percentage [10:5:40]
%
%
%   We should expect performance to be better in trending phases compared to ranging
%
% Author:           Mark Tompkins
% Revision:			4902.23830

%% NEED TO ADD ERROR CHECKING OF INPUTS
%% Defaults
if ~exist('maF','var'), maF = 12; end;
if ~exist('maS','var'), maS = 26; end;
if ~exist('typeMA','var'), typeMA=0; end;
if ~exist('raviF','var'), raviF = 5; end;
if ~exist('raviS','var'), raviS = 65; end;
if ~exist('raviD','var'), raviD = 0; end;
if ~exist('raviM','var'), raviM = 20; end;
if ~exist('raviE','var'), raviE = 0; end;
if ~exist('raviThresh','var'), raviThresh = 20; end;  
if ~exist('scaling','var'), scaling = 1; end;
if ~exist('cost','var'), cost = 0; end;         % default cost
if ~exist('bigPoint','var'), bigPoint = 1; end; % default bigPoint

[fOpen,fClose] = OHLCSplitter(price);

[sma,~,~,lead,lag] = ma2inputsSIG_mex(price,maF,maS,typeMA,scaling,cost,bigPoint);
rav = ravi(price,raviF,raviS,raviD,raviM);

s = sma;

% RAVI is used to filter signals that are not considered trending.
% These would be signals that occur when RAVI < raviThresh
% Effect 0: Remove the signal in trending markets
% Effect 1: Remove the signal in ranging markets
% Effect 2: Reverse the signal in trending markets
% Effect 3: Reverse the signal in ranging markets
if raviE == 0
    % Index period where market is in a trending phase
    indRavi = rav > raviThresh == 1;   
    % Remove these signals
    s(indRavi) = 0;
elseif raviE == 1
    % Index period where market is in a ranging phase
    indRavi = rav < raviThresh == 1;   
    % Remove these signals
    s(indRavi) = 0;
elseif raviE == 2
    % Index period where market is in a trending phase
    indRavi = rav > raviThresh == 1;
    % Reverse thse signals
    s(indRavi) = s(indRavi) * -1;
elseif raviE == 3
    % Index period where market is in a ranging phase
    indRavi = rav < raviThresh == 1;
    s(indRavi) = s(indRavi) * -1;
else
    error('MARAVI:inputArge','Cannot interpret an input of raviE = %d',raviE);
end;

% Normalize signals to +/-2
s = s * 2;

% Drop any repeats
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
    layout = ['6     ';'2     ';'1 3 5 ';'7 9 11'];
    hSub = cellstr(layout);
    ma2inputsSIG_DIS(price,maF,maS,typeMA,scaling,cost,bigPoint,hSub);

    ax(1) = subplot(6,2,[2 4]);
    plot([price,lead,lag]);
    axis (ax(1),'tight');
    grid on
    legend('Price',['Lead ',num2str(maF)],['Lag ',num2str(maS)],'Location', 'NorthWest')
    title(['MA+RAVI Results, Sharpe Ratio = ',num2str(sh,3)])
    
    ax(2) = subplot(6,2,[6 8]);
    ylim([0 100])
    hold on
    plot([rav,raviThresh*ones(size(rav))])
    grid on
    legend(['RAVI Thresh ',num2str(raviThresh),'%'],'Location', 'North')
    title('RAVI')
    
    ax(3) = subplot(6,2,[10 12]);
    plot([sClean,cumsum(r)]), grid on
    legend('Position','Cumulative Return','Location', 'North')
    title(['Final Return = ',thousandSepCash(sum(r))])
    
    linkaxes(ax,'x')

else
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
                varargout{4} = rav; % ravi signal
            case 5
                varargout{5} = lead; % moving average lead
            case 6
                varargout{5} = lag; % moving average lag
            otherwise
                warning('MARAVI:OutputArg',...
                    'Too many output arguments requested, ignoring last ones');
        end %switch
    end %for
end
