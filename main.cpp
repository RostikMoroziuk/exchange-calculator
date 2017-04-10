#include "mainwindow.h"
#include <QObject>
#include <QTranslator>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //зчитуємо потрібні налаштування програми і у відповідності до виставленої мови записуємо потрібний файл
    QSettings* setting = new QSettings("Rostik", "Exchange Expert");
    QTranslator trans;
    if(setting->value("/Settings/languageUkrainian", true).toBool())
    {
        trans.load(QString("fileName_ua.qm"));
        app.installTranslator(&trans);
    }
    else
    {
        trans.load(QString("fileName_en.qm"));
        app.installTranslator(&trans);
    }
    MainWindow w(setting);
    app.setQuitOnLastWindowClosed(false);//при нажатті на хрестик, програма не завершується, а скривається
    w.show();

    return app.exec();
}
