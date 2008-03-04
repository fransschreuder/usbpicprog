#include "mainwindowimpl.h"
#include <QFileDialog>
#include <QtGlobal>
#include <QDebug>
#include "hexparse.h"
//

MainWindowImpl::MainWindowImpl( QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
	setupUi(this);
	connect(quitButton, SIGNAL(clicked()),this,SLOT(quit()));
	connect(openButton,SIGNAL(clicked()),this,SLOT(open()));
	connect(checkHex,SIGNAL(clicked()),this,SLOT(check()));
	connect(programButton,SIGNAL(clicked()),this,SLOT(program()));
	connect(connectButton,SIGNAL(clicked()),this,SLOT(connectProgrammer()));
	connect(eraseButton,SIGNAL(clicked()),this,SLOT(eraseDevice()));
	connect(deviceIdButton,SIGNAL(clicked()),this,SLOT(getId()));
}

void MainWindowImpl::connectProgrammer(void)
{
	_prog = new programmer();
	if (_prog->connected())
	{
		programButton->setEnabled(true);
		deviceIdButton->setEnabled(true);
		eraseButton->setEnabled(true);
	}
	
}

void MainWindowImpl::getId(void)
{
	char id[2];
	_prog->getId(id,2);
}

void MainWindowImpl::eraseDevice(void)
{
	unsigned char msg=0x10;
	_prog->write((char*)&msg,1);
}


void MainWindowImpl::program(void)
{
	QString var;
	QString tmp;
	unsigned int addr;
	bool start=0;
	bool ok;
	//QStringList txt=textEdit->toPlainText().split("\n");

	foreach ( QString line,_txt)
	{
		qDebug()<<"reading ";
		//if we have all data (the next would be an Extended Addr)
		if ((start==1) && (hexParse::blockType(line)==04))
		{
			qDebug ("End of data found");
			return;
		}
		// the data start at 0x020
		if (hexParse::getAddress(line)=="020")
		{
			qDebug("Gogogogo");
			start=1;
		}
		
		if (start==1)
		{
			addr=hexParse::getAddress(line).toUInt();
			var=0x30;
			var.append((addr));
			var.append(hexParse::getData(line));
			qDebug()<<"writing "<<var<< "size  "<<var.size();
			_prog->write(var.toAscii(),var.size());
		}
		
	}
}

void MainWindowImpl::check(void)
{
	//get each line of the document

	QString newText("TYPE	ADDRESS		DATA\n");
	
	foreach ( QString line,_txt)
	{
		
			
		if (hexParse::blockType(line) == 0) 
			newText.append("DATA 	");
		else if (hexParse::blockType(line) == 4)
			newText.append("ExtAddr	");
		else if (hexParse::blockType(line) == 1)
		{
			newText.append("EOF		");
			break;
		}
		else newText.append("Unknown	");
		 
		newText.append("0x"+hexParse::getAddress(line)+"		");
		newText.append(hexParse::getData(line)+"\n");
		
	}
	textEdit->setDocument(new QTextDocument(newText));
	
}

void MainWindowImpl::open(void)
{
	QString path;
	QString txt;
	path=QFileDialog::getOpenFileName(this,"Choose a File",".",tr("Hex File (*.hex)"));
	label1->setText(path);
	
	QFile file(path);
		
	//opening the file
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		qFatal ("Can't Open/Read the file");
	while (!file.atEnd())
	{
		txt.append(file.readLine());
	}
	textEdit->append(txt);
	_txt=textEdit->toPlainText().split("\n");
}
void MainWindowImpl::quit(void)
{
	QCoreApplication::quit();
	
	
}
//
