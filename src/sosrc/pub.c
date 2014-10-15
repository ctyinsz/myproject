#include	"pub.h"

int SDATA_MSetValue(HXMLTREE hXml)
{
	printf("this is comp %s\n",__func__);
	char status[10]={0};
	char parabuf[NODELEN]={0};
	char nodepath[NODEPATHLEN]={0};	
	int i;
	int num =  xml_ElementCount(hXml,COMP"/complist/para");
	
	for(i=0;i<num;i++)
	{
		memset(parabuf,0x00,sizeof(parabuf));
		memset(nodepath,0x00,sizeof(nodepath));
		snprintf(nodepath,sizeof(nodepath)-1,COMP"/complist/para|%d",i+1);
		xml_GetElement(hXml,nodepath,parabuf,sizeof(parabuf)-1);
		printf("para %d:[%s]\n",i,parabuf);
	}	
	
	strcpy(status,"0");
	xml_SetElement(hXml , COMP"/compstatus",status);
	return 0;
}

int SDATA_ValCompare(HXMLTREE hXml)
{
	printf("this is comp %s\n",__func__);
	char status[10]={0};
	
	strcpy(status,"0");
	char parabuf[NODELEN]={0};
	char nodepath[NODEPATHLEN]={0};	
	int i;
	int num =  xml_ElementCount(hXml,COMP"/complist/para");
	
	for(i=0;i<num;i++)
	{
		memset(parabuf,0x00,sizeof(parabuf));
		memset(nodepath,0x00,sizeof(nodepath));
		snprintf(nodepath,sizeof(nodepath)-1,COMP"/complist/para|%d",i+1);
		xml_GetElement(hXml,nodepath,parabuf,sizeof(parabuf)-1);
		printf("para %d:[%s]\n",i,parabuf);
	}

	xml_SetElement(hXml , COMP"/compstatus",status);
	return 0;
}


