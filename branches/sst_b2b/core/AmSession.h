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

#ifndef _AmSession_h_
#define _AmSession_h_

#include "AmRtpStream.h"
#include "AmThread.h"
#include "AmEventQueue.h"
#include "AmRtpAudio.h"
#include "AmDtmfDetector.h"
#include "AmSipMsg.h"
#include "AmSipDialog.h"
#include "AmSipEvent.h"
#include "AmApi.h"
#include "AmSessionEventHandler.h"

#ifdef WITH_ZRTP
#include "zrtp/zrtp.h"
#endif

#include <string>
#include <vector>
#include <queue>
#include <map>
using std::string;
using std::vector;

class AmSessionFactory;
class AmDtmfEvent;

/** @file AmSession.h */

/* definition imported from Ser parser/msg_parser.h */
#define FL_FORCE_ACTIVE 2

#define AM_AUDIO_IN  0
#define AM_AUDIO_OUT 1

/**
 * \brief Implements the default behavior of one session
 * 
 * The session is identified by Call-ID, From-Tag and To-Tag.
 */
class AmSession : 
#ifndef SESSION_THREADPOOL
  public AmThread,
#endif
  public AmEventQueue, 
  public AmEventHandler,
  public AmSipDialogEventHandler
{
  AmMutex      audio_mut;
  // remote (to/from RTP) audio inout
  AmAudio*     input;
  AmAudio*     output;

  // local (to/from audio dev) audio inout
  AmAudio*     local_input;
  AmAudio*     local_output;

  bool use_local_audio[2];
protected:
  vector<SdpPayload *>  m_payloads;
  bool         negotiate_onreply;

  friend class AmRtpAudio;

  /** get new RTP format for the session */
  virtual AmAudioRtpFormat* getNewRtpFormat();

private:
  AmDtmfDetector   m_dtmfDetector;
  AmDtmfEventQueue m_dtmfEventQueue;
  bool m_dtmfDetectionEnabled;

  enum ProcessingStatus { 
    SESSION_PROCESSING_EVENTS,
    SESSION_WAITING_DISCONNECTED,
    SESSION_ENDED_DISCONNECTED
  };
  ProcessingStatus processing_status;

#ifndef SESSION_THREADPOOL
  /** @see AmThread::run() */
  void run();
  void on_stop();
#else
public:
  void start();
  bool is_stopped();

private:
  void stop();
  void* _pid;
#endif

  /** @return whether startup was successful */
  bool startup();

  /** @return whether session continues running */
  bool processingCycle();

  /** clean up session */
  void finalize();

  /** process pending events,  
      @return whether everything went smoothly */
  bool processEventsCatchExceptions();


  AmCondition<bool> sess_stopped;
  AmCondition<bool> detached;

  static volatile unsigned int session_num;
  static AmMutex session_num_mut;

  friend class AmMediaProcessor;
  friend class AmMediaProcessorThread;
  friend class AmSessionContainer;
  friend class AmSessionFactory;
  friend class AmSessionProcessorThread;

  auto_ptr<AmRtpAudio> _rtp_str;
  
protected:
  AmSdp               sdp;

  /** this is the group the media is processed with 
      - by default local tag */
  string callgroup;

  /** do accept early session? */
  bool accept_early_session;

  vector<AmSessionEventHandler*> ev_handlers;

public:

  AmRtpAudio* RTPStream();

#ifdef WITH_ZRTP
  zrtp_conn_ctx_t*    zrtp_session; // ZRTP session
  zrtp_stream_ctx_t*  zrtp_audio;   // ZRTP stream for audio

  /** must be set before session is started! i.e. in constructor */
  bool enable_zrtp;
#endif

  AmSipDialog         dlg;

  /** 
   * \brief Exception occured in a Session
   * 
   * Session (creation) should be aborted and replied with code/reason.
   */
  struct Exception {
    int code;
    string reason;
    Exception(int c, string r) : code(c), reason(r) {}
  };

  /** 
   * Session constructor.
   */
  AmSession();

  virtual ~AmSession();

  /**
   * @see AmEventHandler
   */
  virtual void process(AmEvent*);

  /**
   * add a handler which will be called 
   * for all events in session
   * 
   * @see AmSessionEventHandler
   */
  void addHandler(AmSessionEventHandler*);

  /**
   * Set the call group for this call. 
   * 
   * Note: this must be set before inserting 
   * the session to the scheduler!
   */
  void setCallgroup(const string& cg);
  string getCallgroup() { return callgroup; }

  /** This function removes the session from 
   *  the media processor and adds it again. 
   */
  void changeCallgroup(const string& cg);

  /**
   * Accept the SDP proposal
   * thus setting up audio stream
   */
  int acceptAudio(const string& body,
		  const string& hdrs = "",
		  string*       sdp_reply=0);

  /**
   * Lock audio input & output
   * (inclusive RTP stream)
   */
  void lockAudio();
  /**
   * Unlock audio input & output
   * (inclusive RTP stream)
   */
  void unlockAudio();

  /**
   * Audio input getter .
   * Note: audio must be locked!
   */
  AmAudio* getInput() { return input; }
  /**
   * Audio output getter.
   * Note: audio must be locked!
   */
  AmAudio* getOutput(){ return output;}

  /**
   * Audio input & output set methods.
   * Note: audio will be locked by the methods.
   */
  void setInput(AmAudio* in);
  void setOutput(AmAudio* out);
  void setInOut(AmAudio* in, AmAudio* out);


  /**
   * Local audio input getter .
   * Note: audio must be locked!
   */
  AmAudio* getLocalInput() { return local_input; }
  /**
   * Local audio output getter.
   * Note: audio must be locked!
   */
  AmAudio* getLocalOutput(){ return local_output;}

  /**
   * Local audio input & output set methods.
   * Note: audio will be locked by the methods.
   */
  void setLocalInput(AmAudio* in);
  void setLocalOutput(AmAudio* out);
  void setLocalInOut(AmAudio* in, AmAudio* out);

  /** this switches between local and remote 
   * audio inout 
   */
  void setAudioLocal(unsigned int dir, bool local);
  bool getAudioLocal(unsigned int dir);

  /**
   * Clears input & ouput (no need to lock)
   */
  void clearAudio();

  /** setter for rtp_str->mute */
  void setMute(bool mute) { RTPStream()->mute = mute; }

  /** setter for rtp_str->receiving */
  void setReceiving(bool receive) { RTPStream()->receiving = receive; }

  /** Gets the Session's call ID */
  const string& getCallID() const;

  /** Gets the Session's remote tag */
  const string& getRemoteTag()const ;

  /** Gets the Session's local tag */
  const string& getLocalTag() const;

  /** Sets the Session's local tag if not set already */
  void setLocalTag();

  /** Sets the Session's local tag */
  void setLocalTag(const string& tag);

  /** Sets the URI for the session */
  void setUri(const string& uri);

  /** Gets the current RTP payload */
  const vector<SdpPayload*>& getPayloads();

  /** Gets the port number of the remote part of the session */
  int getRPort();

  /** Set whether on positive reply session should be negotiated */
  void setNegotiateOnReply(bool n) { negotiate_onreply = n; }

  /** get the payload provider for the session */
  virtual AmPayloadProviderInterface* getPayloadProvider();

  /** handle SDP negotiation: only for INVITEs & re-INVITEs */
  virtual void negotiate(const string& sdp_body,
			 bool force_symmetric_rtp,
			 string* sdp_reply);

  /** send an UPDATE in the session */
  virtual void sendUpdate();
  /** send a Re-INVITE (if connected) */
  virtual void sendReinvite(bool updateSDP = true, const string& headers = "");
  /** send an INVITE */
  virtual int sendInvite(const string& headers = "");

  /** set the session on/off hold */
  virtual void setOnHold(bool hold);

  /**
   * Destroy the session.
   * It causes the session to be erased from the active session list
   * and added to the dead session list.
   * @see AmSessionContainer
   */
  virtual void destroy();

  /**
   * Signals the session it should stop.
   * This will cause the session to be able 
   * to exit the main loop.
   * If wakeup is set, a bogus event will 
   * be sent to wake up the session.
   */
  void setStopped(bool wakeup = false);

  /**
   * Has the session already been stopped ?
   */
  bool getStopped() { return sess_stopped.get(); }

  /** Is the session detached from media processor? */
  bool getDetached() { return detached.get(); }

  /**
   * Creates a new Id which can be used within sessions.
   */
  static string getNewId();

  /**
   * Gets the number of running sessions
   */
  static unsigned int getSessionNum();

  /**
   * Entry point for DTMF events
   */
  void postDtmfEvent(AmDtmfEvent *);

  void processDtmfEvents();

  void setInbandDetector(Dtmf::InbandDetectorType t);
  bool isDtmfDetectionEnabled() { return m_dtmfDetectionEnabled; }
  void setDtmfDetectionEnabled(bool e) { m_dtmfDetectionEnabled = e; }
  void putDtmfAudio(const unsigned char *buf, int size, int user_ts);
  /** event handler for apps to use*/
  virtual void onDtmf(int event, int duration);

  /**
   * onStart will be called before everything else.
   */
  virtual void onStart(){}

  /**
   * onInvite will be called if an INVITE or re-INVITE
   * has been received for the session.
   */
  virtual void onInvite(const AmSipRequest& req);

  /**
   * onOutgoingInvite will be called if an INVITE 
   * is sent in the session.
   */
  virtual void onOutgoingInvite(const string& headers) { }

  /**
   * onCancel will be called if a CANCEL for a running
   * dialog has been received. At this point, the CANCEL
   * transaction has been replied with 200.
   *
   * A normal plug-in does not have to do anything special, 
   * as normal dialogs are immediatly replied with 200 
   * or error code. 
   *
   * Note: You are still responsible for responding the 
   *       initial transaction.
   */
  virtual void onCancel(){}

  /**
   * onSessionStart will be called after call setup.
   *
   * Throw AmSession::Exception if you want to 
   * signal any error.
   * 
   * Warning:
   *   Sems will NOT send any BYE on his own.
   */
  virtual void onSessionStart(const AmSipRequest& req){}

  /**
   * onSessionStart method for calls originating 
   * from SEMS.
   *
   * Throw AmSession::Exception if you want to 
   * signal any error.
   * 
   * Warning:
   *   Sems will NOT send any BYE on his own.
   */
  virtual void onSessionStart(const AmSipReply& reply){}


  /**
   * onEarlySessionStart will be called after 
   * 183 early media reply is received and early session 
   * is setup, if accept_early_session is set.
   */
  virtual void onEarlySessionStart(const AmSipReply& reply){}

  /**
   * onRinging will be called after 180 is received. 
   * If local audio is set up, session is added to scheduler. 
   */
  virtual void onRinging(const AmSipReply& reply){}

  /**
   * @see AmDialogState
   */
  virtual void onBye(const AmSipRequest& req);

  /**
   * Entry point for SIP events
   */
  virtual void onSipEvent(AmSipEvent* sip_ev);
  /** Entry point for SIP Requests   */
  virtual void onSipRequest(const AmSipRequest& req);
  /** Entry point for SIP Replies   */
  virtual void onSipReply(const AmSipReply& reply);


  /**
   * entry point for system events
   */
  virtual void onSystemEvent(AmSystemEvent* ev);
  
#ifdef WITH_ZRTP
  /**
   * ZRTP events @see ZRTP
   */
  virtual void onZRTPEvent(zrtp_event_t event, zrtp_stream_ctx_t *stream_ctx);
#endif

  /** This callback is called if RTP timeout encountered */
  virtual void onRtpTimeout();

  /* Called by AmSipDialog when a request is sent */
  virtual void onSendRequest(const string& method,
			     const string& content_type,
			     const string& body,
			     string& hdrs,
			     int flags,
			     unsigned int cseq);

  /* Called by AmSipDialog when a reply is sent */
  virtual void onSendReply(const AmSipRequest& req,
			   unsigned int  code,
			   const string& reason,
			   const string& content_type,
			   const string& body,
			   string& hdrs,
			   int flags);

  /** 
   * called in the session thread before the session is destroyed,
   * i.e. after the main event loop has finished
   */
  virtual void onBeforeDestroy() { }

  // The IP address to put as c= in SDP bodies and to use for Contact:.
  string advertisedIP();
};

inline AmRtpAudio* AmSession::RTPStream() {
  if (NULL == _rtp_str.get()) {
    DBG("creating RTP stream instance for session [%p]\n", 
	this);
    _rtp_str.reset(new AmRtpAudio(this));
  }
  return _rtp_str.get();
}

#endif

// Local Variables:
// mode:C++
// End:

