#include "updater.h"

Updater::Updater(QObject *parent) :
    QObject(parent), downloadedCount(0), totalCount(0)
{
}

void Updater::append(const QStringList &urls)
{
    foreach (QString url, urls) {
        this->append(QUrl::fromEncoded(url.toLocal8Bit()));
    }

    if (this->downloadQueue.isEmpty()) {
        QTimer::singleShot(0, this, SIGNAL(finished()));
    }
}

void Updater::append(const QUrl &url)
{
    if (this->downloadQueue.isEmpty()) {
        QTimer::singleShot(0, this, SLOT(startNextDownload()));
    }

    this->downloadQueue.enqueue(url);
    ++this->totalCount;
}

QString Updater::saveFileName(const QUrl &url)
{
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "download";

    if (QFile::exists(basename)) {
        int i = 0;
        basename += '.';
        while (QFile::exists(basename + QString::number(i)))
            ++i;

        basename += QString::number(i);
    }

    return basename;
}

void Updater::startNextDownload()
{
    if (this->downloadQueue.isEmpty()) {
        emit this->finished();
        return;
    }

    QUrl url = this->downloadQueue.dequeue();

    QString filename = this->saveFileName(url);
    this->output.setFileName(filename);
    if (!this->output.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "Problem opening save file '%s' for download '%s': %s\n",
                qPrintable(filename), url.toEncoded().constData(),
                qPrintable(this->output.errorString()));

        this->startNextDownload();
        return;
    }

    QNetworkRequest request(url);
    this->currentDownload = this->manager.get(request);
    connect(this->currentDownload, SIGNAL(downloadProgress(qint64,qint64)), SLOT(downloadProgress(qint64,qint64)));
    connect(this->currentDownload, SIGNAL(finished()), SLOT(downloadFinished()));
    connect(this->currentDownload, SIGNAL(readyRead()), SLOT(downloadReadyRead()));

    // prepare the output
    printf("Downloading %s...\n", url.toEncoded().constData());
    this->downloadTime.start();
}

void Updater::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    progressBar.setStatus(bytesReceived, bytesTotal);

    // calculate the download speed
    double speed = bytesReceived * 1000.0 / downloadTime.elapsed();
    QString unit;
    if (speed < 1024) {
        unit = "bytes/sec";
    } else if (speed < 1024*1024) {
        speed /= 1024;
        unit = "kB/s";
    } else {
        speed /= 1024*1024;
        unit = "MB/s";
    }

    progressBar.setMessage(QString::fromLatin1("%1 %2")
                           .arg(speed, 3, 'f', 1).arg(unit));
    progressBar.update();
}

void Updater::downloadFinished()
{
    progressBar.clear();
    output.close();

    if (currentDownload->error()) {
        // download failed
        fprintf(stderr, "Failed: %s\n", qPrintable(currentDownload->errorString()));
    } else {
        printf("Succeeded.\n");
        ++downloadedCount;
    }

    currentDownload->deleteLater();
    startNextDownload();
}

void Updater::downloadReadyRead()
{
    this->output.write(currentDownload->readAll());
}

