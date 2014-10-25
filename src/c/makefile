INCLUDE=-I${HOME}/app/myproject/src/include
BINDIR=${HOME}/app/myproject/bin
LIBDIR=${HOME}/app/myproject/lib
TMP=${HOME}/app/tmp
CC=gcc
VPATH=${TMP}:.

LINKLIB=-lmxml -pthread -lsxml -lpubbasic

all:mpp_sds mpp_lisds
mpp_sds:mpp_sds.c
	${CC}  -g ./mpp_sds.c -o  ${BINDIR}/mpp_sds  ${DEBUG} ${CCFLAG} ${LDFLAG} ${LIBINCL} -L${LIBDIR} ${LINKLIB} $(INCLUDE)
mpp_lisds:mpp_lisds.c
	${CC}  -g ./mpp_lisds.c -o  ${BINDIR}/mpp_lisds  ${DEBUG} ${CCFLAG} ${LDFLAG} ${LIBINCL} -L${LIBDIR} ${LINKLIB} $(INCLUDE)
clean:
	rm ${BINDIR}/mpp_sds
	rm ${BINDIR}/mpp_lisds