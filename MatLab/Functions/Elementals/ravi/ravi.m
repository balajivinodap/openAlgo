function [ ind ] = ravi(price,lead,lag,D,M)
%RAVI A measurement which attempts to detect a change between trending and ranging states
%
%	From the work of Tushar S. Chande, PhD. and cited in his book "Beyond Technical Analysis"
%	the Ravi calculation is a measurement of current market "trendiness".  A value greater than
%	3% is claimed to be a measurement of when the market is in a trending phase.  Values less
%	than 3% are a ranging phase.
%
%   We call ATR to normalize price data in the ravi function so we need O | H | L | C as price input.
%
%   INPUTS:     price       	An array of price in the form [O | H | L | C]     
%               lead            Lookback the fast period harmonic mean used to calculate Ravi 
%               lag             Lookback the slow period harmonic mean used to calculate Ravi 
%               D               Detrender option:
%                                   0	-	Ravi (default)
%                                   1	-	ATR
%               M               Mean ravi shift used to calibrate the returned vector.
%                               	e.g.	M = 10 	the mean of the RAVI vector is set to 10%
%
%	OUTPUTS:	ind				RAVI vector
%
%	RAVI(PRICE)				Returns a graph of the 'ravi.m' function with default values.
%	RAVI(PRICE,...)			Returns a graph of the 'ravi.m' function with declared values.
%
%   ind = RAVI(PRICE) 		returns a RAVI vector with default values:
%								Lead	5
%								Lag		65
%								D		0
%								M		20
%

%% MEX code to be skipped
coder.extrinsic('exist','slidefun','atr_mex');

%% Preallocate
rows = size(price,1);
raviF = zeros(rows,1);                          %#ok<NASGU>
raviS = zeros(rows,1);                          %#ok<NASGU>
ind = zeros(rows,1);                            %#ok<NASGU>
indAvg = zeros(rows,1);                        	%#ok<NASGU>

if size(price,2) < 4
    error('RAVI:tooFewInputs', ...
        'We call ATR to normalize price data therefore we need O | H | L | C as price input. Exiting.');
end; %if

fClose = OHLCSplitter(price);

% Removed 'exist' checks for mex
if M < 1
    error('RAVI:inputArgs','Lookback M must be a postive integer. Aborting');
end; %if

if nargin == 2
    warning('RAVI:numInputs',...
        'Defaulting the Lag lookback period to 65.  Use two inputs to avoid this message.');
end; % Present a warning about the number of inputs

raviF = slidefun('harmmean',lead,fClose,'backward');
raviS = slidefun('harmmean',lag,fClose,'backward');

% Determine divisor for measuring the rate of change
if D == 0
    ind = (abs(raviF-raviS)./raviS);
elseif D == 1
    ind = (abs(raviF-raviS)./atr_mex(price));
else
    error('RAVI:inputArg','Unknown input in value ''D''. Aborting.');
end; %if

indAvg = mean(ind);

% Normalize data to a range of 0 - 100.
% Normalize using assumption that 20% or lessrepresents the amount of time we are either consolodating or
% can't tell.  This value should be thought about and revisited.  It may also be a good number to submit
% to a parametric sweep for improvement.
norm = M / indAvg;
ind = ind * norm;

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
%   Revision:      4917.14168
%   Copyright:     (c)2013
%


