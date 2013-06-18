function [STA,TLINE,ITREND] = iTrendSTA(price)
%ITRENDSTA returns a logical STATE for the instantaneous trend based on the work by John Ehlers
%   iTrendSTA returns a logical STATE for a given iTrend and dominant cycle crossover
%
%   INPUTS:     Input 'price' should be an M x 1 array of prices ordinarily transformed e.g. (H + L)/2
%
%   OUTPUT: 	STA values are ITREND referenced where:
%           		1    ITREND > TLINE
%           		0    ITREND = TLINE
%          		   -1    ITREND < TLINE
%
%   STA = ITRENDSTA(PRICE) returns a STATE based upon provided price vector
%
%   NOTE: As this file is designed to be MEX'd all inputs are required.
%

%% MEX code to be skipped
coder.extrinsic('iTrend_mex')

% Preallocate so we can MEX
rows = size(price,1);
STA = zeros(rows,1);                                        
TLINE = zeros(rows,1);                  %#ok<NASGU>
ITREND = zeros(rows,1);                 %#ok<NASGU>

%% Calculations
[TLINE,ITREND] = iTrend_mex(price);

STA(ITREND > TLINE) = 1;
STA(ITREND < TLINE) = -1;

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
%   Revision:      4916.33715
%   Copyright:     (c)2013
%

