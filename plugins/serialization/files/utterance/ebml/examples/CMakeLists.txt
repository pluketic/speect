######################################################################################
##                                                                                  ##
## AUTHOR  : Aby Louw                                                               ##
## DATE    : June 2010                                                              ##
##                                                                                  ##
######################################################################################
##                                                                                  ##
## CMakeList for EBML Utterance loading/saving examples                             ##
##                                                                                  ##
##                                                                                  ##
######################################################################################

speect_example(save_utt)
speect_example(load_utt)

if(NOT "${CMAKE_SPEECT_SOURCE_DIR}" STREQUAL "${CMAKE_SPEECT_BINARY_DIR}")
  speect_file_copy(${CMAKE_CURRENT_SOURCE_DIR}/save_utt.py
    ${CMAKE_CURRENT_BINARY_DIR}/)
endif(NOT "${CMAKE_SPEECT_SOURCE_DIR}" STREQUAL "${CMAKE_SPEECT_BINARY_DIR}")



