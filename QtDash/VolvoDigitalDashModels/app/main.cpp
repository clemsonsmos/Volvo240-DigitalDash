#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFont>
#include <QFontDatabase>
#include <QList>
#include <QSurface>
#include <QtSerialBus>

#include <config.h>

#ifdef RASPBERRY_PI
#include <dash.h>
#include <dash_new.h>
#else
#include <dash_host.h>
#endif

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QFontDatabase::addApplicationFont(":/fonts/HandelGothReg.ttf");
    QFont mFont;
    mFont.setFamily("Handel Gothic");
    app.setFont(mFont);

    //Setup QML
    QQmlApplicationEngine engine;
    QQmlContext * ctxt = engine.rootContext();

    // Initialize Dash
#ifdef RASPBERRY_PI
    //Dash * dash = new Dash(&app, ctxt); //old style dash
    DashNew * dash = new DashNew(&app, ctxt); // new scheme with sensor source -> sensor -> gauge -> model
    ctxt->setContextProperty("RASPBERRY_PI", QVariant(true));
#else
    DashHost * dash = new DashHost(&app, ctxt);
    ctxt->setContextProperty("RASPBERRY_PI", QVariant(false));
#endif
    dash->init();

    if (QCanBus::instance()->plugins().contains(QStringLiteral("socketcan"))) {
        qDebug() << "found socketcan";
        QString errorString;

        const QList<QCanBusDeviceInfo> devices =
                QCanBus::instance()->availableDevices(QStringLiteral("socketcan"), &errorString);
        if (!errorString.isEmpty()) qDebug() << errorString;
        foreach (QCanBusDeviceInfo info, devices) {
            qDebug() << info.name();
        }

        QCanBusDevice *device = QCanBus::instance()->createDevice(
            QStringLiteral("socketcan"), QStringLiteral("can0"), &errorString);
        if (!device) {
            qDebug() << "Error String: " << errorString;
        } else {
            qDebug() << "Attempting connection";
            device->connectDevice();

            QObject::connect(device,  &QCanBusDevice::stateChanged,
                    [=](QCanBusDevice::CanBusDeviceState state) {
                qDebug() << "CAN State changed: " << state;
            }
            );
        }

    }



//    // load main.qml
//    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
//    if (engine.rootObjects().isEmpty())
//        return -1;

//    // connect quit
//    QObject::connect(&engine, SIGNAL(quit()), &app, SLOT(quit()));

//    // Start Dash
//    dash->start();

    return app.exec();
}
