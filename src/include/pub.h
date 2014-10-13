#include <ctype.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iconv.h>
#include <fcntl.h>
#include	"xmle.h"
#include "config.h"
#include "mxml.h"

#define MAXPARANUM 100
#define CFGFILELEN 16384

#define FLOW "/sys/flow"
#define COMP FLOW"/comp"


//��Դ�ļ�����
mxml_type_t	type_cb(mxml_node_t *node);
int loadfile(char *filename,char *buf,unsigned long inlen);
int gbk2utf8(char *srcBuf);
int code_convert(char *from_charset, char *to_charset, char *inbuf, unsigned long inlen, char *outbuf, unsigned long outlen);

//������Դ����
//int exeflow(HXMLTREE lXmlhandle,char *flowbuf);
//int flow_GetParas(mxml_node_t *node_comp, mxml_node_t *tree,const char **paras_out, int maxnum);
//const char *flow_GetStatus(mxml_node_t *node_comp, mxml_node_t *tree, char *statuscfg);
//char *doProcess(const char **paras, const char *snodename, char *statuscfg_out);

int ExeFlow(HXMLTREE lXmlhandle);
int RetrievePara( HXMLTREE lXmlhandle , mxml_node_t *node_comp, mxml_node_t *tree );
const char* NextFlowsn( HXMLTREE lXmlhandle ,mxml_node_t *node_comp, mxml_node_t *tree );


//�����Դ����
int ExeComp(HXMLTREE lXmlhandle);
