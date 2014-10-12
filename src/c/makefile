INCLUDE=-I${HOME}/app/myproject/src/include
BINDIR=${HOME}/app/myproject/bin
LIBDIR=${HOME}/app/myproject/lib
TMP=${HOME}/app/tmp
CC=gcc
VPATH=${TMP}:.

LINKLIB=-lmxml -pthread -lxml

all:mpp_sds
mpp_sds:mpp_sds.c
	${CC}  -g ./mpp_sds.c -o  ${BINDIR}/mpp_sds  ${DEBUG} ${CCFLAG} ${LDFLAG} ${LIBINCL} -L${LIBDIR} ${LINKLIB} $(INCLUDE)
clean:
	rm ${BINDIR}/mpp_sds