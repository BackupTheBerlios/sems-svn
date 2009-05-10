/*
 * $Id$
 *
 * Copyright (C) 2002-2003 Fhg Fokus
 *
 * This file is part of sems, a free SIP media server.
 *
 * sems is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * For a license to use the ser software under conditions
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

#include "AmAudio.h"
#include "AmPlugIn.h"
#include "AmUtils.h"
#include "AmSdp.h"
#include "amci/codecs.h"
#include "log.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <typeinfo>

AmAudioFormat::AmAudioFormat()
  : channels(-1), rate(-1), advertised_rate(-1), codec(NULL),
    frame_length(20), frame_size(20*SYSTEM_SAMPLERATE/1000)
{

}

AmAudioSimpleFormat::AmAudioSimpleFormat(int codec_id)
  : AmAudioFormat(), codec_id(codec_id)
{
  codec = getCodec();
  rate = SYSTEM_SAMPLERATE;
  channels = 1;
}


AmAudioFormat::~AmAudioFormat()
{
  destroyCodec();
}

unsigned int AmAudioFormat::calcBytesToRead(unsigned int needed_samples) const
{
  if (codec && codec->samples2bytes)
    return codec->samples2bytes(h_codec, needed_samples) * channels;

  WARN("Cannot convert samples to bytes\n");
  return needed_samples * channels;
}

unsigned int AmAudioFormat::bytes2samples(unsigned int bytes) const
{
  if (codec && codec->bytes2samples)
    return codec->bytes2samples(h_codec, bytes) / channels;
  WARN("Cannot convert bytes to samples\n");
  return bytes / channels;
}

bool AmAudioFormat::operator == (const AmAudioFormat& r) const
{
  return ( codec && r.codec
	   && (r.codec->id == codec->id) 
	   && (r.bytes2samples(1024) == bytes2samples(1024))
	   && (r.channels == channels)
	   && (r.rate == rate));
}

bool AmAudioFormat::operator != (const AmAudioFormat& r) const
{
  return !(this->operator == (r));
}

void AmAudioFormat::initCodec()
{
  amci_codec_fmt_info_t fmt_i[4];

  fmt_i[0].id=0;

  if( codec && codec->init ) {
    if ((h_codec = (*codec->init)(sdp_format_parameters.c_str(), fmt_i)) == -1) {
      ERROR("could not initialize codec %i\n",codec->id);
    } else {
      string s; 
      int i=0;
      while (fmt_i[i].id) {
	switch (fmt_i[i].id) {
	case AMCI_FMT_FRAME_LENGTH : {
	  frame_length=fmt_i[i].value; 
	} break;
	case AMCI_FMT_FRAME_SIZE: {
	  frame_size=fmt_i[i].value; 
	} break;
	default: {
	  DBG("Unknown codec format descriptor: %d\n", fmt_i[i].id);
	} break;
	}
	i++;
      }
    }  
  } 
}

void AmAudioFormat::destroyCodec()
{
  if( codec && codec->destroy ){
    (*codec->destroy)(h_codec);
    h_codec = 0;
  }
  codec = NULL;
}

void AmAudioFormat::resetCodec() {
  codec = NULL;
  getCodec();
}

amci_codec_t* AmAudioFormat::getCodec()
{
  if(!codec){
    int codec_id = getCodecId();
    codec = AmPlugIn::instance()->codec(codec_id);

    initCodec();
  }
    
  return codec;
}

long AmAudioFormat::getHCodec()
{
  if(!codec)
    getCodec();
  return h_codec;
}

AmAudio::AmAudio()
  : fmt(new AmAudioSimpleFormat(CODEC_PCM16)),
    max_rec_time(-1),
    rec_time(0)
{
  initSrc();
}

AmAudio::AmAudio(AmAudioFormat *_fmt)
  : fmt(_fmt),
    max_rec_time(-1),
    rec_time(0)
{
  initSrc();
}

void AmAudio::initSrc() {
#ifdef USE_LIBSAMPLERATE   
  src_state_in = new src_state();
  src_state_out = new src_state();

  src_state_in->resample_state = NULL;
  src_state_in->resample_buf_samples = 0;
  src_state_out->resample_state = NULL;
  src_state_out->resample_buf_samples = 0;
#endif
}

AmAudio::~AmAudio()
{
#ifdef USE_LIBSAMPLERATE 
  if (NULL != src_state_in) {
    if (src_state_in->resample_state)
      src_delete(src_state_in->resample_state);
    delete src_state_in;
  }
  if (NULL != src_state_out) {
    if (src_state_out->resample_state) 
      src_delete(src_state_out->resample_state);
    
    delete src_state_out;
  }
#endif
}

void AmAudio::setFormat(AmAudioFormat* new_fmt) 
{
  fmt.reset(new_fmt);
  fmt->resetCodec();
}

void AmAudio::close()
{
}

// returns bytes read, else -1 if error (0 is OK)
int AmAudio::get(unsigned int user_ts, unsigned char* buffer, unsigned int time_millisec)
{
  unsigned int nb_samples = time_millisec * fmt->rate / 1000;
  
  int size = calcBytesToRead(nb_samples);
  size = read(user_ts,size);

  if (size <= 0)
    return size;

  size = decode(size);
  
  if (size < 0) {
    DBG("decode returned %i\n",size);
    return -1; 
  }

  /* into internal format */
  size = downMixChannels(size);

  size = downMixRate(size);

  if(size>0)
    memcpy(buffer,(unsigned char*)samples,size);

  return size;
}

