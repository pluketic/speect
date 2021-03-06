######################################################################################
##                                                                                  ##
## AUTHOR  : Aby Louw                                                               ##
## DATE    : 23 May 2012                                                            ##
##                                                                                  ##
######################################################################################
##                                                                                  ##
## CMakeList for Hunpos POS utterance processor plug-in                             ##
##                                                                                  ##
##                                                                                  ##
######################################################################################

find_package(hunpos REQUIRED)

#------------------------------------------------------------------------------------#
#                             Define plug-in                                         #
#------------------------------------------------------------------------------------#

speect_plugin_definition(Hunpos_UttProc "SHunposUttProc" 1 0 0)


#------------------------------------------------------------------------------------#
#                        Configure plugin_info.h.in                                  #
#------------------------------------------------------------------------------------#

set(description "A part-of-speech Utterance Processor using the hunpos HMM tagger.")

# Minimum required Speect Engine version 
set(major_min 1)
set(minor_min 0)

speect_plugin_configure_info(${description} ${major_min} ${minor_min})


#------------------------------------------------------------------------------------#
#                               Source files                                         #
#------------------------------------------------------------------------------------#

# Lists of all the source files
speect_plugin_include_dirs(${HUNPOS_INCLUDE_DIRS})

include(sources)

#------------------------------------------------------------------------------------#
#                        Plug-in shared object                                       #
#------------------------------------------------------------------------------------#

speect_plugin_create(${HUNPOS_LIBRARIES} "m" "dl")


