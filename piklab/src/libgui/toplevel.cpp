/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "toplevel.h"

#include <qpixmap.h>
#include <qiconset.h>
#include <qlayout.h>
#include <qsplitter.h>
#include <qstringlist.h>
#include <qtimer.h>
#include <qprogressbar.h>
#include <qeventloop.h>
#include <qapplication.h>
#include <qtooltip.h>

#include <kstatusbar.h>
#include <klocale.h>
#include <kedittoolbar.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kaction.h>
#include <kcmdlineargs.h>
#include <kio/observer.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <khelpmenu.h>
#include <kmenubar.h>

#include "gui_debug_manager.h"
#include "hex_editor.h"
#include "project_manager.h"
#include "project.h"
#include "global_config.h"
#include "prog_config_widget.h"
#include "editor.h"
#include "device_gui.h"
#include "text_editor.h"
#include "new_dialogs.h"
#include "common/global/process.h"
#include "common/gui/misc_gui.h"
#include "common/gui/purl_gui.h"
#include "devices/list/device_list.h"
#include "progs/base/prog_config.h"
#include "progs/list/prog_list.h"
#include "progs/gui/prog_group_ui.h"
#include "editor_manager.h"
#include "tools/list/compile.h"
#include "object_view.h"
#include "config_gen.h"
#include "tools/list/compile_config.h"
#include "watch_view.h"
#include "coff/base/text_coff.h"
#include "tools/list/tool_list.h"
#include "breakpoint_view.h"
#include "main_global.h"
#include "gui_prog_manager.h"
#include "devices/gui/register_view.h"
#include "likeback.h"
#include "console.h"
#include "devices/base/device_group.h"

//----------------------------------------------------------------------------
KDockWidget *MainWindow::createDock(const QString &name, const QPixmap &icon,
                                    const QString &title, const DockPosition &position)
{
  KDockWidget *dock = createDockWidget(name, icon, 0, title);
  dock->setDockSite(KDockWidget::DockCenter);
  DockData ddata;
  ddata.title = title;
  ddata.position = position;
  ddata.dock = dock;
  ddata.action = new ViewMenuAction(dock);
  connect(ddata.action, SIGNAL(activated(QWidget *)), SLOT(toggleToolView(QWidget *)));
  _docks += ddata;
  initDockPosition(ddata);
  return dock;
}

