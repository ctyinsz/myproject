#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <sys/types.h>
#include <errno.h>
#include	"xmle.h"
#include "config.h"
#include "mxml.h"
#include <fcntl.h>
#include <iconv.h>

#define MAXPARANUM 100
#define CFGFILELEN 16384

mxml_type_t	type_cb(mxml_node_t *node);
int GetParas(mxml_node_t		*node_comp,mxml_node_t		*tree,const char **paras,int maxnum);
	  
char *doProcess(const char **paras,const char *snodename,char *statuscfg);
	  
const char *GetStatus(mxml_node_t		*node_comp,mxml_node_t		*tree,char *statuscfg);

int exeflow(HXMLTREE hXml,char *flowname);

int code_convert(char *from_charset, char *to_charset, char *inbuf, unsigned long inlen, char *outbuf, unsigned long outlen);
int loadfile(char *filename,char *buf,unsigned long inlen);

int main(int argc, char **argv)
{
	HXMLTREE hXml;
	char buffer[CFGFILELEN];
	if((hXml = xml_Create("root"))==FAIL )
		exit(-1);
	
	loadfile("/home/traxex/app/myproject/cfg/mainflow.xml",buffer,CFGFILELEN);
	exeflow(hXml,buffer);
	
	xml_Destroy(hXml);
	return 0;
}
int exeflow(HXMLTREE hXml,char *buffer)
{
  mxml_node_t		*tree,		/* XML tree */
			*node_comp;		/* Node which should be in test.xml */
	mxml_node_t		*node_flowcfg;		/* XML tree */
	mxml_node_t		*node_status;		/* XML tree */
  const char *attr;
  const char *flowsn;/*当前节点序号*/
  const char *snodename;/*当前节点名*/
  char statuscfg[10]={0};/*返回状态*/
  const char *paras[MAXPARANUM];/*参数列表*/
  
  tree = mxmlLoadString(NULL, buffer, type_cb);

  if (!tree)
  {
    fputs("Unable to read XML file!\n", stderr);
    return (1);
  }	

  node_flowcfg = mxmlFindElement(tree,tree,"flowcfg",NULL,NULL,MXML_DESCEND);
  if(node_flowcfg == NULL)
  	return 1;
   
  node_comp = mxmlFindElement(node_flowcfg,tree,"comp","compname","BEGIN",MXML_DESCEND);
  node_status = mxmlFindElement(node_flowcfg,tree,"cfg","compstatus","0",MXML_DESCEND);
  flowsn = mxmlElementGetAttr(node_status,"nextflowsn");
  
//  printf("current flowsn:[%s]\n",flowsn);
  
  for(;node_comp!=NULL;)
  {
	  node_comp = mxmlFindElement(node_flowcfg,tree,"comp","flowsn",flowsn,MXML_DESCEND);
	  snodename = mxmlElementGetAttr(node_comp,"compname");
	  if(strncmp(snodename,"END",3)==0)
	  	break;
	  printf("current comp:[%s],flowsn:[%s]\n",snodename,flowsn);
	  
	  memset(paras,0x00,MAXPARANUM);
	  memset(statuscfg,0x00,sizeof(statuscfg));
	  GetParas(node_comp,tree,paras,MAXPARANUM);
	  
	  doProcess(paras,snodename,statuscfg);
	  
	  flowsn = GetStatus(node_comp,tree,statuscfg);

	  if(strncmp(flowsn,"-8888",5)==0)
	  	break;	  
	  
	} 
  mxmlDelete(tree); 
  return (0);		
}


char *doProcess(const char **paras,const char *snodename,char *statuscfg)
{
	int i=0;
//	printf("comp name:[%s]\n",snodename);
	for(i=0;paras[i]!=NULL;i++)
		printf("para %d:[%s]\n",i,paras[i]);
	
	return strcpy(statuscfg,"0");
	
}

int GetParas(mxml_node_t		*node_comp,mxml_node_t		*tree,const char **paras,int maxnum)
{
	mxml_node_t		*node_paras,*node_paras_child;
	int i=0;
	const char *tmp;
	memset(paras,0x00,maxnum);
	
	if(node_comp == NULL)
		return 0;
	node_paras = mxmlFindElement(node_comp , tree ,"fcompparacfgs" , NULL,NULL, MXML_DESCEND);
	node_paras_child = mxmlGetFirstChild(node_paras);

	for(;node_paras_child!=NULL && i< maxnum ;)
	{
		tmp = mxmlElementGetAttr(node_paras_child,"paracont");
		if(tmp)
		{
			paras[i] = tmp;
//			printf("para %d:[%s]\n",i,tmp);
			i++;
		}
		node_paras_child = mxmlGetNextSibling(node_paras_child);
	}
	return i;
}
	  
const char *GetStatus(mxml_node_t		*node_comp,mxml_node_t		*tree,char *statuscfg)
{
	mxml_node_t		*node_cfgs;
	mxml_node_t		*node_attr;
	
	node_cfgs = mxmlFindElement(node_comp , tree ,"fcompstatcfgs" , NULL,NULL, MXML_DESCEND);	
	node_attr = mxmlFindElement(node_cfgs,tree,"cfg","compstatus",statuscfg,MXML_DESCEND);
		
	return mxmlElementGetAttr(node_attr,"nextflowsn");
}

mxml_type_t				/* O - Data type */
type_cb(mxml_node_t *node)		/* I - Element node */
{
  const char	*type;			/* Type string */


 /*
  * You can lookup attributes and/or use the element name, hierarchy, etc...
  */

  if ((type = mxmlElementGetAttr(node, "type")) == NULL)
    type = node->value.element.name;

  if (!strcmp(type, "integer"))
    return (MXML_INTEGER);
  else if (!strcmp(type, "opaque") || !strcmp(type, "pre"))
    return (MXML_OPAQUE);
  else if (!strcmp(type, "real"))
    return (MXML_REAL);
  else
    return (MXML_TEXT);
}

int code_convert(char *from_charset, char *to_charset, char *inbuf, unsigned long inlen, char *outbuf, unsigned long outlen)
{
  iconv_t h;
  const char *old = outbuf;
 
  h = iconv_open(to_charset, from_charset);
  if ((iconv_t) - 1 == h)
    return -1;
 
  memset(outbuf, 0, outlen);
 
  if (iconv(h, &inbuf, (size_t *)&inlen, &outbuf, (size_t *)&outlen) == -1)
  {
    iconv_close(h);
    return -2;
  }
  iconv_close(h);
  return outbuf - old;
}

int gbk2utf8(char *srcBuf)
{
  char srcCodepage[120] = {"GBK"};
  char desCodepage[120] = {"UTF-8"};
  char *pDesBuf = NULL;

  int ilen,iret;
  ilen = strlen(srcBuf);
  pDesBuf = (char *)malloc(ilen * 4);
	if ( -1 == (iret = code_convert(srcCodepage, desCodepage, srcBuf, ilen, pDesBuf, ilen * 4) ) )
	{
		printf("GBK TO UTF-8 FAIL\n");
		return -1;
	}
	memset(srcBuf,0x00,ilen);
	strcpy(srcBuf,pDesBuf);
	free(pDesBuf);
	return 0;
}

int loadfile(char *filename,char *buf,unsigned long inlen)
{
	FILE			*fp;
	unsigned long i=0;
  if ((fp = fopen(filename, "rb")) == NULL)
  {
    perror(filename);
    return (-1);
  }
  memset(buf,0x00,inlen);
  while(!feof(fp))
  {
  	buf[i++]=getc(fp);
  }
  fclose(fp);
  gbk2utf8(buf);

  return 0;
}
