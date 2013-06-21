function [SIG, R, SH, SNR, LEAD, LAG] = maSnrSIG(price,maF,maS,typeMA,snrThresh,snrEffect,bigPoint,cost,scaling)
%MASNRSIG 2 inputs MA signal generation with a Signal To Noise Ratio based filter
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
%

%% MEX code to be skipped
coder.extrinsic('sharpe','calcProfitLoss','remEchos_mex','ma2inputsSIG_mex','OHLCSplitter','snr_mex')

% Preallocate so we can MEX
rows = size(price,1);
fOpen = zeros(rows,1);                  %#ok<NASGU>
fClose = zeros(rows,1);                 %#ok<NASGU>
LAG = zeros(rows,1);                    %#ok<NASGU>
SIG = zeros(rows,1);                    %#ok<NASGU>
SNR = zeros(rows,1);                   	%#ok<NASGU>
LEAD = zeros(rows,1);                   %#ok<NASGU>
R = zeros(rows,1);
SH = zeros(rows,1);                     %#ok<NASGU>

%% Parse
[fOpen,fClose] = OHLCSplitter(price);

%% Generate signal
[SIG,~,~,LEAD,LAG] = ma2inputsSIG_mex(price,maF,maS,typeMA,bigPoint,cost,scaling);

%% Measure SNR
% Defaults: iMult = .635, qMult = .338
SNR = snr_mex(price,.635,.338);

% Drop signals less than the SNR threshhold
if snrEffect == 0
    % Remove these signals
    SIG(SNR < snrThresh) = 0;
    % Reverse signals less than the SNR threshhold
elseif snrEffect == 1
    % Reverse these signals
    for ii = 1:rows
        if SNR(ii) < snrThresh
            SIG(ii) = SIG(ii) * -1;
        end; %if
    end; %for
else
    error('MASNR:inputArgs','Cannot interpret an input of snrThresh = %d',snrThresh);
end; %if

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
%   Revision:      4920.26064
%   Copyright:     (c)2013
%

