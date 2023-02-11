#include "daemon.h"
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QProcess>
int launchDaemon() {
  auto executable = QCoreApplication::applicationFilePath();
  QProcess process;
  process.setProgram(executable);
  process.setWorkingDirectory("/");
  process.startDetached();
  return 0;
}

int runCommandline(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  QCoreApplication::setApplicationName("LEDaemon");
  QCoreApplication::setApplicationVersion("1.0");
  QCommandLineParser parser;
  parser.setApplicationDescription("bluetooth LED Control Daemon");
  parser.addHelpOption();
  parser.addVersionOption();
  QCommandLineOption daemonOption(QStringList() << "D"
                                                << "daemon",
                                  "Daemonize");
  parser.addOption(daemonOption);
  parser.process(app);
  if (parser.isSet(daemonOption)) {
    return launchDaemon();
  }
  Daemon daemon;
  daemon.initialize();
  return app.exec();
}

int main(int argc, char **argv) {
  if (argc > 1) {
    return runCommandline(argc, argv);
  }
  QCoreApplication app(argc, argv);
  Daemon daemon;
  daemon.initialize();
  return app.exec();
}