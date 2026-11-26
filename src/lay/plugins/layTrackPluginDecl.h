#pragma once

#include <QAction>

#include "layAbstractMenu.h"
#include "layPlugin.h"
#include "layPluginDeclaration.h"

namespace lay
{

class Dispatcher;
class MainWindow;

class TrackPluginDeclaration : public lay::PluginDeclaration
{
public:
    TrackPluginDeclaration(int id);

    ~TrackPluginDeclaration();

    static TrackPluginDeclaration* instance();

    lay::Plugin* create_plugin(geo::Manager* manager, lay::Dispatcher*, lay::LayoutView* view,
                               lay::ResourceTree* tree) const override;
};

} // namespace lay
