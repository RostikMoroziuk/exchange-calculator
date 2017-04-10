#ifndef FILENAME_H
#define FILENAME_H

#include <QDialog>
#include <QPushButton>
#include <QString>
#include <QTranslator>

namespace Ui {
class FileName;
}

class FileName : public QDialog
{
    Q_OBJECT

public:
    explicit FileName(QWidget *parent, QString& str);
    ~FileName();

private:
    Ui::FileName *ui;
    QString& fileName;//ссилки повинні інцілізуватися в списку ініціалізацй

private slots:
    void okClicked();
};

#endif // FILENAME_H
