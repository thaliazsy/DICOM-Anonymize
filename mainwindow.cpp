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
    fileCount=0;
    uploadedFilesCount=0;
    progressValue=0;
    ui->progressBar->setRange(0, 100);
    bool normal = LoadDictionary("dd.txt");   // DICOM tags and VR dictionary
    if(!normal){
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

void MainWindow::getMetadata(QJsonDocument json)
{
    QMap < QString, QString > metadata;
    QJsonObject data = json.object();

    metadata.insert("SOPClassUID", data.value("SOPClassUID").toString());
    metadata.insert("SOPInstanceUID", data.value("SOPInstanceUID").toString());

    QString str = data.value("StudyDate").toString();
    QString v1 = str.mid(0, 4);
    QString v2 = str.mid(4, 2);
    QString v3 = str.mid(6, 2);
    QString ymd = v1 + "-" + v2 + "-" + v3;
    metadata.insert("StudyDate", ymd);

    str = data.value("StudyTime").toString();
    v1 = str.mid(0, 2);
    v2 = str.mid(2, 2);
    v3 = str.mid(4, 2);
    QString hms = v1 + ":" + v2 + ":" + v3;
    metadata.insert("StudyTime", hms);

    metadata.insert("Modality", data.value("Modality").toString());
    metadata.insert("PatientID", data.value("PatientID").toString());
    metadata.insert("PatientName", data.value("PatientName").toString());
    metadata.insert("BodyPartExamined", data.value("BodyPartExamined").toString());
    metadata.insert("StudyInstanceUID", data.value("StudyInstanceUID").toString());
    metadata.insert("SeriesInstanceUID", data.value("SeriesInstanceUID").toString());
    metadata.insert("SeriesNumber", data.value("SeriesNumber").toString());
    metadata.insert("InstanceNumber", data.value("InstanceNumber").toString());

    //create Study
    QString key = metadata.value("StudyInstanceUID");
    if (studies.contains(key)) {
        studies[key].appendToStudy(metadata);
        qDebug("append study");
    } else {
        ImagingStudy temp;
        temp.createStudy(metadata);
        studies.insert(metadata.value("StudyInstanceUID"), temp);
        qDebug("create study");
    }
    //create Study end

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
        ui -> textEdit_result -> append(json.toJson());
        ui -> textEdit_result -> append("\n\n");
        if (type == "instances") {
            QString ID = json.object().value("ID").toString();
            ui -> textEdit -> append(ID);

            //              QNetworkRequest request(url+"/"+ID+"/simplified-tags");
            //              request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
            //              this->m_manager.get(request);
            ui -> progressBar -> setValue(ui -> progressBar -> value() + progressValue);
        } else if (type == "simplified-tags") {
            //generate imaging study
            getMetadata(json);
            uploadedFilesCount++;

            if (uploadedFilesCount == fileCount) {
                //create ImagingStudy & Patient JSON
                QMap < QString, ImagingStudy > ::iterator i;
                for (i = studies.begin(); i != studies.end(); i++) {
                    QByteArray json = i.value().createImagingStudyJSON();
                    qDebug() << json << "\n\n";
                    uploadToFHIR("ImagingStudy", "", json);

                    json = i.value().createPatientJSON();
                    qDebug() << json << "\n\n";
                    uploadToFHIR("Patient", "TCUMI106." + i -> patientID, json);
                }
                ui -> selectBtn -> setEnabled(true);
            }
            ui -> progressBar -> setValue(ui -> progressBar -> value() + progressValue);
        } else {
            QString id = json.object().value("id").toString();
            ui->istudyEdit->setText(url+id);
            ui -> progressBar -> setValue(100);
        }

    }
    ui->selectBtn->setEnabled(true);
}

