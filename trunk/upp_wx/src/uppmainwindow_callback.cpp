#include "uppmainwindow_callback.h"
#include "uppmainwindow.h"


void UppMainWindowCallBack::on_tool1_clicked(wxCommandEvent& event)

{
	event.Skip();
	m_comboBox1->Append(wxT("Hallo"));
	cout<<"Hello World"<<endl;
}


UppMainWindowCallBack::UppMainWindowCallBack(wxWindow* parent, wxWindowID id , const wxString& title , const wxPoint& pos , const wxSize& size , long style ) : UppMainWindow( parent, id, title, pos, size, style )
{
	wxInitAllImageHandlers();
	m_comboBox1->Append(wxT("P12F629"));
	m_comboBox1->Append(wxT("P18F2550"));
	m_comboBox1->Append(wxT("P18F4620"));
}
