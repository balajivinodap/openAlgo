function [cleanS] = remEchos( s )
%REMECHOS Will remove echos froms a provided vector
% The primary purpose of remEchos is to transform a STATE vector in to a SIGNAL vector.
% For example, when attempting to generating signals from a lead / lag moving average STATE function
% a logical output of whether a condition is true or not is provided.
%	Example (where s is a given STATE vector):
%		s(lead>lag) =  1
%		s(lead<=lag) = -1
% 	We can transform the output of this STATE condition as follows:
%		in	[1 1 1 1 1 1 -1 -1 -1 -1 -1 1 1 1 1]	<-- STATE INPUT
%		out	[1 0 0 0 0 0 -1  0  0  0  0 1 0 0 0]	<-- SIGNAL OUTPUT
%	This output is now "actionable" as a SIGNAL
%
%   Note: This function can accept a vectorized matrix

cleanS = s;
[rows,cols] = size(cleanS);

for jj=1:cols
    if(~isempty(cleanS(:,jj)))
        % Index of active trade - starts at first position
        actIdx = 1;
        actSig = cleanS(actIdx,jj);                     % Get the active signal so we can remove echos
        
        % Define the iterate range from first trades to last signal
        for ii=1:rows-1
            if (cleanS(ii+1,jj)==actSig)             	% If the next line is the same, it is an echo. Zero it out.
                cleanS(ii+1,jj) = 0;
            else                                       	% We have a new active signal.  Update.
                if (cleanS(ii+1,jj) ~= 0)              	% Zeros are not new signals
                    actSig = cleanS(ii+1,jj);
                end; %if
            end; %if
        end; %for                                     	% Iterate until complete
    end; %if
end; %for

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


