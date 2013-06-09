function [short,long] = movAvg(asset,lead,lag,alpha)
%MOVAVG Leading and lagging moving averages chart.
%   [SHORT,LONG] = MOVAVG(ASSET,LEAD,LAG,ALPHA) plots leading and lagging
%   moving averages.  ASSET is the security data, LEAD is the number of
%   samples to use in leading average calculation, and LAG is the number
%   of samples to use in the lagging average calculation.  ALPHA is the
%   control parameter which determines what type of moving averages are
%   calculated.  ALPHA = 0 (default) corresponds to a simple moving average,
%   ALPHA = 0.5 to a square root weighted moving average, ALPHA = 1
%   to a linear moving average, ALPHA = 2 to a square weighted moving
%   average, etc.
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
%   MOVAVG(ASSET,3,20,1) plots linear 3 sample leading and 20 sample
%   lagging moving averages.
%
%   [SHORT,LONG] = MOVAVG(ASSET,3,20,1) returns the leading and lagging
%   average data without plotting it.
%
%   See also BOLLING, HIGHLOW, CANDLE, POINTFIG.
%

%% MEX code to be skipped
coder.extrinsic('slidefun','tsmovavg');

%% Error check
if nargin < 4
    alpha = 0; % Default is simple moving average
end
if nargin < 3
    error('METS:movAvg:missingInputs','At least 3 inputs must be provided');
end

[m,n] = size(asset);

if m > 1 && n > 1
    error('METS:movAvg:invalidInputSize','Only the vector for calculation should be provided as input.');
end

if lead > lag
    error('METS:movAvg:badLeadInput','Lead value must be less than or equal to the lag value');
end

asset = asset(:);

r = length(asset);

if lead < 1 || lag < 1
    error('METS:movAvg:badLeadLagInput', 'Lead and lag values must both be greater than zero');
end

if lead > r || lag > r
    error('METS:movAvg:badLeadLagInput', ...
        'Lead (%d) and lag (%d) values must not be greater than data range (%d)',lead,lag,r);
end;

%% Preallocation
a = zeros(r,1);
b = zeros(r,1);

%% Calculation
if alpha < 0
    if alpha == -1
        % calculate smoothing constant (alpha)
        alphas = 2/(lead+1);
        alphal = 2/(lag+1);
        
        % first exponential average is first price
        a(1) = asset(1);
        b(1) = asset(1);
        
        % For large matrices of input data, FOR loops are more efficient
        % than vectorization.
        
        % lagging & leading average
        for j = 2:r
            a(j) = a(j-1) + alphal*(asset(j) - a(j-1));
            b(j) = b(j-1) + alphas*(asset(j) - b(j-1));
        end
    elseif alpha == -2
        b = slidefun('geomean',lead,asset(:,1),'backward');
        a = slidefun('geomean',lag,asset(:,1),'backward');
    elseif alpha == -3
        b = slidefun('harmmean',lead,asset(:,1),'backward');
        a = slidefun('harmmean',lag,asset(:,1),'backward');
    elseif alpha == -4
        % We have defaulted a 10% trimmean value
        b = slidefun('trimmean',lead,asset(:,1),'backward',10);
        a = slidefun('trimmean',lag,asset(:,1),'backward',10);
    elseif alpha == -5
        % tsmovavg expects row input. The '1' advised it is given a column vector
        b = tsmovavg(asset(:,1), 't', lead, 1);
        a = tsmovavg(asset(:,1), 't', lag, 1);
    else
        % Unhandled average type.  Provide error feedback
        error('This type of average calculation is currently unhandled or known.  Exiting.');
    end; %if
    % We were given a numeric input
else
    % Preallocate
    wa = zeros(lag,1);
    wb = zeros(lead,1);
    % compute general moving average (ie simple, linear, etc)
    % build weighting vectors
    ii = 1:lag;
    wa(ii) = (lag - ii + 1).^alpha./sum((1:lag).^alpha);
    ii = 1:lead;
    wb(ii) = (lead - ii + 1) .^alpha/sum((1:lead).^alpha);
    % build moving average vectors by filtering asset through weights
    a = filter(wa,1,asset);
    b = filter(wb,1,asset);
end

if nargout == 0
    % If no output arguments, cannot plot from a MEX
    warning('Cannot generate a plot from within a MEX file.  Change code to use ''movavg''\n');
else
    % output data to workspace
    short = b;
    long = a;
end

%%
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
%   The public sharing of this code does not reliquish, reduce, restrict or
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
%   Author:	Mark Tompkins
%   Revision:	4906.24976
%   Copyright:	(c)2013
%
