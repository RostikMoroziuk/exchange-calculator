#ifndef PARSWORKER_H
#define PARSWORKER_H

#include "parser.h"

class ParsWorker:public QObject
{
    Q_OBJECT
public:
    ParsWorker(QStandardItemModel* , int*);
    QStandardItemModel *tempModel;
    int *tempCount;
private:
    Parser* parser;

private slots:
    void process();

signals:
    void finish();
    void error();
};

#endif // PARSWORKER_H
