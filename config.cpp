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
        if (document.isNull() == false) {
            this->config = document.object();
            this->preferences = config.value("preferences").toObject();

            return;
        }
    }
    this->preferences.insert("startup", false);
    this->config.insert("preferences", this->preferences);
    this->config.insert("version", 108);
    this->write();
}

bool Config::write()
{
    this->config.remove("preferences");
    this->config.insert("preferences", this->preferences);
    QJsonDocument document(this->config);
    QByteArray bytes = document.toJson();
    if (!this->file->open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    this->file->write(bytes);
    this->file->close();
    return true;
}

int Config::version() {
    return (int) this->config.value("version").toDouble();
}

void Config::setVersion(int version)
{
    this->config.remove("version");
    this->config.insert("version", version);
}

bool Config::startup()
{
    return this->preferences.value("startup").toBool();
}

void Config::setStartup(bool startup)
{
    this->config.remove("startup");
    this->config.insert("startup", startup);
}
