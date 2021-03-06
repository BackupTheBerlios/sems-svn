
# diagrams (DSM descriptions)
diag_path=${SEMS_EXEC_PREFIX}/${SEMS_LIBDIR}/sems/dsm/
load_diags=inbound_call,outbound_call

# register these diagrams as 'application' in SEMS
# so it can be used in application= in sems.conf
#register_apps=inbound_call,outbound_call

# for import(mod_name)
mod_path=${SEMS_EXEC_PREFIX}/${SEMS_LIBDIR}/sems/dsm/

# preload modules which have a preload (init) function
#preload_mods=uri

# print raw DSM text while loading to debug log?
# debug_raw_dsm=yes

# DSM to start for in/outbound call if application to execute=dsm
# (from application=xyz in sems.conf, either application=dsm or 
#  application=$(apphdr)/$(ruriparam) etc)
#
# use $(mon_select) to search for call info from monitoring
#  e.g. inbound_start_diag=$(mon_select) and see 
#  below monitor_select options
#
inbound_start_diag=inbound_call
outbound_start_diag=outbound_call

# prompts files (for prompt collection
load_prompts=${SEMS_CFG_PREFIX}/etc/sems/etc/dsm_in_prompts.conf,${SEMS_CFG_PREFIX}/etc/sems/etc/dsm_out_prompts.conf

# prompts which must be loaded (or else...)
#required_prompts=welcome,error

#load prompt sets config from this path
#prompts_sets_path=${SEMS_CFG_PREFIX}/etc/sems/etc/

# load files with this name (+.conf), e.g. mydomain.conf
#load_prompts_sets=mydomain

# if run_invite_event is set to "yes", an "invite" event is run
# before the session is started. can be used e.g. for early media
# of delaying final reply
#
#run_invite_event=yes

# set_param_variables controls whether application parameters
# from the P-App-Param header are set as variables in the DSM 
# dialog. Default: no
#
#set_param_variables=yes

# monitoring_full_stategraph=[yes|no]
# 
# Controls whether to log the full call graph (all states visited)
# to the monitoring record. Note this may take some performance
# and use some memory.
#
# Default: no
#
#monitoring_full_stategraph=yes

# monitoring_full_transitions=[yes|no]
# 
# Controls whether to log the full call graph (all transitions)
# to the monitoring record. Note this may take some performance
# and use some memory.
#
# Default: no
#
#monitoring_full_transitions=yes

# monitor_select_use_caller=[no|from|pai]
#
# for $(mon_select) application selection from monitoring:
#  select application by caller? 
#   from: caller==remote_uri.user (From URI user part)
#   pai:  caller==P-Asserted-Identity user part
#   no:   caller not regarded 
#
# Default: from
#
#monitor_select_use_caller=no

# monitor_select_use_callee=[no|ruri|to]
#
# for $(mon_select) application selection from monitoring:
#  select application by callee? 
#   ruri: callee==r_uri.user (request URI user part)
#   to:   callee==local_uri.user (To URI user part)
#   no:   callee not regarded
#
# Default: ruri
#
#monitor_select_use_callee=no

