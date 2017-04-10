#include "parser.h"

Parser::Parser(QStandardItemModel* model, int* cnt)
{
    sr = new QXmlStreamReader();//xml document parser
    vec = model;
    count = cnt;
}

void read(QXmlStreamReader* sr, QStandardItemModel* model, int* count)
{
    QXmlStreamAttributes attribute = sr->attributes();
    for(int i=0;i<16;i++)
    {
        sr->readNext();//пропускаю неважливу інформацію
    }
    //на початку с
    while(sr->name() !="currencies")//знайде кінець currencies
    {
        attribute = sr->attributes();
        if(attribute.value("id").toString() == "EUR")
        {
            model->setItem(*count, 1, new QStandardItem(attribute.value("ar").toString()));
            model->setItem(*count, 2, new QStandardItem(attribute.value("br").toString()));
        }
        if(attribute.value("id").toString() == "USD")
        {
            model->setItem(*count, 3, new QStandardItem(attribute.value("ar").toString()));
            model->setItem(*count, 4, new QStandardItem(attribute.value("br").toString()));
        }
        if(attribute.value("id").toString() == "RUB")
        {
            model->setItem(*count, 5, new QStandardItem(attribute.value("ar").toString()));
            model->setItem(*count, 6, new QStandardItem(attribute.value("br").toString()));
        }
        sr->readNext();//вичитуємо всі валюти
    }
}

void Parser::setData(QFile& file)
{
    *count = 0;
    vec->clear();

    sr->setDevice(&file);//встановлюємо парсинг на файл
    sr->readNext();//зчитуємо перший елемент у файлі (загальна xml інформація)

    //записуємо дату оновлення
    sr->readNext();//source
    QXmlStreamAttributes attribute;

    sr->readNext();//organizations

    //парсимо кожен банк
    int key;
    sr->readNext();//organization
    while(!sr->atEnd() && !sr->hasError())
    {
        attribute = sr->attributes();
        key = attribute.value("oldid").toInt();
        switch(key)
        {
        case 282:
        {
            vec->setItem(*count, 0, new QStandardItem(QIcon(":/image/header-logo3.png"), "Ідея Банк"));
            read(sr, vec, count);
            (*count)++;
            break;
        }
        case 115:
        {
            vec->setItem(*count, 0, new QStandardItem(QIcon(":/image/f8275003f69d79861b10e7d4ec1560bc.png"), "Альфа-Банк"));
            read(sr, vec, count);
            (*count)++;
            break;
        }
        case 111:
        {
            vec->setItem(*count, 0, new QStandardItem(QIcon(":/image/logo-credit-agricole-2.png"), "Креді Агріколь Банк"));
            read(sr, vec, count);
            (*count)++;
            break;
        }
        case 71:
        {
            vec->setItem(*count, 0, new QStandardItem(QIcon(":/image/nd2_logo3.png"), "Кредобанк"));
            read(sr, vec, count);
            (*count)++;
            break;
        }
        case 308:
        {
            vec->setItem(*count, 0, new QStandardItem(QIcon(":/image/Bank Lviv logo.PNG"),  "Львів Банк"));
            read(sr, vec, count);
            (*count)++;
            break;
        }
        case 199:
        {
            vec->setItem(*count, 0, new QStandardItem(QIcon(":/image/smallLogo_709.png"), "Ощадбанк"));
            read(sr, vec, count);
            (*count)++;
            break;
        }
        case 1080:
        {
            vec->setItem(*count, 0, new QStandardItem(QIcon(":/image/partner-logo-platinum-bank-300x124.png"), "ПЛАТИНУМ БАНК (Platinum Bank TM)"));
            read(sr, vec, count);
            (*count)++;
            break;
        }
        case 274:
        {
            vec->setItem(*count, 0, new QStandardItem(QIcon(":/image/pumb_icon.png"), "ПУМБ"));
            read(sr, vec, count);
            (*count)++;
            break;
        }
        case 131:
        {
            vec->setItem(*count, 0, new QStandardItem(QIcon(":/image/privatbank (1).png"), "ПриватБанк"));
            read(sr, vec, count);
            (*count)++;
            break;
        }
        case 103:
        {
            vec->setItem(*count, 0, new QStandardItem(QIcon(":/image/Platon_rajffazen_bank.png"), "Райффайзен Банк Аваль"));
            read(sr, vec, count);
            (*count)++;
            break;
        }
        case 286:
        {
            vec->setItem(*count, 0, new QStandardItem(QIcon(":/image/ukrsub@3x.png"), "УкрСиббанк"));
            read(sr, vec, count);
            (*count)++;
            break;
        }
        case 244:
        {
            vec->setItem(*count, 0, new QStandardItem(QIcon(":/image/ukreksim-1-big.png"), "Укрексiмбанк"));
            read(sr, vec, count);
            (*count)++;
            break;
        }
        case 192:
        {
            vec->setItem(*count, 0, new QStandardItem(QIcon(":/image/UniCredit-Bank-Logo.png"), "Укрсоцбанк UniCredit Bank TM"));
            read(sr, vec, count);
            (*count)++;
            break;
        }
        default:
            sr->skipCurrentElement();
        }
        while(!sr->atEnd() && !(sr->name()=="organization" && !sr->isEndElement()))
        {
            sr->readNext();
        }

    }
}
