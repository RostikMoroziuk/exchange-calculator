#ifndef PARSER
#define PARSER
#include <QXmlStreamReader>
#include <QNetworkReply>
#include <QFile>
#include <QStandardItemModel>
#include <QApplication>

class Parser:public QObject
{
    Q_OBJECT
public:
    Parser(QStandardItemModel* model, int* cnt);
    ~Parser()
    {
        delete sr;
    }
    void setData(QFile& reply);

private:
    QXmlStreamReader* sr;
    QStandardItemModel* vec;//модель даних (банки і курси)
    int* count;//кількість елементів в моделі
};

#endif // PARSER

