function [varargout] = signalNoiseRatio_DIS(price,iMult,qMult,hSub)
%SIGNALNOISERATIO_DIS A measurement which attempts to detect a change between trending and ranging states
%
% From the work of John Ehlers.
% "[...] if we can estimate the signal amplitude relative to the market â€œnoiseâ€?, then we have a tool that
% estimates the quality of our technical analysis.  With the kind of data we have available to us, let us
% develop a unique definition of noise."
%
%			-- Edits been made in the balance of the description for clairty --
%
% "Consider a sampled signal as a sinewave with the sampling uncertainty as the high and low of each bar.
% The high and low is the uncertainty of each of our perfect sinewave sample points.
% We can make good trades as long as our Signal amplitude is much larger than the average daily range of the bars.
% When half the average daily range becomes equal to the signal amplitude making money on a trade becomes a crapshoot.
% Under this condition we could make an entry at the low of the bar containing the signal high and
% make an exit at the high of the bar containing the signal low for zero profit.  Therefore we will define
% the case where half the average daily trading range is equal to the signal amplitude as our Zero Decibel Signal
% to Noise (SNR) condition.  We want the signal amplitude to be at least twice the noise amplitude (6 dB SNR) to have
% a reasonable chance to make a profit from our analysis"
%
%	DIS		This function calls the elemental function 'snr.m' for calculationand can provide a graphical response.
%
%	INPUTS
%		PRICE	The 'snr.m' function expects O | H | L | C as price input. It must have a minimum of 8 observations.
%		MULT	Inphase multiplier 		(default 0.635)
%		QMULT	Quadrature multiplier	(default 0.338)
%		HSUB	String used to manipulate the graphing of the Ravi vector
%
%	SIGNALNOISERATIO_DIS(PRICE)			Returns a graph of the 'snr.m' function with default values.
%	SIGNALNOISERATIO_DIS(PRICE,...)		Returns a graph of the 'snr.m' function with declared values.
%
%   ind = SIGNALNOISERATIO_DIS(PRICE)	Returns a SNR vector with default values:
%									iMult	0.635
%									qMult	0.338

%% Error check
rows = size(price,1);
if rows < 8
    error('SIGNALNOISERATIO_DIS:dataSizeFailure','signalNoiseRatio_DIS requires a minimum of 8 observations. Exiting.');
end;

%% Defaults
if ~exist('iMult','var'), iMult=.635; end;
if ~exist('qMult','var'), qMult=.338; end;

amp = snr_mex(price,iMult,qMult);

%% Parse
[fClose] = OHLCSplitter(price);

%% If no assignment to variable, show the averages in a chart
if (nargout == 0) && (~exist('hSub','var'))% Plot
    
    % Plot results
    ax(1) = subplot(2,1,1);
    plot(fClose);
    axis (ax(1),'tight');
    grid on
    legend('Close','Location','NorthWest')
    title('Closing Price')
    
    ax(2) = subplot(2,1,2);
    plot(amp); grid on
    legend('SNR','Location','North')
    title('Signal To Noise Ratio')
    linkaxes(ax,'x')
    
elseif (nargout == 0) && exist('hSub','var')% Plot as subplot
    % We pass hSub as a string so we can have asymmetrical graphs
    % The call to char() parses the passed cell array
    ax(1) = subplot(str2num(char(hSub(1))), str2num(char(hSub(2))), str2num(char(hSub(3)))); %#ok<ST2NM>
    plot(fClose);
    axis (ax(1),'tight');
    grid on
    legend('Close','Location','NorthWest')
    title('Closing Price')
    
    ax(2) = subplot(str2num(char(hSub(1))),str2num(char(hSub(2))), str2num(char(hSub(4)))); %#ok<ST2NM>
    plot(amp); grid on
    legend('SNR','Location','North')
    title('Signal To Noise Ratio')
    linkaxes(ax,'x')
else
    for ii = 1:nargout
        switch ii
            case 1
                varargout{1} = amp;
            otherwise
                warning('SIGNALNOISERATIO_DIS:OutputArg',...
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


