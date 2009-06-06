function [rep,list] = mal_load_mist(in, cfg)
%
% MALHEUR - Automatic Malware Analysis on Steroids
% Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
% Berlin Institute of Technology (TU Berlin).
% 
% Synopsis:
%     [rep,list] = mal_load_mist(in, cfg);
%
% Arguments:
%     in:       Cell array of file name or directory name
%     cfg:      Malheur configuration file 
%
% Returns:
%     rep:      Cell array of MIST reports 
%     list:     Cell array of file names
%

% Check for input
if iscellstr(in) 
   list = in
elseif isstr(in)
   % Generate list of files
   d = dir(in);
   list = {};
   for i = 1:length(d)
      if d(i).isdir
         continue;
      end
      list{end+1} = sprintf('%s/%s', in, d(i).name);
   end
else
   error('first arguments needs to be cell array or string');
end

% Call native function
rep = malheur('load_mist', list, cfg);