// returns bytes written, else -1 if error (0 is OK)
int AmAudio::put(unsigned int user_ts, unsigned char* buffer, unsigned int size)
{
  if(!size){
    return 0;
  }

  if((max_rec_time > -1) && (rec_time >= max_rec_time))
    return -1;


  memcpy((unsigned char*)samples,buffer,size);

  /* from internal format */
  size = upMixRate(size);

  int s = encode(size);
  if (s<=0) 
    return s;
  
  unsigned long wr_ts = user_ts;

  // wr_ts =( (long)user_ts * (long)fmt->rate / (long)SYSTEM_SAMPLERATE);
  if (fmt->advertised_rate > 0 && (fmt->advertised_rate != SYSTEM_SAMPLERATE)) {
    if (fmt->advertised_rate > SYSTEM_SAMPLERATE) {
      unsigned int f = fmt->advertised_rate / SYSTEM_SAMPLERATE;
      wr_ts = wr_ts * f; 
    } else {
      unsigned int f = SYSTEM_SAMPLERATE / fmt->advertised_rate ;
      wr_ts = wr_ts / f; 
    }
  }

  incRecordTime(bytes2samples(size));
  return write(wr_ts,(unsigned int)s);
  
}

void AmAudio::stereo2mono(unsigned char* out_buf,unsigned char* in_buf,unsigned int& size)
{
  short* in  = (short*)in_buf;
  short* end = (short*)(in_buf + size);
  short* out = (short*)out_buf;
    
  while (in != end) {
    *(out++) = (*in + *(in+1)) / 2;
    in += 2;
  }

  size /= 2;
}

int AmAudio::decode(unsigned int size)
{
  int s = size;

  if(!fmt.get()){
    DBG("no fmt !\n");
    return s;
  }

  amci_codec_t* codec = fmt->getCodec();
  long h_codec = fmt->getHCodec();

  //     if(!codec){
  // 	ERROR("audio format set, but no codec has been loaded\n");
  // 	abort();
  // 	return -1;
  //     }

  if (codec->decode) {
    s = (*codec->decode)(samples.back_buffer(),samples,s,
			 fmt->channels,fmt->rate,h_codec);
    if (s<0) return s;
    samples.swap();
  }
    
  return s;
}

int AmAudio::encode(unsigned int size)
{
  int s = size;

  // todo: is his check needed? 
  //   if(!fmt.get()){
  //     DBG("no encode fmt\n");
  //     return 0;
  //   }

  amci_codec_t* codec = fmt->getCodec();
  long h_codec = fmt->getHCodec();

  if (codec->encode) {
    s = (*codec->encode)(samples.back_buffer(),samples,(unsigned int) size,
			 fmt->channels,fmt->rate,h_codec);
    if(s<0) return s;
    samples.swap();
  }
    
  return s;
}

