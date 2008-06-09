#ifndef __hexview__
#define __hexview__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/grid.h>
#include <wx/sizer.h>
#include <wx/scrolwin.h>
#include <wx/panel.h>
#include "read_hexfile.h"
#include <iostream>
using namespace std;


class UppHexview : public wxPanel 
{
	private:
		void autoSizeColumns (void);
		void setLabels (void);
		ReadHexFile* readHexFile;
	protected:
		wxScrolledWindow* hexViewScrolledWindow;
		wxStaticText* labelCode;
		wxGrid* codeGrid;
		wxStaticText* labelConfig;
		wxGrid* configGrid;
		wxStaticText* labelData;
		wxGrid* dataGrid;
		
		void OnCodeChanged( wxGridEvent& event );
		void OnDataChanged( wxGridEvent& event );
		void OnConfigChanged( wxGridEvent& event );
		
	
	public:
		bool OnResize(wxSizeEvent& event);
		void putHexFile(ReadHexFile* hexFile);
		UppHexview( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~UppHexview();
	
};

#endif //__hexview__
