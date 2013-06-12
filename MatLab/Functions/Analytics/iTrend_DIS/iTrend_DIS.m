function [ varargout ] = iTrend_DIS(price,hSub)
%ITREND_DIS An indicator based on the work of John Elhers
%   ITREND_DIS returns an instantaneous trend and a tLine value.
%
%   Input 'price' should be of an O | H | L | C form as we use the average
%   of the Open & Close when passed to iTrend.m
%
%   [iTrend, instT] = ITRENDSIGDIS(PRICE)       returns an instantaneous trend and
%                                               an accompanying trendline as simple
%                                               average over the measured dominant
%                                               cycle period based upon a 14-period iTrend.
%
%   [iTrend, instT] = ITRENDSIGDIS(PRICE,hSub)  includes the hSub variable for asymetrical
%                                               graphic output

%% Error check
rows = size(price,1);
if rows < 55
    error('iTrend2inputs:dataSizeFailure','iTrend2inputs requires a minimum of 55 observations. Exiting.');
end;

%% Parse
[~,fHigh,fLow,fClose] = OHLCSplitter(price);
HighLow = (fHigh+fLow)/2;


%% iTrend signal generation using dominant cycle crossing
if nargin > 0
    %% Preallocate
    
    [tLine,instT] = iTrend_mex(HighLow);
    
    %% If no assignment to variable, show the averages in a chart
    if (nargout == 0) && (~exist('hSub','var'))% Plot
        
        % Plot results
        plot([fClose,tLine,instT]);
        grid on
        legend('Close','tLine','instT','Location','NorthWest')
        title('Instantaneous Trend')
        
    elseif (nargout == 0) && exist('hSub','var')% Plot as subplot
        % We pass hSub as a string so we can have asymmetrical graphs
        % The call to char() parses the passed cell array
        ax(1) = subplot(str2num(char(hSub(1))), str2num(char(hSub(2))), str2num(char(hSub(3)))); %#ok<ST2NM>
        plot([fClose,tLine,instT]);
        axis (ax(1),'tight');
        grid on
        legend('Close','tLine','instT','Location','NorthWest')
        title('Instantaneous Trend')
        
    else
        for i = 1:nargout
            switch i
                case 1
                    varargout{1} = tLine;
                case 2
                    varargout{2} = instT;
                otherwise
                    warning('ITREND_DIS:OutputArg',...
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

