function varargout = ma2inputsNumTicksPftSIG_DIS(price,F,S,typeMA,...
                                             minTick,numTicks,openAvg,...
                                             bigPoint,cost,scaling,hSub)
%MA2INPUTSNUMTICKSPFTSIG_DIS returns a trading signal for a simple lead/lag indicator with achieved profits
%   %ma2inputsNumTicksPftSIG_DIS returns a trading signal for a simple lead/lag indicator with achieved 
%   profits derived from 'ma2inputsSTA'
%
%   STA = MA2INPUTSSTA(PRICE, F, S, type) returns a STATE based upon provided lead (F) and lag (S) periods.
%   [barsOut,sigOut,sharpeOut] = numTicksProfit(barsIn,sigIn,sharpeIn,minTick,numTicks,openAvg)
%
%   INPUTS:     price       an array of any [C] or [O | C] or [O | H | L | C]
%               F           fast period
%               S           slow period
%               type        Available average types are:
%                           -5  Triangle (Double smoothed similar to Hull)
%                           -4  Trimmed
%                           -3  Harmonic
%                           -2  Geometric
%                           -1	Exponential
%                            0  Simple
%                          > 0  Weighted e.g. 0.5 Square root weighted, 1 = linear, 2 = square weighted
%               minTick     the minimum increment of the asset being analyzed
%               numTicks    the number of ticks to close a position profitably
%               openAvg     the manner in which to calculate profit targets:
%                            0  per contract (default)
%                            1  position average
%               scaling     sharpe ratio adjuster
%               cost        commission cost for P&L calculation per round turn
%               bigPoint    value of a full tick for P&L calculation
%               hSub        manipulated graphics output for asymetrical sub-graphing
%
%   OUTPUTS:
%               SIG        -1.5 LEAD crossed below LAG.  Sell 1 lot short (reverse if necessary)
%                            0  no action
%                           1.5 LEAD crossed above LAG.  Buy 1 lot long (reverse if necessary)
%                        +/- N  A profit of 'N' contracts was recorded offsetting existing position(s)
%                 R         Absolute return
%                SH         Sharpe ratio calculated using R and adjusted for virtual profit bars
%              LEAD         Lead vector output
%               LAG	        Lag vector output
%
% See also movavg, sharpe, macd, tsmovavg, ma2inputsSTA, ma2inputsSIG_DIS

if nargin == 2
    error('MA2INPUTSNUMTICKSPFTSIG_DIS:NoLagWindowDefined',...
        'When defining a LEAD value LAG value must also be given.')
end; %if

%% Process input args
if ~exist('typeMA', 'var')
    typeMA = 0;
end; %if

if ~exist('scaling','var')
    scaling = 1;
end; %if

if ~exist('cost', 'var')
    cost = 0;
end; %if

if ~exist('bigPoint', 'var')
    bigPoint = 1;
end; %if

if nargin < 2
    % default values - often used in MACD
    S = 26;
    F = 12;
elseif nargin < 3
    error('ma2inputsNumTicksPftSIG_DIS:NoLagWindowDefined',...
        'When defining a LEAD value LAG value must also be given.')
end %if

rows = size(price,1);

%% Input with error check
if (F > S)
    error('ma2inputsNumTicksPftSIG_DIS:invalidInputs', ...
        'LEAD input > LAG input. Catch this before submitting to ''ma2inputsSIG''');
end; %if

if (F > rows) || (S > rows)
    error ('ma2inputsNumTicksPftSIG_DIS:invalidInputs', ...
        'Lookback is greater than the number of observations (%d)',rows);
end; %if

if (minTick < 0)
    error('ma2inputsNumTicksPftSIG_DIS:invalidInputs', ...
        '''minTick'' input was not a positive value. Aborting.');
end; %if

if (numTicks < 0 || floor(numTicks)~= numTicks)
    error('ma2inputsNumTicksPftSIG_DIS:invalidInputs', ...
        '''numTicks'' input was not an integer value >= 0. Aborting.');
end; %if

[barsOut,sigOut,R,SH,LEAD,LAG] = ma2inputsNumTicksPftSIG(price,F,S,typeMA,...
                                             minTick,numTicks,openAvg,...
                                             bigPoint,cost,scaling);

fClose = OHLCSplitter(barsOut);

%% If no assignment to variable, show the averages in a chart
if (nargout == 0) && (~exist('hSub','var'))% Plot
    
    % Plot results
    ax(1) = subplot(2,1,1);
    plot(fClose);
    axis (ax(1),'tight');
    grid on
    legend('Close','Location','NorthWest')
    title(['MA w/ Profits, Annual Sharpe Ratio = ',num2str(SH,3)])
    
    ax(2) = subplot(2,1,2);
    plot([sigOut,cumsum(R)]); grid on
    legend('Position','Cumulative Return','Location','North')
    title(['Lead:',num2str(F),'   Lag:',num2str(S),'   Pft Ticks:',num2str(numTicks),'      Final Return = ',thousandSepCash(sum(R))])
    linkaxes(ax,'x')
    
elseif (nargout == 0) && exist('hSub','var')% Plot as subplot
    % We pass hSub as a string so we can have asymmetrical graphs
    % The call to char() parses the passed cell array
    ax(1) = subplot(str2num(char(hSub(1))), str2num(char(hSub(2))), str2num(char(hSub(3)))); %#ok<ST2NM>
    plot(fClose);
    axis (ax(1),'tight');
    grid on
    legend('Close',['numTicks: ',num2str(numTicks)],'Location','NorthWest')
    title(['MA w/ Profits, Annual Sharpe Ratio = ',num2str(SH,3)])
    
    ax(2) = subplot(str2num(char(hSub(1))),str2num(char(hSub(2))), str2num(char(hSub(4)))); %#ok<ST2NM>
    plot([SIG,cumsum(R)]); grid on
    legend('Position','Cumulative Return','Location','North')
    title(['F:',num2str(F),'   S:',num2str(S),'   Ticks:',num2str(numTicks),'      Final Return = ',thousandSepCash(sum(R))])
    linkaxes(ax,'x')
else
    for i = 1:nargout
        switch i
            case 1
                varargout{1} = barsOut;
            case 2
                varargout{2} = sigOut;
            case 3
                varargout{3} = R;
            case 4
                varargout{4} = SH;
            case 5
                varargout{5} = LEAD;
            case 6
                varargout{6} = LAG;
            otherwise
                warning('MA2INPUTSNUMTICKSPFTSIG_DIS:OutputArg',...
                        'Too many output arguments requested, ignoring last ones');
        end %switch
    end %for
end %if

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


