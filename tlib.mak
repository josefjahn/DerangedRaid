# Microsoft Developer Studio Generated NMAKE File, Based on tlib.dsp
!IF "$(CFG)" == ""
CFG=tlib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to tlib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "tlib - Win32 Release" && "$(CFG)" != "tlib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tlib.mak" CFG="tlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe

!IF  "$(CFG)" == "tlib - Win32 Release"

OUTDIR=.
INTDIR=.
# Begin Custom Macros
OutDir=. 
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\tlib.lib"

!ELSE 

ALL : "$(OUTDIR)\tlib.lib"

!ENDIF 

CLEAN :
        -@erase "$(INTDIR)\tlib.obj"
        -@erase "$(INTDIR)\vc50.idb"
        -@erase "$(OUTDIR)\tlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\include" /D "NDEBUG" /D "WIN32"\
 /D "_WINDOWS" /D "__MSC__" /D "__WIN32__" /Fp"$(INTDIR)\tlib.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=./
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tlib.bsc" 
BSC32_SBRS= \
        
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\tlib.lib" 
LIB32_OBJS= \
        "$(INTDIR)\tlib.obj"

"$(OUTDIR)\tlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "tlib - Win32 Debug"

OUTDIR=.
INTDIR=.
# Begin Custom Macros
OutDir=. 
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\tlib.lib"

!ELSE 

ALL : "$(OUTDIR)\tlib.lib"

!ENDIF 

CLEAN :
        -@erase "$(INTDIR)\tlib.obj"
        -@erase "$(INTDIR)\vc50.idb"
        -@erase "$(OUTDIR)\tlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /I "..\include" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "__MSC__" /D "__WIN32__" /Fp"$(INTDIR)\tlib.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=./
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tlib.bsc" 
BSC32_SBRS= \
        
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\tlib.lib" 
LIB32_OBJS= \
        "$(INTDIR)\tlib.obj"

"$(OUTDIR)\tlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "tlib - Win32 Release" || "$(CFG)" == "tlib - Win32 Debug"
SOURCE=tlib.c
DEP_CPP_TLIB_=\
        "..\include\3dfx.h"\
        "..\include\glide.h"\
        "..\include\glidesys.h"\
        "..\include\glideutl.h"\
        "..\include\sst1vid.h"\
        "tldata.inc"\
        "tlib.h"\
        

"$(INTDIR)\tlib.obj" : $(SOURCE) $(DEP_CPP_TLIB_) "$(INTDIR)"
        $(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

