#include "hexview.h"


#define ROWLABELWIDTH 120
#define OTHERWIDTH 52
#define COLWIDTH 28
UppHexview::UppHexview( wxWindow* parent, wxWindowID id,wxString title, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{

	hexViewBoxSizer = new wxBoxSizer( wxVERTICAL );
	
	hexViewScrolledWindow = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	hexViewScrolledWindow->SetScrollRate( 10, 10 );
	
	bSizer2 = new wxFlexGridSizer( 6,1,-1,-1 );
	
	labelCode = new wxStaticText( hexViewScrolledWindow, wxID_ANY, wxT("Code Memory"), wxDefaultPosition, wxDefaultSize, 0 );
	//labelCode->Wrap( -1 );
	bSizer2->Add( labelCode, 0, wxALL, 5 );
	
	codeGrid = new wxGrid( hexViewScrolledWindow, -1 , wxDefaultPosition, wxDefaultSize,wxTAB_TRAVERSAL | wxBORDER_SUNKEN);
	
	// Grid
	codeGrid->CreateGrid( 0, 24 );
	codeGrid->EnableEditing( true );
	codeGrid->EnableGridLines( false );
	codeGrid->EnableDragGridSize( false );
	codeGrid->SetMargins( 0, 0 );
	
	// Columns
	//codeGrid->EnableDragColMove( false );
	codeGrid->EnableDragColSize( false );
	codeGrid->SetColLabelSize( 30 );
	codeGrid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	codeGrid->EnableDragRowSize( false );
	codeGrid->SetRowLabelSize( ROWLABELWIDTH );
	codeGrid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	//codeGrid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	bSizer2->Add( codeGrid, 1, wxALL|wxALIGN_TOP, 5 );
	
	labelConfig = new wxStaticText( hexViewScrolledWindow, wxID_ANY, wxT("Config Memory"), wxDefaultPosition, wxDefaultSize, 0 );
	//labelConfig->Wrap( -1 );
	bSizer2->Add( labelConfig, 0, wxALL, 5 );
	
	configGrid = new wxGrid( hexViewScrolledWindow, -1 , wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_SUNKEN );
	
	// Grid
	configGrid->CreateGrid( 0, 24 );
	configGrid->EnableEditing( true );
	configGrid->EnableGridLines( false );
	configGrid->EnableDragGridSize( false );
	configGrid->SetMargins( 0, 0 );
	
	// Columns
	//configGrid->EnableDragColMove( false );
	configGrid->EnableDragColSize( false );
	configGrid->SetColLabelSize( 30 );
	configGrid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	configGrid->EnableDragRowSize( true );
	configGrid->SetRowLabelSize( ROWLABELWIDTH );
	configGrid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	//configGrid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	bSizer2->Add( configGrid, 1, wxALL|wxALIGN_TOP, 5 );
	
	labelData = new wxStaticText( hexViewScrolledWindow, wxID_ANY, wxT("Data Memory"), wxDefaultPosition, wxDefaultSize, 0 );
	//labelData->Wrap( -1 );
	bSizer2->Add( labelData, 0, wxALL, 5 );
	
	dataGrid = new wxGrid( hexViewScrolledWindow, -1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_SUNKEN );
	
	// Grid
	dataGrid->CreateGrid( 0, 24 );
	dataGrid->EnableEditing( true );
	dataGrid->EnableGridLines( false );
	dataGrid->EnableDragGridSize( false );
	dataGrid->SetMargins( 0, 0 );
	
	// Columns
	//dataGrid->EnableDragColMove( false );
	dataGrid->EnableDragColSize( false );
	dataGrid->SetColLabelSize( 30 );
	dataGrid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	dataGrid->EnableDragRowSize( false );
	dataGrid->SetRowLabelSize( ROWLABELWIDTH );
	dataGrid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	//dataGrid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	bSizer2->Add( dataGrid, 1, wxALL|wxALIGN_TOP, 5 );
	
	hexViewScrolledWindow->SetSizer( bSizer2 );
	hexViewScrolledWindow->Layout();
	bSizer2->Fit( hexViewScrolledWindow );
	hexViewBoxSizer->Add( hexViewScrolledWindow, 1, wxEXPAND | wxALL, 5 );

	this->SetSizer( hexViewBoxSizer );
	this->Layout();
	setLabels ();
	autoSizeColumns();
	hexViewBoxSizer->Fit( this );
	
	// Connect Events
	codeGrid->Connect( wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( UppHexview::OnCodeChanged ), NULL, this );
	configGrid->Connect( wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( UppHexview::OnConfigChanged ), NULL, this );
	dataGrid->Connect( wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( UppHexview::OnDataChanged ), NULL, this );
	codeGrid->Connect( wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler( UppHexview::OnCodeRightClicked ), NULL, this );
	configGrid->Connect( wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler( UppHexview::OnCodeRightClicked ), NULL, this );
	dataGrid->Connect( wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler( UppHexview::OnCodeRightClicked ), NULL, this );
	this->Connect( wxID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppHexview::OnCopy ) );
	this->Connect( wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppHexview::OnSelectAll ) );
	Refresh();
	readHexFile=NULL;
	
}


