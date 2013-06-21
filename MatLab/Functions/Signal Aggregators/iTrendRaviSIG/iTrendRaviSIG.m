function [SIG, R, SH, iSTA, RAV] = iTrendRaviSIG(price,raviF,raviS,raviD,raviM,raviE,raviThresh,bigPoint,cost,scaling)
%ITRENDRAVISIG_DIS iTrend signal generation with a RAVI based transformer
%   iTrend is dominant cycle signal generate based on the work of John Ehlers
%
%   RAVI is an indicator that shows whether a market is in a ranging or a trending phase.
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

%% MEX code to be skipped
coder.extrinsic('sharpe','calcProfitLoss','remEchos_mex','ravi_mex','OHLCSplitter','iTrendSTA_mex')

% Preallocate so we can MEX
rows = size(price,1);
fOpen = zeros(rows,1);                  %#ok<NASGU>
fHigh = zeros(rows,1);                  %#ok<NASGU>
fLow = zeros(rows,1);                   %#ok<NASGU>
fClose = zeros(rows,1);                 %#ok<NASGU>
highLow = zeros(rows,1);                %#ok<NASGU>
indRavi = zeros(rows,1);                %#ok<NASGU>
iSTA = zeros(rows,1);                   %#ok<NASGU>
SIG = zeros(rows,1);
RAV = zeros(rows,1);                    %#ok<NASGU>
R = zeros(rows,1);

[fOpen,fHigh,fLow,fClose] = OHLCSplitter(price);
highLow = (fHigh + fLow) / 2;

iSTA = iTrendSTA_mex(highLow);

% Convert state to signal
SIG(iSTA < 0) = -1.5;
SIG(iSTA > 0) =  1.5;

RAV = ravi_mex(price,raviF,raviS,raviD,raviM);

% RAVI is used to filter signals that are not considered trending.
% These would be signals that occur when RAVI < raviThresh
% Effect 0: Remove the signal in trending markets;
if raviE == 0
    % Remove these signals
    for ii=1:rows
        if RAV(ii) > raviThresh, SIG(ii) = 0; end; %if
    end; %for
% Effect 1: This was a reverse but has since been changed to undefined
elseif raviE == 1
    % Remove these signals
    for ii=1:rows
        if RAV(ii) < raviThresh, SIG(ii) = 0; end; %if
    end; %for
elseif raviE == 2
    % Index period where market is in a trending phase
    % Reverse thse signals
    for ii=1:rows
        if RAV(ii) > raviThresh, SIG(ii) = SIG(ii) * -1; end; %if
    end; %for
elseif raviE == 3
    % Index period where market is in a ranging phase
    for ii=1:rows
        if RAV(ii) < raviThresh, SIG(ii) = SIG(ii) * -1; end; %if
    end; %for
else
    error('RSIRAVI:inputArge','Cannot interpret an input of raviE = %d',raviE);
end;

if ~isempty(find(SIG,1))
    % Drop any repeats
    SIG = remEchos_mex(SIG);
    
    [~,~,~,R] = calcProfitLoss([fOpen fClose],SIG,bigPoint,cost);
    
    SH = scaling*sharpe(R,0);
else
    % No signal no sharpe.
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
%   Revision:      4906.24976
%   Copyright:     (c)2013
%

