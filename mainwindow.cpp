#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "downloader.h"

#define hours12 (12*60*60)//вказано в секундах

MainWindow::MainWindow(QSettings* set, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), setting(set)
{
    ui->setupUi(this);

    verifyStyle();    //верифікація допустимих стилів
    systemTray = new QSystemTrayIcon(QIcon(":/image/company_logo.png"));
    setSystemTray();
    setToolTipDuration(3000);//встановлюємо час підказки для всієї програми
    count = 0;
    ui->statusBar->showMessage(tr("Готово до роботи"));

    initModel();
    load = new Downloader(model, &count);

    //можливо вибрати тільки однин з чекбоксів
    QButtonGroup* bg = new QButtonGroup(this);
    bg->addButton(ui->eur);
    bg->addButton(ui->usd);
    bg->addButton(ui->rub);

    QButtonGroup* bg2 = new QButtonGroup(this);
    bg2->addButton(ui->buy);
    bg2->addButton(ui->sell);

    ui->tableView->setModel(model);
    ui->bankList->setModel(model);

    connect(ui->update, &QPushButton::clicked, load, &Downloader::getData);
    connect(ui->update, SIGNAL(clicked(bool)), this, SLOT(cleanRemovedItems()));
    connect(ui->update1, SIGNAL(triggered(bool)), load, SLOT(getData()));
    connect(ui->update1, SIGNAL(triggered(bool)), this, SLOT(cleanRemovedItems()));
    connect(load, SIGNAL(downloaded()), this, SLOT(systemTrayMessage()));
    connect(load, SIGNAL(loadStatus(QString)), ui->statusBar, SLOT(showMessage(QString)));
    connect(ui->eur, SIGNAL(clicked(bool)), this, SLOT(setRow()));
    connect(ui->usd, SIGNAL(clicked(bool)), this, SLOT(setRow()));
    connect(ui->rub, SIGNAL(clicked(bool)), this, SLOT(setRow()));
    connect(ui->calc, SIGNAL(clicked(bool)), this, SLOT(calc()));
    connect(load, SIGNAL(ready()), this, SLOT(setRow()));
    connect(ui->currency, SIGNAL(currentIndexChanged(int)), this, SLOT(updCurrency(int)));
    //синхронізація обраного банку
    connect(ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(index(QModelIndex)));
    connect(ui->bankList, SIGNAL(currentIndexChanged(int)), ui->tableView, SLOT(selectRow(int)));
    //зберігання даних у форматі пдф і виведення на друк
    connect(ui->save, SIGNAL(triggered(bool)), this, SLOT(savePDF()));
    connect(ui->saveIn, SIGNAL(triggered(bool)), this, SLOT(savePDFin()));
    connect(ui->print, SIGNAL(triggered(bool)), this, SLOT(print()));
    //синхронізація купівля чи продажу валюти
    connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(selectProcedure(int)));
    connect(ui->toolBar, SIGNAL(visibilityChanged(bool)), this, SLOT(on_showToolBar_triggered()));
    connect(showHide, SIGNAL(triggered(bool)), this, SLOT(slotShowHide()));

    readSettings();
    autoUpdate();//налаштовуємо автооновлення (якшо не потрібно оновлювати, то загурзимо старі дані)

    ui->label_4->setText(tr("Інформація надана ресурсом: "
                         "<A HREF='http://finance.ua/'>finance.ua</A>"));

    localization();
}

MainWindow::~MainWindow()
{
    writeSettings();
    delete ui;
    delete load;
    delete model;
    delete setting;
    delete systemTray;
}

bool MainWindow::languageUkrainian()
{
    return ui->actionUkrainian->isChecked();
}


void MainWindow::closeEvent(QCloseEvent*)
{
    hide();
}


void MainWindow::rowHide(const int &index)
{
    removeItems.clear();
    for(int i=0;i<count;i++)
        if(model->data(model->index(i, index, QModelIndex())).isNull())
        {
            removeItems.push_back(model->takeRow(i));
            count--;
        }

}

