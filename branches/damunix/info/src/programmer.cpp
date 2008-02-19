#include "programmer.h"
//
#define VENDOR_ID 0x10
#define TARGET_ID 0x10 
programmer::programmer(  ) 
{
	USB::Busses buslist;
	USB::Device *device;
	std::list<USB::Device *> miceFound;
	USB::DeviceIDList mouseList;
	
	mouseList.push_front(USB::DeviceID(VENDOR_ID,TARGET_ID));
	miceFound=buslist.match(mouseList);
	
	if (!miceFound.empty())
		qDebug ()<<"Trouvé !!";
	/*for ( device = miceFound.begin(); device; device = miceFound.next() )
	{
		qDebug("Found !!!!!");
		
		
	}
	*/
}
//
