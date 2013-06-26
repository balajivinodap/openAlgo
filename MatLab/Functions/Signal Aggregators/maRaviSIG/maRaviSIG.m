function [SIG, R, SH, RAV, LEAD, LAG] = maRaviSIG(price,maF,maS,typeMA,raviF,raviS,raviD,raviM,raviE,raviThresh,bigPoint,cost,scaling)
%MARAVISIG 2 inputs MA signal generation with a RAVI based transformer
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

%% MEX code to be skipped
coder.extrinsic('sharpe','calcProfitLoss','remEchos_mex','ma2inputsSIG_mex','OHLCSplitter','ravi_mex')

%% Preallocate so we can MEX
rows = size(price,1);
fOpen = zeros(rows,1);                  %#ok<NASGU>
fClose = zeros(rows,1);                 %#ok<NASGU>
SIG = zeros(rows,1);                    %#ok<NASGU>
STA = zeros(rows,1);                    %#ok<NASGU>
LEAD = zeros(rows,1);                 	%#ok<NASGU>
LAG = zeros(rows,1);                    %#ok<NASGU>
RAV = zeros(rows,1);                    %#ok<NASGU>
R = zeros(rows,1);

[fOpen,fClose] = OHLCSplitter(price);

[STA, LEAD, LAG] = ma2inputsSTA_mex(fClose,maF,maS,typeMA);

% Convert state to signal
SIG(STA < 0) = -1.5;
SIG(STA > 0) =  1.5;

% Clear erroneous signals calculated prior to enough data
SIG(1:maS-1) = 0;

RAV = ravi_mex(price,raviF,raviS,raviD,raviM);

% RAVI is used to filter signals that are not considered trending.
% These would be signals that occur when RAVI < raviThresh
% Effect 0: Remove the signal in trending markets
% Effect 1: Remove the signal in ranging markets
% Effect 2: Reverse the signal in trending markets
% Effect 3: Reverse the signal in ranging markets
if raviE == 0
    % Remove these signals
    for ii=1:rows
        if RAV(ii) > raviThresh, SIG(ii) = 0; end; %if
    end; %for
elseif raviE == 1
    % Remove these signals
    for ii=1:rows
        if RAV(ii) < raviThresh, SIG(ii) = 0; end; %if
    end; %for
elseif raviE == 2
    % Reverse thse signals
    for ii=1:rows
        if RAV(ii) > raviThresh, SIG(ii) = SIG(ii) * -1; end; %if
    end; %for
elseif raviE == 3
    for ii=1:rows
        if RAV(ii) < raviThresh, SIG(ii) = SIG(ii) * -1; end; %if
    end; %for
else
    error('MARAVISIG:inputArgs','Cannot interpret an input of raviE = %d',raviE);
end;

% Drop any repeats
SIG = remEchos_mex(SIG);

% Make sure we have at least one trade first
if ~isempty(find(SIG,1))
    [~,~,~,R] = calcProfitLoss([fOpen fClose],SIG,bigPoint,cost);
    SH = scaling*sharpe(R,0);
else
    % No signal so no return or sharpe.
    SH = 0;
end; %if

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
%   Revision:      4925.32157
%   Copyright:     (c)2013
%


