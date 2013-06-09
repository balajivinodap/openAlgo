function [ price ] = importFromTxt( filename )
%IMPORTFROMTXT Import numeric data from a text file as a struct.
%   [price] = IMPORTOPENCLOSEFROMTXT(FILENAME) Reads data from text file FILENAME for the default selection.
%
%   This will create a struct, parsing the column headers as price.textdata and values as price.data.
%   We will return an array of Open and Close
%   This assumes the columns are in standard order of Date | Time | Open | High | Low | Close ...
%   and therefore imported into the struct as Open (:,1) & Close (:,4)
%

%% Import from provided file
try
    tmp = importdata(filename);
catch me
    % Something went terribly wrong
    % Try to get some cogent information for debugging
    disp(me)
end;

%% Parse struct to a simple array with columns Open | High | Low | Close
price = [tmp.data(:,1),tmp.data(:,2),tmp.data(:,3),tmp.data(:,4)];

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
