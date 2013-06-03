function [ varargout ] = OHLCSplitter( price )
%OHLCSPLITTER Parses a given dataset or matrix into individual arrays of Open | High | Low | Close
%   OHLCSPLITTER will parse a given dataset with the assumption that the given data is ordered if
%   the supplied input is a matrix.
%
% Author:		Mark Tompkins
% Revision: 	4902.18930

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
    % One output requested, return Close
    if nargout == 1, varargout{1} = c; end;         % Close
    % Two outputs requested, return Open | Close
    if nargout == 2 && cols >= 2
        varargout{1} = o;                           % Open
        varargout{2} = c;                           % Close
    end; %if
    if nargout == 3, error('Ambiguous output requested.  Three outputs can''t be provided.'); end; 
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
end

