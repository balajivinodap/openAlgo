function [] = optInfo(isPar, range, tenI, numSampled)
%optInfo Display minor feedback to user for expectation of duration
% Optimizations can take long periods of time and the user should be provided
% with some manner of feedback, if nothing more than a 'dirty' expectation of
% when the process should terminate.
%
% An additional challange in parallel processing is the indeterminate speeds
% of various processors allocated to a 'parfor' loop.  We will simply take the
% results of the first 10 iterations and extrapolate an estimate forward.
%
% showOnce limits the output to the initial information
% isPar = are we using parallel processing?
% range = cell array of format {#:#:# #:#:# ...}
% tenI = how long it took for 10 iterations and we'll extrapolate from there
% also reference parameterSweep.m for future development
% numSampled = the amount to subtract from range iterations
%
% Author: Mark Tompkins
%

if ~exist('numSampled','var')
    numSampled = 0;
end; %if

% This will allow us to adjust for current worker pool size
if (matlabpool('size') == 0)
    numCPU = 1;
else
    numCPU = matlabpool('size');
end;

N = length(range);
if N == 1
    % numI = length(range{1});
    % if length(range{1})-numSampled > -1
    if range-numSampled > -1
        numI = range-numSampled;
    else
        numI = 0;
    end; %if
else
    numI = length(range{1});
    for ii = 2:N
        numI = numI * length(range{ii});
    end
    if numI - numSampled > -1
        numI = numI - numSampled;
    else
        numI = 0;
    end; %if
end

strNumI = thousandSep(numI);

% if 'tenI' hasn't been passed, it is the start of an optimization
if ~exist('tenI', 'var')
    disp('*** Beginning Optimization ***');
    formatSpec = '\nOptimization range(s):\n';
    fprintf(formatSpec);
    for ii = 1:length(range)
        if isempty(range{ii})
            formatSpec = '          Parameter %d: Not Given\n';
            fprintf(formatSpec,ii);
        else
            formatSpec = '          Parameter %d: %s\n';
            fprintf(formatSpec,ii,vect2colon(range{ii}));
        end; %if
    end; %for
    disp(' ');

    formatSpec = 'Number of iterations to perform: %s\n\nOptimization started: %s\n\n';
    fprintf(formatSpec,strNumI,datestr(now));
    if (numI > 20)
        formatSpec = 'Sampling first 10 iterations for estimate ...\n';
    else
        formatSpec = 'Processing...\n';
    end;
    fprintf(formatSpec);
% We've been passed tenI. 
% Do we have any more iterations to check? If so calculate a dirty estimate of duration
elseif numI > 0
    estDur = (numI * tenI / 10 );
    if (isPar == 1)
        % add 6 minutes to Parallel process.  Assumed to be to process the results far workers.
        estDur = (estDur / numCPU) + 360;
    end;
    estDur = round(estDur);
    formatSpec = 'Completed 10 iterations.\n  Projected duration of optimization: ';
    fprintf(formatSpec);
    if ((estDur/60) < 1)
        formatSpec = 'less than 1 minute using %d core(s).\n\n';
        fprintf(formatSpec,numCPU);
    else
        formatSpec = '%d minutes using %d core(s).\n\n';
        fprintf(formatSpec,fix(estDur/60),numCPU);
    end;
    
    formatSpec = 'Estimated time of completion: %s\n\nContinuing...\n\n';
    fprintf(formatSpec,datestr(addtodate(now,fix(estDur),'second')));
else
    formatSpec = '\n No iterations left after sample phase, cleaning up...\n\n';
    fprintf(formatSpec);
end;
end

