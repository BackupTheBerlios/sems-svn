set (annrecorder_SRCS
AnnRecorder.cpp
)

set (audio_files
beep.wav
bye.wav
confirm.wav
greeting_set.wav
to_record.wav
welcome.wav
your_prompt.wav
)

SET(sems_module_name annrecorder)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/audio.rules.txt)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/config.rules.txt)
