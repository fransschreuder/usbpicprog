#ifndef HEXPARSE_H
#define HEXPARSE_H
//
#include <QFile>
#include <QObject>
#include <QString>
//
class hexParse : public QObject  
{
Q_OBJECT
public:
	hexParse(void);
	virtual ~hexParse(){
	}
	//check if the file is really good
	static bool check(QString txt);
	static int getSize(QString txt);
	static QString getAddress(QString txt);
	
	//return 00 for a data,
	//		 01 for an end of file,
	//		 04 for extended adressing
	static int blockType(QString txt);
	static QString  getData(QString txt);
private:
	
};
#endif
