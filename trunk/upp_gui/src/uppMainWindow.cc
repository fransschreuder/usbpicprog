#include "config.h"
#include "uppMainWindow.hh"
#include <gtkmm.h>
#include <iostream>
using namespace std;
void uppMainWindow::on_new1_activate()
{
	
	cout<<"new"<<endl;
	
}

void uppMainWindow::on_open1_activate()
{  
}


void uppMainWindow::on_reload_activate()
{  
}

void uppMainWindow::on_save_activate()
{  
}

void uppMainWindow::on_save_as_activate()
{  
}

void uppMainWindow::on_quit_activate()
{  
}

void uppMainWindow::on_program_activate()
{  
}

void uppMainWindow::on_bulkerase_activate()
{  
}

void uppMainWindow::on_read_activate()
{  
}

void uppMainWindow::on_verify_activate()
{  
}

void uppMainWindow::on_about_activate()
{  
}

void uppMainWindow::on_newButton_activate()
{  
	on_new1_activate();
	cout<<"newButton";
}

void uppMainWindow::on_openButton_activate()
{  
	on_open1_activate();
}

void uppMainWindow::on_reloadButton_activate()
{  
	on_reload_activate();
}

void uppMainWindow::on_saveButton_activate()
{  
	on_save_activate();
}
