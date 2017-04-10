#include "filename.h"
#include "ui_filename.h"

FileName::FileName(QWidget *parent, QString& str) :
    QDialog(parent),
    ui(new Ui::FileName), fileName(str)
{
    ui->setupUi(this);

    this->setFixedSize(this->size());
    this->setPalette(parent->palette());

    connect(ui->ok, SIGNAL(clicked(bool)), this, SLOT(okClicked()));
    connect(ui->cancel, SIGNAL(clicked(bool)), this, SLOT(close()));
}

FileName::~FileName()
{
    delete ui;
}

void FileName::okClicked()
{
    fileName = ui->lineEdit->text();
    this->close();
}

