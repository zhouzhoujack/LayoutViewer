#include "layMainWindowDecl.h"
#include "detail/utilsString.h"

#include "myRegistrar.h"
#include "layMenuEntry.h"
#include "layAbstractMenu.h"

namespace lay
{

static MainWindowPluginDeclaration* sp_instance = 0;

// -----------------------------------------------------------------------------
//  MainWindowDecl implementation

MainWindowPluginDeclaration::MainWindowPluginDeclaration(int id) : PluginDeclaration(id) 
{ 
    sp_instance = this; 
}

MainWindowPluginDeclaration::~MainWindowPluginDeclaration() 
{ 
    sp_instance = 0; 
}

void MainWindowPluginDeclaration::create_toobar_action(Dispatcher* dispatcher, QToolBar* toolbar, QActionGroup*)
{
    //  "Open File" button — placed at the far left of the toolbar
    Action* ac0 = new GenericMenuAction(dispatcher, "Open", "cm_open");
    ac0->set_icon(":/folder.png");
    ac0->qaction()->setToolTip(QObject::tr("Open Layout File"));
    ac0->qaction()->setObjectName(QString::fromUtf8("open_toolbar_action"));

    Action* ac = new GenericMenuAction(dispatcher, "Undo", "cm_undo");
    ac->set_icon(":/back.png");
    ac->qaction()->setShortcut(QKeySequence::Undo);

    Action* ac1 = new GenericMenuAction(dispatcher, "Redo", "cm_redo");
    ac1->set_icon(":/forward.png");
    ac1->qaction()->setShortcut(QKeySequence::Redo);

    toolbar->addActions({ac0->qaction(), ac->qaction(), ac1->qaction()});
    toolbar->addSeparator();
}

void MainWindowPluginDeclaration::get_menu_entries(std::vector<lay::MenuEntry>& menu_entries) const
{
    menu_entries.push_back(lay::menu("file_menu", "&File"));
    menu_entries.push_back(lay::menu("view_menu", "&View"));
    menu_entries.push_back(lay::menu("zoom_menu", "&Display"));
    menu_entries.push_back(lay::menu("command_menu", "&Command"));

    menu_entries.push_back(lay::menu_item("file_menu.open", "cm_open", geo::to_string(QObject::tr("Open"))));
    menu_entries.push_back(lay::menu_item("file_menu.setup", "cm_setup", geo::to_string(QObject::tr("Setting"))));
    menu_entries.push_back(lay::separator("file_menu"));
    menu_entries.push_back(lay::menu_item("file_menu.open_recent", "Open Recent"));

    menu_entries.push_back(
        lay::menu_item("view_menu.command", "cm_command_visible", geo::to_string(QObject::tr("Command"))));
    menu_entries.push_back(lay::separator("view_menu"));
    menu_entries.push_back(
        lay::menu_item("view_menu.layers", "cm_layers_visible", geo::to_string(QObject::tr("Layers"))));
    menu_entries.push_back(lay::menu_item("view_menu.cells", "cm_cells_visible", geo::to_string(QObject::tr("Cells"))));
    menu_entries.push_back(
        lay::menu_item("view_menu.tree", "cm_tree_visible", geo::to_string(QObject::tr("Resource Tree"))));

    menu_entries.push_back(lay::menu_item("command_menu.save_script", "cm_command_save_script",
                                          geo::to_string(QObject::tr("Save Script..."))));
    menu_entries.push_back(lay::menu_item("command_menu.load_script", "cm_command_load_script",
                                          geo::to_string(QObject::tr("Load Script..."))));
    menu_entries.push_back(lay::separator("command_menu"));
    menu_entries.push_back(lay::menu_item("command_menu.help", "cm_command_help", geo::to_string(QObject::tr("Help"))));
}

} // namespace lay

static my::Registrar<lay::PluginDeclaration> config_decl(new lay::MainWindowPluginDeclaration(-1), -100, "mainwindow");
