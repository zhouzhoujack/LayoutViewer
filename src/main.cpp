#include <QApplication>

#include "layAppInfo.h"
#include "layApplication.h"
#include "layMainWindow.h"
#include "laySignalHandler.h"
#include "layVersion.h"

int main(int argc, char* argv[])
{
    int result = 0;

    lay::Version::set_name(prg_name);

    std::unique_ptr<lay::ApplicationBase> app;
    app.reset(new lay::GuiApplication(argc, argv));
    lay::enable_signal_handler_gui(false);

    app->parse_cmd(argc, argv);
    app->init_app();

    result = app->run();

    return result;
}
