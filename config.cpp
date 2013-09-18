#include "config.h"

Config::Config(QObject *parent) :
    QObject(parent)
{
    this->file = new QFile(QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/com.robinrump.hodgkinhuxley.json"));

    QJsonObject json;
    if (this->file->exists()) {
        json = this->fromConfig();
        this->pref = json.value("settings").toObject();
    } else {
        QJsonObject p;
        p.insert("startup", true);
        json.insert("preferences", p);
        json.insert("version", 107);
        this->toConfig(json);
    }
}

QJsonObject MainWindow::fromConfig()
{
    QJsonDocument document;
    QByteArray bytes;
    if (!this->config->open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QJsonObject();
    }
    bytes = this->config->readAll();
    this->config->close();
    document = QJsonDocument::fromJson(bytes);
    return document.object();
}

bool MainWindow::toConfig(QJsonObject j)
{
    QJsonDocument document(j);
    QByteArray bytes = document.toJson();
    if (!this->config->open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    this->config->write(bytes);
    this->config->close();
    return true;
}
