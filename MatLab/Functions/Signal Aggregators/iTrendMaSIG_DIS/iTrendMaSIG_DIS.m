function varargout = iTrendMaSIG_DIS(price,M,typeMA,bigPoint,cost,scaling)
%ITRENDMASIG_DIS An indicator based on the work of John Elhers
%   ITRENDMASIG_DIS returns a trading signal for a given iTrend and MA cross as well as a
%   technical indicator.
%
%   S = ITRENDMASIG_DIS(PRICE) returns a trading signal based upon a 14-period
%   iTrend and a Closing price (~ 1 day average).
%   S is the trading signal of values -2, 0, 2 where -2 denotes
%   a sell (short reverse), 0 is neutral, and 2 is buy (long reverse).
%
%   S = ITRENDMASIG_DIS(PRICE,I,T) returns a trading signal for a I-period iTrend and
%   a T-period simple moving average.
%
%   [S,R,SH,ITREND,MA] = ITRENDMASIG_DIS(...) returns the trading signal S, the
%   absolute return in R, the Sharpe Ratio in SH calcualted using R, and
%   the ITREND or MA series.
%

%% Error check
rows = size(price,1);
if rows < 55
    error('iTrendMA:dataSizeFailure',...
        'iTrendMA requires a minimum of 55 observations. Exiting.');
end;

%% Defaults
if ~exist('M','var'), M = 14; end;
if ~exist('typeMA','var'), typeMA = 0; end;
if ~exist('scaling','var'), scaling = 1; end;
if ~exist('cost','var'), cost = 0; end;         % default cost
if ~exist('bigPoint','var'), bigPoint = 1; end; % default bigPoint

%% Parse
[fOpen,fHigh,fLow,fClose] = OHLCSplitter(price);
HighLow = (fHigh+fLow)/2;


%% iTrend signal generation using dominant cycle crossing
if nargin > 0
    %% Preallocate
    returns = zeros(rows,1);                                 
    s = zeros(rows,1);
    
    [tLine] = iTrend_mex(HighLow);
    [ma] = movAvg_mex(fClose,M,M,typeMA);
    
    ma(1:M)=fClose(1:M);
    
    % Create logical STATE conditions
    s(ma>tLine) = 1;
    s(ma<tLine) = -1;
    
    % Convert to SIGNAL
    s = s * 1.5;
    
    % Clear erroneous signals calculated prior to enough data
    % This is specific to iTrend calculations because of the nature of
    % cyclical analysis
    s(1:54) = 0;
    
    % Remove echos
    s = remEchos_mex(s);
    % Set the first position to 1 lot
    % Make sure we have at least one trade first
    if ~isempty(find(s,1))
        [~,~,~,returns] = calcProfitLoss([fOpen fClose],s,bigPoint,cost);
        sharpeRatio=scaling*sharpe(returns,0);
    else
        sharpeRatio= 0;
    end; %if
    
    %% If no assignment to variable, show the averages in a chart
    if (nargout == 0) && (~exist('hSub','var'))% Plot     
        % Center plot window basis monitor (single monitor calculation)
        scrsz = get(0,'ScreenSize');
        figure('Position',[scrsz(3)*.15 scrsz(4)*.15 scrsz(3)*.7 scrsz(4)*.7])
    
        % Plot results
        ax(1) = subplot(2,1,1);
        plot([fClose,tLine,ma]);
        axis (ax(1),'tight');
        grid on
        legend('Close','iTrend',['MA ',num2str(M)],'Location','NorthWest')
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
        plot([fClose,tLine,ma]);
        axis (ax(1),'tight');
        grid on
        legend('Close','iTrend',['MA ',num2str(M)],'Location','NorthWest')
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
                    varargout{5} = ma;
                otherwise
                    warning('ITRENDMA:OutputArg',...
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


