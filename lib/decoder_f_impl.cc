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
        d_history(2), d_thresh(1e-3), d_sampcount(0), d_tot_sampcount(0)
    {
      set_history(d_history);
      arr_bits = (int8_t *) volk_malloc(sizeof(int8_t)*bitbuff_size, volk_get_alignment());
      bit_check = fs*0.5/max_bps;
      d_fs = fs;
      d_currState = 0;
      printf("Detecting Preamble\n");
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
      for(int i = 0; i < numBitsPreamble/2-1; i++)
      {
        if((p[i*2] != 1) || (p[i*2+1] != 0))
          return false;
      }
      printf("Preamble detected!\n");
      return true;
    }

    uint
    decoder_f_impl::getBPS(int estimBPS)
    {
      if(abs(estimBPS - 512) < abs(estimBPS - 1200))
        return 512;
      else if(abs(estimBPS - 1200) < abs(estimBPS - 2400))
        return 1200;
      else
        return 2400;
    }

    bool
    decoder_f_impl::checkFSC(int8_t* p)
    {
      for(int i = 0; i < numFSCBits; i++)
      {
        if(FSCBits[i] != p[i])
        {
          //printf("Bits same:%d\n", i);
          return false;
        }
      }
      printf("FSC detected!\n");
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
      uint idxBit = d_idxBit;

      // Detect preamble
      if(d_currState == 0)
      {
        for(int i = 0; i < noutput_items; i++)
        {
          if(sgn(in[i]) == sgn(in[i-1]))
            sampcount++;
          else if(sampcount > bit_check)
          {
            arr_bits[idxBit++] = sgn(sgn(in[i-1])+1);
            if(idxBit > (numBitsPreamble-1))
            {
              if(checkPreamble(&arr_bits[idxBit-numBitsPreamble]))
              {
                d_estim_bps = d_fs/sampcount;
                d_BPS = getBPS(d_estim_bps);
                d_idxBit = 0;
                d_currState = 1; // Set next state to FSC detect
                printf("BPS:%d\t%d\n",d_estim_bps,d_BPS);
                printf("Detecting FSC\n");
                return i; // End the current work call and tell only i samples are used
              }
              if(idxBit > (23000-1)) idxBit = 0;
            }
            sampcount = 0;
          }
        }
      }

      // Detect FSC
      else if(d_currState == 1)
      {
        uint numSampPerBit = d_fs/d_BPS;
        for(int i; i < noutput_items; i++)
        {
          if((i+numSampPerBit) < (noutput_items+1))
            arr_bits[idxBit++] = sgn(sgn(in[i+numSampPerBit/2])+1);
          else
          {
            d_idxBit = idxBit;
            return i;
          }
          //printf("idxBit:%u\tnoutput_items:%d\n",idxBit,noutput_items);
          if(idxBit > (numFSCBits-1))
          {
            if(checkFSC(&arr_bits[idxBit-numFSCBits]))
            {
              d_idxBit = 0;
              d_currState = 2; // Set next state to store batch bits
              printf("Writing Batches\n");
              return i+numSampPerBit;
            }
            if(idxBit > (2300-1))
            {
              idxBit = 0;
              d_currState = 0; // If FSC not detected, estimate BPS again
              printf("Detecting Preamble\n");
              return i;
            }
          }
          i += (numSampPerBit-1);
        }
      }
      
      d_tot_sampcount += tot_sampcount;
      d_sampcount = sampcount;
      d_idxBit = idxBit;
      //printf("BPS:%d\n",d_estim_bps);
      //printf("idxBit:%d\n",idxBit);
      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace pocsag */
} /* namespace gr */

