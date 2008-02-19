#ifndef HEXPARSE_H
#define HEXPARSE_H
//
#include <QFile>
//
class hexParse  
{

public:
	hexParse(void);
	//check if the file is really good
	static bool check(QString txt);
	
};
#endif
