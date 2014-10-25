#include $(INCLMAKE)
#include ${HOME}/src/etc/makefile.${GAPSPLATFORM}
INCLUDE=-I${HOME}/app/myproject/src/include
MYLDFLAG= -brlt
TMP=${HOME}/app/tmp
CCFLAG=-D__ORACLE__
SHLIBDIR=${HOME}/app/myproject/shlib
LIBDIR=${HOME}/app/myproject/lib
RELEASE_VERSION=2.0.0
CC=gcc ${OS_CCFLAG} -m32
VPATH=${TMP}:.libs:.:
LINKLIB=-lmxml -pthread -lsxml -ldl

.SUFFIXES:
.SUFFIXES:.c .lo
.c.lo:
	libtool --mode=compile --tag=CC ${CC} -g -O -o ${TMP}/$*.lo $(INCLUDE) -c $<

all:libpubbasic.la 
libpubbasic.la:libpubbasic.lo
	libtool --mode=link --tag=CC ${CC}  ${MYLDFLAG} -g  -O -o ${TMP}/$(@:.so=.la) $? -rpath ${LIBDIR}  -release ${RELEASE_VERSION} -export-dynamic -no-undefined  ${INCLUDE} -L${LIBDIR} ${LINKLIB}

install:
	libtool --mode=install install -c ${TMP}/libpubbasic.la ${LIBDIR}
	libtool --mode=finish  ${LIBDIR}

clean:
	@rm -f lib*.a *~ *core ${TMP}/*.lo ${TMP}/*.o ${TMP}/*.la ${TMP}/.libs/*
	@rm -rf ./.libs