//#include "dicomjsontemp.h"
# include	"dicomheader.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include <QEventLoop>

#include "dicomheader.h"   //20210612 add by Jack Hsiao
//20210612 add by Jack Hsiao
void ImagingStudy::addToStudy(vector<DataElement> * AllElement)
{

}

ImagingStudy::ImagingStudy()
{
    numberOfSeries=0;
    numberOfInstances=0;
}

void ImagingStudy::createStudy(QMap<QString, QString> metadata) {
    studyUID=metadata.value("StudyInstanceUID");
    patientID=metadata.value("PatientID");
    patientName=metadata.value("PatientName");
    started=metadata.value("StudyDate") + "T" + metadata.value("StudyTime");

    Series s;
    s.bodyPart=metadata.value("BodyPartExamined");
    s.modality=metadata.value("Modality");
    s.seriesUID=metadata.value("SeriesInstanceUID");
    s.seriesNumber=metadata.value("SeriesNumber");
    s.numberofInstance=0;

    Instances i;
    i.SOPClassUID=metadata.value("SOPClassUID");
    i.instanceUID=metadata.value("SOPInstanceUID");
    i.instanceNumber=metadata.value("InstanceNumber");
    i.filePath=metadata.value("filepath");

    s.instances.append(i);
    numberOfInstances++;
    s.numberofInstance++;

    series.append(s);
    numberOfSeries++;

}

void ImagingStudy::appendToStudy(QMap<QString, QString> metadata) {

    QList<Series>::iterator iterS;
    for(iterS=series.begin(); iterS!= series.end(); iterS++){
        qDebug("msuk iters");
        qDebug(iterS->seriesUID.toUtf8());
        qDebug(metadata.value("SeriesInstanceUID").toUtf8());
        if(iterS->seriesUID==metadata.value("SeriesInstanceUID")){
            qDebug("series yg sama");
            QList<Instances>::iterator iterI;
            for(iterI=iterS->instances.begin(); iterI!= iterS->instances.end(); iterI++){
                if(iterI->instanceUID==metadata.value("SOPInstanceUID")) {
                    qDebug("instance yg sama");
                    break;
                }
            }
            if(iterI==iterS->instances.end()){
                qDebug("instance yg beda, series yg sama");
                Instances i;
                i.SOPClassUID=metadata.value("SOPClassUID");
                i.instanceUID=metadata.value("SOPInstanceUID");
                i.instanceNumber=metadata.value("InstanceNumber");
                i.filePath=metadata.value("filepath");

                iterS->instances.append(i);
                numberOfInstances++;
                iterS->numberofInstance++;
            }
        }
    }
    if(iterS==series.end()){
        qDebug("bkin series & instance baru");
        Series s;
        s.bodyPart=metadata.value("BodyPartExamined");
        s.modality=metadata.value("Modality");
        s.seriesUID=metadata.value("SeriesInstanceUID");
        s.seriesNumber=metadata.value("SeriesNumber");

        Instances i;
        i.SOPClassUID=metadata.value("SOPClassUID");
        i.instanceUID=metadata.value("SOPInstanceUID");
        i.instanceNumber=metadata.value("InstanceNumber");
        i.filePath=metadata.value("filepath");

        s.instances.append(i);
        numberOfInstances++;
        s.numberofInstance++;

        series.append(s);
        numberOfSeries++;
    }
}

QByteArray ImagingStudy::createImagingStudyJSON(){
    QJsonObject obj;

    QJsonArray identifier;
    QJsonObject idenObj;
    idenObj.insert("system", "urn:dicom:uid");
    idenObj.insert("value", "urn:oid:" + studyUID);
    identifier.append(idenObj);

    QJsonObject subject;
    subject.insert("reference", "Patient/TCUMI106." + patientID);

    QJsonArray endpointArr;
    QJsonObject addressObj;
    addressObj.insert("reference", "Endpoint/TCUMI106.Orthanc");
    endpointArr.append(addressObj);

    QJsonArray seriesArr;

    foreach(const Series s, series){
        QJsonObject seriesObj;

        QJsonArray instanceArr;

        foreach(const Instances i, s.instances) {
            QJsonObject instanceObj;

            QJsonObject sopClass;
            sopClass.insert("system", "urn:ietf:rfc:3986");
            sopClass.insert("value", "urn:oid:" + i.SOPClassUID);

            instanceObj.insert("uid", i.instanceUID);
            instanceObj.insert("sopClass", sopClass);
            instanceObj.insert("number", i.instanceNumber);

            instanceArr.append(instanceObj);
        }

        if(s.modality.trimmed()!=""){
            QJsonObject modality;
            modality.insert("system", "http://dicom.nema.org/resources/ontology/DCM");
            modality.insert("code", s.modality);
            seriesObj.insert("modality", modality); //object
        }

        if(s.bodyPart.trimmed()!=""){
            QJsonObject bodySite;
            bodySite.insert("display", s.bodyPart);
            seriesObj.insert("bodySite", bodySite); //object
        }


        seriesObj.insert("uid", s.seriesUID);
        seriesObj.insert("number", s.seriesNumber);
        seriesObj.insert("numberofInstances", s.numberofInstance);
        seriesObj.insert("instance", instanceArr);  //arr

        seriesArr.append(seriesObj);
    }

    obj.insert("resourceType", "ImagingStudy");
    obj.insert("identifier", identifier);
    obj.insert("status", "available");
    obj.insert("subject", subject);
    obj.insert("started", started);
    obj.insert("endpoint", endpointArr);
    obj.insert("numberOfSeries", numberOfSeries);
    obj.insert("numberOfInstances", numberOfInstances);
    obj.insert("series", seriesArr);


  //  obj.insert("id", 876);

    QJsonDocument doc(obj);
    json=doc.toJson();


    QFile tempJSON("D:/DICOM/ImagingStudy-"+studyUID+".json");
    if(tempJSON.open(QIODevice::WriteOnly))
    {
        tempJSON.write(json);
        tempJSON.close();
    }

    return json;
}

QByteArray ImagingStudy::createPatientJSON(){
    QJsonObject obj;

    QJsonArray identifier;
    QJsonObject idenObj;
    idenObj.insert("use", "usual");
    idenObj.insert("system", "Identifier-ID");
    idenObj.insert("value", patientID);
    identifier.append(idenObj);

    QJsonArray name;
    QJsonObject nameObj;
    nameObj.insert("text", patientName);
    name.append(nameObj);

    QJsonObject org;
    org.insert("reference", "Organization/TCUMI106");

    QJsonArray seriesArr;

    obj.insert("resourceType", "Patient");
    obj.insert("id", "TCUMI106." + patientID);
    obj.insert("identifier", identifier);
    obj.insert("name", name);
    obj.insert("managingOrganization", org);

    QJsonDocument doc(obj);
    patientJSON=doc.toJson();

    QFile tempPatient("D:/DICOM/Patient-"+patientID+".json");

    if(tempPatient.open(QIODevice::WriteOnly))
    {
        tempPatient.write(patientJSON);
        tempPatient.close();
    }

    return patientJSON;
}
