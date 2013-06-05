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
% Author:           Mark Tompkins
% Revision:			4903.19400
% All rights reserved.

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

end

