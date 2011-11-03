######################################################################################
##                                                                                  ##
## AUTHOR  : Aby Louw                                                               ##
## DATE    : June 2010                                                              ##
##                                                                                  ##
######################################################################################
##                                                                                  ##
## CMake custom functions for Speect plug-ins SWIG Python interfaces                ##
##                                                                                  ##
##   speect_plugin_swig_python_flags        (Set the SWIG Python compile flags)     ##
##   speect_plugin_swig_python_interface    (Generate SWIG interface file)          ##
##   speect_plugin_swig_python_loader       (Generate SWIG interface file that      ##
##                                           only loads plug-in when imported)      ##
##   speect_find_python                     (Find Python libraries)                 ##
##   speect_plugin_swig_python_wrapper      (Generate SWIG Python wrapper)          ##
##   speect_include_python_directories      (Include Python include directories)    ##
##                                                                                  ##
## See the documentation at each function/macro below.                              ##
##                                                                                  ##
######################################################################################


#------------------------------------------------------------------------------------#
#                     Set the SWIG Python compile flags                              #
#------------------------------------------------------------------------------------#
#
# macro(speect_plugin_swig_python_flags)
#
# Set the SWIG Python compile flags. Call this macro and the ``SPCT_SWIG_FLAGS``
# will be available in your function. This is here so that there is only one place
# where we need to modify the flags if need be.
#
macro(speect_plugin_swig_python_flags)
  # flags for the SWIG generation
  if(WANT_PYTHON_3)
    # extra -py3 and -DSPCT_SWIG_PYTHON_3 flags for Python 3.x
    list(APPEND SPCT_SWIG_FLAGS -Wall -Werror -py3 -DSPCT_SWIG_PYTHON_3)
  else(WANT_PYTHON_3)
    list(APPEND SPCT_SWIG_FLAGS -Wall -Werror)
  endif(WANT_PYTHON_3)
endmacro(speect_plugin_swig_python_flags)


#------------------------------------------------------------------------------------#
#                       Create plug-in SWIG Python interface                         #
#------------------------------------------------------------------------------------#
#
# speect_plugin_swig_python_interface()
#
# Create a plug-in interface file plugin_lowercase_name.i 
# The file will be created in the CMAKE_CURRENT_BINARY_DIR
# 
# The following variables need to be set before calling this macro:
#
# :param swig_classname: The name of the class for which this SWIG Python interface is
#                        defined.
# :type swig_classname: string
# :param swig_includes: SWIG includes (e.g. typemaps.i, exception.i, ...)
# :type swig_includes: CMake list
# :param swig_c_includes: C header files (speect.h and py_native.h are automatically included)
# :type swig_c_includes: CMake list
# :param swig_load_plugin: Whether to load the plug-in on initialization.
# :type swig_load_plugin: bool (0 or 1)
# :param swig_python_docstrings: SWIG Python interfaces files, containing docstrings for
#                                the plug-in class functions.
# :type swig_python_docstrings: CMake list
# :param swig_python_typemaps: SWIG Python typemap files, containing typemaps for
#                               the plug-in class functions.
# :type swig_python_typemaps: CMake list
# :param swig_common: SWIG common interface files, containing SWIG language independent
#                     interface definitions for the plug-in class functions.
# :type swig_common: CMake list
# :param swig_python: SWIG Python interface files, containing SWIG Python specific
#                     interface definitions for the plug-in class functions.
# :type swig_python: CMake list
#
# Note that only swig_classname is required.
#

