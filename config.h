#ifndef CONFIG_H
#define CONFIG_H

#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QObject>
#include <QStandardPaths>
#include <QString>

class Config : public QObject
{
    Q_OBJECT
public:
    explicit Config(QObject *parent = 0);

signals:

public slots:
    bool write();

private:
    QFile *file;

    QJsonObject config;
    QJsonObject preferences;

private slots:


};

#endif // CONFIG_H
