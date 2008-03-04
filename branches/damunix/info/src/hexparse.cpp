#include "hexparse.h"
#include <QDebug>
#include <QStringList>
#include <math.h> 
//
hexParse::hexParse( void) 
{
	
}


bool hexParse::check(QString txt)
{
	QString line;
	qDebug("*****Check !!");
	QStringList text=txt.split("\n");
	qDebug()<<"size ::"<<text.size();
	
	
	foreach (line,text)
	{
		//lines biggin with :
		if (line[0]!=QChar(':'))
			return false;
		
		// get checksum
		QString check=line.right(2);
		//qDebug()<<"Checksum : "<< check.toInt(&ok,16)<<" (0x"<<check<<") je trouve "<<qChecksum(data.toAscii(),data.size());
	}
}

int hexParse::getSize(QString txt)
{
	//get number of bytes
	bool ok;
	QString part1(txt[1]);
	part1.append(txt[2]);

	QByteArray nbBytes(part1.toAscii());
	return nbBytes.toInt(&ok,16);
}

QString hexParse::getAddress(QString txt)
{
	//get address
	bool ok; 
	QString addrS=txt.left(6).right(3);
	int addr= (addrS).toInt(&ok,16);
	return (addrS);
}

int hexParse::blockType(QString txt)
{
	//block mark => 00 normal,01 end of file, 04 extended adress
	bool ok;
	return txt.left(9).right(2).toInt(&ok,10);
	
}

QString  hexParse::getData(QString txt)
{
	//get data
	int nbBytes=getSize(txt);
	QString data=txt.right(2+(nbBytes*2)).left(nbBytes*2);
	return data;
}
//
