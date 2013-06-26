function [SIG, R, SH, TLINE, MA] = iTrendMaSIG(price,M,typeMA,bigPoint,cost,scaling)
%ITRENDMASIG An indicator based on the work of John Elhers
%   ITRENDMASIG returns a trading signal for a given iTrend and MA cross as well as a
%   technical indicator.
%
%   SIG = ITRENDMASIG(PRICE) returns a trading signal based upon a 14-period
%   iTrend and a Closing price (~ 1 day average).
%   SIG is the trading signal of values -2, 0, 2 where -2 denotes
%   a sell (short reverse), 0 is neutral, and 2 is buy (long reverse).
%
%   SIG = ITRENDMASIG(PRICE,I,T) returns a trading signal for a I-period iTrend and
%   a T-period simple moving average.
%
%   [SIG,R,SH,ITREND,MA] = ITRENDMASIG(...) returns the trading signal S, the
%   absolute return in R, the Sharpe Ratio in SH calcualted using R, and
%   the ITREND or MA series.
%

%% MEX code to be skipped
coder.extrinsic('sharpe','calcProfitLoss','remEchos_mex','iTrend_mex','OHLCSplitter','movAvg_mex')

% Preallocate so we can MEX
rows = size(price,1);
fOpen = zeros(rows,1);                  %#ok<NASGU>
fHigh = zeros(rows,1);                  %#ok<NASGU>
fLow = zeros(rows,1);                   %#ok<NASGU>
fClose = zeros(rows,1);                 %#ok<NASGU>
highLow = zeros(rows,1);                %#ok<NASGU>
SIG = zeros(rows,1);
MA = zeros(rows,1);                    %#ok<NASGU>
TLINE = zeros(rows,1);                  %#ok<NASGU>
R = zeros(rows,1);

if rows < 55
    error('iTrendMA:dataSizeFailure',...
        'iTrendMA requires a minimum of 55 observations. Exiting.');
end;


%% Parse
[fOpen,fHigh,fLow,fClose] = OHLCSplitter(price);
HighLow = (fHigh+fLow)/2;

%% iTrend signal generation using dominant cycle crossing
TLINE = iTrend_mex(HighLow);
MA = movAvg_mex(fClose,M,M,typeMA);

MA(1:M)=fClose(1:M);

% Create logical STATE conditions
SIG(MA > TLINE) = 1;
SIG(MA < TLINE) = -1;

% Convert to SIGNAL
SIG = SIG * 1.5;

% Clear erroneous signals calculated prior to enough data
% This is specific to iTrend calculations because of the nature of
% cyclical analysis
SIG(1:54) = 0;

% Remove echos
SIG = remEchos_mex(SIG);
% Set the first position to 1 lot
% Make sure we have at least one trade first
if ~isempty(find(SIG,1))
    [~,~,~,R] = calcProfitLoss([fOpen fClose],SIG,bigPoint,cost);
    SH = scaling*sharpe(R,0);
else
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
%   Revision:      4920.23703
%   Copyright:     (c)2013
%


