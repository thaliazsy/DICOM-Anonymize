using namespace std;
#include <vector>
#include <string.h>

#include "dicomjsontemp.h"

struct S {
  char strData[130];
};



struct   DataElement
    {
    public:
        int			ParentDICOMObjectID;
        int			ElementID;
        bool IsBeginOfObject;
        bool IsEndOfObject;
        vector <int> SQEndings;
        //unsigned short EdgeCondiction;   // 0: normal, 1:first of SQ and Object,  2:first of Object, 3: end of object, 4: end of SQ and object
        unsigned short			Group;
        unsigned short		Element;
        unsigned int	Length;
        char  VRType[3];
        char	VM[5];
        short shortVM;
        char  	Description[128];
        unsigned  int  Offset;
        void		*Data;
        vector <S> StrDataV;
        //char  strData[256];
    //	DataElement * PreviousElement, *NextElement;
    };

struct   usefulElements
    {
    //
    QString patientID;
    QString patientName;
    QString studyUID;
    QString seriesUID;
    QString seriesNumber;
    QString modality;
    QString numberOfInstances;
    QString instanceUID;
    QString sopClassUID;
    QString instanceNumber;

    /*
      identifier	StudyInstanceUID (0020,000D) | Accession Number and Issuer (0080,0050)+(0080,0051) | Study ID (0020,0010)
      series
            uid	(0020,000E)
            number	(0020,0011)
            modality	(0008,0060)
            description	(0008,103E)
            numberOfInstances	(0020,1209)

            instance
                uid	(0008,0018)
                sopClass	(0008,0016)
                number	(0020,0013)


    */

    };

struct	DICOMObjectIndex
    { 	public:
        bool isKnowLength;
        bool IsBeginOfSequence;
        bool IsEndOfSeqence;
        //unsigned short EdgeCondiction;   // 0: normal, 1:first of SQ and Object,  2:first of Object, 3: end of object, 4: end of SQ and object
        int  ItemNumber;
        int	 ParentDataElementID;
        int	 DICOMObjectID;
        unsigned  int     Length;
        unsigned  int Offset;
    };


class	DICOMDataObject
{
public:
unsigned int DICOMObjectCount;
vector<DataElement> AllElement;
vector<DICOMObjectIndex> AllObject;
usefulElements ufElements;
char	DICOMFileName[512];
bool IsExplicitVR;
bool IsLittleEanian;
unsigned int EndOfP10HeaderOffset;
//unsigned int		Offset;
FILE	*fp;
FILE	*Retfp;
unsigned int		fileSize;
QJsonObject FoundObj;
unsigned int elIdx;
bool FindJSONElement(QString g, QString e); //set found obj to FoundObj
QMap<QString, QString> SetJSONElement(QMap<QString, QString> uidMap); //QString g, QString e);
void SetJSONElement();
void SetUsefulElements();
DICOMDataObject();
dicomJSONTemp * jsonTemp;
char * srcDCM;
char * retXML;
char * tempPart10Elements;
char * retDCM;

//int ReadDICOMPart10File(char * FileName);
int ReadDICOMPart10File();
int ReadDICOMFileObject(char * FileName, unsigned int DDOOffset, bool inIsExplicitVR,bool inIsLittleEanian);
bool Deanonymization(char * FileName);
//bool SaveDICOM(char * FileName);
bool SaveDICOM();
bool DecodeRetToXML(char * FileName);
bool DecodeSRToSVG(char * FileName);

~DICOMDataObject(); //�� release  DataStream �� Menory
//bool  DataToString(void * Data,int DataLength, char * VRType, char * ReturnData);
bool  DataToString(DataElement * DataElementP);
//Initial read DICOM file functions
int DecodeDICOMObject(DICOMObjectIndex MyObjectIndex);
//bool DecodeDICOMObject(unsigned int ParentDataElementID, unsigned int DDOOffset);
int ParseSQElement(DataElement ParentElement);
bool SetElementProperities( DataElement *CurElement);




DataElement * GetElementData(unsigned short g, unsigned short e);
bool SetElementValue(unsigned short g, unsigned short e, unsigned int Length, void * Data);
bool SetElementValue(unsigned short g, unsigned short e, string Data);
void  SavePart10Header(FILE	*Retfp);
void  SaveDataElement(FILE	*Retfp, unsigned short g, unsigned short e, char * vr, unsigned int Length, void * Data );
void  SaveDataElement(FILE	*Retfp,  DataElement *CurElement);




char  CheckDeanonymizationType(DataElement * DataElementP);
void  KeepDataElement(FILE	*Retfp,  DataElement *CurElement);
bool  ReplaceZeroLength(FILE	*Retfp,  DataElement *CurElement);
bool  ReplaceWithString(FILE	*Retfp,  DataElement *CurElement, unsigned short strLength, char * strData);
};

/*

bool  KeepDataElement(char * FileName,  DataElement *CurElement);
bool  ReplaceZeroLength(char * FileName,  DataElement *CurElement);
bool  ReplaceWithString(char * FileName,  DataElement *CurElement, unsigned short strLength, char * strData);

KeepDataElement
K - keep (unchanged for non-sequence attributes, cleaned for sequences)

ReplaceWithString
D - replace with a non-zero length value that may be a dummy value and consistent with the VR
C - clean, that is replace with values of similar meaning known not to contain identifying information and consistent with the VR

ReplaceZeroLength
Z - replace with a zero length value, or a non-zero length value that may be a dummy value and consistent with the VR

do not save the element
X - remove

 */
