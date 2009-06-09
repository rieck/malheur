function [K,Y,C] = mal_dot_product(in, cfg)
%
% MALHEUR - Automatic Malware Analysis on Steroids
% Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
% Berlin Institute of Technology (TU Berlin).
% 
% Synopsis:
%     [K,Y,C] = mal_dot_product(in, cfg);
%
% Arguments:
%     in:       Directory or archive containing reports
%     cfg:      Malheur configuration file 
%
% Returns:
%     K:	    Matrix of dot products (kernel matrix n x n)
%     Y:        Label vector (1 x n)
%     C:        Label to class names structure
%

% Call native function
[K,Y,C] = malheur('dot_product', in, cfg);