UppHexview::~UppHexview()
{
	// Disconnect Events
	codeGrid->Disconnect( wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( UppHexview::OnCodeChanged ), NULL, this );
	configGrid->Disconnect( wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( UppHexview::OnCodeChanged ), NULL, this );
	dataGrid->Disconnect( wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( UppHexview::OnDataChanged ), NULL, this );
}


void UppHexview::putHexFile(ReadHexFile* hexFile)
{
	readHexFile=hexFile;
	codeGrid->DeleteRows(0, codeGrid->GetNumberRows(),false);
	codeGrid->AppendRows(((readHexFile->getCodeMemory().size()%codeGrid->GetNumberCols())>0)+readHexFile->getCodeMemory().size()/codeGrid->GetNumberCols(),false);
	
	for(unsigned int i=0;i<readHexFile->getCodeMemory().size();i++)
	{
		codeGrid->SetCellValue(i/(codeGrid->GetNumberCols()),i%(codeGrid->GetNumberCols()) , 
							   wxString::Format(wxT("%02X"),readHexFile->getCodeMemory()[i]));
	}
	configGrid->DeleteRows(0, configGrid->GetNumberRows(),false);
	configGrid->AppendRows(((readHexFile->getConfigMemory().size()%configGrid->GetNumberCols())>0)+readHexFile->getConfigMemory().size()/configGrid->GetNumberCols(),false);
	for(unsigned int i=0;i<readHexFile->getConfigMemory().size();i++)
	{
		configGrid->SetCellValue(i/(configGrid->GetNumberCols()),i%(configGrid->GetNumberCols()) , 
							   wxString::Format(wxT("%02X"),readHexFile->getConfigMemory()[i]));
	}
	dataGrid->DeleteRows(0, dataGrid->GetNumberRows(),false);
	dataGrid->AppendRows(((readHexFile->getCodeMemory().size()%codeGrid->GetNumberCols())>0)+readHexFile->getDataMemory().size()/dataGrid->GetNumberCols(),false);
	
	for(unsigned int i=0;i<readHexFile->getDataMemory().size();i++)
	{
		dataGrid->SetCellValue(i/(dataGrid->GetNumberCols()),i%(dataGrid->GetNumberCols()) , 
							   wxString::Format(wxT("%02X"),readHexFile->getDataMemory()[i]));
	}

	setLabels();
	Fit();
	
}

void UppHexview::setLabels(void)
{
	for(int i=0;i<codeGrid->GetNumberRows();i++)
		codeGrid->SetRowLabelValue(i,wxString::Format(wxT("%06X"),i*codeGrid->GetNumberCols()));
	for(int i=0;i<configGrid->GetNumberRows();i++)
		configGrid->SetRowLabelValue(i,wxString::Format(wxT("%06X"),i*configGrid->GetNumberCols()));
	for(int i=0;i<dataGrid->GetNumberRows();i++)
		dataGrid->SetRowLabelValue(i,wxString::Format(wxT("%06X"),i*dataGrid->GetNumberCols()));
	
	for(int i=0;i<codeGrid->GetNumberCols();i++)
		codeGrid->SetColLabelValue(i,wxString::Format(wxT("%02X"),i));
	for(int i=0;i<configGrid->GetNumberCols();i++)
		configGrid->SetColLabelValue(i,wxString::Format(wxT("%02X"),i));								   
	for(int i=0;i<dataGrid->GetNumberCols();i++)
		dataGrid->SetColLabelValue(i,wxString::Format(wxT("%02X"),i));								   
}

