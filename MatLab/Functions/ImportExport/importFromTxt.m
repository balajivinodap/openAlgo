function [ price ] = importFromTxt( filename )
%IMPORTFROMTXT Import numeric data from a text file as a struct.
%   [price] = IMPORTOPENCLOSEFROMTXT(FILENAME) Reads data from text file FILENAME for the default selection.
%
%   This will create a struct, parsing the column headers as price.textdata and values as price.data.
%   We will return an array of Open and Close
%   This assumes the columns are in standard order of Date | Time | Open | High | Low | Close ...
%   and therefore imported into the struct as Open (:,1) & Close (:,4)
%
% Author: Mark Tompkins
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

end

