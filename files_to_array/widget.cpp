#include "widget.h"
#include "ui_widget.h"
#include <QUrl>
#include <QFileInfo>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->plainTextEdit->setAcceptDrops(false);
    this->setAcceptDrops(true);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::dropEvent(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();
        for (int i = 0; i < urlList.size(); ++i) {
            pathList.append(QFileInfo(urlList.at(i).toLocalFile()).fileName());
        }
        ui->plainTextEdit->setPlainText("'files' => ['" + pathList.join("', '") + "']");
    }
}

void Widget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}