unsigned int AmAudio::downMixRate(unsigned int size)
{
  unsigned int s = size;
  
#ifdef USE_LIBSAMPLERATE 
  if (fmt->rate != SYSTEM_SAMPLERATE) {
    s = resample(src_state_in, (double)SYSTEM_SAMPLERATE / (double)fmt->rate, s);
  }
#endif
 
  return s;
}

unsigned int AmAudio::downMixChannels(unsigned int size) {
  unsigned int s = size;

  if (fmt->channels == 2) {
    stereo2mono(samples.back_buffer(),(unsigned char*)samples,s);
    samples.swap();
  }

  return s;
}

unsigned int AmAudio::upMixRate(unsigned int size)
{
  unsigned int s = size;
  // todo: system_channels, convert from/to stereo ? 

#ifdef USE_LIBSAMPLERATE 
  if (fmt->rate != SYSTEM_SAMPLERATE) {
    s = resample(src_state_out, (double)fmt->rate / (double)SYSTEM_SAMPLERATE, s);
  }
#endif
 
  return s;
}

#ifdef USE_LIBSAMPLERATE 

unsigned int AmAudio::resample(src_state* m_src_state, double factor, unsigned int size) 
{
  if (!m_src_state)
    return size;

  unsigned int s = size;

  if (NULL == m_src_state->resample_state) {
    int src_error;
    // for better quality but more CPU usage, use SRC_SINC_ converters
    m_src_state->resample_state = src_new(SRC_LINEAR, 1, &src_error);
    if (!m_src_state->resample_state) {
      ERROR("samplerate initialization error");
      return size;
    }
  }
  
  if (m_src_state->resample_state) {
    if (m_src_state->resample_buf_samples + PCM16_B2S(s) > PCM16_B2S(AUDIO_BUFFER_SIZE) * 2) {
      WARN("resample input buffer overflow! (%u)\n",
	   m_src_state->resample_buf_samples + PCM16_B2S(s));
    } else {
      signed short* samples_s = (signed short*)(unsigned char*)samples;
      src_short_to_float_array(samples_s, &m_src_state->resample_in[m_src_state->resample_buf_samples], PCM16_B2S(s));
      m_src_state->resample_buf_samples += PCM16_B2S(s);
    }
    
    SRC_DATA src_data;
    src_data.data_in = m_src_state->resample_in;
    src_data.input_frames = m_src_state->resample_buf_samples;
    src_data.data_out = m_src_state->resample_out;
    src_data.output_frames = PCM16_B2S(AUDIO_BUFFER_SIZE);
    src_data.src_ratio = factor;
    src_data.end_of_input = 0;
    
    int src_err = src_process(m_src_state->resample_state, &src_data);
    if (src_err) {
      DBG("resample error: '%s'\n", src_strerror(src_err));
    }else {
      signed short* samples_s = (signed short*)(unsigned char*)samples;
      src_float_to_short_array(m_src_state->resample_out, samples_s, src_data.output_frames_gen);
      s = PCM16_S2B(src_data.output_frames_gen);
      
      if (m_src_state->resample_buf_samples !=  (unsigned int)src_data.input_frames_used) {
	memmove(m_src_state->resample_in, &m_src_state->resample_in[src_data.input_frames_used], 
		(m_src_state->resample_buf_samples - src_data.input_frames_used) * sizeof(float));
      }
      m_src_state->resample_buf_samples -= src_data.input_frames_used;
    }
  }

  return s;
}
#endif

unsigned int AmAudio::calcBytesToRead(unsigned int nb_samples) const
{
  return fmt->calcBytesToRead(nb_samples);
}

unsigned int AmAudio::bytes2samples(unsigned int bytes) const
{
  return fmt->bytes2samples(bytes);
}

void AmAudio::setRecordTime(unsigned int ms)
{
  max_rec_time = ms * (fmt->rate / 1000);
}

int AmAudio::incRecordTime(unsigned int samples)
{
  return rec_time += samples;
}


DblBuffer::DblBuffer()
  : active_buf(0)
{ 
}

DblBuffer::operator unsigned char*()
{
  return samples + (active_buf ? AUDIO_BUFFER_SIZE : 0);
}

unsigned char* DblBuffer::back_buffer()
{
  return samples + (active_buf ? 0 : AUDIO_BUFFER_SIZE);
}

void DblBuffer::swap()
{
  active_buf = !active_buf;
}
