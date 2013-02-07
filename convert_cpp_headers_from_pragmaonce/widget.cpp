#include "widget.h"
#include "ui_widget.h"
#include <QDir>
#include <QDebug>
#include <QFile>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

int convertDir(QDir d, int&auf, int&zu) {
    if (!d.exists())
        return 0;

    int result = 0;
    {
        QStringList l = d.entryList(QDir::Files);
        foreach (QString filename, l) {
            QFile f(d.absoluteFilePath(filename));
            if (!f.open(QFile::ReadWrite))
                continue;
            QByteArray b = f.readAll();

            {
                ++result;
                int a= b.count('{');
                int lzu= b.count('}');
                if (a!=lzu) {
                    qDebug() << d.absoluteFilePath(filename) << a << lzu;
                }
                auf += a;
                zu += lzu;
            }
            continue;
            int index = b.indexOf("#pragma once");
            if (index == -1)
                continue;

            QByteArray guardname = d.absoluteFilePath(filename).replace("/","_").replace(" ","_").replace(".","_").toUtf8();
            b = b.replace("#pragma once","#ifndef "+ guardname+"\n#define "+guardname) + "\n#endif";
            f.seek(0);
            f.write(b);
            f.close();
            ++result;
        }
    }
    {
        QStringList ld = d.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
        foreach (QString dir, ld) {
            result += convertDir(d.absoluteFilePath(dir), auf, zu);
        }
    }
    return result;
}

void Widget::on_btnConvert_clicked()
{
    int a=0,b=0;
    int result = convertDir(ui->lineEdit->text(), a, b);
    ui->label->setText(QString::number(result)+" "+QString::number(a)+" "+QString::number(b));
}
