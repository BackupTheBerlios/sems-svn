set (dsm_SRCS
DSM.cpp
DSMCall.cpp
DSMChartReader.cpp
DSMCoreModule.cpp
DSMElemContainer.cpp
DSMModule.cpp
DSMSession.cpp
DSMStateDiagramCollection.cpp
DSMStateEngine.cpp
)

ADD_SUBDIRECTORY (lib)
ADD_SUBDIRECTORY (mods)

SET(sems_module_name dsm)
SET(sems_config_files dsm_in_prompts dsm_out_prompts)
SET(doc_files
	../../doc/dsm/Readme.dsm.txt
	../../doc/dsm/dsm_syntax.txt
	../../doc/dsm/dsm_todo.txt
)
SET(doc_dirs ../../doc/dsm/examples)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/config.rules.txt)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/doc.rules.txt)