MainWindow::MainWindow()
  : _configGenerator(0), _pikloopsProcess(0), _kfindProcess(0), _forceProgramAfterBuild(false)
{
  Q_ASSERT( Main::_toplevel==0 );
  Main::_toplevel = this;

// status bar
    _actionStatus = new QLabel(statusBar());
    statusBar()->addWidget(_actionStatus);
    _actionProgress = new QProgressBar(statusBar());
    statusBar()->addWidget(_actionProgress);
    _debugStatus = new QLabel(statusBar());
    statusBar()->addWidget(_debugStatus, 0, true);
    _editorStatus = new QLabel(statusBar());
    statusBar()->addWidget(_editorStatus, 0, true);
    _programmerStatus = new ProgrammerStatusWidget(statusBar());
    connect(_programmerStatus, SIGNAL(configure()), SLOT(configureProgrammer()));
    connect(_programmerStatus, SIGNAL(selected(const Programmer::Group &)), SLOT(selectProgrammer(const Programmer::Group &)));
    statusBar()->addWidget(_programmerStatus, 0, true);
    _toolStatus = new ToolStatusWidget(statusBar());
    connect(_toolStatus, SIGNAL(configureToolchain()), SLOT(configureToolchains()));
    connect(_toolStatus, SIGNAL(configure()), SLOT(configureProject()));
    connect(_toolStatus, SIGNAL(selected(const Tool::Group &)), SLOT(selectTool(const Tool::Group &)));
    statusBar()->addWidget(_toolStatus, 0, true);

// interface
    _mainDock = createDockWidget("main_dock_widget", QPixmap());
    _mainDock->setDockSite(KDockWidget::DockCorner);
    _mainDock->setEnableDocking(KDockWidget::DockNone);
    setView(_mainDock);
    setMainDockWidget(_mainDock);

    KIconLoader loader;
    KDockWidget *dock = createDock("project_manager_dock_widget", PURL::icon(PURL::Project),
                                   i18n("Project Manager"), DockPosition(KDockWidget::DockLeft, 20));
    Main::_projectManager = new ProjectManager::View(dock);
    connect(Main::_projectManager, SIGNAL(guiChanged()), SLOT(updateGUI()));
    dock->setWidget(Main::_projectManager);

    dock = createDock("watch_view_dock_widget", loader.loadIcon("piklab_chip", KIcon::Small),
                      i18n("Watch View"), DockPosition("project_manager_dock_widget"));
    Main::_watchView = new Register::WatchView(dock);
    dock->setWidget(Main::_watchView);

    Main::_editorManager = new EditorManager(_mainDock);
    _mainDock->setWidget(Main::_editorManager);
    connect(Main::_editorManager, SIGNAL(guiChanged()), SLOT(updateGUI()));
    connect(Main::_editorManager, SIGNAL(modified(const PURL::Url &)), Main::_projectManager, SLOT(modified(const PURL::Url &)));
    connect(Main::_editorManager, SIGNAL(statusChanged(const QString &)), _editorStatus, SLOT(setText(const QString &)));

    dock = createDock("compile_log_dock_widget", loader.loadIcon("piklab_compile", KIcon::Small),
                      i18n("Compile Log"), DockPosition(KDockWidget::DockBottom, 80));
    Main::_compileLog = new Compile::LogWidget(dock);
    Main::_compileLog->setFocusPolicy(NoFocus);
    dock->setWidget(Main::_compileLog);

    dock = createDock("program_log_dock_widget", loader.loadIcon("piklab_burnchip", KIcon::Small),
                      i18n("Program Log"), DockPosition("compile_log_dock_widget"));
    _programLog = new Log::Widget(dock, "program_log");
    _programLog->setFocusPolicy(NoFocus);
    dock->setWidget(_programLog);

    dock = createDock("breakpoints_dock_widget", loader.loadIcon("piklab_breakpoint_active", KIcon::Small),
                      i18n("Breakpoints"), DockPosition("compile_log_dock_widget"));
    Main::_breakpointsView = new Breakpoint::View(dock);
    Main::_breakpointsView->setFocusPolicy(NoFocus);
    dock->setWidget(Main::_breakpointsView);

    dock = createDock("console_dock_widget", loader.loadIcon("konsole", KIcon::Small),
                      i18n("Konsole"), DockPosition("compile_log_dock_widget"));
    Main::_consoleView = new ConsoleView(dock);
    dock->setWidget(Main::_consoleView);

// managers
    Programmer::manager = new Programmer::GuiManager(this);
    Programmer::manager->setView(_programLog);
    connect(Programmer::manager, SIGNAL(actionMessage(const QString &)),
            _actionStatus, SLOT(setText(const QString &)));
    connect(Programmer::manager, SIGNAL(setTotalSteps(uint)), SLOT(setTotalSteps(uint)));
    connect(Programmer::manager, SIGNAL(addToTotalSteps(uint)), SLOT(addToTotalSteps(uint)));
    connect(Programmer::manager, SIGNAL(addProgress(uint)), SLOT(addProgress(uint)));

    Debugger::manager = new Debugger::GuiManager;
    connect(Debugger::manager, SIGNAL(targetStateChanged()), SLOT(updateGUI()));
    connect(Debugger::manager, SIGNAL(statusChanged(const QString &)),
            _debugStatus, SLOT(setText(const QString &)));
    connect(Debugger::manager, SIGNAL(actionMessage(const QString &)),
            _actionStatus, SLOT(setText(const QString &)));

    Main::_compileManager = new Compile::Manager(this);
    Main::_compileManager->setView(Main::_compileLog);
    connect(Main::_compileManager, SIGNAL(success()), SLOT(compileSuccess()));
    connect(Main::_compileManager, SIGNAL(failure()), SLOT(compileFailure()));
    connect(Main::_compileManager, SIGNAL(updateFile(const Compile::FileData &)),
            SLOT(updateFile(const Compile::FileData &)));

// actions
    // file actions
    KAction *a = KStdAction::openNew(this, SLOT(newSourceFile()), actionCollection());
    a->setText(i18n("&New Source File..."));
    (void)new KAction(i18n("New hex File..."), "filenew", 0, this, SLOT(newHexFile()),
                      actionCollection(), "file_new_hex");
    KStdAction::open(this, SLOT(openFile()), actionCollection());
    KRecentFilesAction *recent = KStdAction::openRecent(this, SLOT(openRecentFile(const KURL &)), actionCollection());
    recent->setMaxItems(20);
    recent->loadEntries(kapp->config(), "recent-files");
    (void)new KAction(i18n("Save All"), 0, 0, Main::_editorManager, SLOT(saveAllFiles()),
                      actionCollection(), "file_save_all");
    KStdAction::close(Main::_editorManager, SLOT(closeCurrentEditor()), actionCollection());
    (void)new KAction(i18n("C&lose All"), 0, 0, Main::_editorManager, SLOT(closeAllEditors()),
                      actionCollection(), "file_close_all");
    (void)new KAction(i18n("Close All Others"), 0, 0, Main::_editorManager, SLOT(closeAllOtherEditors()),
                      actionCollection(), "file_closeother");
    KStdAction::quit(this, SLOT(close()), actionCollection());

    // edit actions

    // view actions
    (void)new KAction(i18n("Back"), "back", Qt::ALT + Qt::Key_Left,
                      Main::_editorManager, SLOT(goBack()), actionCollection(), "history_back");
    (void)new KAction(i18n("Forward"), "forward", Qt::ALT + Qt::Key_Right,
                      Main::_editorManager, SLOT(goForward()), actionCollection(), "history_forward");
    (void)new KAction(i18n("Switch to..."), 0, Qt::CTRL + Qt::Key_Slash,
                      Main::_editorManager, SLOT(switchToEditor()), actionCollection(), "file_switchto");
    (void)new KAction(i18n("Switch Header/Implementation"), 0, Qt::SHIFT + Qt::Key_F12,
                      Main::_editorManager, SLOT(switchHeaderImplementation()), actionCollection(), "view_switch_source");
    (void)new KAction(QString::null, 0, 0,
                      Debugger::manager, SLOT(toggleBreakpoint()), actionCollection(), "toggle_breakpoint");
    (void)new KAction(QString::null, 0, 0,
                      Debugger::manager, SLOT(toggleEnableBreakpoint()), actionCollection(), "enable_breakpoint");
    KActionMenu *toolViewsMenu = new KActionMenu( i18n("Tool Views"), 0, "view_tool_views");
    connect(toolViewsMenu->popupMenu(), SIGNAL(aboutToShow()), SLOT(updateToolViewsActions()));
    actionCollection()->insert(toolViewsMenu);
    a = new KAction(i18n("&Reset Layout"), 0, 0, this, SLOT(resetDockLayout()), actionCollection(), "view_reset_layout");
    toolViewsMenu->insert(a);
    toolViewsMenu->popupMenu()->insertSeparator();
    QValueList<DockData>::iterator it;
    for(it=_docks.begin(); it!=_docks.end(); ++it) toolViewsMenu->insert((*it).action);

    // project actions
    (void)new KAction(i18n("New Project..."), "piklab_createproject",
                      0, this, SLOT(newProject()), actionCollection(), "project_new");
    (void)new KAction(i18n("Open Project..."), "piklab_openproject",
                      0, this , SLOT(openProject()), actionCollection(), "project_open");
    recent = new KRecentFilesAction(i18n("Open Recent Project"),
                      0, this, SLOT(openRecentProject(const KURL &)), actionCollection(), "project_open_recent");
    recent->setMaxItems(20);
    recent->loadEntries(kapp->config(), "recent-projects");
    (void)new KAction(i18n("Project Options..."), "configure",
                      0, this, SLOT(configureProject()), actionCollection(), "project_options");
    (void)new KAction(i18n("Close Project"), "fileclose",
                      0, this, SLOT(closeProject()), actionCollection(), "project_close");
    (void)new KAction(i18n("Add Source File..."), "piklab_addfile",
                      0, Main::_projectManager, SLOT(insertSourceFiles()), actionCollection(), "project_add_source_file");
    (void)new KAction(i18n("Add Object File..."), "piklab_addfile",
                      0, Main::_projectManager, SLOT(insertObjectFiles()), actionCollection(), "project_add_object_file");
    (void)new KAction(i18n("Add Current File"), "piklab_addcurrentfile",
                      0, Main::_projectManager, SLOT(insertCurrentFile()), actionCollection(), "project_add_current_file");

    // build actions
    (void)new KAction(i18n("&Build Project"), "piklab_compile",
                 Qt::Key_F8, this, SLOT(buildProject()), actionCollection(), "build_build_project");
    (void)new KAction(i18n("&Compile File"), 0,
                 Qt::Key_F10, this, SLOT(compileFile()), actionCollection(), "build_compile_file");
    (void)new KAction(i18n("Clean"), "trashcan_empty",
                 0, this, SLOT(cleanBuild()), actionCollection(), "build_clean");
    (void)new KAction(i18n("Stop"), "stop",
                 0, this, SLOT(stopBuild()), actionCollection(), "build_stop");

    // programmer actions
    (void)new KAction(i18n("&Connect"), "connect_creating",
                 0, Programmer::manager, SLOT(connectDevice()), actionCollection(), "prog_connect");
    (void)new KToggleAction(i18n("Device Power"), "piklab_power",
                 0, Programmer::manager, SLOT(toggleDevicePower()), actionCollection(), "prog_power");
    (void)new KAction(i18n("&Disconnect"), "connect_no",
                 0, Programmer::manager, SLOT(disconnectDevice()), actionCollection(), "prog_disconnect");
    (void)new KAction(i18n("&Program"), "piklab_burnchip",
                 Qt::Key_F5, this , SLOT(program()), actionCollection(), "prog_program");
    (void)new KAction(i18n("&Verify"), "piklab_verifychip",
                 Qt::Key_F6, this , SLOT(verify()), actionCollection(), "prog_verify");
    (void)new KAction(i18n("&Read"), "piklab_readchip",
                 Qt::Key_F7, this , SLOT(read()), actionCollection(), "prog_read");
    (void)new KAction(i18n("&Erase"), "piklab_erasechip",
                 0, this, SLOT(erase()), actionCollection(), "prog_erase");
    (void)new KAction(i18n("&Blank Check"), "piklab_blankcheck",
                 0, this, SLOT(blankCheck()), actionCollection(), "prog_blank_check");
    (void)new KAction(i18n("&Run"), "launch",
                 Qt::SHIFT + Qt::Key_F9, Programmer::manager, SLOT(run()), actionCollection(), "prog_run");
    (void)new KAction(i18n("&Stop"), "piklab_stop",
                 0, Programmer::manager, SLOT(halt()), actionCollection(), "prog_stop");
    (void)new KAction(i18n("R&estart"), "piklab_restart",
                 0, Programmer::manager, SLOT(restart()), actionCollection(), "prog_restart");
    (void)new KAction(i18n("&Advanced..."), 0,
                 0, Programmer::manager , SLOT(showAdvancedDialog()), actionCollection(), "prog_advanced");

    // debugger actions
    (void)new KAction(i18n("&Run"), "piklab_run",
                 Qt::SHIFT + Qt::Key_F9, Programmer::manager, SLOT(run()), actionCollection(), "debug_run");
    (void)new KAction(i18n("&Interrupt"), "piklab_debughalt",
                 0, Programmer::manager, SLOT(halt()), actionCollection(), "debug_halt");
    (void)new KAction(i18n("&Step"), "piklab_debug_step",
                 0, Programmer::manager, SLOT(step()), actionCollection(), "debug_next");
    //(void)new KAction(i18n("Step &In"), "piklab_debug_stepin",
    //             0, this, SLOT(debugStepIn()), actionCollection(), "debug_step_in");
    //(void)new KAction(i18n("Step &Out"), "piklab_debug_stepout",
    //             0, this, SLOT(debugStepOut()), actionCollection(), "debug_step_out");
    (void)new KAction(i18n("R&eset"), "piklab_restart",
                 0, Programmer::manager, SLOT(restart()), actionCollection(), "debug_reset");
    (void)new KAction(i18n("Show Program Counter"), 0,
                 0, Debugger::manager, SLOT(showPC()), actionCollection(), "debug_show_pc");
    (void)new KAction(i18n("Clear All Breakpoints"), 0,
                 0, Debugger::manager, SLOT(clearBreakpoints()), actionCollection(), "debug_clear_breakpoints");

    // tools
    (void)new KAction(i18n("&Pikloops..."), 0,
                 0, this , SLOT(runPikloops()), actionCollection(), "tools_pikloops");
    (void)new KAction(i18n("&Find Files..."), "find",
                 0, this , SLOT(runKfind()), actionCollection(), "tools_kfind");
    (void)new KAction(i18n("&Device Information..."), "info",
                 0, this , SLOT(showDeviceInfo()), actionCollection(), "tools_device_information");
    (void)new KAction(i18n("&Config Generator..."), 0,
                 0, this , SLOT(configGenerator()), actionCollection(), "tools_config_generator");
    (void)new KAction(i18n("&Template Generator..."), 0,
                 0, this , SLOT(templateGenerator()), actionCollection(), "tools_template_generator");

    // settings actions
    (void)new KAction(i18n("Configure Toolchains..."), "run", 0,
                      this, SLOT(configureToolchains()), actionCollection(), "options_configure_toolchains");
    (void)KStdAction::preferences(this, SLOT(configure()), actionCollection());

    // help
    (void)new KAction(i18n("Report Bug..."), "likeback_bug", 0,
                      LikeBack::instance(), SLOT(iFoundABug()), actionCollection(), "help_report_bug_piklab");

    setupGUI();
    readDockConfig();

    // LikeBack buttons
    menuBar()->insertItem(new QLabel(menuBar())); // #### first widget is put left-most...
    MenuBarButton *button = new MenuBarButton("likeback_like", menuBar());
    QToolTip::add(button, i18n("I like..."));
    connect(button, SIGNAL(clicked()), LikeBack::instance(), SLOT(iLike()));
    menuBar()->insertItem(button);
    button = new MenuBarButton("likeback_dislike", menuBar());
    QToolTip::add(button, i18n("I do not like..."));
    connect(button, SIGNAL(clicked()), LikeBack::instance(), SLOT(iDoNotLike()));
    menuBar()->insertItem(button);
    button = new MenuBarButton("likeback_bug", menuBar());
    QToolTip::add(button, i18n("I found a bug..."));
    connect(button, SIGNAL(clicked()), LikeBack::instance(), SLOT(iFoundABug()));
    menuBar()->insertItem(button);
    button = new MenuBarButton("configure", menuBar());
    QToolTip::add(button, i18n("Configure email..."));
    connect(button, SIGNAL(clicked()), LikeBack::instance(), SLOT(askEMail()));
    menuBar()->insertItem(button);
    button = new MenuBarButton("help", menuBar());
    connect(button, SIGNAL(clicked()), LikeBack::instance(), SLOT(showWhatsThisMessage()));
    menuBar()->insertItem(button);

    QTimer::singleShot(0, this, SLOT(initialLoading()));
}

