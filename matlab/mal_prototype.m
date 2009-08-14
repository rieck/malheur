function [proto, data] = mal_prototype(in, cfg, varargin)
%
% MALHEUR - Automatic Malware Analysis on Steroids
% Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
% Berlin Institute of Technology (TU Berlin).
% 
% Synopsis:
%     [proto, data] = mal_prototype(in, cfg);
%
% Arguments:
%     in:       Directory or archive containing reports
%     cfg:      Malheur configuration file 
%
% Returns:
%     proto:	Prototype structure
%               .indices   Indices of prototypes (1 x l)
%               .assign    Assignments of prototypes (1 x n)
%     data:     Data structure   
%               .labels    Label vector (1 x n)
%               .names     Label to class names structure
%               .sources   Sources of feature vectors (1 x n)
%

% Call native function
if isempty(varargin)
   [proto, data] = malheur('prototype', in, cfg);
else
   [proto, data] = malheur('prototype', in, cfg, varargin{1});
end
