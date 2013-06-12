function [ varargout ] = iTrendSIG_DIS(price,bigPoint,cost,scaling,hSub)
%ITRENDSIGDIS An indicator based on the work of John Elhers
%   instantaneousTrend returns a trading signal for a given iTrend and moving average crossover
%
%   Input 'price' should be of an O | H | L | C form as we use the average of the Open & Close
%   when passed to iTrend.m
%
%   S = ITRENDSIGDIS(PRICE) returns a trading signal based upon a 14-period
%   iTrend and a Closing price (~ 1 day average).
%
%   S = ITRENDSIGDIS(PRICE,I,T) returns a trading signal for a I-period iTrend and
%   a T-period simple moving average.
%
%   [S,R,SH,ITREND,MA] = ITRENDSIGDIS(...)
%           S       derived trading signal
%           R       absolute return in R
%           SH      derived Sharpe based on R
%           ITREND  iTrend as calculated with a call to iTrend.m
%           MA
%

%% Error check
rows = size(price,1);
if rows < 55
    error('iTrend2inputs:dataSizeFailure','iTrend2inputs requires a minimum of 55 observations. Exiting.');
end;

%% Defaults
if ~exist('scaling','var'), scaling = 1; end;
if ~exist('cost','var'), cost = 0; end;         % default cost
if ~exist('bigPoint','var'), bigPoint = 1; end; % default bigPoint

%% Parse
[fOpen,fHigh,fLow,fClose] = OHLCSplitter(price);
HighLow = (fHigh+fLow)/2;

%% iTrend signal generation using dominant cycle crossing
if nargin > 0
    %% Preallocate
    returns = zeros(rows,1);                                 %#ok<NASGU>
    s = zeros(rows,1);
    
    [tLine,instT] = iTrend_mex(HighLow);
    
    s(instT>tLine) = 	1.5;
    s(instT<tLine) =   -1.5;
    
    % Clear erroneous signals calculated prior to enough data
    s(1:54) = 0;
    
    % Set the first position to 1 lot
    % Make sure we have at least one trade first
    if ~isempty(find(s,1))
        % We have to remove Echos while they are all 2's
        % Clean up repeating information
        s = remEchos_mex(s);
        
        [~,~,~,returns] = calcProfitLoss([fOpen fClose],s,bigPoint,cost);
        sharpeRatio=scaling*sharpe(returns,0);
    else
        returns = 0;
        sharpeRatio= 0;
    end; %if
    
    %% If no assignment to variable, show the averages in a chart
    if (nargout == 0) && (~exist('hSub','var'))% Plot
        
        % Plot results
        ax(1) = subplot(2,1,1);
        plot([fClose,tLine,instT]);
        axis (ax(1),'tight');
        grid on
        legend('Close','iTrend','instT','Location','NorthWest')
        title(['iTrend Results, Annual Sharpe Ratio = ',num2str(sharpeRatio,3)])
        
        ax(2) = subplot(2,1,2);
        plot([s,cumsum(returns)]); grid on
        legend('Position','Cumulative Return','Location','North')
        title(['Final Return = ',thousandSepCash(sum(returns))])
        linkaxes(ax,'x')
        
    elseif (nargout == 0) && exist('hSub','var')% Plot as subplot
        % We pass hSub as a string so we can have asymmetrical graphs
        % The call to char() parses the passed cell array
        ax(1) = subplot(str2num(char(hSub(1))), str2num(char(hSub(2))), str2num(char(hSub(3)))); %#ok<ST2NM>
        plot([fClose,tLine,instT]);
        axis (ax(1),'tight');
        grid on
        legend('Close','iTrend','instT','Location','NorthWest')
        title(['iTrend Results, Annual Sharpe Ratio = ',num2str(sharpeRatio,3)])
        
        ax(2) = subplot(str2num(char(hSub(1))),str2num(char(hSub(2))), str2num(char(hSub(4)))); %#ok<ST2NM>
        plot([s,cumsum(returns)]); grid on
        legend('Position','Cumulative Return','Location','North')
        title(['Final Return = ',thousandSepCash(sum(returns))])
        linkaxes(ax,'x')
    else
        for i = 1:nargout
            switch i
                case 1
                    varargout{1} = s;
                case 2
                    varargout{2} = returns;
                case 3
                    varargout{3} = sharpeRatio;
                case 4
                    varargout{4} = tLine;
                case 5
                    varargout{5} = instT;
                otherwise
                    warning('ITREND2INPUTS:OutputArg',...
                        'Too many output arguments requested, ignoring last ones');
            end %switch
        end %for
    end %if
    
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
%   Revision:      4906.24976
%   Copyright:     (c)2013
%


