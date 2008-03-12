#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H
//
#include <QMainWindow>
#include "ui_mainwindow.h"
#include "programmer.h"
//
class MainWindowImpl : public QMainWindow, public Ui::Programmateur
{
Q_OBJECT
public:
	MainWindowImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
private:
	QString txt;
	programmer * _prog;
	
private slots:
	void open(void);
	void quit(void);
	void check(void);
	void writeCode(void);
	void connectProgrammer(void);
	void eraseDevice (void);
	void getId(void);
	void readCode(void);
	void writeData(void);
	void readData(void);
	void writeConfig(void);
	void readConfig(void);
	void getVersion(void);
	void changePicType(int index);

};
#endif




