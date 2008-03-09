#include "mainwindowimpl.h"
#include <QFileDialog>
#include <QtGlobal>
#include <QDebug>
#include <iostream>
#include "hexparse.h"


//

MainWindowImpl::MainWindowImpl( QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
	setupUi(this);
	connect(quitButton, SIGNAL(clicked()),this,SLOT(quit()));
	connect(openButton,SIGNAL(clicked()),this,SLOT(open()));
	connect(checkHex,SIGNAL(clicked()),this,SLOT(check()));
	connect(programButton,SIGNAL(clicked()),this,SLOT(writeCode()));
	connect(connectButton,SIGNAL(clicked()),this,SLOT(connectProgrammer()));
	connect(eraseButton,SIGNAL(clicked()),this,SLOT(eraseDevice()));
	connect(deviceIdButton,SIGNAL(clicked()),this,SLOT(getId()));
	connect(readprogramButton,SIGNAL(clicked()),this,SLOT(readCode()));
	connect(dataButton,SIGNAL(clicked()),this,SLOT(writeData()));
	connect(rDataButton,SIGNAL(clicked()),this,SLOT(readData()));
	connect(configButton,SIGNAL(clicked()),this,SLOT(writeConfig()));
	connect(rConfigButton,SIGNAL(clicked()),this,SLOT(readConfig()));
	
}

void MainWindowImpl::connectProgrammer(void)
{
	_prog = new programmer();
	if (_prog->connected())
	{
		programButton->setEnabled(true);
		readprogramButton->setEnabled(true);
		deviceIdButton->setEnabled(true);
		eraseButton->setEnabled(true);
		dataButton->setEnabled(true);
		rDataButton->setEnabled(true);
		configButton->setEnabled(true);
		rConfigButton->setEnabled(true);
	}
	
}

void MainWindowImpl::getId(void)
{
	char id[256],text[256];
	_prog->getId(id,64);
	sprintf(text,"%2X %2X", id[0]&0xFF, id[1]&0xFF);
	textEdit->append(text);
}

void MainWindowImpl::readCode(void)
{
	char id[256],text[256];
	int nBytes;
	//_prog->getId(id,64);
	for(int address=0x0;address<0x3FF;address+=32)
	{
		
		if(address==0x0)nBytes=_prog->read_code(id,address,32,1);
		else if(address==0x3FF-31)nBytes=_prog->read_code(id,address,32,2);
		else nBytes=_prog->read_code(id,address,32,0);
		for(int i=0;i<nBytes;i+=16)
		{
			sprintf(text,"%2X%2X %2X%2X %2X%2X %2X%2X %2X%2X %2X%2X %2X%2X %2X%2X ",
				id[i+1]&0xFF,id[i]&0xFF,id[i+3]&0xFF,id[i+2]&0xFF,
				id[i+5]&0xFF,id[i+4]&0xFF,id[i+7]&0xFF,id[i+6]&0xFF,
				id[i+9]&0xFF,id[i+8]&0xFF,id[i+11]&0xFF,id[i+10]&0xFF,
				id[i+13]&0xFF,id[i+12]&0xFF,id[i+15]&0xFF,id[i+14]&0xFF);
			textEdit->append(text);
		}
	}
	
}


void MainWindowImpl::eraseDevice(void)
{
	unsigned char msg[64];
	msg[0]=0x10;
	
	_prog->write((char*)msg,1);
	
	textEdit->append(_prog->readResponse());

}


void MainWindowImpl::writeCode(void)
{
	unsigned char prgrm[100];
	prgrm[0]=0x30;
	prgrm[1]=32;
	prgrm[2]=0; prgrm[3]=0; prgrm[4]=0; //address 0
	prgrm[5]=0; //not last block
	for(int i=0;i<32;i++)prgrm[i+6]=(unsigned char)i;
	_prog->write((const char*)prgrm,38);
	textEdit->append(_prog->readResponse());
	

	prgrm[0]=0x30;
	prgrm[1]=32;
	prgrm[2]=0; prgrm[3]=0; prgrm[4]=32; //address 32
	prgrm[5]=1; //last block
	for(int i=0;i<32;i++)prgrm[i+6]=(unsigned char)i;
	_prog->write((const char*)prgrm,38);
	textEdit->append(_prog->readResponse());
}

