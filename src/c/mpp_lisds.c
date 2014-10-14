#include	"pub.h"



int main(int argc, char **argv)
{
	HXMLTREE hXml;
	char buffer[CFGFILELEN];
	if((hXml = xml_Create("root"))==FAIL )
		exit(-1);

	xml_SetElement(hXml,"/sys/respath","/home/traxex/app/myproject/cfg/");
	xml_SetElement(hXml,FLOW"/flowname","mainflow.xml");
//	loadfile("/home/traxex/app/myproject/cfg/mainflow.xml",buffer,CFGFILELEN);
//	exeflow(hXml,buffer);
	
	ExeFlow(hXml);
	
	xml_Destroy(hXml);
	return 0;
}