MainWindow::~MainWindow()
{}

void MainWindow::readDockConfig()
{
  KDockMainWindow::readDockConfig(kapp->config(), "dock_config");

  // if there is a new dock: it is not displayed by default...
  QMap<QString, QString> entries = kapp->config()->entryMap("dock_config");
  QValueList<DockData>::iterator it;
  for(it=_docks.begin(); it!=_docks.end(); ++it) {
    QMap<QString, QString>::const_iterator eit;
    for(eit=entries.begin(); eit!=entries.end(); ++eit)
      if ( eit.key().startsWith((*it).dock->name()) ) break;
    if ( eit==entries.end() ) initDockPosition(*it);
  }

  // readDockConfig also restore the names/tooltips: what if a new version of the application changes these names...
  for(it=_docks.begin(); it!=_docks.end(); ++it) (*it).dock->setTabPageLabel((*it).title);
  QApplication::postEvent(this, new QEvent(QEvent::CaptionChange));
}

void MainWindow::initialLoading()
{
  ::BusyCursor bc;
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  if ( args->count()!=0 ) { // opened urls provided on the command line
    for (int i = 0; i<args->count(); i++) {
      PURL::Url url(args->url(i));
      if ( url.fileType()==PURL::Project ) {
        if ( Main::_projectManager->project()==0 ) Main::_projectManager->openProject(url);
      } else Main::_editorManager->openEditor(url);
    }
  } else { // otherwise reopen last project/files
    GlobalConfig gc;
    Main::_projectManager->openProject(gc.openedProject());
    PURL::UrlList files = gc.openedFiles();
    PURL::UrlList::const_iterator it = files.begin();
    for (; it!=files.end(); ++it) Main::_editorManager->openEditor(*it);
  }
  updateGUI();
}

