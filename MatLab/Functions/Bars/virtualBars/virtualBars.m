function [ vBars ] = virtualBars(data, inc)
%VIRTUALBARS Transforms a classic Open | Close or Open | High | Low | Close double vector from a lower timeframe to a higher timeframe
%	For instance, transform 1 min observations to 4 minute observations
%
%	INPUTS
%		DATA	A 2N or 4N double array which assumes standard column order of O | C or O | H | L | C
%		INC		The increment modifier to virtualize the data in a multiple form.
%				Example:
%					DATA	A 1 minute observation matrix
%					INC		4
%					output	A virtualized 4 minute observation matrix
%
%   The virtualization logic drops all partial bars from the end of the submitted dataset so that we are
%   left with only full virtualized observations.
%
%	vBars = VIRTUALBARS(PRICE, INC)	Returns an N-dimentional double array of virtualized observations
%
%	NOTE:	The provided output is of a form consistent with the input (i.e. 2N -> 2N | 4N -> 4N)
%

%% MEX code to be skipped
coder.extrinsic('slidefun');

% Preallocate variables so we can MEX
numBars = size(data,1);
hTemp = zeros(numBars,1); %#ok<NASGU>
lTemp = zeros(numBars,1); %#ok<NASGU>

numBars = length(data);

if isa(data,'double')
    % Check if we've been passed 2 or 4 columns.
    % We can accept 2 columns and assume Open | Close
    % or we can accept 4 columns and assume Open | High | Low | Close
    numCols = size(data,2);
    if numCols == 2 || numCols == 4
        o = data(1:inc:(floor(numBars/inc)*inc)-inc+1,1);           % Open
        c = data(inc:inc:floor(numBars/inc)*inc,numCols);           % Close
        vBars = [o c];
        if numCols == 4
            % Get the Highest high and Lowest low in the given increment
            hTemp = slidefun('max', inc, data(:,2),'forward');     	% Moving window of Max's
            h = hTemp(1:inc:end);                                  	% High
            % Trim any overhang
            while size(h,1) > size(o,1)
                h(end)=[];
            end;
            lTemp = slidefun('min', inc, data(:,3),'forward');     	% Moving window of Min's
            l = lTemp(1:inc:end);                                  	% Low
            % Trim any overhang
            while size(l,1) > size(o,1)
                l(end)=[];
            end;
            vBars =	[o h l c];
        end; %if
    else
        % Seems like we might have non-standard input.  Throw an error
        error('VIRTUALBARS:InputArgs','Input needs to be in the format of ''O | C'' or ''O | H | L | C''');
    end; %if
else
    error('VIRTUALBARS:InputArgs','We need a 2N or 4N double array as input.  Datasets (or unexpected data) are not supportedin MEX.');
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

