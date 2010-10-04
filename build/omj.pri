#
# Copyright (c) 2009,2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#


omj {

  # Load project defines (if not already loaded). Fail if can not loaded.
  !include(../inc/build_defines.pri):error(Could not load build_defines.pri)

  # Sanity check - otherwise components may fail e.g. with ODC file "what" target
  PRO_FILE_BASE = $$basename(_PRO_FILE_)
  !contains(PRO_FILE_BASE,$${TARGET}.pro) {
     error(Target name $${TARGET} does not match file name of $${_PRO_FILE_})
  }

  # Allow JAVATARGET to be used for defining the Java project name (java artifacts)
  isEmpty(JAVATARGET): JAVATARGET = $${TARGET}

  !contains(TARGET, javautils): LIBS += -ljavautils

  # Add general project defines
  DEFINES += $${PROJECT_DEFINES}

  !symbian {

     # Check release/debug
     !debug:!release:error(Define either debug or release configuration)

      contains(TEMPLATE,app) {
          DESTDIR = $$(JAVA_BIN_ROOT)/bin
      } else {
          DESTDIR = $$(JAVA_BIN_ROOT)/lib
      }
      LIBS += -lpthread
      LIBS += -L$$(JAVA_BIN_ROOT)/lib -L$$(JAVA_BIN_ROOT)/jsr/lib

      JAVA_ARTIFACTS = $$(JAVA_BIN_ROOT)/javabuild/$${JAVATARGET}
      JAVAH_ARTIFACTS = $$(JAVA_BIN_ROOT)/javabuild/inc.javah

      debug {
          DEFINES += LOGGING_ON NGJAVA_LOGGER_ON
      }
  }

  symbian {

      # If core is not in QT, there is no QT. If there is no QT, there is no 
      # core/gui
      !contains(QT, core): CONFIG -= qt
      !contains(CONFIG, qt): QT -= core gui

      # If we are building for QT, then use simple QT (enables 
      # changes in qt configuraiton)
      contains(PROJECT_DEFINES,RD_JAVA_UI_QT): CONFIG *= qt

      # Set correct UID2 for DLLs
      isEmpty(TARGET.UID2): contains(TEMPLATE,lib) {

         # UID2 0x1000008d is KSharedLibraryUidValue from e32uid.h, 
         # but stddlls have 0x20004c45
         stdbinary: TARGET.UID2 = 0x20004c45
         else: TARGET.UID2 = 0x1000008d
      }       

      # Load UID3s for Symbian
      include(symbian_uids.pri)

      DEFINES += __SYMBIAN32__
      DEFINES += J9EPOC32

      LIBS += -llibpthread

      # Include platform paths so that MMP macros work in below
      MMP_RULES += \
        "$${LITERAL_HASH}include <platform_paths.hrh>" \

      # Use QT defined includes only when really compiling for QT. Otherwise
      # use the S60 platform paths
      contains(QT, core) {
        INCLUDEPATH *= $$APP_LAYER_SYSTEMINCLUDE
        INCLUDEPATH *= $$OS_LAYER_LIBC_SYSTEMINCLUDE
        INCLUDEPATH *= $$OS_LAYER_SSL_SYSTEMINCLUDE
      } else {
        INCLUDEPATH -= $$APP_LAYER_SYSTEMINCLUDE \
                       $$OS_LAYER_LIBC_SYSTEMINCLUDE \
                       $$OS_LAYER_SSL_SYSTEMINCLUDE \
                       $${EPOCROOT}epoc32/include
        MMP_RULES += \
          " APP_LAYER_SYSTEMINCLUDE" \
          " OS_LAYER_LIBC_SYSTEMINCLUDE"\
          " OS_LAYER_SSL_SYSTEMINCLUDE"
      }

      !contains(TEMPLATE,app) {
          # DLLs have by default these capabilities
          TARGET.CAPABILITY = all -tcb
      }

      JAVA_ARTIFACTS = /epoc32/build/jrt/javabuild/$${JAVATARGET}
      JAVAH_ARTIFACTS = /epoc32/build/jrt/javabuild/inc.javah

      # Specify defblock always. Qmake revisions have diffent variants of
      # this, so we have to make sure that we define a deffile ourselves
      !contains(TEMPLATE,app): !contains(CONFIG,stdbinary) {
          MMP_RULES -= EXPORTUNFROZEN
          isEmpty(defBlock): defBlock = "deffile ./~/$${TARGET}.def"
          MMP_RULES += $${defBlock}
      }
      MMP_RULES *= "PAGED"

      # By default Symbian toolchain exports all classes on a DLL, unless
      # NONSHARABLE_CLASS macro is used. Qt exports only classes which
      # __declspec(dllexport), which is usually made defining
      # "MYSHAREDLIB_LIB" (see http://doc.trolltech.com/4.6/sharedlibrary.html)
      # This reverts the exporting to Symbian's convention

      MMP_RULES -= $$MMP_RULES_DONT_EXPORT_ALL_CLASS_IMPEDIMENTA

      stl {
        # If STL is configured and not full qt, add the corresponding
        # platform macro
        contains(DEFINES, RD_JAVA_STDCPPV5) {
          MMP_RULES += " SYSTEMINCLUDE OS_LAYER_PUBLIC_EXPORT_PATH(stdapis/stlportv5)"
        } else {
          MMP_RULES += " OS_LAYER_STDCPP_SYSTEMINCLUDE"
        }
      }

  }

  # Set version if not defined
  isEmpty(VERSION):!isEmpty(RD_JAVA_BIN_VERSION): VERSION = $${RD_JAVA_BIN_VERSION}

  # Define generated paths
  JXE_SOURCE_PATH    = $${JAVA_ARTIFACTS}
  LOOKUP_SOURCE_PATH = $${JAVA_ARTIFACTS}
  JAVAH_INCLUDE_PATH = $${JAVAH_ARTIFACTS}

  # Setup local include directories
  INC_DIRS = 

  # Platform incs
  symbian: exists($${_PRO_FILE_PWD_}/../inc.s60): INC_DIRS += ../inc.s60
  !symbian: exists($${_PRO_FILE_PWD_}/../inc.linux): INC_DIRS += ../inc.linux

  # Find the project depth
  TRY_DIR=$${_PRO_FILE_PWD_}
  for(a, 1..10): {
     TRY_DIR = $$join(TRY_DIR,,,/..)
     exists($${TRY_DIR}/build/omj.pri) DEPTH=$${a}
  }

  # Make include paths
  INC_DIR = inc
  for(a, 1..$${DEPTH}) {
     INC_DIR = $$join(INC_DIR,,../,)
     exists($${_PRO_FILE_PWD_}/$${INC_DIR}): INC_DIRS += $${INC_DIR}
  }

  # Add javah path. 
  # Symbian: Use MMP_RULES, as the path does not exist yet there
  # Linux: Java is built before qmake, so path does exist
  !symbian: INC_DIRS += $${JAVAH_INCLUDE_PATH}
  symbian: MMP_RULES += "SYSTEMINCLUDE $${JAVAH_INCLUDE_PATH}"


  # Define function "findfiles(path, wildcard)", which will find wildcard
  # files from the path, and from one subdirectory beneath it.
  defineReplace(findfiles) {
     # Note that the "files" function is undocumented qmake
     foundfiles = $$files($${1}/$${2})
     foundall = $$files($${1}/*)
     for(d, foundall) : exists($${d}/$${2}): foundfiles += $$files($${d}/$${2})
     return($$foundfiles)   
  }


  !javaonly: isEmpty(SOURCES) {
     # Make source paths if not defined
     SOURCES += $$findfiles($${_PRO_FILE_PWD_}/../src, *.cpp)
     symbian: SOURCES += $$findfiles($${_PRO_FILE_PWD_}/../src.s60, *.cpp)
     !symbian: SOURCES += $$findfiles($${_PRO_FILE_PWD_}/../src.linux, *.cpp)
  } else {
     # Expand also explicit sources, as Qmake fails to expand if CONFIG has no qt
     EXPANDED_SOURCES=
     for(s,SOURCES) {
        EXPANDED_SOURCE = $$files($${_PRO_FILE_PWD_}/$${s})
        isEmpty(EXPANDED_SOURCE): EXPANDED_SOURCES += $${s}
        else:                     EXPANDED_SOURCES += $${EXPANDED_SOURCE}
     }
     SOURCES = $${EXPANDED_SOURCES}
  }

  # Find source directories
  SRCDIRS = $$dirname(SOURCES)
  SRCDIRS = $$unique(SRCDIRS)
  INC_DIRS += $${SRCDIRS}

  # Append and make unique
  INCLUDEPATH += $${INC_DIRS}
  INCLUDEPATH = $$unique(INCLUDEPATH)
  DEPENDPATH += $${INC_DIRS}
  DEPENDPATH =$$unique(DEPENDPATH)

  # Remove explicitly marked "system" libs
  LIBS -= $${REMOVELIBS}

  javaonly {
    symbian {
      # Setup all required for romization
      INCLUDEPATH += $$(JAVA_SRC_ROOT)/inc
    } else {
      # Nothing to be done for Linux
      TEMPLATE=subdirs
      SUBDIRS=
    }
  }

  java|javaonly:symbian {
     MMP_RULES += \
       "SOURCEPATH $${LOOKUP_SOURCE_PATH}" \
       "SOURCE lookup.cpp" \
       "SOURCEPATH $${JXE_SOURCE_PATH}" \
       "SOURCE jxe.c"
  }

  symbian:exists($${_PRO_FILE_PWD_}/exports.inf) {
    # Set exports when the file exists
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include \"exports.inf\""
  }

  staticdata|contains(PROJECT_DEFINES, RD_JAVA_EPOCALLOWDLLDATA_FIX) {
     # If project has static data, we need define following on symbian
     # targets, to ensure that accidental static data additions are caught
     # by arm linker

     MMP_RULES += \
         "$${LITERAL_HASH}ifdef WINSCW" \
         "LIBRARY ewsd.lib" \
         "$${LITERAL_HASH}endif"
     contains(PROJECT_DEFINES, RD_JAVA_S60_RELEASE_5_0) {
       # S60 5.0 does not allow static data on DLLs to be loaded to multiple
       # processes, therefore this must be variable between armv5 and
       # winscw. It generates qmake warning on later QTs, but don't be
       # offended.
       MMP_RULES += \
           "$${LITERAL_HASH}ifndef WINSCW" \
           "EPOCALLOWDLLDATA" \      
           "$${LITERAL_HASH}endif"
     } else {
       TARGET.EPOCALLOWDLLDATA = 1
     }
   }
}