void MainWindow::saveSettings()
{
  ::BusyCursor bc;
  writeDockConfig(kapp->config(), "dock_config");
  static_cast<KRecentFilesAction *>(Main::action("project_open_recent"))->saveEntries(kapp->config(), "recent-projects");
  static_cast<KRecentFilesAction *>(Main::action("file_open_recent"))->saveEntries(kapp->config(), "recent-files");
  GlobalConfig gc;
  gc.writeOpenedProject(Main::project() ? Main::project()->url() : PURL::Url());
  if ( Main::project() ) Main::_projectManager->closeProject();
  PURL::UrlList toSave;
  const PURL::UrlList files = Main::_editorManager->files();
  PURL::UrlList::const_iterator it;
  for (it=files.begin(); it!=files.end(); ++it)
    if ( !Main::_projectManager->isExternalFile(*it) ) toSave.append(*it);
  gc.writeOpenedFiles(toSave);
}

void MainWindow::openRecentFile(const KURL &kurl)
{
  Main::_editorManager->openFile(PURL::Url(kurl));
}

void MainWindow::configureToolbar()
{
  saveMainWindowSettings(KGlobal::config(), "MainWindow");
  KEditToolbar dlg(actionCollection());
  connect(&dlg, SIGNAL(newToolbarConfig()), SLOT(applyToolbarSettings()));
  dlg.exec();
}

void MainWindow::applyToolbarSettings()
{
  createGUI();
  applyMainWindowSettings(KGlobal::config(), "MainWindow");
}

void MainWindow::configure(ConfigCenter::Type showType)
{
  stopOperations();
  ConfigCenter dialog(showType, this);
  dialog.exec();
  Programmer::manager->clear();
  updateGUI();
  Debugger::manager->update();
}

