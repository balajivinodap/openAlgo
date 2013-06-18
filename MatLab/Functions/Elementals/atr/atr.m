function atr = atr(price,M)
%ATR Average True Range
%
%	The average true range attempts to modify price data in such a way that it 'corrects'
%	various gaps that may otherwise be evident.  This had a significantly higher degree
%	of relevance when markets were not electronic and had many hours between sessions.
%	The primary modifier of the data is smoothed by an exponential average of elemental
%	bar components High, Low, and Close.
%
%   INPUTS:     price       	An array of any [C] or [O | C] or [O | H | L | C]
%				M				Exponential moving average lookback period (default 20)
%
%	OUTPUTS:	ATR				Average true range vector
%
%   atr = ATR(PRICE,M) 			returns the average true range using a a specified
%					 			lookback of 'M' observations when M is an integer > 0
%

%% MEX code to be skipped
coder.extrinsic('OHLCSplitter', 'movAvg_mex');

% Parse data
if size(price,2) < 4
    error('ATR:priceDimensions','We need O | H | L | C as an input.  Aborting.');
end; %if

if M < 1
    error('ATR:observations','The lookback must be such that M >= 1.  Aborting.');
end; %if

rows = size(price,1);

%% Preallocation
h_m_l = zeros(rows,1);  %#ok<NASGU>
h_m_c = zeros(rows,1);  %#ok<NASGU>
l_m_c = zeros(rows,1);  %#ok<NASGU>
fHigh = zeros(rows,1);  %#ok<NASGU>
fLow = zeros(rows,1);   %#ok<NASGU>
fClose = zeros(rows,1); %#ok<NASGU>
tr = zeros(rows,1);     %#ok<NASGU>

[~,fHigh,fLow,fClose] = OHLCSplitter(price);

%% True range
h_m_l = fHigh-fLow;                                 % high - low
h_m_c = [0;abs(fHigh(2:end)-fClose(1:end-1))];      % abs(high - close)
l_m_c = [0;abs(fLow(2:end)-fClose(1:end-1))];       % abs(low - close)
tr = max([h_m_l,h_m_c,l_m_c],[],2);                 % true range

%% Average true range
atr = movAvg_mex(tr,M,M,-1);                        % '-1' calls to exponential calculation

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
%   Revision:      4917.14572
%   Copyright:     (c)2013
%






