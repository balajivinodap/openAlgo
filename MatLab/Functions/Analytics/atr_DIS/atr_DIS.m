function varargout = atr_DIS(price,M,hSub)
%ATR_DIS Average True Range
%
%	The average true range attempts to modify price data in such a way that it 'corrects'
%	various gaps that may otherwise be evident.  This had a significantly higher degree
%	of relevance when markets were not electronic and had many hours between sessions.
%	The primary modifier of the data is smoothed by an exponential average of elemental
%	bar components High, Low, and Close.
%
%   INPUTS:     price       	n array of any [C] or [O | C] or [O | H | L | C]
%				M				Exponential moving average lookback period (default 20)
%				hSub			An embedded variable passed from 'DIS' files for poisitioning graphical feedback
%
%	OUTPUTS:	ATR				Average true range vector
%
%   atr_DIS = ATR(PRICE)        returns the average true range using a default lookback
%                               of 20 observations
%   
%   atr_DIS = ATR(PRICE,M)      returns the average true range using a a specified
%                               lookback of 'M' observations when M is an integer > 0
%   
%	atr_DIS = ATR(PRICE,M,hSub) returns the average true range using a a specified
%                               lookback of 'M' observations while allowing the parameters
%                               of hSub to be passed for graphing

%% Error check

%% Validate inputs
if ~exist('M','var')
    M = 20;                   % default ema lookback
elseif M < 1
    warning('Invalid input %d for ATR lookback. Value should be an integer where M > 0. \nUsing default of M = 20');
    M = 20;
end; %if

fClose = OHLCSplitter(price);
        
%% Average true range
atr = atr_mex(price, M);                     	% '-1' calls to exponential calculation
        
%% Plot if requested
if nargout == 0 && (~exist('hSub','var'))		% Plot
	% Center plot window basis monitor (single monitor calculation)
    scrsz = get(0,'ScreenSize');
    figure('Position',[scrsz(3)*.15 scrsz(4)*.15 scrsz(3)*.7 scrsz(4)*.7])
    
    ax(1) = subplot(2,1,1);
    plot(fClose);
    axis (ax(1),'tight');
    grid on
    legend('Price','Location', 'NorthWest')
    title('Price')
    
    ax(2) = subplot(2,1,2);
    plot(atr)
    grid on
    legend('ATR Bars ',num2str(M),'Location', 'Best')
    title('Average True Range')
    linkaxes(ax,'x')
    
elseif (nargout == 0) && exist('hSub','var')% Plot as subplot
    % We pass hSub as a string so we can have asymmetrical graphs
	% The call to char() parses the passed cell array
    ax(1) = subplot(str2num(char(hSub(1))),str2num(char(hSub(2))),str2num(char(hSub(3)))); %#ok<ST2NM>
    plot(fClose);
    axis (ax(1),'tight');
    grid on
    legend('Price','Location', 'NorthWest')
    title('Price')
    set(gca,'xticklabel',{})
    
    ax(2) = subplot(str2num(char(hSub(1))),str2num(char(hSub(2))),str2num(char(hSub(4)))); %#ok<ST2NM>
    plot(atr)
    grid on
    legend('ATR Bars ',num2str(M),'Location', 'Best')
    title('Average True Range')
    linkaxes(ax,'x')
else
    %% Return values
    for ii = 1:nargout
        switch ii
            case 1
                varargout{1} = atr; % atr
            otherwise
                warning('ATR:OutputArg',...
                    'Too many output arguments requested, ignoring last ones');
        end %switch
    end %for
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
%   Revision:      4917.13438
%   Copyright:     (c)2013
%
