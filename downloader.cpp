#include "downloader.h"
#include <QThread>

Downloader::Downloader(QStandardItemModel* model, int *cnt):tempModel(model), tempCount(cnt)
{
    manager = new QNetworkAccessManager();

    connect(manager, &QNetworkAccessManager::finished, this, &Downloader::result);
}

Downloader::~Downloader()
{
    delete manager;
}

void Downloader::getData()
{
    QUrl url("http://resources.finance.ua/ua/public/currency-cash.xml");//формуємо адресу
    QNetworkRequest request(url); //створюємо запит
    manager->get(request);//робимо сам запит
}

void Downloader::result(QNetworkReply* reply)
{
    if(reply->error())//не вдалося підключитися, скачати
    {
        QApplication::beep();
        QMessageBox::warning(this, tr("Помилка"), tr("Не вдалося оновити дані. Провітре з'єднання з Інтернетом."));
        emit loadStatus(tr("Помилка: ") + reply->errorString());
    }
    else
    {
        QFile file(QCoreApplication::applicationDirPath()+"/exchange.xml");//дані скачано, записуємо їх у файл
        if(file.open(QFile::WriteOnly))
        {
            file.write(reply->readAll());
            file.close();
            emit downloaded();
        }
        emit loadStatus(tr("Дані оновлено"));
        LoadData();//записуємо їх
    }
}

void Downloader::LoadData()
{
    ParsWorker *parser = new ParsWorker(tempModel, tempCount);
    QThread* thread = new QThread();
    parser->moveToThread(thread);
    connect(thread, SIGNAL(started()), parser, SLOT(process()));
    connect(parser, SIGNAL(finish()), this, SIGNAL(ready()));
    connect(parser, SIGNAL(finish()), thread, SLOT(quit()));
    connect(parser, SIGNAL(error()), this, SLOT(getData()));
    connect(parser, SIGNAL(finish()), parser, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start(QThread::NormalPriority);//зразу задаємо пріоритет для даного потоку
}
