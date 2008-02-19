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
			
		//get number of bytes
		QString part1(line[1]);
		part1.append(line[2]);
		qDebug()<<"val ="<<part1;
		QByteArray nbBytes(part1.toAscii());
		bool ok;
		qDebug()<<"nombre d'octets ::"<<nbBytes.toInt(&ok,16);
		
		//get address 
		QString addrS=line.left(7).right(4);
		int addr= (addrS).toInt(&ok,16);
		qDebug()<<"adresse a écrire "<<addrS << " ce qui donne "<<addr;
		
		//get data
		QString data=line.right(2+(nbBytes.toInt(&ok,16))*2).left((nbBytes.toInt(&ok,16))*2);
		qDebug()<<"octets : "<<line.right(2+(nbBytes.toInt(&ok,16))*2)<<"puis "<<line.right(2+(nbBytes.toInt(&ok,16))*2).left((nbBytes.toInt(&ok,16))*2);
		qDebug()<<" valeur en decimal"<<data.toInt(&ok,16);
		// get checksum
		QString check=line.right(2);
		qDebug()<<"Checksum : "<< check.toInt(&ok,16)<<" (0x"<<check<<") je trouve "<<qChecksum(data.toAscii(),data.size());
	}
}

//
