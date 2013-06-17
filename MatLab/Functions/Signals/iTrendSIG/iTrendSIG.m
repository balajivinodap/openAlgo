function [SIG,R,SH,ITREND,TLINE] = iTrendSIG(price,bigPoint,cost,scaling)
%ITRENDSIG Returns a trading signal based on the work of John Elhers
%   iTrendSIG returns a trading signal for a given iTrend and dominant cycle crossover
%
%   Input 'price' should be of an O | H | L | C form as we use the average of the High & Low
%   when passed to iTrend.m
%
%   [S,R,SH,ITREND,MA] = ITRENDSIG(...)
%           S       derived trading signal
%           R       absolute return in R
%           SH      derived Sharpe based on R
%           ITREND  iTrend as calculated with a call to iTrend.m
%           MA
%

%% MEX code to be skipped
coder.extrinsic('sharpe','calcProfitLoss','remEchos_mex','iTrendSTA_mex','OHLCSplitter')

% Preallocate so we can MEX
rows = size(price,1);
fOpen = zeros(rows,1);               	%#ok<NASGU>
fClose = zeros(rows,1); 				%#ok<NASGU>
fHigh = zeros(rows,1);               	
fLow = zeros(rows,1);                   
R = zeros(rows,1);						
SIG = zeros(rows,1);					
STA = zeros(rows,1);					%#ok<NASGU>
TLINE = zeros(rows,1);                	%#ok<NASGU>
ITREND = zeros(rows,1);               	%#ok<NASGU>

%% Error check
if rows < 55
    error('iTrend2inputs:dataSizeFailure',...
		'iTrend2inputs requires a minimum of 55 observations. Exiting.');
end;

%% Parse
[fOpen,fClose] = OHLCSplitter(price);

% Price passed to iTrend
highsLows = (fHigh + fLow) / 2;

%% iTrend signal generation using dominant cycle crossing
[STA, TLINE, ITREND] = iTrendSTA_mex(highsLows);
    
% Convert state to signal
SIG(STA < 0) = -1.5;
SIG(STA > 0) =  1.5;
    
% Clear erroneous signals calculated prior to enough data
SIG(1:54) = 0;
	
if(~isempty(find(SIG,1)))
	% Clean up repeating information for PNL
	SIG = remEchos_mex(SIG);
		
	% Generate PNL
	[~,~,~,R] = calcProfitLoss([fOpen fClose],SIG,bigPoint,cost);
		
	% Calculate sharpe ratio
	SH=scaling*sharpe(R,0);
else
    % No signals - no sharpe.
    SH= 0;
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
%   Revision:      4916.33715
%   Copyright:     (c)2013
%