void MainWindow::configureToolchains()
{
  stopOperations();
  ToolchainsConfigCenter dialog(Main::toolGroup(), this);
  dialog.exec();
  Programmer::manager->clear();
  updateGUI();
  Debugger::manager->update();
}

void MainWindow::selectProgrammer(const Programmer::Group &group)
{
  if ( group.name()==Main::programmerGroup().name() ) return;
  bool debugInitialized = Debugger::manager->coff();
  stopOperations();
  GlobalConfig gc;
  gc.writeProgrammerGroup(group);
  Programmer::manager->clear();
  updateGUI();
  if (debugInitialized) Debugger::manager->init();
  else Debugger::manager->update();
}

void MainWindow::selectTool(const Tool::Group &group)
{
  if ( group.name()==Compile::Config::toolGroup(Main::project()).name() ) return;
  bool debugInitialized = Debugger::manager->coff();
  stopOperations();
  Compile::Config::setToolGroup(Main::project(), group);
  updateGUI();
  if (debugInitialized) Debugger::manager->init();
  else Debugger::manager->update();
}

void MainWindow::setDevice(const QString &device)
{
  if ( device==i18n(Device::AUTO_DATA.label) ) Compile::Config::setDevice(Main::project(), Device::AUTO_DATA.name);
  else Compile::Config::setDevice(Main::project(), device);
  updateGUI();
}

void MainWindow::showDeviceInfo()
{
  DeviceChooser::Dialog d(Main::device(), (Main::project() ? DeviceChooser::Choose : DeviceChooser::ChooseWithAuto), this);
  if ( d.exec() ) {
    setDevice(d.device());
    updateGUI();
  }
}

void MainWindow::configGenerator()
{
  PURL::FileType ftype = (Main::currentEditor() ? Main::currentEditor()->fileType() : PURL::Nb_FileTypes);
  PURL::SourceFamily family = (ftype!=PURL::Nb_FileTypes ? PURL::DATA[ftype].sourceFamily : PURL::SourceFamily(PURL::SourceFamily::Nb_Types));
  PURL::ToolType type = (family!=PURL::SourceFamily::Nb_Types ? family.data()->toolType : PURL::ToolType(PURL::ToolType::Assembler));
  ConfigGenerator dialog(this);
  dialog.set(Main::deviceData(), Main::toolGroup(), type);
  dialog.exec();
}

void MainWindow::templateGenerator()
{
  PURL::FileType ftype = (Main::currentEditor() ? Main::currentEditor()->fileType() : PURL::Nb_FileTypes);
  PURL::SourceFamily family = (ftype!=PURL::Nb_FileTypes ? PURL::DATA[ftype].sourceFamily : PURL::SourceFamily(PURL::SourceFamily::Nb_Types));
  PURL::ToolType type = (family!=PURL::SourceFamily::Nb_Types ? family.data()->toolType : PURL::ToolType(PURL::ToolType::Assembler));
  TemplateGenerator dialog(this);
  dialog.set(Main::deviceData(), Main::toolGroup(), type);
  dialog.exec();
}

bool MainWindow::queryClose()
{
  stopOperations();
  Main::setState(Main::Closing);
  saveSettings();
  if ( !Main::_editorManager->closeAllEditors() ) {
    Main::setState(Main::Idle);
    return false;
  }
  return true;
}

void MainWindow::newSourceFile()
{
  NewFileDialog dialog(Main::project(), this);
  if ( dialog.exec()!=QDialog::Accepted ) return;
  if ( !dialog.url().exists() && !dialog.url().create(*Main::_compileLog) ) return;
  Main::_editorManager->openEditor(dialog.url());
  if ( dialog.addToProject() ) Main::_projectManager->insertFile(dialog.url());
}

void MainWindow::newHexFile()
{
  if ( Main::device()==Device::AUTO_DATA.name ) {
    MessageBox::sorry(i18n("You need to specify a device to create a new hex file."), Log::Show);
    return;
  }
  QString s;
  for (uint i=0; true; i++) {
    s = i18n("Hex") + (i==0 ? QString::null : QString::number(i));
    if ( Main::_editorManager->findEditor(s)==0 ) break;
  }
  HexEditor *editor = new HexEditor(s, Main::_editorManager);
  editor->memoryRead();
  Main::_editorManager->addEditor(editor);
}

bool MainWindow::openFile()
{
  QString filter;
  filter += PURL::sourceFilter(PURL::SimpleFilter);
  filter += "\n" + PURL::filter(PURL::Hex);
  filter += "\n" + PURL::projectFilter(PURL::SimpleFilter);
  filter += "\n*|" + i18n("All Files");
  PURL::Url url = PURL::getOpenUrl(":open_file", filter, this ,i18n("Open File"));
  if ( url.fileType()==PURL::Project || url.fileType()==PURL::PikdevProject ) {
    stopOperations();
    if ( !Main::_projectManager->openProject(url) ) return false;
    updateGUI();
    return true;
  }
  return Main::_editorManager->openFile(url);
}