macro(speect_plugin_swig_python_interface)
  set(filename "${CMAKE_CURRENT_BINARY_DIR}/${plugin_lowercase_name}.i")
  set(swig_deps)  # dependencies of interface file.
  
  # write header info 
  file(WRITE ${filename} "/* The ${swig_classname} plug-in SWIG/Python interface definition. */\n\n")
  file(APPEND ${filename} "/*\n * ${plugin_lowercase_name}.i is auto generated by the \"speect_plugin_swig_python_interface\"\n")
  file(APPEND ${filename} " * macro in speect/swig/python/cmake/swigPythonFunctions.cmake, do not edit ${plugin_lowercase_name}.i\n */\n\n")

  # docstring
  file(APPEND ${filename} "%module(docstring=\"Speect Plug-in for ${swig_classname} class\") @plugin_lowercase_name@\n\n")

  # version and license info
  file(APPEND ${filename} "%pythoncode\n")
  file(APPEND ${filename} "%{\n\t__version__ = \"${${plugin_lowercase_name}_VERSION}\"\n\t__license__= \"mit\"\n%}\n\n")

  # SWIG includes
  if(swig_includes)
    file(APPEND ${filename} "/* SWIG includes. */\n")
    foreach(include_file ${swig_includes})
      file(APPEND ${filename} "%include \"${include_file}\"\n")
    endforeach(include_file ${swig_includes})
    file(APPEND ${filename} "\n")
  endif(swig_includes)
  
  # C header files (speect.h and py_native.h are always included) 
  file(APPEND ${filename} "/* Speect Engine header & plug-in headers. */\n")
  file(APPEND ${filename} "%header\n%{\n")
  file(APPEND ${filename} "#include \"speect.h\"\n") # speect.h
  file(APPEND ${filename} "#include \"py_native.h\"\n") # py_native.h
  foreach(include_file ${swig_c_includes})
    file(APPEND ${filename} "#include \"${include_file}\"\n")
 #   list(APPEND swig_deps ${include_file})
  endforeach(include_file ${swig_c_includes})
  file(APPEND ${filename} "%}\n\n")

  # enable autodoc SWIG directive
  file(APPEND ${filename} "/* enable autodoc */\n%feature(\"autodoc\", \"1\");\n\n")

  # Load the plug-in on initialization ? 
  if(swig_load_plugin)
    file(APPEND ${filename} "/* Load the ${swig_classname} class plug-in */\n")
    file(APPEND ${filename} "%init\n%{\n\t{\n")
    file(APPEND ${filename} "\t\ts_erc rv = S_SUCCESS;\n")
    file(APPEND ${filename} "\t\tSPlugin *plugin;\n\n\n")
    file(APPEND ${filename} "\t\tplugin = s_pm_load_plugin(\"@plugin_lowercase_name@.spi\", &rv);\n")
    file(APPEND ${filename} "\t\tif (rv != S_SUCCESS)\n")
    file(APPEND ${filename} "\t\t\tSWIG_Error(SWIG_RuntimeError, \"Failed to load ${swig_classname} plug-in\");\n")
    file(APPEND ${filename} "\n\t\tS_UNUSED(plugin);\n")
    file(APPEND ${filename} "\t}\n%}\n\n")
  endif(swig_load_plugin)

  # import the Speect Engine Python interface
  file(APPEND ${filename} "/* import the Speect Engine Python interface */\n")
  file(APPEND ${filename} "%import \"${CMAKE_BINARY_DIR}/engine/swig/python/speect.i\"\n\n")
  list(APPEND swig_deps ${CMAKE_BINARY_DIR}/engine/swig/python/speect.i)

  # include the Python docstrings
  if(swig_python_docstrings)
    file(APPEND ${filename} "/* ${swig_classname} Python documentation strings. */\n")
    foreach(include_file ${swig_python_docstrings})
      file(APPEND ${filename} "%include \"${CMAKE_CURRENT_SOURCE_DIR}/${include_file}\"\n")
      list(APPEND swig_deps ${CMAKE_CURRENT_SOURCE_DIR}/${include_file})
    endforeach(include_file ${swig_python_docstrings})
    file(APPEND ${filename} "\n")
  endif(swig_python_docstrings)

  # include the Python typemaps
  if(swig_python_typemaps)
    file(APPEND ${filename} "/* ${swig_classname} Python typemaps. */\n")
    foreach(include_file ${swig_python_typemaps})
      file(APPEND ${filename} "%include \"${CMAKE_CURRENT_SOURCE_DIR}/${include_file}\"\n")
      list(APPEND swig_deps ${CMAKE_CURRENT_SOURCE_DIR}/${include_file})
    endforeach(include_file ${swig_python_typemaps})
    file(APPEND ${filename} "\n")
  endif(swig_python_typemaps)

  # SWIG common interface definition
  if(swig_common)
    file(APPEND ${filename} "/* ${swig_classname} SWIG common interface definition. */\n")
    foreach(include_file ${swig_common})
      file(APPEND ${filename} "%include \"${CMAKE_CURRENT_SOURCE_DIR}/../common/${include_file}\"\n")
      list(APPEND swig_deps ${CMAKE_CURRENT_SOURCE_DIR}/../common/${include_file})
    endforeach(include_file ${swig_common})
    file(APPEND ${filename} "\n")
  endif(swig_common)

  # SWIG Python interface definition
  if(swig_python)
    file(APPEND ${filename} "/* ${swig_classname} SWIG Python interface definition. */\n")
    foreach(include_file ${swig_python})
      file(APPEND ${filename} "%include \"${CMAKE_CURRENT_SOURCE_DIR}/${include_file}\"\n")
      list(APPEND swig_deps ${CMAKE_CURRENT_SOURCE_DIR}/${include_file})
    endforeach(include_file ${swig_python})
    file(APPEND ${filename} "\n")
  endif(swig_python)

  # SWIG flags
  speect_plugin_swig_python_flags()

  # add the generated interface file dependencies
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${plugin_lowercase_name}.i
    PROPERTIES 
    SWIG_FLAGS "${SPCT_SWIG_FLAGS}"
    GENERATED TRUE
    )
