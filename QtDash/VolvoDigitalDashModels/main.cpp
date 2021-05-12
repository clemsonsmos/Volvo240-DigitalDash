#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <QFont>
#include <QFile>
#include <QTextStream>
#include <QFontDatabase>

#include <tachometer_model.h>
#include <accessory_gauge_model.h>
#include <speedometer_model.h>
#include <temp_and_fuel_gauge_model.h>

#define RASPBERRY_PI

static TachometerModel tachModel;
static SpeedometerModel speedoModel;
static TempAndFuelGaugeModel tempFuelModel;
static AccessoryGaugeModel oilPressureModel;
static AccessoryGaugeModel oilTemperatureModel;
static AccessoryGaugeModel ambientTemperatureModel;
static AccessoryGaugeModel boostModel;
static AccessoryGaugeModel voltMeterModel;



void initializeModels()
{
    /** Init tach **/
    tachModel.setMaxRpm(7000);
    tachModel.setRedLine(6000);
    tachModel.setRpm(0);

    /** Init Speedo **/
    speedoModel.setMaxValue(120);
    speedoModel.setMinValue(0);
    speedoModel.setUnits("mph");
    speedoModel.setCurrentValue(0);
    speedoModel.setTopValue(0);
    speedoModel.setTopUnits("°F");

    /** Init temp/fuel gauge **/
    tempFuelModel.setMinTemp(120);
    tempFuelModel.setMaxTemp(250);
    tempFuelModel.setCurrentTemp(0);
    tempFuelModel.setTempUnits("°F");
    tempFuelModel.setFuelLevel(0);
    tempFuelModel.setHighTempAlarm(220.0);
    tempFuelModel.setLowFuelAlarm(10);

    /** Init accessory gauges **/
    oilPressureModel.setMinValue(0.0);
    oilPressureModel.setMaxValue(5.0);
    oilPressureModel.setUnits("bar");
    oilPressureModel.setCurrentValue(0.0);
    oilPressureModel.setHighAlarm(4.5);
    oilPressureModel.setLowAlarm(1.0);

    oilTemperatureModel.setMinValue(120.0);
    oilTemperatureModel.setMaxValue(300.0);
    oilTemperatureModel.setUnits("°F");
    oilTemperatureModel.setCurrentValue(0.0);
    oilTemperatureModel.setHighAlarm(260.0);

    boostModel.setMinValue(-20.0);
    boostModel.setMaxValue(30.0);
    boostModel.setUnits("psi");
    boostModel.setCurrentValue(0.0);
    boostModel.setHighAlarm(18.0);

    voltMeterModel.setMinValue(10.0);
    voltMeterModel.setMaxValue(16.0);
    voltMeterModel.setUnits("V");
    voltMeterModel.setCurrentValue(0.0);
    voltMeterModel.setLowAlarm(11.0);
    voltMeterModel.setHighAlarm(15.0);
}

void updateGaugesRPi()
{
    static double speed = 0.0;
    static double oilPressure = 0.0;
    static double boost = 0.0;
    static double oilTemperature = 115.0;
    static double volts = 13.0;
    static double fuel = 100.0;
    static double rpm = 0;

    QString tempPath = "/sys/class/thermal/thermal_zone0/temp";
    QString adcPath = "/sys/class/i2c-adapter/i2c-1/1-0048/iio:device0";
    QFile tempFile(tempPath);
    QTextStream tempStream(&tempFile);
    tempFile.open(QIODevice::ReadOnly);

    if(tempFile.isOpen())
    {
        QString coreTemp = tempStream.readLine();
        float temp = coreTemp.toFloat();
        oilTemperatureModel.setCurrentValue(((temp/1000.0) * 9.0/5.0)+32.0);
        tempFuelModel.setCurrentTemp(((temp/1000.0) * 9.0/5.0)+32.0);
    }

    voltMeterModel.setCurrentValue(volts);
    volts += 0.01;
    if( volts > 16.0 )
    {
        volts = 10.0;
    }

    boostModel.setCurrentValue((boost*8.94)-14.53);
    boost += 0.005;
    if( boost > 5.0 )
    {
        boost = 0.0;
    }

    oilTemperatureModel.setCurrentValue(oilTemperature);
    oilTemperature += 0.5;
    if( oilTemperature > 300)
    {
        oilTemperature = 115.0;
    }

    oilPressureModel.setCurrentValue(oilPressure);
    oilPressure += 0.005;
    if( oilPressure > 5.0)
    {
        oilPressure = 0.0;
    }

    speedoModel.setCurrentValue(speed);
    speed += 0.25;
    if( speed > 120.0 )
    {
        speed = 0.0;
    }

    tempFuelModel.setFuelLevel(fuel);
    fuel -= 0.05;
    if( fuel < 0.0 )
    {
        fuel = 100.0;
    }

    tachModel.setRpm(rpm);
    rpm += 10;
    if( rpm > 7200 )
    {
        rpm = 0.0;
    }

    tempFile.close();
}

