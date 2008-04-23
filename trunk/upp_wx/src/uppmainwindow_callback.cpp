#include "uppmainwindow_callback.h"
#include "uppmainwindow.h"


void UppMainWindowCallBack::on_open(wxCommandEvent& event)
{
	event.Skip();
	m_comboBox1->Append(wxT("Open"));
	cout<<"Open"<<endl;
}

void UppMainWindowCallBack::on_help(wxCommandEvent& event)
{
	event.Skip();
	m_comboBox1->Append(wxT("Help"));
	cout<<"Help"<<endl;
}

void UppMainWindowCallBack::on_program(wxCommandEvent& event)
{
	event.Skip();
	m_comboBox1->Append(wxT("Program"));
	cout<<"Program"<<endl;
}

UppMainWindowCallBack::UppMainWindowCallBack(wxWindow* parent, wxWindowID id , const wxString& title , const wxPoint& pos , const wxSize& size , long style ) : UppMainWindow( parent, id, title, pos, size, style )
{
	m_comboBox1->Append(wxT("P12F629"));
	m_comboBox1->Append(wxT("P18F2550"));
	m_comboBox1->Append(wxT("P18F4620"));
}
