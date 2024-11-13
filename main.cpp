#include <iostream>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QDir>
#include <QEventLoop>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    cout << "" << endl;
    cout << "===================================================" << endl;
    cout << "RENDEZ-VOUS WITH ISS" << endl;
    cout << "Authors: Recep Kamrak, Emil Mammadli, Burak Saglam" << endl;
    cout << "Middle East Technical University" << endl;
    cout << "===================================================" << endl;

    // Create data directory
    QDir().mkpath("./data");

    QNetworkAccessManager manager;
    QEventLoop loop;

    QNetworkRequest request(QUrl("https://live.ariss.org/iss.txt"));
    QNetworkReply *reply = manager.get(request);

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if(reply->error() == QNetworkReply::NoError) {
        QFile file("./data/file.txt");
        if(file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();
            cout << "[Debug] TLE ile downloaded successfully!" << endl;
        }
    } else {
        cout << "Download failed: " << reply->errorString().toStdString() << endl;
    }

    reply->deleteLater();
    return 0;
}
