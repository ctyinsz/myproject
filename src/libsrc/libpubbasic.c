#include	"pub.h"

/******************************************************************************
函数名称:  ExeFlow
函数功能:  执行流程
输入参数:  HXMLTREE--XML参数树
输出参数:  
返 回 值:  无
******************************************************************************/
int ExeFlow(HXMLTREE lXmlhandle)
{
	printf("in function %s\n",__func__);
	int iret;
	char respath[100+1]={0};
	char flowname[512]={0};
	char buffer[CFGFILELEN+1]={0};
	char filename[512]={0};
	xml_GetElement(lXmlhandle,"/sys/respath",respath,sizeof(respath));
	xml_GetElement(lXmlhandle,FLOW"/flowname",flowname,sizeof(flowname));
	strcpy(filename,respath);
	strcat(filename,flowname);
	loadfile(filename,buffer,CFGFILELEN);
	
	mxml_node_t		*tree;
	mxml_node_t		*node_comp;
	mxml_node_t		*node_flowcfg;
	mxml_node_t		*node_status;
  const char *flowsn;/*当前节点序号*/
  const char *snodename;/*当前节点名*/
  	
	tree = mxmlLoadString(NULL, buffer, type_cb);
  if (!tree)
  {
    fputs("Unable to read XML file!\n", stderr);
    return (1);
  }	
  node_flowcfg = mxmlFindElement(tree,tree,"flowcfg",NULL,NULL,MXML_DESCEND);
  if(!node_flowcfg)
  	return 1;	
  node_comp = mxmlFindElement(node_flowcfg,tree,"comp","compname","BEGIN",MXML_DESCEND);
  node_status = mxmlFindElement(node_flowcfg,tree,"cfg","compstatus","0",MXML_DESCEND);
  flowsn = mxmlElementGetAttr(node_status,"nextflowsn");
  
  for(;node_comp!=NULL;)
  {
  	node_comp = mxmlFindElement(node_flowcfg,tree,"comp","flowsn",flowsn,MXML_DESCEND);
//  	snodename = mxmlElementGetAttr(node_comp,"compname");
//	  if(strncmp(snodename,"END",3)==0)
//	  	break;  	
  	iret = RetrievePara(lXmlhandle,node_comp,tree);
  	if(iret)
  		break;
  	
  	ExeComp(lXmlhandle);
  	
  	flowsn = NextFlowsn(lXmlhandle,node_comp,tree);
  	
	  if(strncmp(flowsn,"-8888",5)==0)
	  	break;	
  }
  
  mxmlDelete(tree); 
  return (0);		 
}

/******************************************************************************
函数名称:  ExeComp
函数功能:  处理模块
输入参数:  HXMLTREE--XML参数树
输出参数:  
返 回 值:  无
******************************************************************************/
int ExeComp(HXMLTREE lXmlhandle)
{
	printf("in function %s\n",__func__);
	char buf[100]={0};
	char nodepath[512]={0};
	char status[10]={0};
	int i=0;
	int num = 0;
	num = xml_ElementCount(lXmlhandle,COMP"/complist/para");
	if(num == 0)
		return 0;
	
	for(i=0;i<num;i++)
	{
		memset(buf,0x00,sizeof(buf));
		memset(nodepath,0x00,sizeof(nodepath));
		snprintf(nodepath,sizeof(nodepath)-1,COMP"/complist/para|%d",i+1);
		xml_GetElement(lXmlhandle,nodepath,buf,sizeof(buf)-1);
		printf("para %d:[%s]\n",i,buf);
	}
	
	/*
	TODO
	*/
	strcpy(status,"0");
	
//	xml_GetElement(lXmlhandle , COMP"/compstatus",status,sizeof(status));
	xml_SetElement(lXmlhandle , COMP"/compstatus",status);
	return 0;		
}

/******************************************************************************
函数名称:  RetrievePara
函数功能:  将模块所有的参数导出到XML参数树
输入参数:  node_comp -- 待处理父节点，tree--待处理树，HXMLTREE--XML参数树
输出参数:  
返 回 值:  无
******************************************************************************/
int RetrievePara( HXMLTREE lXmlhandle , mxml_node_t *node_comp, mxml_node_t *tree )
{
	printf("in function %s\n",__func__);
	mxml_node_t		*node_paras,*node_paras_child;
	int i=0;
	const char *tmp;
	char nodepath[512]={0};
	
	if(node_comp == NULL)
		return -1;
		
	tmp = mxmlElementGetAttr(node_comp,"compname");
	
	xml_SetElement(lXmlhandle , COMP"/compname",tmp);
	if(strncmp(tmp,"END",3)==0)
		return 1;
		
	xml_DelElement(lXmlhandle,COMP"/complist");

	node_paras = mxmlFindElement(node_comp , tree ,"fcompparacfgs" , NULL,NULL, MXML_DESCEND);
	node_paras_child = mxmlGetFirstChild(node_paras);		
	
	for(i=0;node_paras_child!=NULL ;)
	{
		tmp = mxmlElementGetAttr(node_paras_child,"paracont");
		if(tmp)
		{
			memset(nodepath,0x00,sizeof(nodepath));
			snprintf(nodepath,sizeof(nodepath),COMP"/complist/para|%d",i+1);
			xml_SetElement(lXmlhandle , nodepath ,tmp);
			i++;
		}
		node_paras_child = mxmlGetNextSibling(node_paras_child);
	}
	return 0;	
}

/******************************************************************************
函数名称:  NextFlowsn
函数功能:  根据模块返回值查找流程树上的下一个节点
输入参数:  node_comp -- 待处理父节点，tree--待处理树，HXMLTREE--XML参数树
输出参数:  输入属性值对应的nextflowsn属性值
返 回 值:  无
******************************************************************************/	  
const char* NextFlowsn( HXMLTREE lXmlhandle ,mxml_node_t *node_comp, mxml_node_t *tree )
{
	char status[10]={0};
	mxml_node_t		*node_cfgs;
	mxml_node_t		*node_attr;
	
	xml_GetElement(lXmlhandle , COMP"/compstatus",status,sizeof(status));
	
	node_cfgs = mxmlFindElement(node_comp , tree ,"fcompstatcfgs" , NULL,NULL, MXML_DESCEND);	
	node_attr = mxmlFindElement(node_cfgs,tree,"cfg","compstatus",status,MXML_DESCEND);
		
	return mxmlElementGetAttr(node_attr,"nextflowsn");	
}

/******************************************************************************
函数名称:  type_cb
函数功能:  文件载入句柄
输入参数:  node -- mxml_node_t 句柄
输出参数:  
返 回 值:  无
******************************************************************************/
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
/******************************************************************************
函数名称:  gbk2utf8
函数功能:  将GBK转换为UTF8
输入参数:  srcBuf -- 待处理字符串
输出参数:  
返 回 值:  无
******************************************************************************/
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
/******************************************************************************
函数名称:  loadfile
函数功能:  将资源文件载入到缓冲区
输入参数:  filename -- 待处理资源文件，buf--缓冲区，inlen--缓冲区长度
输出参数:  
返 回 值:  无
******************************************************************************/
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
