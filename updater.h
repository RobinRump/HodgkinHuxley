#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QFile>
#include <QQueue>
#include <QTime>
#include <QTimer>
#include <QUrl>
#include <QNetworkAccessManager>

class Updater : public QObject
{
    Q_OBJECT
public:
    explicit Updater(QObject *parent = 0);

    void append(const QUrl &url);
    void append(const QStringList &urls);
    QString saveFileName(const QUrl &url);

signals:
    void finished();

private slots:
    void startNextDownload();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();
    void downloadReadyRead();

private:
    QNetworkAccessManager manager;
    QQueue<QUrl> downloadQueue;
    QNetworkReply *currentDownload;
    QFile output;
    QTime downloadTime;
    TextProgressBar progressBar;

    int downloadedCount;
    int totalCount;
};

};

#endif // UPDATER_H