endmacro(speect_plugin_swig_python_interface)

 
#------------------------------------------------------------------------------------#
#                       Create plug-in SWIG Python loader                            #
#------------------------------------------------------------------------------------#
#
# speect_plugin_swig_python_loader()
#
# Create a plug-in loader file plugin_lowercase_name.i 
# This is just a simple wrapper that loads the plug-in in Python when imported.
#

macro(speect_plugin_swig_python_loader)
  set(filename "${CMAKE_CURRENT_BINARY_DIR}/${plugin_lowercase_name}.i")
  set(swig_deps)  # dependencies of interface file.
  
  # write header info 
  file(WRITE ${filename} "/* Definition of a simple Python importer for the ${plugin_lowercase_name}.spi plug-in. */\n\n")
  file(APPEND ${filename} "/*\n * ${plugin_lowercase_name}.i is auto generated by the \"speect_plugin_swig_python_loader\"\n")
  file(APPEND ${filename} " * macro in speect/swig/python/cmake/swigPythonFunctions.cmake, do not edit ${plugin_lowercase_name}.i\n */\n\n")

  # docstring
  file(APPEND ${filename} "%module(docstring=\"A simple wrapper to load the ${plugin_lowercase_name}.spi plug-in.\") @plugin_lowercase_name@\n\n")

  # C header files (speect.h is always included) 
  file(APPEND ${filename} "/* Speect Engine header. */\n")
  file(APPEND ${filename} "%header\n%{\n")
  file(APPEND ${filename} "#include \"speect.h\"\n") # speect.h
  file(APPEND ${filename} "%}\n\n")

  # Load the plug-in on initialization 
  file(APPEND ${filename} "/* Load the ${plugin_lowercase_name}.spi plug-in */\n")
  file(APPEND ${filename} "%init\n%{\n\t{\n")
  file(APPEND ${filename} "\t\ts_erc rv = S_SUCCESS;\n")
  file(APPEND ${filename} "\t\tSPlugin *plugin;\n\n\n")
  file(APPEND ${filename} "\t\tplugin = s_pm_load_plugin(\"@plugin_lowercase_name@.spi\", &rv);\n")
  file(APPEND ${filename} "\t\tif (rv != S_SUCCESS)\n")
  file(APPEND ${filename} "\t\t\tSWIG_Error(SWIG_RuntimeError, \"Failed to load '${plugin_lowercase_name}.spi' plug-in\");\n")
  file(APPEND ${filename} "\t\t\nS_UNUSED(plugin);\n")
  file(APPEND ${filename} "\t}\n%}\n\n")

  # SWIG flags
  speect_plugin_swig_python_flags()

  # add the generated interface file dependencies
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${plugin_lowercase_name}.i
    PROPERTIES 
    SWIG_FLAGS "${SPCT_SWIG_FLAGS}"
    GENERATED TRUE
    )  
endmacro(speect_plugin_swig_python_loader)


#------------------------------------------------------------------------------------#
#                       Include Python include directories                           #
#------------------------------------------------------------------------------------#
#
# macro(speect_include_python_directories)
#
# PYTHON_INCLUDE_PATH is deprecated in older CMAKE versions, use this macro 
#
macro(speect_include_python_directories)
  if(DEFINED PYTHON_INCLUDE_DIRS)
    include_directories(${PYTHON_INCLUDE_DIRS}) 
  else(DEFINED PYTHON_INCLUDE_PATH)
    include_directories(${PYTHON_INCLUDE_PATH})  
  endif(DEFINED PYTHON_INCLUDE_DIRS)
endmacro(speect_include_python_directories)


