#include "istudy.h"

iStudy::iStudy()
{
}

QString iStudy::addToStudy(usefulElements * uElements, QString retJSONpath)
{
    //START: create series
    int seriesExist = -1;

    for(int i=0; i<seriesArr.count(); i++){
        QJsonObject o = seriesArr.at(i).toObject();
        if(uElements->seriesUID == o.value("uid").toString()){
            seriesExist=i;
            break;
        }
    }
    if(seriesExist!=-1){
        printf("exists");
        QJsonObject o = seriesArr.at(seriesExist).toObject();
        QJsonArray arr = o.value("instance").toArray();
        instanceObj = addNewInstance(uElements);
        arr.append(instanceObj);
        o.insert("instance", arr);

        seriesArr.removeAt(seriesExist);
        seriesArr.append(o);
    }
    else {
        //create new series
        seriesObj = QJsonObject();
        seriesObj.insert("uid", uElements->seriesUID);
        seriesObj.insert("number", uElements->seriesNumber.toInt());
        seriesObj.insert("modality", uElements->modality);
        seriesObj.insert("numberOfInstances", uElements->numberOfInstances);
        //seriesObj.insert("endpoint")
        //seriesObj.insert("bodySite", uElements);

        //START: create new instance
        instanceArr = QJsonArray();
        instanceObj = addNewInstance(uElements);
        instanceArr.append(instanceObj);
        seriesObj.insert("instance", instanceArr);  //insert instance array to series
        //END: create new instance

        seriesArr.append(seriesObj);

    }
    obj.insert("series", seriesArr);
    //END: create series


    if(identifier.isEmpty()) {
        idenObj.insert("system", "urn:dicom:uid");
        idenObj.insert("value", "urn:oid:" + uElements->studyUID);
        identifier.append(idenObj);
        QJsonObject subject;
        subject.insert("reference", "Patient/" + patientID);
        studyUID=uElements->studyUID;

        QJsonArray endpointArr;
        QJsonObject addressObj;
        addressObj.insert("reference", "Endpoint/" + uElements->studyUID);
        endpointArr.append(addressObj);
        obj.insert("endpoint", endpointArr);

        obj.insert("subject", subject);
        obj.insert("identifier", identifier);
        obj.insert("status", "available");
        obj.insert("resourceType", "ImagingStudy");
        obj.insert("id", uElements->studyUID);
    }

    QJsonDocument doc(obj);
    json=doc.toJson();

    QString returnFile = retJSONpath + "/ImagingStudy-"+uElements->studyUID+".json";

    QFile tempJSON(returnFile);
    if(tempJSON.open(QIODevice::WriteOnly))
    {
        tempJSON.write(json);
        tempJSON.close();
    }

    return returnFile;


  /*   int i,TotalElement;
     QString g ,e;
     char  charG[5], charE[5]; //, strData[128];
     TotalElement = AllElement->size();
     idenObj.insert("system", "urn:dicom:uid");
     idenObj.insert("value", "urn:oid:" + studyUID);
     identifier.append(idenObj);


     subject.insert("reference", "Patient/TCUMI106." + patientID);

     addressObj.insert("reference", "Endpoint/TCUMI106.Orthanc");
     endpointArr.append(addressObj);


     for(i=0;i< TotalElement;i ++)
     {   sprintf(charG, "%04X", (* AllElement)[i].Group);
         sprintf(charE, "%04X",(* AllElement)[i].Element);
         g = QString(charG);
         e = QString(charE);
         if(FindJSONElement(g, e) == true  )
             { if (FoundObj.value("type").toString()=="Keep" )
                  FoundObj["value"] = QString( (char *) AllElement[i].Data);
             }
      }
  */
}

QJsonObject iStudy::addNewInstance(usefulElements * uElements)
{
    QJsonObject obj;
    obj.insert("uid", uElements->instanceUID);
    obj.insert("sopClass", uElements->sopClassUID);
    obj.insert("number", uElements->instanceNumber.toInt());
    return obj;
}

QString iStudy::createEndpointJSON(QString retJSONpath)
{
    QJsonObject obj;

    obj.insert("resourceType", "Endpoint");
    obj.insert("id", studyUID);

    QJsonArray payloadArr;
    QJsonObject textObj;
    textObj.insert("text", "DICOM");
    payloadArr.append(textObj);
    obj.insert("payloadType", payloadArr);

    QJsonArray payloadMimeArr;
    payloadMimeArr.append("application/dicom");
    obj.insert("payloadMimeType", payloadMimeArr);

    obj.insert("address", endpoint+"/dicom-web/studies/"+ studyUID);


    QJsonDocument doc(obj);
    json=doc.toJson();

    QString returnFile = retJSONpath + "/Endpoint-"+ studyUID+".json";

    QFile tempJSON(returnFile);
    if(tempJSON.open(QIODevice::WriteOnly))
    {
        tempJSON.write(json);
        tempJSON.close();
    }
    return returnFile;
}

QByteArray iStudy::createImagingStudyJSON()
{
/*

    foreach(const Series s, series){

        foreach(const Instances i, s.instances) {

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
*/
    return json;
}
