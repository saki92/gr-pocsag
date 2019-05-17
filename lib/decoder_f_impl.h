/* -*- c++ -*- */
/* 
 * Copyright 2019 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_POCSAG_DECODER_F_IMPL_H
#define INCLUDED_POCSAG_DECODER_F_IMPL_H

#include <pocsag/decoder_f.h>

namespace gr {
  namespace pocsag {

    class decoder_f_impl : public decoder_f
    {
     private:
      // Nothing to declare in this block.
      uint d_history;
      uint d_sampcount = 0;
      uint d_tot_sampcount;
      float d_thresh;
      int8_t *arr_bits;
      const uint16_t max_bps = 2400;
      uint bit_check;
      uint d_estim_bps;
      const uint bitbuff_size = 1200;
      const uint numBitsPreamble = 576;
      float d_fs;
      uint d_bit_count = 0;

     public:
      decoder_f_impl(float fs);
      ~decoder_f_impl();
      int8_t sgn(float val);
      bool checkPreamble(int8_t* p);

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace pocsag
} // namespace gr

#endif /* INCLUDED_POCSAG_DECODER_F_IMPL_H */

