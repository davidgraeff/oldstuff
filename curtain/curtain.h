#ifndef curtain_H
#define curtain_H

#include <QtCore/QObject>
#include <QSslSocket>

class curtain : public QObject
{
Q_OBJECT
public:
    curtain();
    virtual ~curtain();
private:

private Q_SLOTS:
    void message(const QString& msg);
    void serverJSON(const QVariantMap& data);
	void stateChanged();
	void onInputData();
};

#endif // curtain_H
