// generated 2008/4/11 20:28:45 CEST by frans@ubuntu.(none)
// using glademm V2.6.0
//
// DO NOT EDIT THIS FILE ! It was created using
// glade-- /home/frans/usbpicprog/trunk/upp_gui/usbpicprog.glade
// for gtk 2.12.0 and gtkmm 2.12.0
//
// Please modify the corresponding derived classes in ./src/uppMainWindow.hh and./src/uppMainWindow.cc

#ifndef _UPPMAINWINDOW_GLADE_HH
#  define _UPPMAINWINDOW_GLADE_HH

// Since all your widgets were private I made them all public.
// To differentiate between accessable (e.g. entries, output labels)
// and unaccessible widgets (e.g. static labels, containers)
// you should use the 'visibility' property (only visible in C++ mode)


#if !defined(GLADEMM_DATA)
#define GLADEMM_DATA 
#include <gtkmm/accelgroup.h>

class GlademmData
{  
        
        Glib::RefPtr<Gtk::AccelGroup> accgrp;
public:
        
        GlademmData(Glib::RefPtr<Gtk::AccelGroup> ag) : accgrp(ag)
        {  
        }
        
        Glib::RefPtr<Gtk::AccelGroup>  getAccelGroup()
        {  return accgrp;
        }
};
#endif //GLADEMM_DATA

#include <gtkmm/window.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/image.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/menu.h>
#include <gtkmm/menubar.h>
#include <gtkmm/button.h>
#if GTKMM_MAJOR_VERSION==2 && GTKMM_MINOR_VERSION>2
#include <gtkmm/toolitem.h>
#endif //
#if GTKMM_MAJOR_VERSION==2 && GTKMM_MINOR_VERSION>2
#endif //
#if GTKMM_MAJOR_VERSION==2 && GTKMM_MINOR_VERSION>2
#endif //
#if GTKMM_MAJOR_VERSION==2 && GTKMM_MINOR_VERSION>2
#endif //
#if GTKMM_MAJOR_VERSION==2 && GTKMM_MINOR_VERSION>2
#endif //
#if GTKMM_MAJOR_VERSION==2 && GTKMM_MINOR_VERSION>2
#endif //
#include <gtkmm/combobox.h>
#if GTKMM_MAJOR_VERSION==2 && GTKMM_MINOR_VERSION>2
#endif //
#include <gtkmm/toolbar.h>
#include <gtkmm/textview.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/box.h>

class uppMainWindow_glade : public Gtk::Window
{  
        
        GlademmData *gmm_data;
public:
        class Gtk::Window * uppMainWindow;
        class Gtk::ImageMenuItem * newMenuItem;
        class Gtk::ImageMenuItem * openMenuItem;
        class Gtk::Image * image9;
        class Gtk::ImageMenuItem * reloadMenuItem;
        class Gtk::ImageMenuItem * saveMenuItem;
        class Gtk::ImageMenuItem * save_asMenuItem;
        class Gtk::MenuItem * separatormenuitem1;
        class Gtk::ImageMenuItem * quitMenuItem;
        class Gtk::Menu * menuFile_menu;
        class Gtk::MenuItem * menuFile;
        class Gtk::MenuItem * programMenuItem;
        class Gtk::MenuItem * bulkeraseMenuItem;
        class Gtk::MenuItem * readMenuItem;
        class Gtk::MenuItem * verifyMenuItem;
        class Gtk::Menu * menuActions_menu;
        class Gtk::MenuItem * menuActions;
        class Gtk::MenuItem * menuView;
        class Gtk::MenuItem * aboutMenuItem;
        class Gtk::Menu * menuHelp_menu;
        class Gtk::MenuItem * menuHelp;
        class Gtk::MenuBar * uppMainMenu;
        class Gtk::Image * image4;
        class Gtk::Button * newButton;
#if GTKMM_MAJOR_VERSION==2 && GTKMM_MINOR_VERSION>2
        class Gtk::ToolItem * newButtonToolItem;
#endif //
        class Gtk::Image * image5;
        class Gtk::Button * openButton;
#if GTKMM_MAJOR_VERSION==2 && GTKMM_MINOR_VERSION>2
        class Gtk::ToolItem * openButtonToolItem;
#endif //
        class Gtk::Image * image6;
        class Gtk::Button * reloadButton;
#if GTKMM_MAJOR_VERSION==2 && GTKMM_MINOR_VERSION>2
        class Gtk::ToolItem * reloadButtonToolItem;
#endif //
        class Gtk::Image * image7;
        class Gtk::Button * saveButton;
#if GTKMM_MAJOR_VERSION==2 && GTKMM_MINOR_VERSION>2
        class Gtk::ToolItem * saveButtonToolItem;
#endif //
        class Gtk::Image * image8;
        class Gtk::Button * saveAsButton;
#if GTKMM_MAJOR_VERSION==2 && GTKMM_MINOR_VERSION>2
        class Gtk::ToolItem * saveAsButtonToolItem;
#endif //
#if GTKMM_MAJOR_VERSION==2 && GTKMM_MINOR_VERSION>2
        class Gtk::ToolItem * picTypeToolItem;
#endif //
        class Gtk::ComboBox * picTypeCombo;
#if GTKMM_MAJOR_VERSION==2 && GTKMM_MINOR_VERSION>2
        class Gtk::ToolItem * toolitem10;
#endif //
        class Gtk::Toolbar * toolBar;
        class Gtk::TextView * hexView;
        class Gtk::ScrolledWindow * scrolledwindow1;
        class Gtk::Statusbar * statusBar;
        class Gtk::VBox * topBox;
protected:
        
        uppMainWindow_glade();
        
        ~uppMainWindow_glade();
private:
        virtual void on_new_activate() = 0;
        virtual void on_open_activate() = 0;
        virtual void on_reload_activate() = 0;
        virtual void on_save_activate() = 0;
        virtual void on_save_as_activate() = 0;
        virtual void on_quit_activate() = 0;
        virtual void on_program_activate() = 0;
        virtual void on_bulkerase_activate() = 0;
        virtual void on_read_activate() = 0;
        virtual void on_verify_activate() = 0;
        virtual void on_about_activate() = 0;
        virtual void on_newButton_clicked() = 0;
        virtual void on_openButton_clicked() = 0;
        virtual void on_reloadButton_clicked() = 0;
        virtual void on_saveButton_clicked() = 0;
        virtual void on_saveAsButton_clicked() = 0;
};
#endif
