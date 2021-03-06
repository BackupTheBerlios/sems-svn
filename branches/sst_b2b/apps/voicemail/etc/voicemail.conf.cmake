#
# optional parameter: smtp_server=<hostname>
#
# - sets address of smtp server
#
# Default: localhost
#smtp_server=mail

# optional parameter: smtp_port=<port>
#
# - sets port of smtp server
#
# Default: 25
#smtp_port=25

# if set, this overrides the email address, meaning that
# the voicemail will always be sent to that address.
# email_address = blub@mail.domain.net
#

# These are needed if you keep audio and template files in file system:
#
# path from which announcements are played:
announce_path=${SEMS_AUDIO_PREFIX}/sems/audio/voicemail/
#
# announcement played if no user/domain specific prompt found
default_announce=default_en.wav
#
# path for email templates:
email_template_path=${SEMS_CFG_PREFIX}/etc/sems/

# These are needed if you keep audio and template files in MySQL database:
#mysql_host=localhost
#mysql_user=sems
#mysql_passwd=sems
#mysql_db=sems

# These are independent on where audio and template files are kept:

#
# maximum voicemail message length, in seconds.
# After the maximum time, the call is ended by SEMS.
#
max_record_time=30

#
# optional: minimum voicemail message length, in milliseconds
# no voicemail is sent if the recorded message is shorter than
# or equal to the value set here.
# example - do not send voicemail shorter than 1.5s: 
#  min_record_time=1500
# default: 0
#min_record_time=0

# rec_file_ext : extension to recorded file, determines file
# format (wav, mp3; need mp3 plugin loaded for mp3)
# default is 'wav'
#
# example: 
#   rec_file_ext=mp3
#

# try_personal_greeting=[yes|no]
# optional: try to get a personal greeting from msg_storage mod?
# these can be recorded with annrecorder app
#
# Default:
#  try_personal_greeting=no

# mail_header_vars=var1;var2;var3;...
#
# Variables that are substituted in the email template with 
# values from App-Param header (comma-separated list).
# 
# Example:
#   mail_header_vars=msg_count;sender_count
#
#   P-App-Param: user=...;eml=...;msg_count=10;sender_count=2
#
#   email.template: In your voicebox are %msg_count% messages, 
#                   %sender_count% from this sender.
#
# Default: none
