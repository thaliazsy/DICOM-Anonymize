#ifndef IMAGINGSTUDY_H
#define IMAGINGSTUDY_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "dicomheader.h"   //20210612 add by Jack Hsiao
class ImagingStudy
{

public:
    ImagingStudy();

    struct Instances{
        QString instanceUID;
        QString SOPClassUID;
        QString instanceNumber;
        QString filePath;
    };

    struct Series {
        QString seriesUID;
        QString seriesNumber;
        QString modality;
        QString bodyPart;
        int numberofInstance;
        QList<Instances> instances;
    };

    QString endpoint;
    QString studyUID;
    QString patientID;
    QString patientName;
    QString started;
    int numberOfSeries;
    int numberOfInstances;
    QList<Series> series;
    QByteArray json;
    QByteArray patientJSON;


    void createStudy(QMap<QString, QString> metadata);
    void appendToStudy(QMap<QString, QString> metadata);
    QByteArray createImagingStudyJSON();
    QByteArray createPatientJSON();
};

#endif // IMAGINGSTUDY_H
