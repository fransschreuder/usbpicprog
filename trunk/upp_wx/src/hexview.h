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
#include <wx/menu.h>
#include <wx/utils.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>
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
        wxFlexGridSizer* bSizer2;
       	wxBoxSizer* hexViewBoxSizer;
		wxScrolledWindow* hexViewScrolledWindow;
		wxStaticText* labelCode;
		wxGrid* codeGrid;
		wxStaticText* labelConfig;
		wxGrid* configGrid;
		wxStaticText* labelData;
		wxGrid* dataGrid;
		void OnCopy (wxCommandEvent& event);
		void OnSelectAll (wxCommandEvent& event);
		void OnCodeRightClicked (wxGridEvent& event );
		void OnCodeChanged( wxGridEvent& event );
		void OnDataChanged( wxGridEvent& event );
		void OnConfigChanged( wxGridEvent& event );
		
	
	public:
        void Copy(void);
        void SelectAll(void);
		void putHexFile(ReadHexFile* hexFile);
		UppHexview( wxWindow* parent, wxWindowID id = wxID_ANY, wxString title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~UppHexview();
	
};

#endif //__hexview__
