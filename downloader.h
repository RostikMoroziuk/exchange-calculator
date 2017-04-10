#ifndef DOWNLOADER
#define DOWNLOADER
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>
#include <QFile>
#include "parsworker.h"
#include <QStandardItemModel>

class Downloader:public QDialog //діалог, шоб могти виводити повідомлення в меседжбоксах
{
    Q_OBJECT
public:
    Downloader(QStandardItemModel* model, int* cnt);
    ~Downloader();
private:
    QNetworkAccessManager *manager;
    QStandardItemModel *tempModel;
    int *tempCount;

public slots:
    void getData();
    void result(QNetworkReply* reply);
    void LoadData();//дані загружаються з старого файла

signals:
    void loadStatus(const QString& info);
    void ready();
    void downloaded();//дає знати шо дані оновилися і потрібно оновити змінну останнього оновлення
};

#endif // DOWNLOADER

