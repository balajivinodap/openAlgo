function [ tLine,iTrend ] = iTrend(price)
%ITREND An elemental calculation of the instantaneous trend
%   ITREND returns the instantaneous trend as a simple average over the dominant cycle
%   This is based on the work of John Ehlers.
%
%	Input 'price' should be an M x 1 array of prices ordinarily transformed e.g. (H + L)/2,
%	but can be any logical vector array such as (H+L+C)/3
%
%   INPUTS:     price		An M x 1array of pre-transformed price
%
%	OUTPUTS		tLine		Vector output of the dominant cycle
%				iTrend		Vector output of the instantaneous trend
%
%   T = ITREND(PRICE) returns the trendline over the dominant cycle
%
%   [T,I] = ITREND(PRICE) returns the trendline T
%

%% Error check
rows = size(price,1);

if rows < 55
    error('iTrend:dataSizeFailure','iTrend requires a minimum of 55 observations. Exiting.');
end;

%% Preallocation
deltaPhase = zeros(rows,1);
inPhase = zeros(rows,1);
instPeriod = zeros(rows,1);
iTrend = zeros(rows,1);
period = zeros(rows,1);
phase = zeros(rows,1);
quad = zeros(rows,1);
tLine = zeros(rows,1);
value1 = zeros(rows,1);
value2 = zeros(rows,1);
value3 = zeros(rows,1);
value4 = zeros(rows,1);
value5 = zeros(rows,1);


%% {Compute InPhase and Quadrature components}

for ii = 7:rows
    value1(ii) = price(ii) - price(ii-6);
end; %for

for ii = 4:rows
    value2(ii) = value1(ii-3);
    inPhase(ii) = (.33 * value2(ii)) + (.67 * inPhase(ii-1));
end; %for

for ii = 7:rows
    value3(ii) = (.75 * (value1(ii) - value1(ii-6))) + ...
        (.25 * (value1(ii-2) - value1(ii-4)));
    quad(ii) = (.2 * value3(ii)) + (.8 * quad(ii-1));
end;

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
    for jj = 0:period(ii)+1
        tLine(ii) = tLine(ii) + price(ii-jj);
    end; %for jj
    if period(ii) > 0, tLine(ii) = tLine(ii) / (period(ii)+2); end;
    iTrend(ii) = (.33 * (price(ii) + (.5 * (price(ii)-price(ii-3))))) + ...
        (.67 * iTrend(ii-1));
    if rows < 26
        tLine(ii) = price(ii);
        iTrend(ii) = price(ii);
    end; %if
    
end; %for ii

tLine(1:40)=price(1:40);
iTrend(1:54)=price(1:54);

%%
%   -------------------------------------------------------------------------
%                                  _    _ 
%         ___  _ __   ___ _ __    / \  | | __ _  ___   ___  _ __ __ _ 
%        / _ \| '_ \ / _ \ '_ \  / _ \ | |/ _` |/ _ \ / _ \| '__/ _` |
%       | (_) | |_) |  __/ | | |/ ___ \| | (_| | (_) | (_) | | | (_| |
%        \___/| .__/ \___|_| |_/_/   \_\_|\__, |\___(_)___/|_|  \__, |
%             |_|                         |___/                 |___/
%   -------------------------------------------------------------------------
%        This code is distributed in the hope that it will be useful,
%
%                      	   WITHOUT ANY WARRANTY
%
%                  WITHOUT CLAIM AS TO MERCHANTABILITY
%
%                  OR FITNESS FOR A PARTICULAR PURPOSE
%
%                          expressed or implied.
%
%   Use of this code, pseudocode, algorithmic or trading logic contained
%   herein, whether sound or faulty for any purpose is the sole
%   responsibility of the USER. Any such use of these algorithms, coding
%   logic or concepts in whole or in part carry no covenant of correctness
%   or recommended usage from the AUTHOR or any of the possible
%   contributors listed or unlisted, known or unknown.
%
%   Any reference of this code or to this code including any variants from
%   this code, or any other credits due this AUTHOR from this code shall be
%   clearly and unambiguously cited and evident during any use, whether in
%   whole or in part.
%
%   The public sharing of this code does not relinquish, reduce, restrict or
%   encumber any rights the AUTHOR has in respect to claims of intellectual
%   property.
%
%   IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
%   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
%   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
%   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
%   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
%   STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
%   ANY WAY OUT OF THE USE OF THIS SOFTWARE, CODE, OR CODE FRAGMENT(S), EVEN
%   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
%
%   -------------------------------------------------------------------------
%
%                             ALL RIGHTS RESERVED
%
%   -------------------------------------------------------------------------
%
%   Author:        Mark Tompkins
%   Revision:      4917.14811
%   Copyright:     (c)2013
%


