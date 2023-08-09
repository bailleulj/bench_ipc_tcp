# brief RegenTsLstFiles: crée les fichiers ts_lst qui, avec certaines versions de qt,
# ne sont pas créés tout seuls lorsqu'on compile from scratch (=>erreur de build)
# details : ces fichiers contiennent une liste des fichiers à scanner pour rechercher des textes à traduire
# param source_files: listes des fichiers cpp
# param header_files: headers
# param resource_files : .ui, .rc, etc.
function(RegenTsLstFiles all_files )

    set(TSLST_FR "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}_fr.ts_lst_file")
    set(TSLST_EN "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}_en.ts_lst_file")

    FILE(WRITE ${TSLST_FR}
            ${CMAKE_CURRENT_SOURCE_DIR})
    FILE(WRITE ${TSLST_EN}
            ${CMAKE_CURRENT_SOURCE_DIR})

    foreach (ts_input_file ${all_files})
        FILE(APPEND ${TSLST_EN} "\n${ts_input_file}")
        FILE(APPEND ${TSLST_FR} "\n${ts_input_file}")
    endforeach ()

endfunction(RegenTsLstFiles)

# NB: parfois, il suffit de recharger le projet
#
# example :
# FILE(GLOB_RECURSE header_files ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
# FILE(GLOB_RECURSE source_files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
# FILE(GLOB_RECURSE resource_files ${CMAKE_CURRENT_SOURCE_DIR}/res/*.*)
#
# include( ${CMAKE_SOURCE_DIR}/functions.cmake)
# RegenTsLstFiles( "${source_files};${header_files};${resource_files}" )



# @function setHomeEnv
# @brief : defines HOME environment variable if not defined, as one could expect (C:\Users\toto)
# This may happen on Windows with newer cmake versions. In that case, it is split up between 2 variables
# @return : side effect on HOME; has '/' separators for use with cmake paths
function(setHomeEnv)
    # TODO : Linux case
    if (NOT DEFINED ENV{HOME})
        set(ENV_HOME "$ENV{HOMEDRIVE}$ENV{HOMEPATH}")
        string(REPLACE "\\" "/" ENV_HOME_SLASH ${ENV_HOME})
        set(ENV{HOME} ${ENV_HOME_SLASH})
        message(STATUS "HOME environment variable defined for Windows: $ENV{HOME} ")
    endif ()
endfunction(setHomeEnv)


# @macro include_boost_package. Macros are REQUIRED with find_package because functions create local scopes
# @brief: définit l'usage de boost pour tout le projet
# @param BOOST_PATH: chemin où se trouve le paquetage Boost désiré
# @usage: include_boost_package( ${BOOST_INCLUDEDIR} ) avec la variable définie dans user.cmake
macro(include_boost_package BOOST_PATH)
    set(SAVE_CMP "${CMAKE_PREFIX_PATH}")
    set(CMAKE_PREFIX_PATH "${BOOST_PATH}")

    set(Boost_USE_STATIC_LIBS TRUE)
    add_definitions( -DBOOST_ALL_NO_LIB -DBOOST_UUID_FORCE_AUTO_LINK) ## -lrt ) # -ltr pour interprocess

    find_package(Boost COMPONENTS system filesystem thread regex locale serialization chrono program_options REQUIRED)
    set(CMAKE_PREFIX_PATH ${SAVE_CMP})
endmacro(include_boost_package)


# @macro include_pcl_package. Macros are REQUIRED with find_package because functions create local scopes
# pareil que boost
macro(include_pcl_package PCL_PATH)

    if(NOT PCL_PATH IN_LIST CMAKE_PREFIX_PATH)
        list(APPEND CMAKE_PREFIX_PATH "${PCL_INCLUDEDIR}")
    endif ()

    find_package(PCL 1.11
            REQUIRED COMPONENTS
            common features filters io io_ply kdtree keypoints ml octree recognition registration sample_consensus
            search segmentation stereo surface tracking # d'après pcl.props
            geometry
            )
    add_definitions(${PCL_DEFINITIONS}) # options de compilation requises par PCL

    message(WARNING "PCL trouvé: ${PCL_DEFINITIONS} ${PCL_LIBRARIES} ${PCL_LIBRARY_DIRS}")


endmacro(include_pcl_package)