void MainWindow::setRow()//додаємо всі банки
{
    model->setHorizontalHeaderItem(0, new QStandardItem(tr("Банк")));
    model->setHorizontalHeaderItem(1, new QStandardItem(tr("Купити €")));
    model->setHorizontalHeaderItem(2, new QStandardItem(tr("Продати €")));
    model->setHorizontalHeaderItem(3, new QStandardItem(tr("Купити $")));
    model->setHorizontalHeaderItem(4, new QStandardItem(tr("Продати $")));
    model->setHorizontalHeaderItem(5, new QStandardItem(tr("Купити Ք")));
    model->setHorizontalHeaderItem(6, new QStandardItem(tr("Продати Ք")));
    if(count>0)
    {
        for(int i=0;i<removeItems.size();i++)
        {
            model->insertRow(0, removeItems.at(i));
            count++;
        }
        if(ui->eur->isChecked())//eur
        {
            ui->tableView->showColumn(1);
            ui->tableView->showColumn(2);
            ui->tableView->hideColumn(3);
            ui->tableView->hideColumn(4);
            ui->tableView->hideColumn(5);
            ui->tableView->hideColumn(6);

            rowHide(1);
        }

        else if(ui->usd->isChecked())//usd
        {
            ui->tableView->showColumn(3);
            ui->tableView->showColumn(4);
            ui->tableView->hideColumn(1);
            ui->tableView->hideColumn(2);
            ui->tableView->hideColumn(5);
            ui->tableView->hideColumn(6);

            rowHide(3);
        }
        else if(ui->rub->isChecked())//rub
        {
            ui->tableView->showColumn(5);
            ui->tableView->showColumn(6);
            ui->tableView->hideColumn(1);
            ui->tableView->hideColumn(2);
            ui->tableView->hideColumn(3);
            ui->tableView->hideColumn(4);

            rowHide(5);
        }
    }

    ui->tableView->resizeColumnsToContents();
}

