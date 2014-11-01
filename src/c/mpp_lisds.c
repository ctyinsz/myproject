#include	"pub.h"



int main(int argc, char **argv)
{
	HXMLTREE hXml;
	char buffer[CFGFILELEN];
	if((hXml = xml_Create("root"))==FAIL )
		exit(-1);

	xml_SetElement(hXml,"/sys/respath",RESPATH);
	xml_SetElement(hXml,FLOW"/flowname","mainflow");
//	loadfile("/home/traxex/app/myproject/cfg/mainflow.xml",buffer,CFGFILELEN);
//	exeflow(hXml,buffer);
	char str[]="mainflow.xml";
	printf("开始执行流程:[%s]\n",str);
	ExeFlow(hXml);
	
	xml_Destroy(hXml);
	return 0;
}
