Actions: 
 conference.join(string roomname [, string mode])
   mode = "" | speakonly | listenonly
   Throws "conference" Exception if conference can not be joined (currently never).

 conference.leave()
   destroy conference channel. Close playlist first!!!!!
   * Sets $errno (script).

 conference.rejoin(string roomname [, string mode])
   mode = "" | speakonly | listenonly
   Throws "conference" Exception if conference can not be joined (currently never).

 conference.postEvent(string roomname, int event_id)
   * Sets $errno (arg).

 conference.setPlayoutType(string type)
   where type is one of ["adaptive", "jb", "simple"]
