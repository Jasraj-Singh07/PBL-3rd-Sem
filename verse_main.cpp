#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "verse.h"
int main(int argc,char**argv){QGuiApplication app(argc,argv);app.setOrganizationName("ChessVerse");app.setApplicationName("ChessVerse");Verse v;QQmlApplicationEngine e;e.rootContext()->setContextProperty("verse",&v);e.load(QUrl("qrc:/Main.qml"));if(e.rootObjects().isEmpty())return 1;return app.exec();}