void MainWindow::updateGUI()
{
  bool idle = ( Main::_state==Main::Idle );
  switch (Main::_state) {
  case Main::Closing: return;
  case Main::Idle:
    BusyCursor::stop();
    _actionStatus->hide();
    _actionProgress->hide();
    break;
  case Main::Compiling:
    BusyCursor::start();
    _actionStatus->show();
    _actionProgress->show();
    _actionStatus->setText(Main::_compileManager->label());
    makeWidgetDockVisible(Main::_compileLog);
    break;
  case Main::Programming:
    BusyCursor::start();
    _actionStatus->show();
    _actionProgress->show();
    makeWidgetDockVisible(_programLog);
    break;
  }

  // update editor actions
  Main::_editorManager->updateTitles();
  Main::action("file_save_all")->setEnabled(Main::currentEditor());
  Main::action("file_close")->setEnabled(Main::currentEditor());
  Main::action("file_close_all")->setEnabled(Main::currentEditor());
  Main::action("file_closeother")->setEnabled(Main::_editorManager->nbEditors()>1);
  Main::action("options_configure")->setEnabled(idle);
  PURL::FileType currentType = (Main::currentEditor() ? Main::currentEditor()->fileType() : PURL::Nb_FileTypes);
  bool isSource = ( PURL::DATA[currentType].group==PURL::Source );
  bool isHeader = ( PURL::DATA[currentType].group==PURL::Header );
  Main::action("view_switch_source")->setEnabled(isSource || isHeader);
  Main::action("history_back")->setEnabled(Main::editorManager().history().hasBack());
  Main::action("history_forward")->setEnabled(Main::editorManager().history().hasForward());

  // update project
  bool inProject = ( Main::currentEditor() && (currentType==PURL::Nb_FileTypes || Main::currentEditor()->url().isEmpty() || Main::_projectManager->contains(Main::currentEditor()->url())) );
  if ( Main::project()==0 && !inProject ) {
    if ( Main::currentEditor()==0 ) Main::_projectManager->closeProject();
    else if ( isSource ) Main::_projectManager->setStandalone(Main::currentEditor()->url(), currentType);
    else {
      PURL::FileType type;
      PURL::Url purl = Main::_projectManager->standaloneGenerator(Main::currentEditor()->url(), type);
      if ( type!=PURL::Nb_FileTypes ) Main::_projectManager->setStandalone(purl, type);
      else if ( currentType==PURL::Hex ) Main::_projectManager->setStandalone(purl, PURL::Hex);
    }
  }
  if ( Main::currentEditor() ) Main::_projectManager->select(Main::currentEditor());

  // update project actions
  Main::action("project_new")->setEnabled(idle);
  Main::action("project_open")->setEnabled(idle);
  Main::action("project_close")->setEnabled(Main::project() && idle);
  Main::action("project_options")->setEnabled(Main::project() && idle);
  Main::action("project_add_source_file")->setEnabled(Main::project() && idle);
  Main::action("project_add_object_file")->setEnabled(Main::project() && idle);
  Main::action("project_add_current_file")->setEnabled(Main::project() && !inProject && idle && isSource);

  // update build actions
  _toolStatus->setText(" " + Main::toolGroup().label() + " ");
  bool hexProject = ( Main::_projectManager->projectUrl().fileType()==PURL::Hex );
  Main::action("build_build_project")->setEnabled((Main::project() || (inProject && !hexProject) ) && idle);
  Main::action("build_compile_file")->setEnabled(!hexProject && isSource && inProject && idle);
  Main::action("build_clean")->setEnabled((Main::project() || inProject) && idle);
  Main::action("build_stop")->setEnabled(Main::_state==Main::Compiling);

  // update programmer status
  _programmerStatus->setText(" " + Main::programmerGroup().statusLabel() + " ");
  QFont f = font();
  bool supported = (Main::deviceData() ? Main::programmerGroup().isSupported(Main::deviceData()->name()) : false);
  f.setItalic(!supported);
  _programmerStatus->setFont(f);
  bool isProgrammer = ( Main::programmerGroup().properties() & ::Programmer::Programmer );
  PURL::Url purl = Main::_projectManager->projectUrl();
  bool hasHex = ( currentType==PURL::Hex || Main::_projectManager->contains(purl.toFileType(PURL::Hex)) );
  Main::action("prog_connect")->setEnabled(idle);
  Main::action("prog_read")->setEnabled(isProgrammer && idle);
  Main::action("prog_program")->setEnabled(isProgrammer && hasHex && idle);
  Main::action("prog_verify")->setEnabled(isProgrammer && hasHex && idle);
  Main::action("prog_erase")->setEnabled(isProgrammer && idle);
  Main::action("prog_blank_check")->setEnabled(isProgrammer && idle);
  Programmer::State pstate = (Main::programmer() ? Main::programmer()->state() : Programmer::NotConnected);
  static_cast<KToggleAction *>(Main::action("prog_power"))->setEnabled(isProgrammer && idle && pstate!=Programmer::NotConnected);
  Main::action("prog_disconnect")->setEnabled(idle && pstate!=Programmer::NotConnected);
  bool isDebugger = ( Main::programmerGroup().properties() & ::Programmer::Debugger );
  bool resetAvailable = ( Main::programmerGroup().properties() & Programmer::CanReleaseReset );
  Main::action("prog_run")->setEnabled(idle && resetAvailable && !isDebugger && pstate!=Programmer::Running);
  Main::action("prog_stop")->setEnabled(idle && !isDebugger && pstate==Programmer::Running);
  Main::action("prog_restart")->setEnabled(idle && !isDebugger && pstate==Programmer::Running);
  const Programmer::GroupUI *pgui = static_cast<const Programmer::GroupUI *>(Main::programmerGroup().gui());
  Main::action("prog_advanced")->setEnabled(idle && pgui->hasAdvancedDialog());

  // update debugger status
  Debugger::manager->updateDevice();
  Main::action("debug_run")->setEnabled(idle && isDebugger && pstate!=Programmer::Running);
  Main::action("debug_halt")->setEnabled(idle && isDebugger && pstate==Programmer::Running);
  Main::action("debug_next")->setEnabled(idle && isDebugger && pstate!=Programmer::Running);
  Main::action("debug_reset")->setEnabled(idle && isDebugger && (pstate==Programmer::Running || pstate==Programmer::Halted));
  Main::action("debug_show_pc")->setEnabled(idle && isDebugger && Debugger::manager->coff()!=0 && Debugger::manager->pc().isInitialized() );

  Main::_projectManager->updateGUI();

  // caption
  QString caption;
  if ( Main::project() ) caption += Main::project()->name();
  if ( Main::currentEditor() ) {
    if ( Main::project() ) caption += " - ";
    caption += Main::currentEditor()->url().filepath();
  }
  setCaption(KApplication::kApplication()->makeStdCaption(caption));

  emit stateChanged();
}

