//#include "dicomjsontemp.h"
# include	"dicomheader.h"
dicomJSONTemp::dicomJSONTemp()
{

}
bool  dicomJSONTemp::loadFile(QString fileName)
{   QFile jsonFile(fileName);
    jsonFile.open(QFile::ReadOnly);
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonFile.readAll(), &error);
    if (error.error == QJsonParseError::NoError)
    { if (jsonDocument.isArray())
        {rootJsonAarray = jsonDocument.array();
        return true;
        }
        else
          { qDebug() << "loaded file root is not a JSON array";
            return false;
           }
    }
    else
    { qDebug() << error.errorString();
      return false;
     }
    //需要呼叫 QFile.close ?
}

QString dicomJSONTemp::getJSONData(QString g, QString e, QString valueName)
{
    int i, size;
    QString g1,e1;
    size = rootJsonAarray.size();
    QJsonValue jValue;
    for(i=0; i<size ;i++)
       {g1 =rootJsonAarray.at(i).toObject().value("g").toString();
        e1 =rootJsonAarray.at(i).toObject().value("e").toString();

        if(g1 == g && e1 == e)
            return rootJsonAarray.at(i).toObject().value(valueName).toString();
        }
    return "";
}