void MainWindow::calc()
{
    //moel - це таблиця, в якій кожен банк - рядок, 0 колонка - назва банку, 1 і 2 - куп\прод євро, 3 і 4 - долара, 5 і 6 - рубля
    double price = 0.0;
    int currencyIndex = ui->currency->currentIndex(),
            currency2Index = ui->currency_2->currentIndex();
    if(ui->buy->isChecked())//якшо купити
    {
        if(currencyIndex==0)//eur
        {
            if(currency2Index == 0)//"UAH"
                price = ui->money->value()*model->item(ui->bankList->currentIndex(), 1)->text().toDouble();
            else if(currency2Index == 1)//USD
                price = ui->money->value()*model->item(ui->bankList->currentIndex(), 1)->text().toDouble()/model->item(ui->bankList->currentIndex(), 3)->text().toDouble();
            else if(currency2Index == 2)//RUB
                price = ui->money->value()*model->item(ui->bankList->currentIndex(), 1)->text().toDouble()/model->item(ui->bankList->currentIndex(), 5)->text().toDouble();
        }
        else if(currencyIndex==1)//uah
        {
            if(currency2Index == 0)//eur
                price = ui->money->value()/model->item(ui->bankList->currentIndex(), 1)->text().toDouble();
            else if(currency2Index == 1)//USD
                price = ui->money->value()/model->item(ui->bankList->currentIndex(), 3)->text().toDouble();
            else if(currency2Index == 2)//RUB
                price = ui->money->value()/model->item(ui->bankList->currentIndex(), 5)->text().toDouble();
        }
        else if(currencyIndex==2)//usd
        {
            if(currency2Index == 0)//eur
                price = ui->money->value()*model->item(ui->bankList->currentIndex(), 3)->text().toDouble()/model->item(ui->bankList->currentIndex(), 1)->text().toDouble();
            else if(currency2Index == 1)//uah
                price = ui->money->value()*model->item(ui->bankList->currentIndex(), 3)->text().toDouble();
            else if(currency2Index == 2)//RUB
                price = ui->money->value()*model->item(ui->bankList->currentIndex(), 3)->text().toDouble()/model->item(ui->bankList->currentIndex(), 5)->text().toDouble();
        }
        else if(currencyIndex==3)//rub
        {
            if(currency2Index == 0)//eur
                price = ui->money->value()*model->item(ui->bankList->currentIndex(), 5)->text().toDouble()/model->item(ui->bankList->currentIndex(), 1)->text().toDouble();
            else if(currency2Index == 1)//uah
                price = ui->money->value()*model->item(ui->bankList->currentIndex(), 5)->text().toDouble();
            else if(currency2Index == 2)//usd
                price = ui->money->value()*model->item(ui->bankList->currentIndex(), 5)->text().toDouble()/model->item(ui->bankList->currentIndex(), 3)->text().toDouble();
        }
    }
    else//продати
    {
        if(currencyIndex==0)//eur
        {
            if(currency2Index == 0)//"UAH"
                price = ui->money->value()*model->item(ui->bankList->currentIndex(), 2)->text().toDouble();
            else if(currency2Index == 1)//USD
                price = ui->money->value()*model->item(ui->bankList->currentIndex(), 2)->text().toDouble()/model->item(ui->bankList->currentIndex(), 4)->text().toDouble();
            else if(currency2Index == 2)//RUB
                price = ui->money->value()*model->item(ui->bankList->currentIndex(), 2)->text().toDouble()/model->item(ui->bankList->currentIndex(), 6)->text().toDouble();
        }
        else if(currencyIndex==1)//uah
        {
            if(currency2Index == 0)//eur
                price = ui->money->value()/model->item(ui->bankList->currentIndex(), 2)->text().toDouble();
            else if(currency2Index == 1)//USD
                price = ui->money->value()/model->item(ui->bankList->currentIndex(), 4)->text().toDouble();
            else if(currency2Index == 2)//RUB
                price = ui->money->value()/model->item(ui->bankList->currentIndex(), 6)->text().toDouble();
        }
        else if(currencyIndex==2)//usd
        {
            if(currency2Index == 0)//eur
                price = ui->money->value()*model->item(ui->bankList->currentIndex(), 4)->text().toDouble()/model->item(ui->bankList->currentIndex(), 2)->text().toDouble();
            else if(currency2Index == 1)//uah
                price = ui->money->value()*model->item(ui->bankList->currentIndex(), 4)->text().toDouble();
            else if(currency2Index == 2)//RUB
                price = ui->money->value()*model->item(ui->bankList->currentIndex(), 4)->text().toDouble()/model->item(ui->bankList->currentIndex(), 6)->text().toDouble();
        }
        else if(currencyIndex==3)//rub
        {
            if(currency2Index == 0)//eur
                price = ui->money->value()*model->item(ui->bankList->currentIndex(), 6)->text().toDouble()/model->item(ui->bankList->currentIndex(), 2)->text().toDouble();
            else if(currency2Index == 1)//uah
                price = ui->money->value()*model->item(ui->bankList->currentIndex(), 6)->text().toDouble();
            else if(currency2Index == 2)//usd
                price = ui->money->value()*model->item(ui->bankList->currentIndex(), 6)->text().toDouble()/model->item(ui->bankList->currentIndex(), 4)->text().toDouble();
        }
    }

    ui->value1->setText(QString::number(price));
}

void MainWindow::updCurrency(int index)
{
    ui->currency_2->clear();
    switch(index)
    {
    case 0:
        ui->currency_2->addItems(QStringList()<<"UAH"<<"USD"<<"RUB");
        break;
    case 1:
        ui->currency_2->addItems(QStringList()<<"EUR"<<"USD"<<"RUB");
        break;
    case 2:
        ui->currency_2->addItems(QStringList()<<"EUR"<<"UAH"<<"RUB");
        break;
    case 3:
        ui->currency_2->addItems(QStringList()<<"EUR"<<"UAH"<<"USD");
        break;
    }
}


