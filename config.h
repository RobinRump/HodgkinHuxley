#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>

class Config : public QObject
{
    Q_OBJECT
public:
    explicit Config(QObject *parent = 0);

signals:

public slots:

};

#endif // CONFIG_H
