
#include "EarlyRecord.h"

#include "log.h"
#include "AmConfigReader.h"
#include "AmUtils.h"
#include "AmPlugIn.h"

#define MOD_NAME "early_record"

EXPORT_SESSION_FACTORY(EarlyRecordFactory,MOD_NAME);

string EarlyRecordFactory::RecordDir;

EarlyRecordFactory::EarlyRecordFactory(const string& _app_name)
  : AmSessionFactory(_app_name)
{
}

int EarlyRecordFactory::onLoad()
{
    return 0;
}

AmSession* EarlyRecordFactory::onInvite(const AmSipRequest& req)
{
  return new EarlyRecordDialog(NULL);
}

// auth with di_dial
AmSession* EarlyRecordFactory::onInvite(const AmSipRequest& req,
					AmArg& session_params)
{
  UACAuthCred* cred = NULL;
  if (session_params.getType() == AmArg::AObject) {
    ArgObject* cred_obj = session_params.asObject();
    if (cred_obj)
      cred = dynamic_cast<UACAuthCred*>(cred_obj);
  }
  
  AmSession* s = new EarlyRecordDialog(cred); 
  
  if (NULL == cred) {
    WARN("discarding unknown session parameters.\n");
  } else {
    AmSessionEventHandlerFactory* uac_auth_f = 
      AmPlugIn::instance()->getFactory4Seh("uac_auth");
    if (uac_auth_f != NULL) {
      DBG("UAC Auth enabled for new announcement session.\n");
      AmSessionEventHandler* h = uac_auth_f->getHandler(s);
      if (h != NULL )
	s->addHandler(h);
    } else {
      ERROR("uac_auth interface not accessible. "
	    "Load uac_auth for authenticated dialout.\n");
    }		
  }

  return s;
}

EarlyRecordDialog::EarlyRecordDialog(UACAuthCred* credentials)
: cred(credentials)
{
  // this sets the session to run onEarlySessionStart
  accept_early_session = true;
}

EarlyRecordDialog::~EarlyRecordDialog()
{
}

void EarlyRecordDialog::onEarlySessionStart(const AmSipReply& req) {
  DBG("Early Session Start\n");
  msg_filename = "/tmp/" + getLocalTag() + ".wav";
  
  if(a_msg.open(msg_filename,AmAudioFile::Write,false))
    throw string("EarlyRecordDialog: couldn't open ") + 
      msg_filename + string(" for writing");
  
  setInput(&a_msg);
  setMute(true);
}

void EarlyRecordDialog::onSessionStart(const AmSipReply& req)
{
  setInOut(NULL, NULL);

  a_msg.close();

  // replay the recorded early media
  msg_filename = "/tmp/" + getLocalTag() + ".wav";
  
  if(a_msg.open(msg_filename,AmAudioFile::Read,false))
    throw string("EarlyRecordDialog: couldn't open ") + 
      msg_filename + string(" for writing");

  setOutput(&a_msg);
}

void EarlyRecordDialog::onBye(const AmSipRequest& req)
{
    DBG("onBye: stopSession\n");
    setStopped();
}

void EarlyRecordDialog::process(AmEvent* event)
{

  AmAudioEvent* audio_event = dynamic_cast<AmAudioEvent*>(event);
  if(audio_event && (audio_event->event_id == AmAudioEvent::cleared)){
    dlg.bye();
    setStopped();
    return;
  }

  AmSession::process(event);
}

inline UACAuthCred* EarlyRecordDialog::getCredentials() {
  return cred.get();
}
