#include "layTrackPluginDecl.h"

#include "myRegistrar.h"
#include "layAbstractMenu.h"
#include "layConfigurationDialog.h"
#include "layConverters.h"
#include "layDispatcher.h"
#include "layMainWindow.h"
#include "layMouseTracker.h"
#include "layPlugin.h"
#include "laySelectConfig.h"
#include "laySelectPlugin.h"
#include "laySelectionConfigPage.h"

namespace lay
{

static TrackPluginDeclaration* inst = 0;

// -----------------------------------------------------------------------------
//  TrackPluginDecl implementation

TrackPluginDeclaration::TrackPluginDeclaration(int id) : PluginDeclaration(id) { inst = this; }

TrackPluginDeclaration::~TrackPluginDeclaration() { inst = 0; }

TrackPluginDeclaration* TrackPluginDeclaration::instance() { return inst; }

lay::Plugin* TrackPluginDeclaration::create_plugin(geo::Manager*, lay::Dispatcher*, lay::LayoutView* view,
                                                   lay::ResourceTree*) const
{
    return new MouseTracker(view);
}

} // namespace lay

static my::Registrar<lay::PluginDeclaration> track_plugin(new lay::TrackPluginDeclaration(-1), 0, "track");
