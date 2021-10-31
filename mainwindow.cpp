#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "imagingstudy.h"


#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QFileDialog>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&this->m_manager, &QNetworkAccessManager::finished, this, &MainWindow::finished);
    DICOMCount=0;
    uploadedDICOMCount=0;
    uploadDICOMsuccess = 0;
    progressValue=0;
    ui->progressBar->setRange(0, 100);
    bool normal = LoadDictionary("dd.txt");   // DICOM tags and VR dictionary
    if(normal){
        QString jsonTemp = "map.json"; //與 dd.txt 一樣，需存放在程式編譯結果目錄 (exe的上一層)
        if (myDICOMJson.loadFile(jsonTemp) == true) {
            qDebug() << myDICOMJson.getJSONData("0002", "0001", "type");
        } else {
            qDebug() << "load DICOM JSON error";
            QMessageBox msgBox;
            msgBox.setText("map.json load failed! Closing application...");
            msgBox.exec();
            QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);     //close app
        }
    }
    else {
        QMessageBox msgBox;
        msgBox.setText("Dictionary load failed! Closing application...");
        msgBox.exec();
        QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);     //close app
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::finished(QNetworkReply * reply)
{
    QString url = reply -> url().toString();

    QString type = url.split('/').last();
    ui -> textEdit -> append(type);

    QString result = QString(reply -> readAll());
    QJsonParseError error;
    QJsonDocument json = QJsonDocument::fromJson(result.toUtf8(), & error);
    if (error.error == QJsonParseError::NoError && json.isObject()) {
        //ui -> textEdit_result -> append(json.toJson());
        //ui -> textEdit_result -> append("\n\n");
        if (type == "instances") {
            QString status = json.object().value("Status").toString();
            ui -> textEdit -> append(result);
            uploadedDICOMCount++;
            if(status=="Success") {
                uploadDICOMsuccess++;
            }
            if (uploadedDICOMCount == DICOMCount) {
                if(uploadDICOMsuccess == uploadedDICOMCount) {  //all upload success
                    ui->progressBar->setValue(100);
                    ui->uploadFHIRBtn->setEnabled(true);
                }
                else {  //there is unsuccessful upload
                    QMessageBox msgBox;
                    msgBox.setText("Upload to DICOM server failed! Closing application...");
                    msgBox.exec();
                    QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);     //close app
                }
            }
            ui -> progressBar -> setValue(ui -> progressBar -> value() + progressValue);
        } else {
            QString resourceType = json.object().value("resourceType").toString();

            if(resourceType=="Endpoint") { //Endpoint upload success, proceed upload ImagingStudy
                ui->progressBar->setValue(50);

                QFile istudy(ui->istudyEdit->text());
                if (istudy.open(QFile::ReadOnly)) {
                    QByteArray data = istudy.readAll();
                    QJsonDocument doc = QJsonDocument::fromJson(data);
                    QJsonObject obj;

                    //check doc
                    if(!doc.isNull()) {
                        obj = doc.object();
                        QString resourceType = obj.value("resourceType").toString();
                        QString id = obj.value("id").toString();    //return "" if no id inside JSON
                        uploadToFHIR(resourceType, id, data);   //upload ImagingStudy
                    }
                }
            }
            else if(resourceType=="ImagingStudy") {
                ui->progressBar->setValue(100);
                ui->selectBtn->setEnabled(true);
            }
            ui->textEdit_result->append(result);
        }

    }
}

