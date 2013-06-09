function varargout = iTrendRaviSIG_DIS(price,raviF,raviS,raviD,raviM,raviE,raviThresh,scaling,cost,bigPoint)
%ITRENDRAVISIG_DIS iTrend signal generation with a RAVI based transformer
%   iTrend is dominant cycle signal generate based on the work of John Ehlers
%
%   RAVI is an indicator that shows whether a market is in a ranging or a trending phase.
%   
%
%   This produces a logically valid signal output
%
%   raviF:          RAVI lead moving average period (default = 5)
%   raviS:          RAVI lag moving average period  (default = 65)
%   raviD:          RAVI denominator (0: MA (default)   1: ATR)
%   raviM:          RAVI mean shift (default = 20)
%   raviE:          RAVI effects
%                       Effect 0: Remove the signal in trending markets (default = 0)
%                       Effect 1: Remove the signal in ranging markets
%                       Effect 2: Reverse the signal in trending markets
%                       Effect 3: Reverse the signal in ranging markets
%   raviThresh:     RAVI threshold where the market changes from Ranging to Trending (default UNKNOWN)
%                   We are uncertain of a good raviThresh reading.  We need to sweep for this and update
%                   with our findings.  Recommended sweep is similar to RSI percentage [10:5:40]
%
% <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
% NEEEDS WORK
% <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
%% NEED TO ADD ERROR CHECKING OF INPUTS
%% Defaults
if ~exist('raviF','var'), raviF = 5; end;
if ~exist('raviS','var'), raviS = 65; end;
if ~exist('raviD','var'), raviD = 0; end;
if ~exist('raviM','var'), raviM = 20; end;
if ~exist('raviE','var'), raviE = 0; end;           
if ~exist('scaling','var'), scaling = 1; end;
if ~exist('cost','var'), cost = 0; end;         % default cost
if ~exist('bigPoint','var'), bigPoint = 1; end; % default bigPoint

[fOpen,fClose] = OHLCSplitter(price);

siTrend = iTrendSIG_DIS(price,scaling,cost,bigPoint);
rav = ravi(price,raviF,raviS,raviD,raviM);

s = siTrend;
    
% RAVI is used to filter signals that are not considered trending.
% These would be signals that occur when RAVI < raviThresh
% Effect 0: Remove the signal in trending markets;
if raviE == 0
    % Index period where market is in a trending phase
    indRavi = rav > raviThresh == 1;   
    % Remove these signals
    s(indRavi) = 0;
% Effect 1: This was a reverse but has since been changed to undefined
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
    error('RSIRAVI:inputArge','Cannot interpret an input of raviE = %d',raviE);
end;

% Normalize signals to +/-2
s = s * 2;

% Drop any repeats
sClean = remEchos_mex(s);

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
    end;

    [~,~,~,r] = calcProfitLoss([fOpen fClose],sClean,bigPoint,cost);

    sh = scaling*sharpe(r,0);
else
	% No signal so no return or sharpe.
    r = zeros(length(price),1);
    sh = 0;
end; %if

%% Plot if requested
if nargout == 0

    figure()
    % Not using MEX so we get a graphical response
    % Each element must be the same length - nonsense - thanks MatLab
    % http://www.mathworks.com/help/matlab/matlab_prog/cell-arrays-of-strings.html
    %layout = ['3';'2';'1';'3';'5'];
    layout = ['6     ';'2     ';'1 3 5 ';'7 9 11'];
    hSub = cellstr(layout);
    iTrendSIG_DIS(price,scaling,cost,bigPoint,hSub);
    
    %layout = ['3';'2';'4'];
    layout = ['6  ';'2  ';'6 8'];
    hSub = cellstr(layout);
    ravi_DIS(price,raviF,raviS,raviD,raviM,hSub);
    
    %ax(1) = subplot(3,2,2);
    ax(1) = subplot(6,2,[2 4]);
    plot(fClose);
    axis (ax(1),'tight');
    grid on
    legend('Price','Location', 'NorthWest')
    title(['iTrend & RAVI, Sharpe Ratio = ',num2str(sh,3)])
   
    %ax(2) = subplot(3,2,6);
    ax(2) = subplot(6,2,[10 12]);
    plot([sClean,cumsum(r)]), grid on
    legend('Position','Cumulative Return','Location','North')
    title(['Final Return = ',thousandSepCash(sum(r))])
    linkaxes(ax,'x')
else
    %% Return values
    for i = 1:nargout
        switch i
            case 1
                varargout{1} = sign(s); % signal 
            case 2
                varargout{2} = r; % return (pnl)
            case 3
                varargout{3} = sh; % sharpe ratio
            case 4
                varargout{4} = ri; % rsi signal
            case 5
                varargout{5} = rav; % RAVI
            otherwise
                warning('RSIRAVI:OutputArg',...
                    'Too many output arguments requested, ignoring last ones');
        end %switch
    end %for
end% if

end

