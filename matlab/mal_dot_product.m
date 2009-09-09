function [dot, data] = mal_dot_product(in, cfg)
%
% MALHEUR - Automatic Malware Analysis on Steroids
% Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
% Berlin Institute of Technology (TU Berlin).
% 
% Synopsis:
%     [dot, data] = mal_dot_product(in, cfg);
%
% Arguments:
%     in:       Directory or archive containing reports
%     cfg:      Malheur configuration file 
%
% Returns:
%     dot:     Dot product matrix (n x n)
%     data:     Data structure   
%               .labels    Label vector (1 x n)
%               .names     Label to class names structure
%               .sources   Sources of feature vectors (1 x n)
%

% Call native function
[dot, data] = malheur('dot_product', in, cfg);