void MainWindowImpl::writeData(void)
{
	unsigned char prgrm[100];
	unsigned char datasize=32;
	prgrm[0]=0x50;
	prgrm[1]=datasize;
	prgrm[2]=0; prgrm[3]=0; //address 0
	prgrm[4]=1; //first block
	for(int i=0;i<datasize;i++)prgrm[i+5]=(unsigned char)i+2;
	_prog->write((const char*)prgrm,datasize+5);
	textEdit->append(_prog->readResponse());
	
	prgrm[0]=0x50;
	prgrm[1]=datasize;
	prgrm[2]=0; prgrm[3]=datasize; //address 0
	prgrm[4]=2; //last block
	for(int i=0;i<datasize;i++)prgrm[i+5]=(unsigned char)(i+2+datasize);
	_prog->write((const char*)prgrm,datasize+5);
	textEdit->append(_prog->readResponse());
}

void MainWindowImpl::readData(void)
{
		char id[256],text[256];
	int nBytes;
	//_prog->getId(id,64);
	//for(int address=0x0;address<0x7FFF;address+=32)
	//{
		
		//if(address==0x0)nBytes=_prog->read_code(id,address,32,1);
		//else if(address==0x7FFF-31)nBytes=_prog->read_code(id,address,32,2);
		int address=0;
		nBytes=_prog->read_data(id,address,32,3);
		for(int i=0;i<nBytes;i+=16)
		{
			sprintf(text,"%2X%2X %2X%2X %2X%2X %2X%2X %2X%2X %2X%2X %2X%2X %2X%2X ",
				id[i+1]&0xFF,id[i]&0xFF,id[i+3]&0xFF,id[i+2]&0xFF,
				id[i+5]&0xFF,id[i+4]&0xFF,id[i+7]&0xFF,id[i+6]&0xFF,
				id[i+9]&0xFF,id[i+8]&0xFF,id[i+11]&0xFF,id[i+10]&0xFF,
				id[i+13]&0xFF,id[i+12]&0xFF,id[i+15]&0xFF,id[i+14]&0xFF);
			textEdit->append(text);
		}
		address=32;
		nBytes=_prog->read_data(id,address,32,3);
		for(int i=0;i<nBytes;i+=16)
		{
			sprintf(text,"%2X%2X %2X%2X %2X%2X %2X%2X %2X%2X %2X%2X %2X%2X %2X%2X ",
				id[i+1]&0xFF,id[i]&0xFF,id[i+3]&0xFF,id[i+2]&0xFF,
				id[i+5]&0xFF,id[i+4]&0xFF,id[i+7]&0xFF,id[i+6]&0xFF,
				id[i+9]&0xFF,id[i+8]&0xFF,id[i+11]&0xFF,id[i+10]&0xFF,
				id[i+13]&0xFF,id[i+12]&0xFF,id[i+15]&0xFF,id[i+14]&0xFF);
			textEdit->append(text);
		}
	//}
	
}

void MainWindowImpl::writeConfig(void)
{
	unsigned char prgrm[100]="\x00\x00\x00\x00\x00\x00\x3F\xCF\x3F\x1F\x00\x87\xE5\x00\x0F\xC0\x0F\xE0\x0F\x40";
	
	prgrm[0]=0x70;
	prgrm[1]=13;
	prgrm[2]=0x30; prgrm[3]=0; prgrm[4]=0; //address 0
	prgrm[5]=1; //last block
	
	_prog->write((const char*)prgrm,19);
	textEdit->append(_prog->readResponse());
}

void MainWindowImpl::readConfig(void)
{
  char id[256],text[256];
	int nBytes;

		int address=0x300000;
		
		nBytes=_prog->read_code(id,address,14,3);
		//for(int i=0;i<nBytes;i+=16)
		//{
			sprintf(text,"%2X%2X %2X%2X %2X%2X %2X%2X %2X%2X %2X%2X %2X%2X ",
				id[0]&0xFF,id[1]&0xFF,id[2]&0xFF,id[3]&0xFF,
				id[4]&0xFF,id[5]&0xFF,id[6]&0xFF,id[7]&0xFF,
				id[8]&0xFF,id[9]&0xFF,id[10]&0xFF,id[11]&0xFF,
				id[12]&0xFF,id[13]&0xFF);
			textEdit->append(text);
		//}
	//}
	
}


void MainWindowImpl::check(void)
{
	//get each line of the document
	QStringList txt=textEdit->toPlainText().split("\n");
	QString newText("TYPE	ADDRESS		DATA\n");
	
	foreach ( QString line,txt)
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

}
void MainWindowImpl::quit(void)
{
	QCoreApplication::quit();
	
	
}
//
