function [SIG, R, SH] = maRsiSIG(price,N,M,typeMA,Mrsi,thresh,typeRSI,isSignal,bigPoint,cost,scaling)
% MA+RSI in function call
%%
%
%   isSignal:   0 - Filter (default)    We can either combine the signals from
%               1 - Signal              each element, MA + RSI or we can use
%                                       the RSI as a filter condition for MA.

%% MEX code to be skipped
coder.extrinsic('sharpe','calcProfitLoss','remEchos_mex','ma2inputsSTA_mex','OHLCSplitter','rsiSTA_mex')

% Preallocate so we can MEX
rows = size(price,1);
fOpen = zeros(rows,1);                  %#ok<NASGU>
fClose = zeros(rows,1);                 %#ok<NASGU>
SIG = zeros(rows,1);                    
R = zeros(rows,1);
staMA = zeros(rows,1);                  %#ok<NASGU>
staRsi = zeros(rows,1);             	%#ok<NASGU>

if length(Mrsi) == 1
    Mrsi = [15*Mrsi Mrsi];
end

[fOpen,fClose] = OHLCSplitter(price);

staMA = ma2inputsSTA_mex(price,N,M,typeMA);
% NOTE: rsiSTA returns a 1 when oversold and -1 when overbought
staRsi = rsiSTA_mex(price,Mrsi,thresh,typeRSI);

%%  The RSI is either used as a signal generator or a filter condition for another signal
%   If we are using it to generate a signal, we should return only an actionable signal with no repeats
%   If we are using it as a filter, we should return the state of Overbought | Oversold including repeats
%   For this specific 'marsiMETS' case, we combine it with a simple moving average

%% Use RSI as FILTER
if isSignal == 0
    % Aggregate the two states
    SIG = (staMA + staRsi);
    
    % Any instance where the |sum| of the 2 signals is ~= 2 means both conditions are not met
    % Drop those instances
    SIG(abs(SIG)~=2) = 0;
    
    % Refine to a signal
    SIG = sign(SIG) * 1.5;
    
    %% Use RSI as SIGNAL
elseif isSignal == 1
    % Aggregate the two signals normalizing them to +/- 1.5
    SIG = sign(staMA + staRsi) * 1.5;
end; %if

%% Drop any repeats for PNL
SIG = remEchos_mex(SIG);

%% Make sure we have at least one trade first
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
%   Revision:      4925.31406
%   Copyright:     (c)2013
%

