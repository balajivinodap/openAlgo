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
% Author:           Mark Tompkins
% Revision:			4902.23511

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
end