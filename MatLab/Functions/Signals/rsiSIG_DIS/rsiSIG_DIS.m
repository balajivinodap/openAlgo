function varargout = rsiSIG_DIS(price,M,thresh,type,scaling,cost,bigPoint,hSub)
%RSISIG_DIS RSI signal generator from rsiSIG_mex with graphical feedback
% RSISIG_DIS RSI signal generator from rsiSIG_mex with graphical feedback
% RSISIG trading strategy.  Note that the trading signal is generated when the
% RSISIG value is above/below the upper/lower threshold.
% M serves some detrending function which we must investigate

%   NOTE: It is important to consider that an RSI signal generator really has 3 states.
%           Above Threshold is Overbought
%           Below Threshold is Oversold
%           There is also a neutral region between +/- Threshold and 50%
%
%   This should be considered prior to adding or removing any Echos to this signal.
%   We are defining this as an elementary signal and echos that are produced should be
%   passed out to any function call.  For calculating a direct PNL, the signal should first
%   be cleaned with remEchoMEX_mex.
%
%   Signals with Echos represent a market state, not a signal.
%
%   For RSI in oversold state, "we should be long".
%   Removing the Echos from rsiSTA produces that buy signal.
%

%% Defaults and parsing
if ~exist('scaling','var'), scaling = 1; end;
if ~exist('type','var'), type=0; end;
if ~exist('cost','var'), cost = 0; end;         % default cost
if ~exist('bigPoint','var'), bigPoint = 1; end; % default bigPoint

if ~exist('thresh','var')
    thresh = [30 70]; % default threshold
else
    if numel(thresh) == 1 % scalar value
        thresh = [100-thresh, thresh];
    else
        if thresh(1) > thresh(2)
            thresh= thresh(2:-1:1);
        end
    end
end

if ~exist('M','var')
    M = 0; % no detrending
    N = 14; % default value for rsi calc
    % We can't exceed the lookback for the RSI Detrender
elseif M > size(price,1)
    M = size(price,1);
    warning('Detrender reduced to match number of observations.');
elseif numel(M) > 1
    N = M(1);
    if M(2) < 0
        M = 15 * N;
    else
        M = M(2);
    end; % if
else
    % M is the detrend average
    % It would appear we are taking a multiple of M below
    % to capture a longer moving average to detrend
    N = M(1);
    M = 15*M(1);
end

[fClose] = OHLCSplitter(price);
[s,r,sh,ri,ma,thresh] = rsiSIG_mex(price,[N M],thresh,type,scaling,cost,bigPoint);

%% Plot if requested
if nargout == 0 && (~exist('hSub','var'))% Plot
    ax(1) = subplot(3,1,1);
    if M == 0
        ma = price;
    end; %if
    plot([fClose,ma]);
    axis (ax(1),'tight');
    grid on
    legend('Price',['Detrender ',num2str(M)],'Location', 'NorthWest')
    title(['RSI Results, Sharpe Ratio = ',num2str(sh,3)])
    
    ax(2) = subplot(3,1,2);
    ylim([0 100])
    axis manual;
    hold on;
    plot([ri,thresh(2)*ones(size(ri)),thresh(1)*ones(size(ri))])
    grid on
    legend(['RSI Bars ',num2str(N)],['RSI Upper ',num2str(thresh(2)),'%'],...
        ['RSI Lower ',num2str(thresh(1)),'%'],'Location', 'North')
    title('RSI')
    
    ax(3) = subplot(3,1,3);
    plot([s,cumsum(r)]), grid on
    legend('Position','Cumulative Return','Location','North')
    title(['Final Return = ',thousandSepCash(sum(r))])
    linkaxes(ax,'x')
    
elseif (nargout == 0) && exist('hSub','var')% Plot as subplot
    % We pass hSub as a string so we can have asymmetrical graphs
    % The call to char() parses the passed cell array
    ax(1) = subplot(str2num(char(hSub(1))),str2num(char(hSub(2))),str2num(char(hSub(3)))); %#ok<ST2NM>
    if M == 0
        ma = price;
    end; %if
    plot([fClose,ma]);
    axis (ax(1),'tight');
    grid on
    legend('Price',['Detrender ',num2str(M)],'Location', 'NorthWest')
    title(['RSI Results, Sharpe Ratio = ',num2str(sh,3)])
    
    ax(2) = subplot(str2num(char(hSub(1))),str2num(char(hSub(2))),str2num(char(hSub(4)))); %#ok<ST2NM>
    ylim([0 100])
    axis manual;
    hold on;
    plot([ri,thresh(2)*ones(size(ri)),thresh(1)*ones(size(ri))])
    grid on
    legend(['RSI Bars ',num2str(N)],['RSI Upper ',num2str(thresh(2)),'%'],...
        ['RSI Lower ',num2str(thresh(1)),'%'],'Location', 'North')
    title('RSI')
    
    ax(3) = subplot(str2num(char(hSub(1))),str2num(char(hSub(2))),str2num(char(hSub(5)))); %#ok<ST2NM>
    plot([s,cumsum(r)]), grid on
    legend('Position','Cumulative Return','Location','North')
    title(['Final Return = ',thousandSepCash(sum(r))])
    linkaxes(ax,'x')
else
    %% Return values
    for ii = 1:nargout
        switch ii
            case 1
                varargout{1} = s; % signal
            case 2
                varargout{2} = r; % return (pnl)
            case 3
                varargout{3} = sh; % sharpe ratio
            case 4
                varargout{4} = ri; % rsi signal
            case 5
                varargout{5} = ma; % moving average
            case 6
                varargout{6} = thresh; % threshold
            otherwise
                warning('RSI:OutputArg',...
                    'Too many output arguments requested, ignoring last ones');
        end %switch
    end %for
end; %if

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
