#
# Makefile.am for core/cim/src
#

AM_CXXFLAGS = -DY2LOG=\"cim\" -DSUSEVERSION=\"${SUSEVERSION}\"	\
	-DLOCALEDIR=\"${localedir}\"  -fno-inline

plugin_LTLIBRARIES = libpy2CIM.la

libpy2CIM_la_SOURCES =				\
	CIMFunctions.cc	CIMFunctions.h		\
	Y2CIMComponent.cc Y2CIMComponent.h	\
	Y2CCCIM.cc Y2CCCIM.h

libpy2CIM_la_LDFLAGS = -version-info 2:0
libpy2CIM_la_LIBADD = \
	/usr/lib/libycp.la		\
	/usr/lib/liby2.la		\
	-ly2util -lowclient

INCLUDES =					\
	-I$(includedir)/YaST2/ycp		\
	-I$(includedir)/YaT2/y2			\
	-I$(includedir)

BUILT_SOURCES = CIMBuiltinTable.h CIMBuiltinCalls.h

CIMBuiltinTable.h : CIMFunctions.h
	/usr/share/YaST2/data/devtools/bin/generateYCPWrappers CIMFunctions.h CIMBuiltinCalls.h CIMBuiltinTable.h

CIMBuiltinCalls.h : CIMFunctions.h
	/usr/share/YaST2/data/devtools/bin/generateYCPWrappers CIMModuleFunctions.h CIMBuiltinCalls.h CIMBuiltinTable.h

CLEANFILES = $(BUILT_SOURCES)

