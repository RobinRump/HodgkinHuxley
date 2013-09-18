#include "config.h"

Config::Config(QObject *parent) :
    QObject(parent)
{
    this->file = new QFile(QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/com.robinrump.hodgkinhuxley.json"));

    QJsonObject json;
    if (this->file->exists()) {
        QJsonDocument document;
        QByteArray bytes;
        if (!this->file->open(QIODevice::ReadOnly | QIODevice::Text)) {
            this->config = QJsonObject();
            return;
        }
        bytes = this->file->readAll();
        this->file->close();
        document = QJsonDocument::fromJson(bytes);
        this->config = document.object();
        this->preferences = config.value("preferences").toObject();
    } else {
        this->preferences.insert("startup", true);
        this->config.insert("preferences", this->preferences);
        this->config.insert("version", 107);
        this->write();
    }
}

bool Config::write()
{
    QJsonDocument document(this->config);
    QByteArray bytes = document.toJson();
    if (!this->file->open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    this->file->write(bytes);
    this->file->close();
    return true;
}
