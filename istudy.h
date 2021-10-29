#ifndef ISTUDY_H
#define ISTUDY_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "dicomheader.h"   //20210612 add by Jack Hsiao

class iStudy
{
public:
    iStudy();
    QString addToStudy(usefulElements * uElements, QString retJSONpath); //20210612 add by Jack Hsiao
    QJsonObject addNewInstance(usefulElements * uElements);
    QByteArray createImagingStudyJSON();
    QString createEndpointJSON(QString retJSONpath);

    QJsonObject obj;

    QJsonArray identifier;
    QJsonObject idenObj;
    QJsonObject subject;
    QJsonArray endpointArr;
    QJsonObject addressObj;

    QJsonArray seriesArr;
    QJsonObject seriesObj;

    QJsonArray instanceArr;

    QJsonObject instanceObj;
    QJsonObject sopClass;

    QString endpoint;
    //QString status;
    QString patientID;
    QString patientName;
    QString studyUID;

    /*
      "status": "available",
  "subject": {
    "reference": "Patient/dicom"
  },
     */
    /*
    QString studyUID;
    QString patientID;
    QString patientName;
    QString started;
    int numberOfSeries;
    int numberOfInstances; */
  //  QList<Series> series;
    QByteArray json;
    QByteArray patientJSON;
};

#endif // ISTUDY_H