void MainWindow::on_selectBtn_clicked()
{
    QString jsonTemp;
    jsonTemp = "map.json"; //與 dd.txt 一樣，需存放在程式編譯結果目錄 (exe的上一層)
    dicomJSONTemp myDICOMJson;
    if (myDICOMJson.loadFile(jsonTemp) == true) {
        qDebug() << myDICOMJson.getJSONData("0002", "0001", "type");
    } else {
        qDebug() << "load DICOM JSON error";
    }

    // 基於指定的目錄，針對每一個 DICOM，迴圈重複跑以下程式。以產生匿名 DICOM 檔及 imagingStudies
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(tr("DICOM files (*.dcm)"));

    if (dialog.exec())
        fileNames = dialog.selectedFiles();
    if (fileNames.length() > 0) {
        ui -> uploadOrthancBtn -> setEnabled(true);

        fileCount = fileNames.length();

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
        ui -> textEdit -> append(dirPath);
        //END: get destination folder path

        //START: create ImagingStudy
        iSty.patientID ="2171";
        iSty.patientName ="anonymous";
        iSty.endpoint ="FaithOrthanc";
        //END: create ImagingStudy

        for (int i=0; i<fileCount; i++) {
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

            //char* srcDCM = (filename+".dcm").toUtf8().data();  //"D:\\ttt\\IM-0001-0070.DCM";
            //char* retXML = (filename+".xml").toUtf8().data();  //"D:\\ttt\\IM-0001-0070.xml";
            char tempPart10Elements[] = "D:\\ttt\\temp10EUS2.dcm";

            // retDCM 為匿名的 DICOM 檔，或可放在目前目錄的子目錄，如 D:\\ttt\\did\mo.dcm
            //char* retDCM = (filename+"-anon.dcm").toUtf8().data();   //"D:\\ttt\\IM-0001-0070-anon.dcm";

            DICOMDataObject DDO;
            DDO.jsonTemp = & myDICOMJson;
            DDO.srcDCM = srcDCM;
            DDO.retXML = retXML;
            DDO.tempPart10Elements = tempPart10Elements;
            DDO.retDCM = retDCM;

            DDO.ReadDICOMPart10File(); //decode header
            DDO.DecodeRetToXML(retXML);

            //START: anonymize
            DDO.SetJSONElement();
            DDO.SaveDICOM();    //save dicom and create a new DDO for
            //END: anonymize

            DICOMDataObject anonDDO;
            anonDDO.srcDCM = retDCM;
            anonDDO.ReadDICOMPart10File();

            anonDDO.SetUsefulElements();
            iSty.addToStudy(&(anonDDO.ufElements), dirPath);
        }
        ui -> textEdit_result -> append(iSty.json);
        ui -> textEdit_result -> append("\n\n");
    }
    ui -> progressBar -> reset();
}


void MainWindow::on_uploadOrthancBtn_clicked()
{
    ui -> selectBtn -> setEnabled(false);
    ui -> uploadOrthancBtn -> setEnabled(false);
    ui -> uploadFHIRBtn -> setEnabled(false);

    foreach(QString fileName, fileNames) {
        QFile f(fileName);
        if (f.open(QFile::ReadOnly)) {
            QByteArray data = f.readAll();

            qDebug("Upload to DICOM server start\n");

            QUrl url(ui -> dicomEdit -> text() + "/instances");

            QNetworkRequest request(url);
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/dicom");
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

    //START: upload json (from left textEdit) to fhir server
   /* QByteArray data = ui->textEdit->toPlainText().toUtf8();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj;

        //check doc
    if(!doc.isNull()) {
        obj = doc.object();
        QString resourceType = obj.value("resourceType").toString();
        QString id = obj.value("id").toString();    //return "" if no id inside JSON
        uploadToFHIR(resourceType, id, data);
    }*/
    //END: upload json (from left textEdit) to fhir server

    uploadToFHIR("ImagingStudy", "", iSty.json);
}

void MainWindow::uploadToFHIR(QString resourceType, QString id, const QByteArray data)
{

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QUrl url(ui -> fhirEdit -> text() + "/" + resourceType + "/" + id);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json+fhir");
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






