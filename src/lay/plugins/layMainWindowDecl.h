#pragma once

#include "layPluginDeclaration.h"

namespace lay
{

class MainWindowPluginDeclaration : public lay::PluginDeclaration
{
public:
    MainWindowPluginDeclaration(int id);
    ~MainWindowPluginDeclaration();

    void create_toobar_action(Dispatcher* dispatcher, QToolBar* , QActionGroup* ) override;

    void get_menu_entries(std::vector<lay::MenuEntry>& menu_entries) const override;
};

} // namespace lay
