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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <cmath>
#include <volk/volk.h>
#include "decoder_f_impl.h"

namespace gr {
  namespace pocsag {

    decoder_f::sptr
    decoder_f::make(float fs)
    {
      return gnuradio::get_initial_sptr
        (new decoder_f_impl(fs));
    }

    /*
     * The private constructor
     */
    decoder_f_impl::decoder_f_impl(float fs)
      : gr::sync_block("decoder_f",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make(0, 0, 0)),
        d_history(2), d_thresh(0.5e-3), d_sampcount(0), d_tot_sampcount(0)
    {
      set_history(d_history);
      arr_bits = (int8_t *) volk_malloc(sizeof(int8_t)*bitbuff_size, volk_get_alignment());
      bit_check = 1/max_bps*fs*0.5;
      d_fs = fs;
    }

    /*
     * Our virtual destructor.
     */
    decoder_f_impl::~decoder_f_impl()
    {
      volk_free(arr_bits);
    }

    int8_t
    decoder_f_impl::sgn(float val)
    {
      return (0.0F < val) - (0.0F > val);
    }

    bool
    decoder_f_impl::checkPreamble(int8_t* p)
    {
      //printf("inside checkPreamble");
      for(int i = 0; i < numBitsPreamble/2-1; i++)
      {
        if((p[i*2] != 1) || (p[i*2+1] != 0))
          return false;
      }
      printf("Preamble detected!\n");
      return true;
    }

    int
    decoder_f_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const float *in = (const float *) &((const float *)input_items[0])[history()-1];
      uint sampcount = d_sampcount;
      uint tot_sampcount = 0;
      uint bit_count = d_bit_count;

      // Do <+signal processing+>
      for(int i = 0; i < noutput_items; i++)
      {
        if(abs(in[i] - in[i-1]) < d_thresh)
        {
          tot_sampcount++;
          sampcount++;
        }
        else if((sampcount > bit_check) && (sgn(in[i]) != sgn(in[i-1])))
        {
          arr_bits[bit_count++] = sgn(sgn(in[i-1])+1);
          if(bit_count > (numBitsPreamble-1))
          {
            if(checkPreamble(&arr_bits[bit_count-numBitsPreamble]))
            {
              d_estim_bps = d_fs/sampcount;
              printf("BPS:%d\n",d_estim_bps);
            }
            if(bit_count > (1120-1)) bit_count = 0; 
          }
          sampcount = 0;
        }
      }
      d_tot_sampcount += tot_sampcount;
      d_sampcount = sampcount;
      d_bit_count = bit_count;
      //printf("BPS:%d\n",d_estim_bps);
      //printf("bit_count:%d\n",bit_count);
      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace pocsag */
} /* namespace gr */

