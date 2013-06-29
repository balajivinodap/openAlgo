function [dFile, defFile, scaling] = dataSelect( dataSet)
%DATASELECT A trivial file to select the data file to be loaded
%   dataSelect is a trivial file to select the data file to be loaded
%   It helps to maintain order when adding and removing time series data CSV files
%
%   INPUTS:     dataSet     A selection from one of the available CSV files.
%                           Selection key: Contract|interval|interval period|
%                           e.g. Contract = KC | interval = 1 | interval period = D(ay)
%                           Currently available:
%                           KC1D   (KC)    Arabica Futures     Daily 	5 years
%                           KC1M   (KC)    Arabica Futures     1 min 	1 year
%                           ES5S   (ES)    E-Mini S&P          5 sec   6 mos
%
%   OUTPUTS:    dFile       The full UNC file path and name of the CSV file
%               defFile     Associated UNC file path and name of the symbol definition
%               scaling     Sharpe ratio scaling factor for given file
%

switch lower(dataSet)
    case {'kc1d'}
        disp('Loading (KC) Arabica Futures Daily 5yrs');
        dFile = '\\DISKSTATION\Matlab\HgGit\openAlgo Sample Data\KC\@KC 5yr Daily.txt';
        defFile = '\\DISKSTATION\Matlab\HgGit\openAlgo Sample Data\KC\symbolDef.txt';
        scaling = sqrt(252);
    case {'kc1m'}
        disp('Loading (KC) Arabica Futures 1 Minute 1yr');
        dFile = '\\DISKSTATION\Matlab\HgGit\openAlgo Sample Data\KC\@KC 1yr 1min.txt';
        defFile = '\\DISKSTATION\Matlab\HgGit\openAlgo Sample Data\KC\symbolDef.txt';
        scaling = sqrt(252*60*11);
    case {'es5s'}
        disp('Loading (ES) E-Mini S&P 5 Seconds 6mo');
        dFile = '\\DISKSTATION\Matlab\Data\ES\@ES 6mos 5sec.txt';
        defFile = '\\DISKSTATION\Matlab\Data\ES\symbolDef.txt';
        scaling = sqrt(252*60*11*12);
    otherwise
        error('Unknown requested data file.  Aborting...');
end; %switch

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
%	Redistribution and use in source and binary forms, with or without
%	modification, are permitted provided that the following conditions are met:
%
%	1. Redistributions of source code must retain the below copyright notice,
%	this list of conditions and the following disclaimer.
%	2. Redistributions in binary form must reproduce the below copyright notice,
%   this list of conditions and the following disclaimer in the documentation
%   and/or other materials provided with the distribution.
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
%   Revision:      4928.15718
%   Copyright:     (c)2013
%


