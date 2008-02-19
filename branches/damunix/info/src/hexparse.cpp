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
	qDebug()<<"taille ::"<<text.size();
	
	foreach (line,text)
	{
		//la ligne commence par :
		if (line[0]!=QChar(':'))
			return false;
		//récupération du nombre d'octets à copier
		QString part1(line[1]);
		part1.append(line[2]);
		qDebug()<<"val ="<<part1;
		QByteArray nbBytes(part1.toAscii());
		//nbBytes.append(part1);
		bool ok;
		qDebug()<<"nombre d'octets ::"<<nbBytes.toInt(&ok,16);
		
		//récupération de l'adresse 
		QString addrS=line.left(7).right(4);
		int addr= (addrS).toInt(&ok,16);
		qDebug()<<"adresse a écrire "<<addrS << " ce qui donne "<<addr;
		
		//récupération de la donnée
		QString data=line.right(2+(nbBytes.toInt(&ok,16))*2).left((nbBytes.toInt(&ok,16))*2);
		qDebug()<<"octets : "<<line.right(2+(nbBytes.toInt(&ok,16))*2)<<"puis "<<line.right(2+(nbBytes.toInt(&ok,16))*2).left((nbBytes.toInt(&ok,16))*2);
		qDebug()<<" valeur en decimal"<<data.toInt(&ok,16);
		// récupération du checksum
		QString check=line.right(2);
		qDebug()<<"Checksum : "<< check.toInt(&ok,16)<<" (0x"<<check<<") je trouve "<<qChecksum(data.toAscii(),data.size());
	}
}
//