void MainWindow::updateToolViewsActions()
{
  QValueList<DockData>::iterator it;
  for(it=_docks.begin(); it!=_docks.end(); ++it) (*it).action->setChecked((*it).dock->mayBeHide());
}

void MainWindow::initDockPosition(const DockData &ddata)
{
  const DockPosition &pos = ddata.position;
  ddata.dock->manualDock(manager()->getDockWidgetFromName(pos.parent), pos.pos, pos.space);
}

void MainWindow::resetDockLayout()
{
  QValueList<DockData>::iterator it;
  for (it=_docks.begin(); it!=_docks.end(); ++it) initDockPosition(*it);
}

void MainWindow::toggleToolView(QWidget *widget)
{
  static_cast<KDockWidget *>(widget)->changeHideShowState();
}

void MainWindow::runKfind()
{
  if (_kfindProcess) return;
  _kfindProcess = new ::Process::StringOutput;
  QString path;
  PURL::Url url = Main::projectManager().projectUrl();
  if ( !url.isEmpty() ) path = url.path();
  _kfindProcess->setup("kfind", path, false);
  connect(_kfindProcess, SIGNAL(done(int)), SLOT(kfindDone()));
  if ( !_kfindProcess->start(0) )
    MessageBox::sorry(i18n("Could not run \"kfind\""), Log::Show);
}

void MainWindow::kfindDone()
{
  delete _kfindProcess;
  _kfindProcess = 0;
}

void MainWindow::runPikloops()
{
  if (_pikloopsProcess) return;
  _pikloopsProcess = new ::Process::StringOutput;
  _pikloopsProcess->setup("pikloops", QStringList(), false);
  connect(_pikloopsProcess, SIGNAL(done(int)), SLOT(pikloopsDone()));
  if ( !_pikloopsProcess->start(0) )
    MessageBox::detailedSorry(i18n("Could not run \"pikloops\""), i18n("The Pikloops utility (%1) is not installed in your system.").arg("http://pikloops.sourceforge.net"), Log::Show);
}

void MainWindow::pikloopsDone()
{
  delete _pikloopsProcess;
  _pikloopsProcess = 0;
}

//-----------------------------------------------------------------------------
void MainWindow::compileFile()
{
  Editor *e = Main::currentEditor();
  if ( e && e->isModified() ) e->save(); // buffer is systematically saved
  stopOperations();
  Main::_compileLog->clear();
  bool generated = (e ? Main::_projectManager->isExternalFile(e->url()) : false);
  // #### FIXME: we should only remove the generated files that are "below" this one...
  if ( Main::project()==0 || !generated ) Main::_projectManager->removeExternalFiles();
  PURL::Url url = (e && !generated ? e->url() : Main::_projectManager->projectUrl());
  Main::_compileManager->compileFile(Compile::TodoItem(url, generated));
  Main::setState(Main::Compiling);
}

void MainWindow::buildProject()
{
  if ( Main::project()==0 ) {
    compileFile();
    return;
  }
  // save modified buffers
  PURL::UrlList files = Main::project()->absoluteFiles();
  PURL::UrlList::const_iterator it;
  for (it=files.begin(); it!=files.end(); ++it) {
    // save modified editors
    Editor *e = Main::_editorManager->findEditor(*it);
    if ( e && e->isModified() ) e->save();
  }
  bool tmp = _forceProgramAfterBuild;
  stopOperations();
  _forceProgramAfterBuild = tmp;
  Main::_compileLog->clear();
  Main::_projectManager->removeExternalFiles();
  Compile::LinkType ltype = (Main::programmerGroup().name()=="icd2_debugger" ? Compile::Icd2Linking : Compile::NormalLinking);
  Main::_compileManager->buildProject(ltype);
  Main::setState(Main::Compiling);
}

void MainWindow::compileFailure()
{
  _forceProgramAfterBuild = false;
  Main::setState(Main::Idle);
}

void MainWindow::compileSuccess()
{
  if ( !Main::_compileManager->compileOnly() ) {
    Main::_projectManager->setModified(false);
    if ( Debugger::manager->init() ) {
      const QStringList &included = Debugger::manager->coff()->filenames();
      QStringList::const_iterator it;
      for (it=included.begin(); it!=included.end(); ++it) {
        PURL::Directory dir = (Main::project() ? Main::project()->directory() : Main::projectManager().projectUrl().directory());
        PURL::Url url = PURL::Url::fromPathOrUrl(*it).toAbsolute(dir);
        if ( !url.exists() ) continue;
        Main::_projectManager->addExternalFile(url, ProjectManager::Included);
      }
    }
    GlobalConfig gc;
    if ( _forceProgramAfterBuild || gc.programAfterBuild() ) program();
  }
  _forceProgramAfterBuild = false;
  Main::setState(Main::Idle);
}

void MainWindow::updateFile(const Compile::FileData &fdata)
{
  if ( fdata.actions & Compile::InProject ) {
    if ( fdata.actions & Compile::Generated ) Main::_projectManager->addExternalFile(fdata.url, ProjectManager::Generated);
    else if ( fdata.actions & Compile::Included ) Main::_projectManager->addExternalFile(fdata.url, ProjectManager::Included);
    else Q_ASSERT(false);
  }
  if ( fdata.actions & Compile::Show ) {
    Editor *e = Main::_editorManager->openEditor(fdata.url);
    if (e) e->setReadOnly(true);
  }
}

