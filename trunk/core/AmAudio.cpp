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

/** \brief structure to hold loaded codec instances */
struct CodecContainer
{
  amci_codec_t *codec;
  int frame_size;
  int frame_length;
  int frame_encoded_size;
  long h_codec;
};

AmAudioRtpFormat::AmAudioRtpFormat(const vector<SdpPayload *>& payloads)
  : AmAudioFormat(), m_payloads(payloads), m_currentPayload(-1)
{
  for (vector<SdpPayload *>::iterator it = m_payloads.begin();
	  it != m_payloads.end(); ++it)
  {
    m_sdpPayloadByPayload[(*it)->payload_type] = *it;
  }
  setCurrentPayload(m_payloads[0]->payload_type);
}

int AmAudioRtpFormat::setCurrentPayload(int payload)
{
  if (m_currentPayload != payload)
  {
    std::map<int, SdpPayload *>::iterator p = m_sdpPayloadByPayload.find(payload);
    if (p == m_sdpPayloadByPayload.end())
    {
      ERROR("Could not find payload <%i>\n", payload);
      return -1;
    }
    std::map<int, amci_payload_t *>::iterator pp = m_payloadPByPayload.find(payload);
    if (pp == m_payloadPByPayload.end())
    {
      m_currentPayloadP = AmPlugIn::instance()->payload(p->second->int_pt);
      if (m_currentPayloadP == NULL)
      {
	ERROR("Could not find payload <%i>\n", payload);
	return -1;
      }
      m_payloadPByPayload[payload] = m_currentPayloadP;
    }
    else
      m_currentPayloadP = pp->second;
    m_currentPayload = payload;
    sdp_format_parameters = p->second->sdp_format_parameters;

    std::map<int, CodecContainer *>::iterator c = m_codecContainerByPayload.find(payload);
    if (c == m_codecContainerByPayload.end())
    {
      codec = NULL;
      getCodec();
      if (codec)
      {
	CodecContainer *cc = new CodecContainer();
	cc->codec = codec;
	cc->frame_size = frame_size;
	cc->frame_length = frame_length;
	cc->frame_encoded_size = frame_encoded_size;
	cc->h_codec = h_codec;
	m_codecContainerByPayload[payload] = cc;
      }
    }
    else
    {
      codec = c->second->codec;
      frame_size = c->second->frame_size;
      frame_length = c->second->frame_length;
      frame_encoded_size = c->second->frame_encoded_size;
      h_codec = c->second->h_codec;
    }
    if (m_currentPayloadP && codec) {
      channels = m_currentPayloadP->channels;
      rate = m_currentPayloadP->sample_rate;
    } else {
      ERROR("Could not find payload <%i>\n", payload);
      return -1;
    }
  }
  return 0;
}

AmAudioRtpFormat::~AmAudioRtpFormat()
{
  for (std::map<int, CodecContainer *>::iterator it = m_codecContainerByPayload.begin(); it != m_codecContainerByPayload.end(); ++it)
    delete it->second;
}

AmAudioFormat::AmAudioFormat()
  : channels(-1), rate(-1), codec(NULL),
    frame_length(20), frame_size(160), frame_encoded_size(320)
{

}

AmAudioSimpleFormat::AmAudioSimpleFormat(int codec_id)
  : AmAudioFormat(), codec_id(codec_id)
{
  codec = getCodec();
  rate = 8000;
  channels = 1;
}


AmAudioFormat::~AmAudioFormat()
{
  destroyCodec();
}

unsigned int AmAudioFormat::calcBytesToRead(unsigned int needed_samples) const
{
  if (codec && codec->samples2bytes)
    return codec->samples2bytes(h_codec, needed_samples) * channels; // FIXME: channels

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
	case AMCI_FMT_ENCODED_FRAME_SIZE: {
	  frame_encoded_size=fmt_i[i].value; 
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
}

AmAudio::AmAudio(AmAudioFormat *_fmt)
  : fmt(_fmt),
    max_rec_time(-1),
    rec_time(0)
{
}

AmAudio::~AmAudio()
{
}

void AmAudio::close()
{
}

// returns bytes read, else -1 if error (0 is OK)
int AmAudio::get(unsigned int user_ts, unsigned char* buffer, unsigned int nb_samples)
{
  int size = calcBytesToRead(nb_samples);

  size = read(user_ts,size);
  //DBG("size = %d\n",size);
  if(size <= 0){
    return size;
  }

  size = decode(size);
  if(size < 0) {
    DBG("decode returned %i\n",size);
    return -1; 
  }
  size = downMix(size);
    
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

  if(max_rec_time > -1 && rec_time >= max_rec_time)
    return -1;


  memcpy((unsigned char*)samples,buffer,size);

  unsigned int s = encode(size);
  if(s>0){
    //DBG("%s\n",typeid(this).name());
    incRecordTime(bytes2samples(size));
    return write(user_ts,(unsigned int)s);
  }
  else{
    return s;
  }
}

void AmAudio::stereo2mono(unsigned char* out_buf,unsigned char* in_buf,unsigned int& size)
{
  short* in  = (short*)in_buf;
  short* end = (short*)(in_buf + size);
  short* out = (short*)out_buf;
    
  while(in != end){
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

  if(codec->decode){
    s = (*codec->decode)(samples.back_buffer(),samples,s,
			 fmt->channels,fmt->rate,h_codec);
    if(s<0) return s;
    samples.swap();
  }
    
  return s;
}

int AmAudio::encode(unsigned int size)
{
  int s = size;

  //     if(!fmt.get()){
  // 	DBG("no encode fmt\n");
  // 	return 0;
  //     }

  amci_codec_t* codec = fmt->getCodec();
  long h_codec = fmt->getHCodec();

  if(codec->encode){
    s = (*codec->encode)(samples.back_buffer(),samples,(unsigned int) size,
			 fmt->channels,fmt->rate,h_codec);
    if(s<0) return s;
    samples.swap();
  }
    
  return s;
}

unsigned int AmAudio::downMix(unsigned int size)
{
  unsigned int s = size;
  if(fmt->channels == 2){
    stereo2mono(samples.back_buffer(),(unsigned char*)samples,s);
    samples.swap();
  }

  return s;
}

unsigned int AmAudio::getFrameSize()
{

  if (!fmt.get())
    fmt.reset(new AmAudioSimpleFormat(CODEC_PCM16));

  return fmt->frame_size;
}

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



int AmAudioRtpFormat::getCodecId()
{
  if(!m_currentPayloadP){
    ERROR("AmAudioRtpFormat::getCodecId: could not find payload %i\n", m_currentPayload);
    return -1;
  }
  else 
    return m_currentPayloadP->codec_id;
}