void updateGauges() {
    QString tempPath = "/sys/class/hwmon/hwmon2/temp2_input";
    QString rpmPath = "/sys/class/hwmon/hwmon3/fan1_input";
    QString battPath = "/sys/class/power_supply/BAT0/voltage_now";
    QString fuelLevelPath = "/sys/class/power_supply/BAT0/capacity";
    QString speedPath = "/proc/cpuinfo";

    QFile tempFile(tempPath);
    QFile rpmFile(rpmPath);
    QFile battFile(battPath);
    QFile fuelFile(fuelLevelPath);
    QFile speedFile(speedPath);

    QTextStream tempStream(&tempFile);
    QTextStream rpmStream(&rpmFile);
    QTextStream battStream(&battFile);
    QTextStream fuelStream(&fuelFile);
    QTextStream speedStream(&speedFile);

    tempFile.open(QIODevice::ReadOnly);
    rpmFile.open(QIODevice::ReadOnly);
    battFile.open(QIODevice::ReadOnly);
    fuelFile.open(QIODevice::ReadOnly);
    speedFile.open(QIODevice::ReadOnly);

    if(tempFile.isOpen())
    {
        QString coreTemp = tempStream.readLine();
        float temp = coreTemp.toFloat();
        oilTemperatureModel.setCurrentValue(((temp/1000.0) * 9.0/5.0)+32.0);
        tempFuelModel.setCurrentTemp(((temp/1000.0) * 9.0/5.0)+32.0);
    }

    if(rpmFile.isOpen())
    {
        QString rpmString = rpmStream.readLine();
        int rpm = rpmString.toInt();
        tachModel.setRpm(rpm * 1.5f);
        boostModel.setCurrentValue( ((float)rpm/1000.0)*5.75 );
        oilPressureModel.setCurrentValue( ((float)rpm / 1000.0) );
    }

    if(battFile.isOpen())
    {
        QString voltage = battStream.readLine();
        float volts = voltage.toInt();
        voltMeterModel.setCurrentValue(volts/1.0e6);
    }

    if(fuelFile.isOpen())
    {
        QString fuelLevel = fuelStream.readLine();
        int level = fuelLevel.toInt();
        tempFuelModel.setFuelLevel(level);
    }

    if(speedFile.isOpen())
    {
        QString cpuSpeed = "";
        while(!cpuSpeed.contains("cpu MHz"))
        {
            cpuSpeed = speedStream.readLine();
        }
        cpuSpeed = cpuSpeed.right(cpuSpeed.indexOf(": "));
        qreal speed = cpuSpeed.toFloat();
        speedoModel.setCurrentValue(speed/2800.0 * 120.0);
    }

    tempFile.close();
    rpmFile.close();
    battFile.close();
    fuelFile.close();
    speedFile.close();
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);


#ifndef RASPBERRY_PI
    QFontDatabase::addApplicationFont(":/fonts/aribkl.ttf");
    QFont mFont;
    mFont.setFamily("Arial Black");
    app.setFont(mFont);
#else
    QFontDatabase::addApplicationFont(":/fonts/aribkl.ttf");
    QFont font;
    font.setFamily("Arial Black");
    app.setFont(font);
#endif


    QQmlApplicationEngine engine;

    QQmlContext *ctxt = engine.rootContext();
    ctxt->setContextProperty("rpmModel", &tachModel);
    ctxt->setContextProperty("speedoModel", &speedoModel);
    ctxt->setContextProperty("tempFuelModel", &tempFuelModel);
    ctxt->setContextProperty("oilPModel", &oilPressureModel);
    ctxt->setContextProperty("oilTModel", &oilTemperatureModel);
    ctxt->setContextProperty("outsideTempModel", &ambientTemperatureModel);
    ctxt->setContextProperty("boostModel", &boostModel);
    ctxt->setContextProperty("voltMeterModel", &voltMeterModel);

    initializeModels();

#ifndef RASPBERRY_PI
    QTimer rpmTimer;
    rpmTimer.setInterval(100);
    QObject::connect(&rpmTimer, &QTimer::timeout, &app, &updateGauges);
    rpmTimer.start();
#else
    QTimer rpmTimer;
    rpmTimer.setInterval(100);
    QObject::connect(&rpmTimer, &QTimer::timeout, &app, &updateGaugesRPi);
    rpmTimer.start();
#endif

    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    QObject::connect(&engine, SIGNAL(quit()), &app, SLOT(quit()));

    return app.exec();
}
