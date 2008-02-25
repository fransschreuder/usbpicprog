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
}

void MainWindowImpl::connectProgrammer(void)
{
	_prog = new programmer();
	programButton->setEnabled(true);
}

void MainWindowImpl::program(void)
{
	_prog->write("toto");
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
