#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2019 <+YOU OR YOUR COMPANY+>.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import pocsag_swig as pocsag
import numpy as np

class qa_decoder_f (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        src_data = []
        fs = 50000.0
        numsamp = 21
        for i in range(0,576-1):
            ones = np.ones(numsamp)
            zeros = np.zeros(numsamp)
            src_data = np.append(src_data, np.append(ones, zeros))
        exp_bps = 2400
        src = blocks.vector_source_c(src_data)
        decoder = pocsag.decoder_f(fs)
        self.tb.connect(src,decoder)
        # set up fg
        self.tb.run ()
        self.assertEqual(1,1)
        # check data


if __name__ == '__main__':
    gr_unittest.run(qa_decoder_f, "qa_decoder_f.xml")
