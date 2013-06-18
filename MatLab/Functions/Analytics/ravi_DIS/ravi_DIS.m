function [ varargout ] = ravi_DIS(price,lead,lag,D,M,hSub)
%RAVI_DIS A measurement which attempts to detect a change between trending and ranging states
%
%	From the work of Tushar S. Chande, PhD. and cited in his book "Beyond Technical Analysis"
%	the Ravi calculation is a measurement of current market "trendiness".  A value greater than
%	3% is claimed to be a measurement of when the market is in a trending phase.  Values less
%	than 3% are a ranging phase. 
%
%	DIS		This function calls the elemental function 'ravi.m' for calculationand can provide a graphical response.
%
%	INPUTS
%			PRICE	We call ATR to normalize price data in the ravi function so we need O | H | L | C as price input.
%			LEAD	Observation period for the fast period harmonic mean used to calculate Ravi measurement
%			LAG		Observation period for the slow period harmonic mean used to calculate Ravi measurement
%			D		Detrender option:
%						0	-	Ravi (default)
%						1	-	ATR 
%			M		Mean ravi shift used to calibrate the returned vector.
%						e.g.	M = 10 	the mean of the RAVI vector is set to 10%
%			HSUB	String used to manipulate the graphing of the Ravi vector
%
%	RAVI_DIS(PRICE)				Returns a graph of the 'ravi.m' function with default values.
%	RAVI_DIS(PRICE,...)			Returns a graph of the 'ravi.m' function with declared values.
%
%   ind = RAVI_DIS(PRICE) 		returns a RAVI vector with default values:
%									Lead	5
%									Lag		65
%									D		0
%									M		20				

if size(price,2) < 4
    error('ravi_DIS:tooFewInputs', ...
            'We call ATR to normalize price data therefore we need O | H | L | C as price input. Exiting.');
end; %if

ind=ravi_mex(price,lead,lag,D,M);

% Determine divisor for measuring the rate of change
if D == 0
    div = ' Slow MA';
elseif D == 1
    div = ' ATR';
else
    error('ravi_DIS:inputArg','Unknown input in value ''D''. Aborting.');
end;

%% Plot if requested
if nargout == 0 && (~exist('hSub','var'))% Plot
    
    ax(1) = subplot(2,1,1);
    plot(fClose);
    axis (ax(1),'tight');
    grid on
    title('Price')
    
    plotTitle = strcat('Normalized RAVI - Using  ',div);
    
    ax(2) = subplot(2,1,2);
    ylim([0 100])
    axis manual;
    hold on
    plot(ind);
    axis (ax(1),'tight');
    grid on
    title(plotTitle)
    linkaxes(ax,'x')
    
elseif (nargout == 0) && exist('hSub','var')% Plot as subplot
	% We pass hSub as a string so we can have asymmetrical graphs
	% The call to char() parses the passed cell array
    plotTitle = strcat('Normalized RAVI - Using  ',div);
    
    ax(1) = subplot(str2num(char(hSub(1))),str2num(char(hSub(2))),str2num(char(hSub(3)))); %#ok<ST2NM>
	axis manual;
    plot(ind);
    axis (ax(1),'tight');
    ylim([0 100]);
    hold on
    grid on
    title(plotTitle)
else    
    for ii = 1:nargout
        switch ii
            case 1
                varargout{1} = ind; 
            otherwise
                warning('ravi_DIS:OutputArg',...
                    'Too many output arguments requested, ignoring last ones');
        end; %switch
    end; %for
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
%   Revision:      4917.14168
%   Copyright:     (c)2013
%


