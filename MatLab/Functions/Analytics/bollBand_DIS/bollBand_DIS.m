function varargout = bollBand_DIS(price,period,maType,devUp,devDwn,hSub)
%BOLLBAND_DIS Bollinger Bands
%   bollBand_DIS (Bollinger Bands) are a technical analysis tool invented by John Bollinger in the 1980s,
%   and a term trademarked by him in 2011. Having evolved from the concept of trading bands,
%   Bollinger Bands and the related indicators %b and bandwidth can be used to measure the
%   "highness" or "lowness" of the price relative to previous trades.
%   Bollinger Bands are a volatility indicator similar to the Keltner channel. ~Wikipedia
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
%   INPUTS:     price		An array of price
%               period      Lookback period (default 20)
%               maType
%               devUp       Number of upward standard deviations (default 2)
%               devDwn      Number of downward standard deviations (default -2)
%               hSub			An embedded variable passed from 'DIS' files for poisitioning graphical feedback
%
%	OUTPUTS		lBand		Lower Bollinger band    (MA - Kstd)
%				mBand		Midline average         (MA)
%               uBand       Upper Bollinger band    (MA + Kstd)

%% Defaults
if ~exist('period','var')
    period = 20;
end; %if

if ~exist('maType','var')
    maType = 0;
end; %if

if ~exist('devUp','var')
    devUp = 2;
end; %if

if ~exist('devDwn','var')
    devDwn = 2;
end;

fClose = OHLCSplitter(price);

%% Bollinger Band
[lBand, mAvg, uBand] = bollBand_mex(fClose, period, maType, devUp, devDwn);

%% Plot if requested
%% If no assignment to variable, show the averages in a chart
if (nargout == 0) && (~exist('hSub','var'))% Plot
    
    % Plot results
    %ax(1) = subplot(2,1,1);
    plot([fClose,uBand,mAvg,lBand]);
    %axis (ax(1),'tight');
    grid on
    legend('Close',['Upper ',num2str(devUp),'\sigma'],['Midline ',num2str(period),' Type ',num2str(maType)],['Lower ',num2str(devDwn),'\sigma'],'Location','NorthWest')
    title('Bollinger Band')
    
elseif (nargout == 0) && exist('hSub','var')% Plot as subplot
    % We pass hSub as a string so we can have asymmetrical graphs
    % The call to char() parses the passed cell array
    ax(1) = subplot(str2num(char(hSub(1))), str2num(char(hSub(2))), str2num(char(hSub(3)))); %#ok<ST2NM>
    plot([fClose,uBand,lBand]);
    axis (ax(1),'tight');
    grid on
    legend('Close',['uBand ',num2str(devUp)],['lBand ',num2str(devDwn)],'Location','NorthWest')
    title('Bollinger Band')
else
    for i = 1:nargout
        switch i
            case 1
                varargout{1} = lBand;
            case 2
                varargout{2} = mAvg;
            case 3
                varargout{3} = uBand;
            otherwise
                warning('BOLLBAND_DIS:OutputArg','Too many output arguments requested, ignoring last ones');
        end %switch
    end %for
end %if
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
%   Revision:      4919.21996
%   Copyright:     (c)2013
%
