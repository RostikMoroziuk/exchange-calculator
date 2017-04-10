#include "parsworker.h"

ParsWorker::ParsWorker(QStandardItemModel* sm, int* cnt)
{
    tempModel = sm;
    tempCount = cnt;
    parser = NULL;
}

void ParsWorker::process()
{
    QFile file(QCoreApplication::applicationDirPath()+"/exchange.xml");
    if(file.open(QFile::ReadOnly))
    {
        parser = new Parser(tempModel, tempCount);
        parser->setData(file);

        file.close();
        emit finish();
    }
    else//якшо файл не знайдено - автоматично намагаємося скачати файл
    {
        emit error();
    }
}

