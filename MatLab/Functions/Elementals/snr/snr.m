function amp = snr(price,iMult,qMult)

%% MEX code to be skipped
coder.extrinsic('OHLCSplitter');

%% Error check
rows = size(price,1);
if rows < 8
    error('snr:dataSizeFailure','snr requires a minimum of 8 observations. Exiting.');
end;

%% Preallocation
inPhase = zeros(rows,1);
quad = zeros(rows,1);
amp = zeros(rows,1);
range = zeros(rows,1);
value1 = zeros(rows,1);
value2 = zeros(rows,1);
fHigh = zeros(rows,1);  %#ok<NASGU>
fLow = zeros(rows,1); %#ok<NASGU>

%% Parse
[~,fHigh,fLow,~] = OHLCSplitter(price);
HighLow = (fHigh+fLow)/2;

%% {Compute "Noise" as the average range}
for ii = 2:rows
    %% {Detrend Price}
    if ii >= 8
        value1(ii) = HighLow(ii) - HighLow(ii-7);
    end;
    range(ii) = .2*(fHigh(ii) - fLow(ii)) + .8*range(ii-1);
end; %for

%% {Compute Hilbert Transform outputs}
for ii = 5:rows
    inPhase(ii) = 1.25*value1(ii-4) - iMult*value1(ii-2) + iMult*inPhase(ii-3);
    quad(ii) = value1(ii-2) - qMult*value1(ii) + qMult*quad(ii-2);
end; %for

%% {Compute smoothed signal amplitude}
for ii = 2:rows
    value2(ii) = .2*(inPhase(ii)*inPhase(ii) + quad(ii)*quad(ii)) + .8*value2(ii-1);
    %% {Compute smoothed SNR in Decibels,...
    %  guarding against a divide by zero error, and compensating for filter loss}
    if value2(ii) < .001, value2(ii) = .001; end;   %prevent div by 0
    if range(ii) > 0
        amp(ii) = .25*(10*log(value2(ii)/(range(ii)*range(ii)))/log(10) + 1.9) + .75*amp(ii-1);
        if amp(ii) < 0, amp(ii) = 0; end;
    end; %if
end; %for

    


