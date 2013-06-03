function [ tLine ] = iTrend_v2(price,iMult,qMult)
%ITREND_v2 An elemental calculation of the instantaneous trend
%   ITREND_v2 returns the instantaneous trend as a simple average over the dominant cycle
%   This is based on the work of John Ehlers
%
%   T = ITREND_V2(PRICE) returns the trendline over the dominant cycle
%
%   [T] = ITREND_V2(PRICE) returns the trendline T
%
%   PRICE is ordinarily (H+L)/2 but can be any simple array
%
% Author: Mark Tompkins

%% MEX code to be skipped
coder.extrinsic('exist');

%% Error check
rows = size(price,1);

if rows < 55
    error('iTrend_v2:dataSizeFailure','iTrend_v2 requires a minimum of 55 observations. Exiting.');
end;

%% Defaults

if ~exist('iMult','var'), iMult = .635; end;
if ~exist('qMult','var'), qMult = .338; end;

%% Preallocation
deltaPhase = zeros(rows,1);
inPhase = zeros(rows,1);
instPeriod = zeros(rows,1);
period = zeros(rows,1);
phase = zeros(rows,1);
quad = zeros(rows,1);
tLine = zeros(rows,1);
value3 = zeros(rows,1);
value4 = zeros(rows,1);
value5 = zeros(rows,1);

%% {Detrend Price}
for ii = 8:rows
    value3(ii) = price(ii) - price(ii-7);
end; %for

%% {Compute InPhase and Quadrature components}
for ii = 5:rows
    inPhase(ii) = 1.25*value3(ii-4) - iMult*value3(ii-2) + iMult*inPhase(ii-3);
    quad(ii) = value3(ii-2) - qMult*value3(ii) + qMult*quad(ii-2);
end; %for

%% {Use ArcTangent to compute the current phase}
for ii = 2:rows
    if abs(inPhase(ii) + inPhase(ii-1)) > 0
        phase(ii) = atand(abs((quad(ii)+quad(ii-1))/(inPhase(ii)+inPhase(ii-1))));
    end; %if
    % % % {Resolve the ArcTangent ambiguity}
    if inPhase(ii) < 0 && quad(ii) > 0, phase(ii) = 180 - phase(ii); end;
    if inPhase(ii) < 0 && quad(ii) < 0, phase(ii) = 180 + phase(ii); end;
    if inPhase(ii) > 0 && quad(ii) < 0, phase(ii) = 360 - phase(ii); end;
end; %for

%% {Compute a differential phase, resolve phase wraparound, and limit delta phase errors}
for ii = 2:rows
    deltaPhase(ii) = phase(ii-1) - phase(ii);
    if phase(ii-1) < 90 && phase(ii) > 270, deltaPhase(ii) = 360 + phase(ii-1) - phase(ii); end;
    if deltaPhase(ii) < 1, deltaPhase(ii) = 1; end;
    if deltaPhase(ii) > 60, deltaPhase(ii) = 60; end;
end; %for

%% {Sum DeltaPhases to reach 360 degrees. The sum is the instantaneous period.}
for ii = 41:rows
    for jj=0:40
        value4(ii) = value4(ii) + deltaPhase(ii-jj);
        if value4(ii) > 360 && instPeriod(ii) == 0
            instPeriod(ii) = jj;
        end; %if
    end; %for jj
    %% {Resolve Instantaneous Period errors and smooth}
    if instPeriod(ii) == 0, instPeriod(ii) = instPeriod(ii-1); end;
    value5(ii) = (.25 * instPeriod(ii)) + (.75 * value5(ii-1));

    %% {Compute Trendline as simple average over the measured dominant cycle period}
    period(ii) = fix(value5(ii));
    for jj = 0:period(ii)-1
        tLine(ii) = tLine(ii) + price(ii-jj);
    end; %for jj
    if period(ii) > 0, tLine(ii) = tLine(ii) / period(ii); end;
    if rows < 26
        tLine(ii) = price(ii);
    end; %if
    
end; %for ii
    tLine(1:40)=price(1:40);
end

