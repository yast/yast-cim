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

libpy2CIM_la_LDFLAGS = -version-info 2:0 -L${libdir}
libpy2CIM_la_LIBADD = \
	-lycp		\
	-ly2		\
	-ly2util -lowclient

INCLUDES =					\
	-I$(includedir)

BUILT_SOURCES = CIMBuiltinTable.h CIMBuiltinCalls.h

# "foo bar: baz" is wrong
# see "info '(automake)Multiple Outputs'"
%BuiltinCalls.h %BuiltinTable.h : %Functions.h
	${ydatadir}/devtools/bin/generateYCPWrappers $< $*BuiltinCalls.h $*BuiltinTable.h

CLEANFILES = $(BUILT_SOURCES)