#------------------------------------------------------------------------------------#
#                                 Find Python                                        #
#------------------------------------------------------------------------------------#
#
# macro(speect_find_python)
#
# Find Python libraries and include files. This macro can search for Python 3, based
# on the WANT_PYTHON_3 option in speect/cmake/spctOptions.cmake. It also checks if
# the value of WANT_PYTHON_3 has changed between cmake invocations, if not then the
# previously set values of PYTHON_LIBRARIES, PYTHON_INCLUDE_DIRS, PYTHON_INCLUDE_PATH
# and PYTHON_DEBUG_LIBRARIES will be used (this was required because CMake did not
# update these values when WANT_PYTHON_3 was changed).
#
macro(speect_find_python)
  if((NOT DEFINED SPCT_INTERNAL_WANT_PYTHON_3) OR (NOT SPCT_INTERNAL_WANT_PYTHON_3 STREQUAL WANT_PYTHON_3))
    set(SPCT_INTERNAL_WANT_PYTHON_3 ${WANT_PYTHON_3} CACHE INTERNAL "Previously set value of WANT_PYTHON_3")
    unset(PYTHON_DEBUG_LIBRARY CACHE)
    unset(PYTHON_LIBRARY CACHE)
    unset(PYTHON_INCLUDE_DIR CACHE)
    unset(PYTHON_INCLUDE_PATH CACHE)
    unset(PYTHON_INCLUDE_DIRS)
    unset(PYTHON_LIBRARIES)
    unset(PYTHON_DEBUG_LIBRARIES)
    
    # add the directory that this file resides in to the CMake module path
    # so that we can use FindPythonLibsSpeect.cmake
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/swig/python/cmake)
    if(WANT_PYTHON_3)
      # Additional Python versions to search for. Stops when it finds one,
      # so later versions first.
      set(Python_ADDITIONAL_VERSIONS 3.2 3.1 3.0)
    else(WANT_PYTHON_3)
      set(Python_ADDITIONAL_VERSIONS "")
    endif(WANT_PYTHON_3)
    
    find_package(PythonLibsSpeect REQUIRED)
    set(SPCT_INTERNAL_PYTHON_LIBRARIES ${PYTHON_LIBRARIES} CACHE INTERNAL "Currently set value of PYTHON_LIBRARIES")
    set(SPCT_INTERNAL_PYTHON_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS} CACHE INTERNAL "Currently set value of PYTHON_INCLUDE_DIRS")
    set(SPCT_INTERNAL_PYTHON_INCLUDE_PATH ${PYTHON_INCLUDE_PATH} CACHE INTERNAL "Currently set value of PYTHON_INCLUDE_PATH")
    set(SPCT_INTERNAL_PYTHON_DEBUG_LIBRARIES ${PYTHON_DEBUG_LIBRARIES} CACHE INTERNAL "Currently set value of PYTHON_DEBUG_LIBRARIES")
  else((NOT DEFINED SPCT_INTERNAL_WANT_PYTHON_3) OR (NOT SPCT_INTERNAL_WANT_PYTHON_3 STREQUAL WANT_PYTHON_3))
    set(PYTHON_LIBRARIES ${SPCT_INTERNAL_PYTHON_LIBRARIES})
    set(PYTHON_INCLUDE_DIRS ${SPCT_INTERNAL_PYTHON_INCLUDE_DIRS})
    set(PYTHON_INCLUDE_PATH ${SPCT_INTERNAL_PYTHON_INCLUDE_PATH})
    set(PYTHON_DEBUG_LIBRARIES ${SPCT_INTERNAL_PYTHON_DEBUG_LIBRARIES})
  endif((NOT DEFINED SPCT_INTERNAL_WANT_PYTHON_3) OR (NOT SPCT_INTERNAL_WANT_PYTHON_3 STREQUAL WANT_PYTHON_3))
endmacro(speect_find_python)


#------------------------------------------------------------------------------------#
#                       Create plug-in SWIG Python wrapper                           #
#------------------------------------------------------------------------------------#
#
# macro(speect_plugin_swig_python_wrapper)
#
# Create a plug-in wrapper library and 
# Python loader (_@plugin_lowercase_name@.so and @pplugin_lowercase_name@.py)
#

macro(speect_plugin_swig_python_wrapper)
  # find swig
  find_package(SWIG REQUIRED)

  # include swig
  include(${SWIG_USE_FILE})

  # find Python 
  speect_find_python()

  # include Python
  speect_include_python_directories()

  # include Speect 
  include_directories(${SPCT_INCLUDE_DIRS})

  # include Speect Python native 
  include_directories(${SPCT_PYTHON_NATIVE_INCLUDE_DIRS})

  # clear C flags, we don't want the same stuff as Speect
  set(CMAKE_C_FLAGS "")

  # wrapper file properties
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${plugin_lowercase_name}PYTHON_wrap.c 
    PROPERTIES 
    COMPILE_FLAGS ${SWIG_C_COMPILE_FLAGS}
    GENERATED TRUE
    )

  # swig dependencies
  set(SWIG_MODULE_${plugin_lowercase_name}_EXTRA_DEPS "${swig_deps}")

  # SWIG Python wrapper output directory
  set(CMAKE_SWIG_OUTDIR ${CMAKE_BINARY_DIR}/swig/python/speect)

  # create the wrapper with SWIG
  swig_add_module(${plugin_lowercase_name} python ${CMAKE_CURRENT_BINARY_DIR}/${plugin_lowercase_name}.i)

  # link with Python and Speect Engine
  swig_link_libraries(${plugin_lowercase_name} 
    ${PYTHON_LIBRARIES} 
    ${SPCT_LIBRARIES_TARGET}
    ${SPCT_PYTHON_NATIVE_LIBRARY_TARGET})

  # set the output directory
  set_target_properties(${SWIG_MODULE_${plugin_lowercase_name}_REAL_NAME}
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/swig/python/speect/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/swig/python/speect/lib
    )
endmacro(speect_plugin_swig_python_wrapper)

