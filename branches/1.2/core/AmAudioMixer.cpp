/*
 * $Id$
 *
 * Copyright (C) 2008 IPTEGO GmbH
 *
 * This file is part of sems, a free SIP media server.
 *
 * sems is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
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

#include "AmAudioMixer.h"

AmAudioMixer::AmAudioMixer() {
  sink_channel = mixer.addChannel();
  sink_connector = 
    new AmAudioMixerConnector(mixer, sink_channel, NULL, 
			      &srcsink_mut, &sinks);
}

AmAudioMixer::~AmAudioMixer() {
  mixer.removeChannel(sink_channel);
  for (std::map<AmAudioMixerConnector*, unsigned int>::iterator 
	 it=sources.begin(); it != sources.end(); it++) {    
    mixer.removeChannel(it->second);
    delete it->first;
  }
  delete sink_connector;
}

AmAudio* AmAudioMixer::addSource() {
  srcsink_mut.lock();
  unsigned int src_channel = mixer.addChannel();
  // the first source will process the media in the mixer channel
  AmAudioMixerConnector* conn = 
    new AmAudioMixerConnector(mixer, src_channel, 
			      sources.empty() ? sink_connector : NULL);
  sources[conn] = src_channel;
  srcsink_mut.unlock();
  return conn;
}

void AmAudioMixer::releaseSource(AmAudio* s) {
  srcsink_mut.lock();
  std::map<AmAudioMixerConnector*, unsigned int>::iterator it=
    sources.find((AmAudioMixerConnector*)s);
  if (it==sources.end()) {
    srcsink_mut.unlock();
    ERROR("source [%p] is not part of this mixer.\n", s);
    return;
  }
  mixer.removeChannel(it->second);
  delete s;
  sources.erase(it);
  srcsink_mut.unlock();
}

void AmAudioMixer::addSink(AmAudio* s) {
  srcsink_mut.lock();
  sinks.insert(s);
  srcsink_mut.unlock();
}

void AmAudioMixer::releaseSink(AmAudio* s) {
  srcsink_mut.lock();
  sinks.erase(s);
  srcsink_mut.unlock();
}

int AmAudioMixerConnector::get(unsigned int user_ts, unsigned char* buffer, unsigned int nb_samples) {
  // in fact GCP here only needed for the mixed channel
  mixer.GetChannelPacket(channel, user_ts, buffer, nb_samples);

  if ((audio_mut != NULL) && (sinks != NULL)) {
    audio_mut->lock();
    // write to all sinks
    for (std::set<AmAudio*>::iterator it=sinks->begin(); 
	 it != sinks->end(); it++) {
      (*it)->put(user_ts, buffer, nb_samples);
    }
    audio_mut->unlock();
  }

  return nb_samples;
}

int AmAudioMixerConnector::put(unsigned int user_ts, unsigned char* buffer, unsigned int size) {
  mixer.PutChannelPacket(channel, user_ts, buffer, size);

  if (mix_channel != NULL) {
    // we are processing the media of the mixed channel as well
    ShortSample mix_buffer[SIZE_MIX_BUFFER];    
    mix_channel->get(user_ts, (unsigned char*)mix_buffer, size);
  }
  return size;
}
