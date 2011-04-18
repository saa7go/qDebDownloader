#include <QApplication>
#include <QStringList>
#include <QNetworkAccessManager>
#include "aptweb.h"
#include <QDebug>
#include <QTextStream>

QTextStream cout(stdout);

class Object : public QObject
{
    Q_OBJECT
public:
    Object(){ }

public slots:
    void printDownloadList(const QStringList &packages)
    {
        foreach(const QString &url, packages)
            cout << url << endl;
        qApp->quit();
    }
};

void usage(const QString &appName)
{
    QStringList distribustion;
    distribustion << "Ubuntu 10.10 \"Maverick Meerkat\" i386"
    << "Ubuntu 10.10 \"Maverick Meerkat\" amd64"
    << "Ubuntu 10.04 \"Lucid Lynx\" i386"
    << "Ubuntu 10.04 \"Lucid Lynx\" amd64"
    << "Ubuntu 9.10 \"Karmic Koala\" i386"
    << "Ubuntu 9.04 \"Jaunty Jackalope\" i386"
    << "Ubuntu 8.10 \"Intrepid Ibex\" i386"
    << "Ubuntu 8.04 \"Hardy Heron\" i386"
    << "Ubuntu 7.10 \"Gutsy Gibbon\" i386"
    << "XUbuntu 7.10 \"Gutsy Gibbon\" i386"
    << "Ubuntu 7.04 \"Feisty Fawn\" i386"
    << "Ubuntu 6.10 \"Edgy Eftt\" Server i386"
    << "Ubuntu 6.06 \"Dapper Drake\" i386";

    cout << "Usage:" << endl;
    cout << QString("\t%1 distribution_number package_name [other_package_name]").arg(appName) << endl;
    cout << endl << "Distribution number:" << endl;
    for(int i = 0; i < distribustion.count(); i++)
    {
        cout << QString("\t%1 : %2").arg(QString::number(i)).arg(distribustion.at(i)) << endl;
    }
    cout << endl;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QStringList args = app.arguments();
    if(args.count() < 3)
    {
//        cout << args.at(0) << " package_name1 " << "[package_nameN]" << endl;
        usage(args.at(0));
        return 1;
    }

    AptWeb::Distribution dist;
    switch(args.at(1).toInt())
    {
    case 0:
        dist = AptWeb::Ubuntu_MaverickMeerkat_i386; break;
    case 1:
        dist = AptWeb::Ubuntu_MaverickMeerkat_amd64; break;
    case 2:
        dist = AptWeb::Ubuntu_LucidLynx_i386; break;
    case 3:
        dist = AptWeb::Ubuntu_LucidLynx_amd64; break;
    case 4:
        dist = AptWeb::Ubuntu_KarmicKoala_i386; break;
    case 5:
        dist = AptWeb::Ubuntu_JauntyJackalope_i386; break;
    case 6:
        dist = AptWeb::Ubuntu_IntrepidIbex_i386; break;
    case 7:
        dist = AptWeb::Ubuntu_HardyHeron_i386; break;
    case 8:
        dist = AptWeb::Ubuntu_GutsyGibbon_i386; break;
    case 9:
        dist = AptWeb::XUbuntu_GutsyGibbon_i386; break;
    case 10:
        dist = AptWeb::Ubuntu_FeistyFawn_i386; break;
    case 11:
        dist = AptWeb::Ubuntu_EdgyEft_Server_i386; break;
    case 12:
        dist = AptWeb::Ubuntu_DapperDrake_i386; break;
    default:
        usage(args.at(0)); return 1;
    }

    QStringList packageNames;
    for(int i = 2; i < args.count(); i++)
        packageNames << args.at(i);

    Object obj;
    QNetworkAccessManager manager;
    AptWeb aptWeb(&manager);

    QObject::connect(&aptWeb, SIGNAL(downloadList(QStringList)), &obj, SLOT(printDownloadList(QStringList)));

    aptWeb.findPackage(packageNames, dist);

    return app.exec();
}

#include "main.moc"