void UppHexview::autoSizeColumns(void)
{
	/*codeGrid->AutoSizeColumns(true);
	dataGrid->AutoSizeColumns(true);
	configGrid->AutoSizeColumns(true);*/
	for(int i=0;i<codeGrid->GetNumberCols();i++)
	{
		codeGrid->SetColSize(i, COLWIDTH);
		configGrid->SetColSize(i, COLWIDTH);
		dataGrid->SetColSize(i,COLWIDTH);
	}
}

void UppHexview::OnCopy (wxCommandEvent& event)
{
	/*wxClipboard *clipboard = new wxClipboard();
	wxTextDataObject *dataobj = new wxTextDataObject(wxT(""));
	cout<<codeGrid->GetSelectedCells().<<endl;
	
	dataobj->SetData(datastr.Length(),&datastr);
	if(	clipboard->Open());
	{
		clipboard->SetData(dataobj);
		clipboard->Close();
	}
	//delete clipboard;
	*/
	cerr<<"not implemented yet"<<endl;
}

void UppHexview::OnSelectAll (wxCommandEvent& event)
{
	codeGrid->SelectAll();
	configGrid->SelectAll();
	dataGrid->SelectAll();
}

void UppHexview::OnCodeRightClicked (wxGridEvent& event)
{
	wxMenu oMenu(wxT("Edit"));
	wxPoint oPos;
	oMenu.Append(wxID_COPY, wxT("Copy"));
	oMenu.Append(wxID_SELECTALL, wxT("Select All"));
	oPos=ScreenToClient(wxGetMousePosition());
//wxGetMousePosition();
	
	PopupMenu(&oMenu, oPos.x, oPos.y);
	
}

void UppHexview::OnCodeChanged (wxGridEvent& event )
{
	int Position=event.GetCol()+(event.GetRow()*codeGrid->GetNumberCols());
	
	int Data;
	wxString CellData=codeGrid->GetCellValue(event.GetRow(),event.GetCol());
	sscanf(CellData.mb_str(wxConvUTF8),"%X",&Data);
	if((Data>=0)&&(Data<=0xFF))
	{
		readHexFile->putCodeMemory(Position,Data);
	}
	
	CellData.Printf(wxT("%02X"),readHexFile->getCodeMemory(Position));
	codeGrid->SetCellValue(event.GetRow(),event.GetCol(),CellData);
}

void UppHexview::OnConfigChanged (wxGridEvent& event )
{
	int Position=event.GetCol()+(event.GetRow()*configGrid->GetNumberCols());
	
	int Data;
	wxString CellData=configGrid->GetCellValue(event.GetRow(),event.GetCol());
	sscanf(CellData.mb_str(wxConvUTF8),"%X",&Data);
	if((Data>=0)&&(Data<=0xFF))
	{
		readHexFile->putConfigMemory(Position,Data);
	}
	
	CellData.Printf(wxT("%02X"),readHexFile->getConfigMemory(Position));
	configGrid->SetCellValue(event.GetRow(),event.GetCol(),CellData);
}

void UppHexview::OnDataChanged (wxGridEvent& event )
{
	int Position=event.GetCol()+(event.GetRow()*dataGrid->GetNumberCols());
	
	int Data;
	wxString CellData=dataGrid->GetCellValue(event.GetRow(),event.GetCol());
	sscanf(CellData.mb_str(wxConvUTF8),"%X",&Data);
	if((Data>=0)&&(Data<=0xFF))
	{
		readHexFile->putDataMemory(Position,Data);
	}
	
	CellData.Printf(wxT("%02X"),readHexFile->getDataMemory(Position));
	dataGrid->SetCellValue(event.GetRow(),event.GetCol(),CellData);
}