void MainWindow::index(QModelIndex index)
{
    ui->bankList->setCurrentIndex(index.row());
}


void MainWindow::savePDF()//зберігаємо через клас FileName
{
    QPrinter printer;
    FileName* name = new FileName(this, outputFileName);
    name->show();
    if(name->exec()==QDialog::Accepted)
    {
        verifyFileName(outputFileName);
        printer.setOutputFileName(outputFileName);

        QPainter paint(&printer);//налаштовуємо пейнтр
        draw(&paint);
        paint.end();
    }

    delete name;
}


void MainWindow::savePDFin()//зберігаємо через діалог
{
    QPrinter printer;
    QString adress = QFileDialog::getSaveFileName(this, tr("Зберегти файл"), tr("Курс валют"), "*.pdf");
    if(!adress.isEmpty())
    {
        verifyFileName(adress);
        printer.setOutputFileName(adress);
        QPainter painter(&printer);
        draw(&painter);
        painter.end();
    }
}


void MainWindow::verifyFileName(QString &fileName)
{
    QRegExp pdfContains(".pdf");
    pdfContains.setPatternSyntax(QRegExp::Wildcard);
    if(!pdfContains.exactMatch(fileName))//якшо не містить вкінці розширення, то воно додається автоматично
    {
        fileName+=".pdf";
    }
    ui->statusBar->showMessage(tr("Збережено у файл")+ fileName);
}

void MainWindow::verifyStyle()
{
    foreach (QString str, QStyleFactory::keys())
    {
        if(str=="Windows")
            ui->actionWindows->setEnabled(true);
        else if(str=="WindowsXP")
            ui->actionWindowsXP->setEnabled(true);
        else if(str=="WindowsVista")
            ui->actionWindowsVista->setEnabled(true);
        else if(str=="Fusion")
            ui->actionFusion->setEnabled(true);
        else if(str=="Mac")
            ui->actionMac->setEnabled(true);
    }
}

void MainWindow::initModel()
{
    //загрузчик
    model = new QStandardItemModel(0, 6);//назва банку, і три валюти (куп єв, прод єв, куп дол, прод дол)
    model->setHorizontalHeaderItem(0, new QStandardItem(tr("Банк")));
    model->setHorizontalHeaderItem(1, new QStandardItem(tr("Купити €")));
    model->setHorizontalHeaderItem(2, new QStandardItem(tr("Продати €")));
    model->setHorizontalHeaderItem(3, new QStandardItem(tr("Купити $")));
    model->setHorizontalHeaderItem(4, new QStandardItem(tr("Продати $")));
    model->setHorizontalHeaderItem(5, new QStandardItem(tr("Купити Ք")));
    model->setHorizontalHeaderItem(6, new QStandardItem(tr("Продати Ք")));
}

void MainWindow::localization()
{
    //локалізація (реалізована в конструкторі, бо змінюється тільки при запуску програми
    if(!ui->actionUkrainian->isChecked())//по замовчуванн виставлена українська локалізація, тому змінюємо тільки якшо вибрана англійсьа
    {
        ui->updateDate->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
        ui->money->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
        ui->value1->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    }
}

void MainWindow::setSystemTray()
{
    systemTray->setToolTip(tr("Exchange expert. Конвертер валют"));

    contextMenu = new QMenu(this);
    showHide = new QAction(tr("Показати/сховати"), this);
    contextMenu->addActions(QList<QAction*>()<<showHide<<ui->update1<<ui->autoStart<<ui->about<<ui->actionExit);
    systemTray->setContextMenu(contextMenu);
    systemTray->show();
}

