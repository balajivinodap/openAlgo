function importSymbolDef( filename)
%IMPORTSYMBOLDEF Import various properties specific to a given traded symbol.
%   IMPORTSYMBOLDEF(FILENAME) Reads data from text file FILENAME for the default selection.
%
%   This will import all variables from a given text file, provided the format is:.
%   Variable1Name, Variable2Name,...
%   Variable1Value, Variable2Value,...
%   Currently the deliminator is specificed as a comma ','
%   Values are expected to at line 5 (Headers) and line 6(Values)
%
%   Expected assignments bigPoint, minTick
%

%% Make sure the file exists
if ~exist(filename,'file')
    warning('importSymbolDef.txt not found. Defaulting expected values to 1');
    % Assigning expected values
    assignin('base','bigPoint',1);
    assignin('base','minTick',1);
else
    delimiterIn = ',';
    tmpStruct = importdata(filename,delimiterIn);
    % Iterate through each detected header / value pair
    for ii=1:size(tmpStruct.colheaders,2)
        assignin('base',tmpStruct.colheaders{ii},tmpStruct.data(1,ii));
    end;
    clearvars tmpStruct;
end; %if

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
