function [ vBars ] = virtualBars( data,inc )
%VIRTUALBARS Transforms a dataseries from a lower timeframe to higher (e.g. 1min data to 4min, 5min etc.)
%   This function allows the transformation of lower period observations in time to higher.  In its
%   current form we are only virtualizing Open and Close.  Further coding to handle High and Low should
%   be considered. We are accepting either a dataset or a double array as input.
%   Vars:   'data' is either a 2 or 4 double array which assumes standard column order of O|C or O | H | L | C
%           'inc' is the increment to virtualize the data to.  We must already know the input increment
%           e.g. 1 minute observations dataset to 4 minute virtualized would be inc = 4
%   The virtualization logic drops all partial bars from the end of the submitted dataset so that we are
%   left with only full virtualized observations.  There may be need to have partial observations formed
%   which we can revisit.
%
% Author:               Mark Tompkins
% Last Revision:        June 2, 2013

%% MEX code to be skipped
coder.extrinsic('slidefun');

% Preallocate variables so we can MEX
numBars = size(data,1);
hTemp = zeros(numBars,1); %#ok<NASGU>
lTemp = zeros(numBars,1); %#ok<NASGU>

%% Check if we've been passed a dataset or a double array

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
        error('Input needs to be in the format of ''O | C'' or ''O | H | L | C''');
    end; %if
else
    error('We need either a double array.  Datasets (or unexpected data) are not supportedin MEX.');
end; %if

end