void MainWindow::autoUpdate()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    // автоматично оновлюється кожні 12 годин
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), load, SLOT(getData()));//оновлюємо дані кожні 12 годин від останнього оновлення
    timer->start(hours12*1000);
    //не потрібно провіряти на те чи є якісь дані, бо вони обов'язково будуть! Якшо прога запускається вперше, вона автоматично оновить дані
    if(QFileInfo(QCoreApplication::applicationDirPath()+"/exchange.xml").lastModified().secsTo(currentDateTime)>hours12)//якшо дата останнього оновлення дальше 12 годин, зразу оновлюємо дані
    {
        load->getData();
    }
    else
    {
        //зразу загружаємо старі дані
        load->LoadData();
        //оновлюємо в такий час шоб було 12 годин від останнього (один раз)
        QTimer::singleShot((hours12 - QFileInfo(QCoreApplication::applicationDirPath()+"/exchange.xml").lastModified().secsTo(currentDateTime))*1000, load, SLOT(getData()));
    }
}


void MainWindow::print()
{
    QPrinter* printer = new QPrinter();
    QPrintDialog dialog(printer, this);//створюємо діалогове вікно в якому задаються всі налаштування для друку (всі налаштування задаються в printer)
    if(dialog.exec() == QDialog::Accepted)
    {
        QPainter paint(printer);
        draw(&paint);
        paint.end();
        ui->statusBar->showMessage(tr("Надруковано"));
    }
    delete printer;
}

void MainWindow::selectProcedure(int index)
{
    if(index == 1 || index == 3 || index == 5)
        ui->buy->setChecked(true);
    else if(index == 2 || index == 4 || index == 6)
        ui->sell->setChecked(true);
}


