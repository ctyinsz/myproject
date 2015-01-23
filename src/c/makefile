INCLUDE=-I${HOME}/app/myproject/src/include
BINDIR=${HOME}/app/myproject/bin
LIBDIR=${HOME}/app/myproject/lib
TMP=${HOME}/app/tmp
CC=gcc
VPATH=${TMP}:.

LINKLIB=-lmxml -pthread -lsxml -lpub -lbase

.SUFFIXES:
.SUFFIXES: .c .o
.c.o:
	${CC} -g -O ${DEBUG} ${CCFLAG} ${INCLUDE} -c $< -o ${TMP}/$*.o 

all:mpp_sds mpp_sds2 mpp_lisds
mpp_sds:mpp_sds.o
	${CC}  -g $? -o ${BINDIR}/$@  ${DEBUG} ${CCFLAG} ${LDFLAG} ${LIBINCL} -L${LIBDIR} ${LINKLIB} 
mpp_sds2:dataproc.o thread_producer.o thread_consumer.o mpp_sds2.o 
	${CC}  -g $? -o  ${BINDIR}/$@  ${DEBUG} ${CCFLAG} ${LDFLAG} ${LIBINCL} -L${LIBDIR} ${LINKLIB}
mpp_lisds:mpp_lisds.o
	${CC}  -g $? -o  ${BINDIR}/$@  ${DEBUG} ${CCFLAG} ${LDFLAG} ${LIBINCL} -L${LIBDIR} ${LINKLIB} 
#test:test.o
#	${CC}  -g ./test.c -o  ${BINDIR}/test  ${DEBUG} ${CCFLAG} ${LDFLAG} ${LIBINCL} -L${LIBDIR} ${LINKLIB} $(INCLUDE)
clean:
	rm -f ${TMP}/*.o
	rm ${BINDIR}/mpp_sds
	rm ${BINDIR}/mpp_sds2
	rm ${BINDIR}/mpp_lisds
#	rm ${BINDIR}/test