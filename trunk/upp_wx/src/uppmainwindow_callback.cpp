#include "uppmainwindow_callback.h"
#include "uppmainwindow.h"

void UppMainWindowCallBack::on_new(wxCommandEvent& event)
{
	event.Skip();
	m_comboBox1->Append(wxT("New"));
	cout<<"New"<<endl;
}

void UppMainWindowCallBack::on_open(wxCommandEvent& event)
{
	event.Skip();
	m_comboBox1->Append(wxT("Open"));
	cout<<"Open"<<endl;
}

void UppMainWindowCallBack::on_refresh(wxCommandEvent& event)
{
	event.Skip();
	m_comboBox1->Append(wxT("Refresh"));
	cout<<"Refresh"<<endl;
}

void UppMainWindowCallBack::on_save(wxCommandEvent& event)
{
	event.Skip();
	m_comboBox1->Append(wxT("Save"));
	cout<<"Save"<<endl;
}

void UppMainWindowCallBack::on_save_as(wxCommandEvent& event)
{
	event.Skip();
	m_comboBox1->Append(wxT("Save as"));
	cout<<"Save as"<<endl;
}

void UppMainWindowCallBack::on_exit(wxCommandEvent& event)
{
	event.Skip();
	
	m_comboBox1->Append(wxT("exit"));
	cout<<"exit"<<endl;
	Close();
}


void UppMainWindowCallBack::on_program(wxCommandEvent& event)
{
	event.Skip();
	m_comboBox1->Append(wxT("Program"));
	cout<<"Program"<<endl;
}

void UppMainWindowCallBack::on_read(wxCommandEvent& event)
{
	event.Skip();
	m_comboBox1->Append(wxT("Read"));
	cout<<"Read"<<endl;
}

void UppMainWindowCallBack::on_verify(wxCommandEvent& event)
{
	event.Skip();
	m_comboBox1->Append(wxT("Verify"));
	cout<<"Verify"<<endl;
}

void UppMainWindowCallBack::on_erase(wxCommandEvent& event)
{
	event.Skip();
	m_comboBox1->Append(wxT("Erase"));
	cout<<"Program"<<endl;
}

void UppMainWindowCallBack::on_blankcheck(wxCommandEvent& event)
{
	event.Skip();
	m_comboBox1->Append(wxT("Blankcheck"));
	cout<<"Blankcheck"<<endl;
}

void UppMainWindowCallBack::on_autodetect(wxCommandEvent& event)
{
	event.Skip();
	m_comboBox1->Append(wxT("Autodetect"));
	cout<<"Autodetect"<<endl;
}

void UppMainWindowCallBack::on_connect(wxCommandEvent& event)
{
	event.Skip();
	m_comboBox1->Append(wxT("Connect"));
	cout<<"Connect"<<endl;
}

void UppMainWindowCallBack::on_disconnect(wxCommandEvent& event)
{
	event.Skip();
	m_comboBox1->Append(wxT("Disconnect"));
	cout<<"Disconnect"<<endl;
}

void UppMainWindowCallBack::on_help(wxCommandEvent& event)
{
	event.Skip();
	m_comboBox1->Append(wxT("Help"));
	cout<<"Help"<<endl;
}

void UppMainWindowCallBack::on_about(wxCommandEvent& event)
{
	event.Skip();
	m_comboBox1->Append(wxT("About"));
	cout<<"About"<<endl;
}

void UppMainWindowCallBack::on_combo_changed(wxCommandEvent& event)
{
	event.Skip();
	m_comboBox1->Append(wxT("Combo changed"));
	cout<<"Combo changed"<<endl;
}


UppMainWindowCallBack::UppMainWindowCallBack(wxWindow* parent, wxWindowID id , const wxString& title , const wxPoint& pos , const wxSize& size , long style ) : UppMainWindow( parent, id, title, pos, size, style )
{
	//m_comboBox1->SetSize (wxSize(100, 24));
	m_comboBox1->Append(wxT("P12F629"));
	m_comboBox1->Append(wxT("P18F2550"));
	m_comboBox1->Append(wxT("P18F4620"));
}