void MainWindow::draw(QPainter *painter)
{
    painter->setFont(QFont(QString("Times"), 12));
    painter->setPen(QPen(QColor(Qt::black)));
    painter->setRenderHint(QPainter::Antialiasing, true);
    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableView->resize(ui->tableView->columnWidth(0)+ui->tableView->columnWidth(1)+ui->tableView->columnWidth(2)+ui->tableView->columnWidth(3)+ui->tableView->columnWidth(4)+ui->tableView->columnWidth(5)+ui->tableView->columnWidth(6)+ui->tableView->verticalHeader()->width(),//змінюємо розмір шоб вміщувалися всі елементи
                          (ui->tableView->rowHeight(0))*count+ui->tableView->horizontalHeader()->height()+2);//+2 шоб не було скролу
    ui->tableView->render(painter);//захоплюмо пейнтером область і скрінимо її. Скрін опиняється в потрібному файлі
    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

void MainWindow::writeSettings()
{
    setting->beginGroup("/Settings");
        setting->setValue("/style", styleName);
        setting->setValue("/autoStart", ui->autoStart->isChecked());
        setting->setValue("/languageUkrainian", ui->actionUkrainian->isChecked());
        setting->setValue("/updating", ui->updateDate->dateTime());
    setting->endGroup();

    //додаємо або видаляємо програму з автозапуску
    if(ui->autoStart->isChecked())
    {
        QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        settings.setValue("Exchange Expert", QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
        settings.sync();
    }
    else
    {
        QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        settings.remove("Exchange Expert");
    }
}

void MainWindow::readSettings()
{
    setting->beginGroup("/Settings");
        styleName = setting->value("/style", "WindowsVista").toString();
        if(styleName == "Windows")
            ui->actionWindows->setChecked(true);
        else if(styleName == "WindowsXP")
           ui->actionWindowsXP->setChecked(true);
        else if(styleName == "WindowsVista")
           ui->actionWindowsVista->setChecked(true);
        else if(styleName == "Fusion")
           ui->actionFusion->setChecked(true);
        QApplication::setStyle(QStyleFactory::create(styleName));

        ui->autoStart->setChecked(setting->value("/autoStart", false).toBool());
        ui->actionUkrainian->setChecked(setting->value("/languageUkrainian", true).toBool());
        ui->updateDate->setDateTime(setting->value("/updating").toDateTime());
    setting->endGroup();
    if(!ui->actionUkrainian->isChecked())
        ui->actionEnglish->setChecked(true);
}


//встановлюємо потрібний стиль
void MainWindow::on_actionWindows_triggered()
{
    QApplication::setStyle(QStyleFactory::create(QString("Windows")));
    styleName = "Windows";
    ui->actionFusion->setChecked(false);
    ui->actionWindows->setChecked(true);
    ui->actionWindowsXP->setChecked(false);
    ui->actionWindowsVista->setChecked(false);
    ui->actionMac->setChecked(false);
}

void MainWindow::on_actionWindowsXP_triggered()
{
    QApplication::setStyle(QStyleFactory::create(QString("WindowsXP")));
    styleName = "WindowsXP";
    ui->actionFusion->setChecked(false);
    ui->actionWindows->setChecked(false);
    ui->actionWindowsXP->setChecked(true);
    ui->actionWindowsVista->setChecked(false);
    ui->actionMac->setChecked(false);
}

void MainWindow::on_actionWindowsVista_triggered()
{
    QApplication::setStyle(QStyleFactory::create(QString("WindowsVista")));
    styleName = "WindowsVista";
    ui->actionFusion->setChecked(false);
    ui->actionWindows->setChecked(false);
    ui->actionWindowsXP->setChecked(false);
    ui->actionWindowsVista->setChecked(true);
    ui->actionMac->setChecked(false);
}

void MainWindow::on_actionFusion_triggered()
{
    QApplication::setStyle(QStyleFactory::create(QString("Fusion")));
    styleName = "Fusion";
    ui->actionFusion->setChecked(true);
    ui->actionWindows->setChecked(false);
    ui->actionWindowsXP->setChecked(false);
    ui->actionWindowsVista->setChecked(false);
    ui->actionMac->setChecked(false);
}

void MainWindow::on_actionMac_triggered()
{
    QApplication::setStyle(QStyleFactory::create(QString("Mac")));
    styleName = "Mac";
    ui->actionFusion->setChecked(false);
    ui->actionWindows->setChecked(false);
    ui->actionWindowsXP->setChecked(false);
    ui->actionWindowsVista->setChecked(false);
    ui->actionMac->setChecked(true);
}

void MainWindow::slotShowHide()
{
    setVisible(!isVisible());
}

void MainWindow::systemTrayMessage()
{
    systemTray->showMessage(tr("Оновлення"), tr("Дані оновлено"));
    ui->updateDate->setDateTime(QDateTime::currentDateTime());
}

void MainWindow::cleanRemovedItems()
{
    removeItems.clear();
}

void MainWindow::on_actionEnglish_triggered()
{
    ui->actionUkrainian->setChecked(false);
    ui->actionEnglish->setChecked(true);
    QMessageBox::information(this, tr("Налаштування мови"), tr("Для зміни мови перегрузіть програму"));
}

void MainWindow::on_actionUkrainian_triggered()
{
    ui->actionUkrainian->setChecked(true);
    ui->actionEnglish->setChecked(false);
    QMessageBox::information(this, tr("Налаштування мови"), tr("Для зміни мови перегрузіть програму"));
}

void MainWindow::on_about_triggered()
{
    QMessageBox::about(this, tr("Про програму"), tr("Створена Морозюком Ростиславом з навчальною метою (вивчення фреймворка Qt). \n"
                                             "Версія програми: 4.0"));
}

void MainWindow::on_aboutQt_triggered()
{
    QMessageBox::aboutQt(this, tr("Про Qt"));
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::beep();//звук
    int button = QMessageBox::question(this, tr("Вихід"), tr("Закрити програму?"), tr("Так"), tr("Ні"));
    if(button == 0)
        QApplication::exit(0);
}

void MainWindow::on_showToolBar_triggered()
{
    if(ui->showToolBar->isChecked())
        ui->toolBar->show();
    else
        ui->toolBar->hide();
}
