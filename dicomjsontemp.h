#ifndef DICOMJSONTEMP_H
#define DICOMJSONTEMP_H
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include <QDir>


class dicomJSONTemp
{
public:
    dicomJSONTemp();
    QJsonDocument jsonDocument;
    QJsonObject rootJson_obj;  //=jsonDocument.object();
    QJsonArray rootJsonAarray; // =jsonDocument.array();
    bool loadFile(QString fileName);
    QString getJSONData(QString g, QString e, QString valueName);

};

#endif // DICOMJSONTEMP_H
