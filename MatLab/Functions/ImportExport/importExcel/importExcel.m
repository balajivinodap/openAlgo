function [data,bigPoint] = importExcel()
%IMPORTEXCEL Allow selection of any Excel file for data import
% Allow selection of any Excel file for data import
%
% Prompt for specific file with file browser
%
%
% Author:           Mark Tompkins
% Revision:			4902.23478
[fname,fpath] = uigetfile({'*.xlsx';'*.xls'});
if fpath==0, error('no file selected'); 
end

% !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
% !  NOTE - Datasets are no longer the preferred data structure type  !
% !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
%
% Loading data as a Matlab dataset so we can properly automate using Column Headers from Excel
% Column Headers imported must be in a standardized Date | Open | High | Low | Close naming scheme
% The order of columns imported should be irrelevant.
data = dataset('xlsfile', fullfile(fpath,fname));

% Check for the BigPoint.txt file in the same directory as the data file.
% BigPoint.txt is the dollar adjustment factor from the imported data's decimal point to 
% real dollars.
if exist(fullfile(fpath,'BigPoint.txt'), 'file')
    bigPoint = load(fullfile(fpath,'BigPoint.txt'));
else
    % File does not exist.  Advise user and continue.
    bigPoint = 1;                               % Set bigPoint to 1 for later calculations
    warningMessage = sprintf('Warning: BigPoint.txt file does not exist in:\n%s\n\nP&L calculations will not reflect actual dollars.', fpath);
    uiwait(msgbox(warningMessage));
end;


end

