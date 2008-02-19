#include "hexparse.h"
#include <QDebug>
#include <QStringList>
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
		int addr= (line[3].digitValue())*1000+(line[4].digitValue())*100+(line[5].digitValue())*10+(line[6].digitValue());
		
	}
}
//