void MainWindow::cleanBuild()
{
  stopOperations();
  Main::_compileLog->clear();
  if ( Main::project() ) {
    Compile::LinkType ltype = (Main::programmerGroup().name()=="icd2_debugger" ? Compile::Icd2Linking : Compile::NormalLinking);
    Main::_compileManager->cleanProject(ltype);
  } else {
    PURL::FileType type;
    PURL::Url url = Main::_projectManager->standaloneGenerator(Main::currentEditor()->url(), type);
    Main::_compileManager->cleanFile(url);
  }
  Main::_projectManager->removeExternalFiles();
}

void MainWindow::stopBuild()
{
  Main::_compileManager->kill();
}

//-----------------------------------------------------------------------------
void MainWindow::keyPressEvent(QKeyEvent *)
{
  // #### FIXME: have a stop button somewhere instead...
  // if ( e->key()==Key_Escape ) stopOperations();
}

void MainWindow::stopOperations()
{
  if ( Main::_state==Main::Programming ) {
    MessageBox::sorry(i18n("Programming in progress. Cannot be aborted."), Log::Show);
    return;
  }
  stopBuild();
  Programmer::manager->stop();
  Debugger::manager->clear();
}

void MainWindow::newProject()
{
  stopOperations();
  Main::_projectManager->newProject();
  updateGUI();
  Main::_compileLog->clear();
}

void MainWindow::openProject()
{
  stopOperations();
  Main::_projectManager->openProject();
  updateGUI();
  Main::_compileLog->clear();
}

void MainWindow::openRecentProject(const KURL &url)
{
  stopOperations();
  Main::_projectManager->openProject(PURL::Url(url));
  updateGUI();
  Main::_compileLog->clear();
}

void MainWindow::configureProject()
{
  stopOperations();
  if ( Main::project()==0 ) configure(ConfigCenter::Standalone);
  else Main::_projectManager->editProject();
  updateGUI();
}

void MainWindow::closeProject()
{
  stopOperations();
  Main::_projectManager->closeProject();
  updateGUI();
  Main::_compileLog->clear();
}

//----------------------------------------------------------------------------
HexEditor *MainWindow::getHexEditor()
{
  if ( Main::_projectManager->isModified() || Main::_projectManager->needsRecompile() ) {
    MessageBox::Result res = MessageBox::Yes;
    GlobalConfig gc;
    if ( !gc.autoRebuildModified() ) {
      res = MessageBox::questionYesNoCancel(i18n("The project hex file may not be up-to-date since some project files have been modified."),
                                            i18n("Recompile First"), i18n("Continue Anyway"));
      if ( res==MessageBox::Cancel ) return 0;
    }
    if ( res==MessageBox::Yes ) {
      _forceProgramAfterBuild = true;
      buildProject();
      return 0;
    }
  }
  if ( Main::currentEditor() && Main::currentEditor()->fileType()==PURL::Hex )
    return static_cast<HexEditor *>(Main::currentEditor());
  PURL::Url purl = Main::_projectManager->projectUrl();
  HexEditor *editor = static_cast<HexEditor *>(Main::_editorManager->openEditor(purl.toFileType(PURL::Hex)));
  if ( editor==0 ) return 0;
  editor->setReadOnly(true);
  return editor;
}

void MainWindow::erase()
{
  Programmer::manager->erase(Device::MemoryRange());
}

void MainWindow::blankCheck()
{
  Programmer::manager->blankCheck(Device::MemoryRange());
}

void MainWindow::program()
{
  HexEditor *editor = getHexEditor();
  if ( editor==0 ) return;
  if ( Main::programmerGroup().isDebugger() && !Main::_projectManager->contains(editor->url()) ) {
    MessageBox::sorry(i18n("<qt>It is not possible to start a debugging session with an hex file not generated with the current project.</qt>"), Log::Show);
    return;
  }
  Programmer::manager->program(*editor->memory(), Device::MemoryRange());
}

void MainWindow::verify()
{
  HexEditor *editor = getHexEditor();
  if ( editor==0 ) return;
  Programmer::manager->verify(*editor->memory(), Device::MemoryRange());
}

void MainWindow::read()
{
  QString s = i18n("Read");
  Editor *e = Main::_editorManager->findEditor(s);
  if (e) Main::_editorManager->closeEditor(e, true);
  HexEditor *editor = new HexEditor(s, Main::_editorManager);
  editor->setDevice();
  if ( Programmer::manager->read(*editor->memory(), Device::MemoryRange()) ) {
    editor->memoryRead();
    Main::_editorManager->addEditor(editor);
  } else delete editor;
}

void MainWindow::setTotalSteps(uint total)
{
  //KIO::Job *job = new KIO::SimpleJob(KURL(), 0, 0, false);
  //int id = Observer::self()->newJob(job, true);
  //Observer::self()->slotTotalSize(job, total);
  //Observer::self()->slotInfoMessage(job, "test");
  //qDebug("set total steps: %i", total);
  _actionProgress->setTotalSteps(total);
  _actionProgress->setProgress(0);
  QApplication::eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
}

void MainWindow::addToTotalSteps(uint add)
{
  _actionProgress->setTotalSteps(_actionProgress->totalSteps() + add);
  //qDebug("add to total steps: %i %i", add, _actionProgress->totalSteps());
}

void MainWindow::addProgress(uint add)
{
  //qDebug("add progress %i %i/%i", add, _actionProgress->progress() + add, _actionProgress->totalSteps());
  _actionProgress->setProgress(_actionProgress->progress() + add);
  QApplication::eventLoop()->processEvents(QEventLoop::ExcludeUserInput); // #### DANGER !!!!
}
