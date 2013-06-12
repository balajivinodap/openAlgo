function state = wprSTA(price,N,thresh)
%WPRSTA returns a logical STATE for from the 'willpctr.m' function by The MathWorks, Inc.
% WPRSTA returns a logical STATE for from the 'willpctr.m' function by The MathWorks, Inc.
% which is a value that is above/below an upper/lower threshold intended to locate
% overbought and oversold conditions.
% N serves as an optional lookback period (default 14 observations)
%   NOTES: It is important to consider that an RSI STATE really has 3 states.
%            1      Above upper threshold is overbought
%           -1      Below lower threshold is oversold
%            0      There is also a neutral region between thresholds and 50%
%
%           WPR is normally used with stocks where there is a natural upward price bias
%

%% MEX code to be skipped
coder.extrinsic('OHLCSplitter','willpctr')

% WPR works with negative values in a range from 0 to -100;
if numel(thresh) == 1 % scalar value
    thresh = [(100-abs(thresh))*-1, abs(thresh)*-1];
else
    thresh = abs(thresh)*-1;
end;
if thresh(1) < thresh(2)
    thresh= thresh(2:-1:1);
end;

if thresh(1) > 0, thresh(1) = thresh(1) * -1; end;
if thresh(2) > 0, thresh(2) = thresh(2) * -1; end;

% Preallocate so we can MEX
rows = size(price,1);
fClose = zeros(rows,1);                                     %#ok<NASGU>
fHigh = zeros(rows,1);                                      %#ok<NASGU>
fLow = zeros(rows,1);                                       %#ok<NASGU>
state = zeros(rows,1);
w = zeros(rows,1);                                          %#ok<NASGU>

if size(price,2) == 4
    [~, fHigh, fLow, fClose] = OHLCSplitter(price);
else
    error('wprMETS:InputArg',...
        'We need as input O | H | L | C.');
end; %if

%% williams %r
w = willpctr([fHigh fLow fClose],N);

%% generate signal
% Crossing the upper threshold (overbought)
indx = w > thresh(1);
state(indx) = -1;

indx = w < thresh(2);
state(indx) = 1;

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

