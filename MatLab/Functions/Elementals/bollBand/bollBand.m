function [lBand, mAvg, uBand] = bollBand(price, period, maType, devUp, devDwn)
%BOLLBAND An elemental calculation of the bollinger band
%   BOLLBAND (Bollinger Bands) are a technical analysis tool invented by John Bollinger in the 1980s, 
%   and a term trademarked by him in 2011. Having evolved from the concept of trading bands, 
%   Bollinger Bands and the related indicators %b and bandwidth can be used to measure the 
%   "highness" or "lowness" of the price relative to previous trades. 
%   Bollinger Bands are a volatility indicator similar to the Keltner channel. ~Wikipedia
%
%	Input 'price' should be an M x 1 array of prices ordinarily 'Close'
%
%   Available average types are:
%       -5  Triangle (Double smoothed similar to Hull)
%       -4  Trimmed  (10%)
%       -3  Harmonic
%       -2  Geometric
%       -1	Exponential
%        0  Simple
%       >0  Weighted e.g. 0.5 Square root weighted, 1 = linear, 2 = square weighted
%
%   INPUTS:     price		An M x 1 array of price
%               period      Lookback period (default 20)
%               devUp       Number of upward standard deviations (default 2)
%               devDwn      Number of downward standard deviations (default -2)
%
%	OUTPUTS		lBand		Lower Bollinger band    (MA - K?)
%				mBand		Midline average         (MA)
%               uBand       Upper Bollinger band    (MA + K?)

%% MEX code to be skipped
coder.extrinsic('movAvg_mex');

%% Error check
rows = size(price,1);

if (devDwn < 0)
    devDwn = devDwn * -1;
end; %if

%% Preallocation
lBand = nan(rows,1);                    %#ok<NASGU>
mAvg = nan(rows,1);                     %#ok<NASGU>
uBand = nan(rows,1);                    %#ok<NASGU>
stdAdj = zeros(rows,1);             	%#ok<NASGU>

stdAdj = std(price);

%BollingerBand = Average( Price, Length ) + NumDevs * StandardDev( Price, Length, 1 ) ;
mAvg = movAvg_mex(price, period, period, maType);

% Restore nans
mAvg(1:period) = NaN;

lBand = mAvg + (devUp * stdAdj);
uBand = mAvg - (devDwn * stdAdj);

end

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
%   Revision:      4919.19705
%   Copyright:     (c)2013
%
