#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QMainWindow>

#include "dicomheader.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_selectBtn_clicked();
    void on_uploadOrthancBtn_clicked();
    void on_uploadFHIRBtn_clicked();
    void finished(QNetworkReply* reply);
    void uploadToFHIR(QString resourceType, QString id, const QByteArray data);
    void clearDir(const QString path);
    void getMetadata(QJsonDocument json);

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager m_manager;
    QStringList fileNames;
    QString dirPath;
    int DICOMCount;
    int uploadedDICOMCount;
    int progressValue;
    QMap<QString, ImagingStudy> studies;
    DICOMDataObject DDO;
    iStudy iSty;
    QMap<QString, QString> uidMap;
    dicomJSONTemp myDICOMJson;
};
#endif // MAINWINDOW_H
