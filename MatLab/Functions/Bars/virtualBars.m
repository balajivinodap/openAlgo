function [ vBars ] = virtualBars( data,inc )
%VIRTUALBARS Transform data series from a lower timeframe to higher (e.g. 1min data to 4min, 5min etc.)
%   This function allows the transformation of lower period observations in time to higher.  In its
%   current form we are only virtualizing Open and Close.  Further coding to handle High and Low should
%   be considered.
%   Vars:   'data' is a properly formed dataset having an 'Open' and 'Close' column (property)
%           'inc' is the increment to virtualize the data to.  We must already know the input increment
%           e.g. 1 minute observations dataset to 4 minute virtualized would be inc = 4
%   The virtualization logic drops all partial bars from the end of the submitted dataset so that we are
%   left with only full virtualized observations.  There may be need to have partial observations formed
%   which we can revisit.
%
%	Author: Mark Tompkins

numBars   = length(data);

if(isa(data, 'dataset')) && exist('t','var')
    % we've been passed a dataset   
    vBars = dataset({data.Open(1:inc:(floor(numBars/inc)*inc)-inc+1),'Open'},...
                    {data.Close(inc:inc:floor(numBars/inc)*inc),'Close'});
else
    % we should warn here that we've not been passed a dataset and / or t value
    numCols = size(data,2);
    if numCols == 2 || numCols == 4
        %vBars = dataset({data.Open(1:inc:(floor(numBars/inc)*inc)-inc+1),'Open'}, {data.Close(inc:inc:floor(numBars/inc)*inc),'Close'});
        o = data(1:inc:(floor(numBars/inc)*inc)-inc+1,1);           % Open
        c = data(inc:inc:floor(numBars/inc)*inc,numCols);           % Close
        vBars = [o c];
        if numCols == 4
            % Get the Highest high and Lowest low in the given increment
            hTemp = slidefun(@max, inc, data(:,2),'forward');           % Moving window of Max's
            h = hTemp(1:inc:end);                                       % High
            % Trim any overhang
            while size(h,1) > size(o,1)
                h(end)=[];
            end;
            lTemp = slidefun(@min, inc, data(:,3),'forward');           % Moving window of Min's
            l = lTemp(1:inc:end);                                       % Low
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
end; %if

end

