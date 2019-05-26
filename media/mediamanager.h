#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

#include <QObject>

class MediaManager : public QObject
{
    Q_OBJECT
public:
    explicit MediaManager(const QString& login, const QString& password, QObject *parent = 0);
    ~MediaManager();

    void setLoginPasword(const QString& login, const QString& password);

    void getMediaUri(const QString& xaddr);

signals:
    void mediaUri(QString uri);

public slots:
};

#endif // MEDIAMANAGER_H
