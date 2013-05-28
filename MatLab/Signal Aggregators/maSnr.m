function varargout = maSnr(price,maF,maS,typeMA,snrThresh,snrEffect,scaling,cost,bigPoint)
%MASNR 2 inputs MA signal generation with a Signal To Noise Ratio based filter
%   MA is a classic 2 input crossover signal generator
%   SNR is an indicator that measures the dominant cycle's signal to noise ratio.
%       The SNR is measured in decibels and uses a logarithmic scale
%   
%   By using an SNR filter on a MA signal, we should have a noticable improvement on the
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
%   snrThresh:      SNR threshold to calibrate when the dominant cycle is clear enough (6db is default)
%   snrEffect:      SNR effects
%                   	Effect 0:   Drop signals less than the SNR threshhold
%                       Effect 1:   Reverse signals that are generated when SNR is less than the threshold

%% Defaults
if ~exist('maF','var'), maF = 12; end;
if ~exist('maS','var'), maS = 26; end;
if ~exist('typeMA','var'), typeMA=0; end;
if ~exist('snrThresh','var'), snrThresh = 6; end;
if ~exist('snrEffect','var'), snrEffect = 0; end;
if ~exist('scaling','var'), scaling = 1; end;
if ~exist('cost','var'), cost = 0; end;         % default cost
if ~exist('bigPoint','var'), bigPoint = 1; end; % default bigPoint

%% Parse
[fOpen,fClose] = OHLCSplitter(price);

%% Generate signal
[sma,~,~,lead,lag] = movAvg2inputsMEX_mex(price,maF,maS,typeMA,scaling,cost,bigPoint);
s = sma;

%% Measure SNR
% Defaults: iMult = .635, qMult = .338
snr = snrMEX_mex(price,.635,.338);

% Index period where the SNR is below the threshold
indSnr = snr < snrThresh == 1;

% Drop signals less than the SNR threshhold
if snrEffect == 0
    % Remove these signals
    s(indSnr) = 0;
% Reverse signals less than the SNR threshhold
elseif snrEffect == 1
    % Reverse these signals
    s(indSnr) = s(indSnr) * -1;
else
    error('MASNR:inputArgs','Cannot interpret an input of snrThresh = %d',snrThresh);
end; %if

% Normalize signals to +/-2
s = s * 2;

% Drop any repeats
sClean = remEchosMEX_mex(s);

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

    [~,~,~,r] = calcProfitLossCPP([fOpen fClose],sClean,bigPoint,cost);

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
    movAvg2inputs(price,maF,maS,typeMA,scaling,cost,bigPoint,hSub);

    ax(1) = subplot(6,2,[2 4]);
    plot([price,lead,lag]);
    axis (ax(1),'tight');
    grid on
    legend('Price',['Lead ',num2str(maF)],['Lag ',num2str(maS)],'Location', 'NorthWest')
    title(['MA+SNR Results, Sharpe Ratio = ',num2str(sh,3)])
    
    ax(2) = subplot(6,2,[6 8]);
    plot([snr,snrThresh*ones(size(snr))])
    grid on
    legend(['SNR Thresh ',num2str(snrThresh),'db'],'Location', 'North')
    title('SNR')
    
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
                varargout{4} = snr; % ravi signal
            case 5
                varargout{5} = lead; % moving average lead
            case 6
                varargout{5} = lag; % moving average lag
            otherwise
                warning('MASNR:OutputArg',...
                    'Too many output arguments requested, ignoring last ones');
        end %switch
    end %for
end; %if



