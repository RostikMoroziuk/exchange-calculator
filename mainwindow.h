#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "downloader.h"
#include "parser.h"
#include <QCloseEvent>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QPainter>
#include "filename.h"
#include <QRegExp>
#include <QStyleFactory>
#include <QSettings> //налаштування програми
#include <QFileDialog>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QDateTime>
#include <QTimer>
#include <QFileSystemWatcher>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QSettings* set, QWidget *parent = 0);
    ~MainWindow();
    bool languageUkrainian();

protected:
    void closeEvent(QCloseEvent*);

private:
    Ui::MainWindow *ui;
    Downloader* load;
    QStandardItemModel *model;
    QList<QList<QStandardItem*> > removeItems;//зберігаються тимчасво видалені банки
    int count;//кількість елементів в моделі
    QString outputFileName;//задає назву файла для зберігання
    QString styleName;//зберігає назву поточного стилю
    QSettings* setting;
    QSystemTrayIcon* systemTray;
    QMenu* contextMenu;
    QAction* showHide;
    QTimer* timer;//автоматично оновлюємо дані кожні 12 годин

    void rowHide(const int& index);//видалити елемент, який не має потрібної валюти
    void draw(QPainter* painter);
    void writeSettings();//викликається при закритті програми. Записує всі збережені налаштування
    void readSettings();// викликається в конструкторі. Завантажує всі налаштування
    void verifyFileName(QString& );
    void verifyStyle();//визаначаємо доступні стилі для платформи
    void initModel();//ініціалізуємо модель
    void localization();//встановлюємо локалізацію для програми
    void setSystemTray();//створюється іконка в правому нижньому куті
    void autoUpdate();

private slots:
    void setRow();
    void calc();
    void updCurrency(int);//змінюємо валюти в які можна сконвертувати
    void index(QModelIndex);
    void savePDF();//зберігаємо дані про курс валют в пдф-форматі
    void savePDFin();
    void print();
    void selectProcedure(int);//шо сортуємо, те автоматично вибирається для обчислення
    void on_actionWindows_triggered();
    void on_actionWindowsXP_triggered();
    void on_actionWindowsVista_triggered();
    void on_actionFusion_triggered();
    void on_actionEnglish_triggered();
    void on_actionUkrainian_triggered();
    void on_about_triggered();
    void on_aboutQt_triggered();
    void on_actionExit_triggered();
    void on_showToolBar_triggered();
    void on_actionMac_triggered();
    void slotShowHide();
    void systemTrayMessage();//повідомляємо що дані були оновлені і оновлюємо дату оновлення
    void cleanRemovedItems();//видалити тимчасово збережені банки

signals:
};

#endif // MAINWINDOW_H
