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
}

void MainWindowImpl::check(void)
{
	hexParse::check(txt);
}

void MainWindowImpl::open(void)
{
	QString path;
	path=QFileDialog::getOpenFileName(this,"Choose a File",".",tr("Hex File (*.hex)"));
	label1->setText(path);
	
	QFile file(path);
	qDebug ()<<"Ouverture du fichier  "<<path<<"taille du fichier "<<file.size();
	
	//ouverture du fichier
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
