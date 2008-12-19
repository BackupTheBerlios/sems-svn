/*
 * $Id$
 *
 * Copyright (C) 2007 iptego GmbH
 *
 * This file is part of sems, a free SIP media server.
 *
 * sems is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * For a license to use the sems software under conditions
 * other than those described here, or to purchase support for this
 * software, please contact iptel.org by e-mail at the following addresses:
 *    info@iptel.org
 *
 * sems is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "AmAudioMixIn.h"
#include "SampleArray.h"

#define IS_FINISH_B_MIX    (flags & AUDIO_MIXIN_FINISH_B_MIX) 
#define IS_ONLY_ONCE       (flags & AUDIO_MIXIN_ONCE)
#define IS_IMMEDIATE_START (flags & AUDIO_MIXIN_IMMEDIATE_START)

AmAudioMixIn::AmAudioMixIn(AmAudio* A, AmAudioFile* B, 
			   unsigned int s, double l,
			   unsigned int flags) 
  :   A(A),B(B), s(s), l(l), 
      mixing(false), next_start_ts_i(false),
      flags(flags)
{
}

AmAudioMixIn::~AmAudioMixIn() { }

int AmAudioMixIn::get(unsigned int user_ts, unsigned char* buffer, 
		      unsigned int nb_samples) {
  if (!mixing) {
    if (!next_start_ts_i) {
      next_start_ts_i = true;
      next_start_ts = IS_IMMEDIATE_START ? 
	user_ts : user_ts + s*DEFAULT_SAMPLE_RATE;
    }
    if (!ts_less()(user_ts, next_start_ts)) {
      DBG("starting mix-in\n");
      mixing = true;
      next_start_ts = user_ts + s*DEFAULT_SAMPLE_RATE;
    }
  } 
  
  if (NULL == A)
    return -1;

  B_mut.lock();

  if (!mixing || NULL == B) {
    B_mut.unlock();
    return A->get(user_ts, buffer, nb_samples);
  } else {
    if (l < 0.01) { // epsilon 
      // only play back from B
      int res = B->get(user_ts, buffer, nb_samples);
      if (res <= 0) { // B empty
	res = A->get(user_ts, buffer, nb_samples);
	mixing = false;
	if (IS_ONLY_ONCE)
	  B = NULL;
	else
	  B->rewind();
      }
      B_mut.unlock();
      return  res;
    } else {      // mix the two
      int res = 0;
      short* pdest = (short*)buffer;
      // get audio from A
      int len = A->get(user_ts, (unsigned char*)mix_buf, nb_samples);

      if ((len<0) && !IS_FINISH_B_MIX) { // A finished
	B_mut.unlock();
	return len;
      }
      for (int i=0; i<(PCM16_B2S(len)); i++) {
	pdest[i]=(short)(((double)mix_buf[i])*(1.0-l));
      }

      res = len;

      // clear the rest
      unsigned int len_from_a = 0;
      if (res>0)
	len_from_a=(unsigned int)res;
      
      if (PCM16_S2B(nb_samples) != len_from_a)
	memset((void*)&pdest[len_from_a>>1], 0, 
	       (nb_samples<<1) - len_from_a);
      
      // add audio from B
      len = B->get(user_ts, (unsigned char*)mix_buf, nb_samples);
      if (len<0) { // B finished
	mixing = false;
	
	if (IS_ONLY_ONCE)
	  B = NULL;
	else
	  B->rewind();
      } else {
	for (int i=0; i<(PCM16_B2S(len)); i++)  {
	    pdest[i]+=(short)(((double)mix_buf[i])*l);
	  }
	       if (len>res) // audio from B is longer than from A
		 res = len;
	     }
	B_mut.unlock();
	
	return res;
      }
    }
  }

  int AmAudioMixIn::put(unsigned int user_ts, unsigned char* buffer, unsigned int size) {
    ERROR("writing not supported\n");
    return -1;
  }

  void AmAudioMixIn::mixin(AmAudioFile* f) {
    B_mut.lock();
    B = f;
    mixing = next_start_ts_i = false; /* so that mix in will re-start */
    B_mut.unlock();
  }

