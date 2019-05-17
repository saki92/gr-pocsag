/* -*- c++ -*- */

#define POCSAG_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "pocsag_swig_doc.i"

%{
#include "pocsag/decoder_f.h"
%}


%include "pocsag/decoder_f.h"
GR_SWIG_BLOCK_MAGIC2(pocsag, decoder_f);
