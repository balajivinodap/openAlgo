function [ varargout ] = OHLCSplitter( price )
%OHLCSPLITTER Parses a given dataset or matrix into individual arrays of Open | High | Low | Close
%   OHLCSPLITTER will parse a given dataset with the assumption that the given data is ordered if
%   the supplied input is a matrix.
%
%	The requested output can be of any:
%		close 					= OHLCSPLITTER(price)
%		[open, close] 			= OHLCSPLITTER(price)
%		[open, high, low, close]= OHLCSPLITTER(price)
%
%	NOTE:	The order of the outputs is determinate
%

% Check if we've been given a dataset or matrix
if(isa(price, 'dataset'))
    if exist('price.Open','var'), o = price.Open; end;
    if exist('price.High','var'), h = price.High; end;
    if exist('price.Low','var'), l = price.Low; end;
    if exist('price.Close','var'), c = price.Close; end;
else
    % Dimensions
    cols  = size(price,2);
    % Given 1 column return Close
    if cols == 1, c = price; end;
    % Given 2 columns return Open | Close
    if cols == 2
        o = price(:,1);
        c = price(:,2);
    end; %if
    % Given 3 columns reject
    if cols == 3, error('Ambiguous input data given.  Three input columns can''t be parsed.'); end;
    % Given 4 or more columns return Open | High | Low | Close
    if cols == 4
        o = price(:,1);
        h = price(:,2);
        l = price(:,3);
        c = price(:,4);
    end; %if
end; %if

%% Return values
% No output requested, error
if nargout ==0
    error('OHLCSPLITTER:OutputArg','No output requested. Exiting.');
end; %if

% One output requested, return Close
if nargout == 1, varargout{1} = c; end;         % Close

% Two outputs requested, return Open | Close
if nargout == 2 && cols >= 2
    varargout{1} = o;                           % Open
    varargout{2} = c;                           % Close
end; %if

if nargout == 3, error('OHLCSPLITTER:OutputArg','Ambiguous output requested.  Three outputs can''t be provided.'); end;

if nargout == 4 && cols == 4
    varargout{1} = o;                           % Open
    varargout{2} = h;                           % Close
    varargout{3} = l;                           % Open
    varargout{4} = c;                           % Close
end; %if

if nargout > 4, warning('OHLCSplitter:OutputArg',...
        'Too many output arguments requested, ignoring last ones');
    varargout{1} = o;                           % Open
    varargout{2} = h;                           % Close
    varargout{3} = l;                           % Open
    varargout{4} = c;                           % Close
end;

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

