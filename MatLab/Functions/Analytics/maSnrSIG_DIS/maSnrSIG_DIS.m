function varargout = maSnrSIG_DIS(price,maF,maS,typeMA,snrThresh,snrEffect,bigPoint,cost,scaling)
%MASNRSIG_DIS 2 inputs MA signal generation with a Signal To Noise Ratio based filter
%   MA is a classic 2 input crossover signal generator
%   SNR is an indicator that measures the dominant cycle's signal to noise ratio.
%       The SNR is measured in decibels and uses a logarithmic scale
%
%   By using an SNR filter on a MA signal, we should have a noticable improvement on the
%   resulting performance.
%
%   This produces a logically valid signal output
%
%   maF:            MA lead lookback period
%   maS:            MA lag lookback period
%   typeMA:         MA type from the following selection
%                       -4  Trimmed
%                       -3  Harmonic
%                       -2  Geometric
%                       -1	Exponential
%                        0  Simple  (default)
%                       >0  Weighted e.g. 0.5 Square root weighted, 1 = linear, 2 = square weighted
%   snrThresh:      SNR threshold to calibrate when the dominant cycle is clear enough (6db is default)
%   snrEffect:      SNR effects
%                   	Effect 0:   Drop signals less than the SNR threshhold
%                       Effect 1:   Reverse signals that are generated when SNR is less than the threshold
%

%% Defaults
if ~exist('maF','var'), maF = 12; end;
if ~exist('maS','var'), maS = 26; end;
if ~exist('typeMA','var'), typeMA=0; end;
if ~exist('snrThresh','var'), snrThresh = 6; end;
if ~exist('snrEffect','var'), snrEffect = 0; end;
if ~exist('scaling','var'), scaling = 1; end;
if ~exist('cost','var'), cost = 0; end;         % default cost
if ~exist('bigPoint','var'), bigPoint = 1; end; % default bigPoint

%% Parse
fClose = OHLCSplitter(price);

[SIG, R, SH, SNR, LEAD, LAG] = maSnrSIG_mex(price,maF,maS,typeMA,snrThresh,snrEffect,bigPoint,cost,scaling);

if nargout == 0
	% Center plot window basis monitor (single monitor calculation)
    scrsz = get(0,'ScreenSize');
    figure('Position',[scrsz(3)*.15 scrsz(4)*.15 scrsz(3)*.7 scrsz(4)*.7])
    
    % Each element must be the same length - nonsense - thanks MatLab
    % http://www.mathworks.com/help/matlab/matlab_prog/cell-arrays-of-strings.html
    layout = ['6     ';'2     ';'1 3 5 ';'7 9 11'];
    hSub = cellstr(layout);
    ma2inputsSIG_DIS(price,maF,maS,typeMA,bigPoint,cost,scaling,hSub);
    
    ax(1) = subplot(6,2,[2 4]);
    plot([fClose,LEAD,LAG]);
    axis (ax(1),'tight');
    grid on
    legend('Price',['Lead ',num2str(maF)],['Lag ',num2str(maS)],'Location', 'NorthWest')
    title(['MA+SNR Results, Sharpe Ratio = ',num2str(SH,3)])
    set(gca,'xticklabel',{})
    
    ax(2) = subplot(6,2,[6 8]);
    plot([SNR,snrThresh*ones(size(SNR))])
    grid on
    legend(['SNR Thresh ',num2str(snrThresh),'db'],'Location', 'North')
    title('SNR')
    set(gca,'xticklabel',{})
    
    ax(3) = subplot(6,2,[10 12]);
    plot([SIG,cumsum(R)]), grid on
    legend('Position','Cumulative Return','Location', 'North')
    title(['Final Return = ',thousandSepCash(sum(R))])
    
    linkaxes(ax,'x')
else
    %% Return values
    for ii = 1:nargout
        switch ii
            case 1
                varargout{1} = SIG; % signal
            case 2
                varargout{2} = R; % return (pnl)
            case 3
                varargout{3} = SH; % sharpe ratio
            case 4
                varargout{4} = SNR; % ravi signal
            case 5
                varargout{5} = LEAD; % moving average lead
            case 6
                varargout{5} = LAG; % moving average lag
            otherwise
                warning('MASNR:OutputArg',...
                    'Too many output arguments requested, ignoring last ones');
        end %switch
    end %for
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