void MainWindow::on_selectBtn_clicked()
{
    uploadedDICOMCount=0;

    // 基於指定的目錄，針對每一個 DICOM，迴圈重複跑以下程式。以產生匿名 DICOM 檔及 imagingStudies
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(tr("DICOM files (*.dcm)"));

    if (dialog.exec())
        fileNames = dialog.selectedFiles();
    if (fileNames.length() > 0) {
        ui -> uploadOrthancBtn -> setEnabled(true);

        DICOMCount = fileNames.length();

        //START: display file names
        QString files = "";
        foreach(QString f, fileNames) {
            files += f + "; ";
        }
        ui -> filesEdit -> setText(files);
        //END: display file names

        //START: get destination folder path
        QFileInfo f(fileNames[0]);
        dirPath = f.path();
        //END: get destination folder path

        //START: create ImagingStudy
        iSty.patientID ="2171";
        iSty.patientName ="anonymous";
        iSty.endpoint = ui->dicomEdit->text();
        //END: create ImagingStudy

        for (int i=0; i<DICOMCount; i++) {
            QFileInfo f(fileNames[i]);
            QString filename = f.completeBaseName();

            QByteArray byte = fileNames[i].toUtf8();
            char* srcDCM = new char[byte.size()+1];
            strcpy(srcDCM, byte.data());

            byte = (dirPath+"/"+filename+".xml").toUtf8();
            char* retXML = new char[byte.size()+1];
            strcpy(retXML, byte.data());

            byte = (dirPath+"/"+filename+"-anon.dcm").toUtf8();
            char* retDCM = new char[byte.size()+1];
            strcpy(retDCM, byte.data());

            byte = (dirPath+"/tempPart10Elements.dcm").toUtf8();
            char* tempPart10Elements = new char[byte.size()+1];
            strcpy(tempPart10Elements, byte.data());

            //char* srcDCM = (filename+".dcm").toUtf8().data();  //"D:\\ttt\\IM-0001-0070.DCM";
            //char* retXML = (filename+".xml").toUtf8().data();  //"D:\\ttt\\IM-0001-0070.xml";
            //char tempPart10Elements[] = "D:\\ttt\\temp10EUS2.dcm";

            // retDCM 為匿名的 DICOM 檔，或可放在目前目錄的子目錄，如 D:\\ttt\\did\mo.dcm
            //char* retDCM = (filename+"-anon.dcm").toUtf8().data();   //"D:\\ttt\\IM-0001-0070-anon.dcm";

            //START: create a DICOM object, define the file path for input and output
            DICOMDataObject DDO;
            DDO.jsonTemp = & myDICOMJson;
            DDO.srcDCM = srcDCM;
            DDO.retXML = retXML;
            DDO.tempPart10Elements = tempPart10Elements;
            DDO.retDCM = retDCM;
            //END: create a DICOM object

            DDO.ReadDICOMPart10File(); //decode header
            DDO.DecodeRetToXML(retXML); //output to XML



            //START: anonymize
            DDO.SetUsefulElements();
            //uidMap = DDO.SetJSONElement(uidMap);   //replace original data with anonymized data
            uidMap = DDO.SetJSONElement(uidMap);
            DDO.SaveDICOM();    //output anonymized dicom file
            //END: anonymize

            DICOMDataObject anonDDO;    //create new DDO for anonymized image
            anonDDO.srcDCM = retDCM;    //get anonymized image file
            anonDDO.ReadDICOMPart10File();  //read metadata
            anonDDO.SetUsefulElements();
            QString istudyFile = iSty.addToStudy(&(anonDDO.ufElements), dirPath);    //add to imaging studys
            ui->istudyEdit->setText(istudyFile);
        }
        //QString endpointfile = iSty.createEndpointJSON(dirPath);
        QString ed = iSty.createEndpointJSON(dirPath);
        ui->endpointEdit->setText(ed);

        ui -> textEdit -> append(iSty.json + "\n\n");
    }
    ui -> progressBar -> reset();
}

void MainWindow::on_uploadOrthancBtn_clicked()
{
    ui -> selectBtn -> setEnabled(false);
    ui -> uploadOrthancBtn -> setEnabled(false);
    ui -> uploadFHIRBtn -> setEnabled(false);

    foreach(QString fileName, fileNames) {
        QFileInfo file (fileName);
        QString dirPath = file.path();
        QString filename = file.completeBaseName();
        QFile f(dirPath + '/'+filename+"-anon.dcm");
        if (f.open(QFile::ReadOnly)) {
            QByteArray data = f.readAll();

            qDebug("Upload to DICOM server start\n");

            //QUrl url(ui -> dicomEdit -> text() + "/servers/faith/stow");
            QUrl url(ui -> dicomEdit -> text() + "/instances");

            QNetworkRequest request(url);
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/dicom");
            //request.setRawHeader("Authorization", "Bearer 9204aeb9-d1cb-4511-8641-1f2c964f7c6a");
            m_manager.post(request, data);
        }
    }
    progressValue = 50 / fileNames.length();
    ui -> progressBar -> setValue(ui -> progressBar -> value() + progressValue);
}

void MainWindow::on_uploadFHIRBtn_clicked()
{
    ui -> selectBtn -> setEnabled(false);
    ui -> uploadOrthancBtn -> setEnabled(false);
    ui -> uploadFHIRBtn -> setEnabled(false);

    QFile endpoint(ui->endpointEdit->text());

    //START: upload Endpoint (ImagingStudy upload in finished() function if Endpoint upload success
    if (endpoint.open(QFile::ReadOnly)) {
        QByteArray data = endpoint.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj;

        //check doc
        if(!doc.isNull()) {
            obj = doc.object();
            QString resourceType = obj.value("resourceType").toString();
            QString id = obj.value("id").toString();    //return "" if no id inside JSON
            uploadToFHIR(resourceType, id, data);   //upload Endpoint


        }
    }
}

void MainWindow::uploadToFHIR(QString resourceType, QString id, const QByteArray data)
{

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QUrl url(ui -> fhirEdit -> text() + "/" + resourceType + "/" + id);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json+fhir");
    //request.setRawHeader("Authorization", "Bearer 87944918-1f86-418e-8418-398e2e4ddee6");
    if (id.trimmed() == "") {
        this -> m_manager.post(request, data);
    } else {
        this -> m_manager.put(request, data);
    }
}

void MainWindow::clearDir(const QString path)
{
    QDir dir(path);
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    foreach(QString dirItem, dir.entryList()) {
        dir.remove(dirItem);
    }
}






