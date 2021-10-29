# include	"dicomheader.h"



DataElement * DICOMDataObject	::	GetElementData(unsigned short g, unsigned short e)
{int i,TotalElement;
 TotalElement = AllElement.size();
 for(i=0;i< TotalElement;i ++)
       { if(AllElement[i].Group==g && AllElement[i].Element == e )
            return &AllElement[i];
     }
   return NULL;
}

bool DICOMDataObject::SetElementValue(unsigned short g, unsigned short e, string strData)
{   unsigned int Length;
    void * Data;

    Length = strData.length();
    Data = (void *) strData.c_str();
    return SetElementValue( g,  e,  Length, Data);

}

bool DICOMDataObject::SetElementValue(unsigned short g, unsigned short e, unsigned int Length, void * Data)
{
    int i,TotalElement;
     TotalElement = AllElement.size();
     for(i=0;i< TotalElement;i ++)
           {
            if(AllElement[i].Group==g && AllElement[i].Element == e )
               {free (AllElement[i].Data);
                //     AllElement[i].StrDataV 未設定
                AllElement[i].Length = Length;
                AllElement[i].Data = malloc(Length);
                memcpy(AllElement[i].Data, Data, Length);
                return true;
               }
           }
   return false;
}
DICOMDataObject	::	DICOMDataObject()
{


}

DICOMDataObject	::	~DICOMDataObject()
{
 //   if(fp) fclose(fp);
}

/*
DICOM File Format  : http://dicom.nema.org/dicom/2013/output/chtml/part10/chapter_7.html
This header consists of a 128 byte File Preamble, followed by a 4 byte DICOM prefix.
Except for the 128 byte preamble and the 4 byte prefix, the File Meta Information shall be encoded using the Explicit VR Little Endian Transfer Syntax (UID=1.2.840.10008.1.2.1)
*/


bool DICOMDataObject:: DataToString(DataElement * DataElementP)
     {
        unsigned int	DataLength;
        char  VRType[3];
       // char	VM[5];
        short shortVM;
        char * ReturnData;
       // char  	Description[128];
        //unsigned  int  Offset;
        void		*Data, * revData;
        //vector <S> StrData;
        DataLength =  DataElementP->Length;
        shortVM = DataElementP->shortVM;
        unsigned int i;
        for(i=0;i<3;i++)
         VRType[i]=  DataElementP->VRType[i];
        S SstrData;
        ReturnData = SstrData.strData;
       if(DataLength ==0)   { // DataElementP->shortVM =0;
                         sprintf(ReturnData,"");
                          DataElementP->StrDataV.push_back(SstrData);
                         return true;      }

     unsigned int UL;
     int SL;
     unsigned short US;
     short SS;

     float FL;
     double FD;
     // Reference:  ftp://dicom.nema.org/medical/DICOM/2013/output/chtml/part05/sect_6.2.html
     /*"AT": Ordered pai r of 16-bit unsigned integers that is the value of a Data Element Tag.
        Example: A Data Element Tag of (0018,00FF) would be encoded as a series of 4 bytes in a Little-Endian Transfer Syntax as 18H,00H,FFH,00H and in a Big-Endian Transfer Syntax as 00H,18H,00H,FFH.
        Should not presented in DICOM data type? And should be reviced when we have time.
        */
     for(i=0;i< shortVM;i++) {

    /*     double ReverseDouble(double inDouble );
         float ReverseFloat(float inFloat );
         unsigned short ReverseUnsignedShort(unsigned short inShort );
         unsigned int ReverseUnsignedInt(unsigned int inInt );
         int ReverseInt(int inInt );
         short ReverseShort( short inShort );
         */


     if(strcmp(VRType, "AT") == 0 )
          { Data =(void*)((char*) DataElementP->Data + i*4);
           UL=(*((unsigned int *) Data));

           if(IsLittleEanian==false)
              {UL = ReverseUnsignedInt(UL);
               revData =(void*)(&UL);
               memcpy( Data, revData , 4);
               }
           sprintf(ReturnData,"%d",UL);
           }
     else  if(strcmp(VRType, "UL") == 0 )
          { Data =(void*)((char*) DataElementP->Data + i*4);
              UL=(*((unsigned int *) Data));
           if(IsLittleEanian == false)
              {UL = ReverseUnsignedInt(UL );
               revData =(void*)(&UL);
               memcpy( Data, revData , 4);
               }
           sprintf(ReturnData,"%d",UL);
           }
     else if(strcmp(VRType, "SL") == 0)
          { Data =(void*)((char*) DataElementP->Data + i*4);
           SL=(*((int *) Data));
           if(IsLittleEanian == false)
               {SL = ReverseInt(SL );
               revData =(void*)(&SL);
               memcpy( Data, revData , 4);
               }
           sprintf(ReturnData,"%d",SL);
           }
     else if(strcmp(VRType, "US") == 0)
          { Data =(void*)((char*) DataElementP->Data + i*2);
            US=(*(( unsigned short*) Data));
           if(IsLittleEanian == false)
               {US = ReverseUnsignedShort(US );
               revData =(void*)(&US);
               memcpy( Data, revData , 2);
               }
           sprintf(ReturnData,"%d",US);
          }
     else  if(strcmp(VRType, "SS") == 0)
          { Data =(void*)((char*) DataElementP->Data + i*2);
           SS=(*((short *) Data));
           if(IsLittleEanian == false)
               {SS = ReverseUnsignedInt(SS );
               revData =(void*)(&SS);
               memcpy( Data, revData , 2);
               }
           sprintf(ReturnData,"%d",SS);
           }
     else  if(strcmp(VRType, "FL") == 0)
          { Data =(void*)((char*) DataElementP->Data + i*4);
           FL=(*((float *) Data));
           if(IsLittleEanian == false)
                 {FL = ReverseFloat(FL );
                  revData =(void*)(&FL);
                  memcpy( Data, revData , 4);
                  }
          sprintf(ReturnData,"%f",FL);
           }
     else   if(strcmp(VRType, "FD") == 0)
          { Data =(void*)((char*) DataElementP->Data + i*8);
           FD=(*((double *) Data));
           if(IsLittleEanian == false)
                 {FD = ReverseDouble(FD );
                 revData =(void*)(&FL);
                 memcpy( Data, revData , 8);
                 }
           sprintf(ReturnData,"%f",FD);

           }
     else if(strcmp(VRType, "OB") == 0 ||strcmp(VRType, "OW") == 0 ||strcmp(VRType, "SQ") == 0)
          {sprintf(ReturnData,"");
             }

     else {
          Data = DataElementP->Data ;
          char *strData;
          strData =(char *) Data;
          int i, maxLength;
          maxLength = DataLength;
          if(maxLength >128) maxLength =128;
          for(i=0; i< maxLength;i++)
             { if(strData[i] >= 32 && strData[i] <127) // displayabe ACSII code
                  ReturnData[i]= strData[i];
              else  ReturnData[i]= 0;
             }
           ReturnData[maxLength] = 0;
         }
     DataElementP->StrDataV.push_back(SstrData);
     }
      return true;
     }


/*
bool DICOMDataObject:: DataToString(void * Data,int DataLength, char * VRType, char * ReturnData)
     {

     if(DataLength ==0)   { sprintf(ReturnData,"");      return true;      }

     unsigned int UL;
     int SL;
     unsigned short US;
     short SS;

     if(strcmp(VRType, "AT") == 0 )
          {UL=(*((unsigned int *) Data));
           sprintf(ReturnData,"%d",UL);
           return true;
           }

     if(strcmp(VRType, "UL") == 0 )
          {UL=(*((unsigned int *) Data));
           if(IsLittleEanian == false)  UL = ReverseUnsignedInt(UL );
           sprintf(ReturnData,"%d",UL);
           return true;
           }
     if(strcmp(VRType, "SL") == 0)
          {SL=(*((int *) Data));
           if(IsLittleEanian == false)  SL = ReverseInt(SL );
           sprintf(ReturnData,"%d",SL);
           return true;
           }

     if(strcmp(VRType, "US") == 0)
          {US=(*(( unsigned short*) Data));
           if(IsLittleEanian == false)  US = ReverseUnsignedShort(US );
           sprintf(ReturnData,"%d",US);
           return true;
           }
      if(strcmp(VRType, "SS") == 0)
          {SS=(*((short *) Data));
           if(IsLittleEanian == false)  SS = ReverseUnsignedInt(SS );
           sprintf(ReturnData,"%d",SS);
           return true;
           }


     float FL;
     double FD;
     if(strcmp(VRType, "FL") == 0)
          {FL=(*((float *) Data));
           if(IsLittleEanian == false)  FL = ReverseFloat(FL );
          sprintf(ReturnData,"%f",FL);
           return true;
           }
     if(strcmp(VRType, "FD") == 0)
          {FD=(*((double *) Data));
           if(IsLittleEanian == false)  FD = ReverseDouble(FD );
           sprintf(ReturnData,"%f",FD);
           return true;
           }

     if(strcmp(VRType, "OB") == 0 ||strcmp(VRType, "OW") == 0 ||strcmp(VRType, "SQ") == 0)
          {sprintf(ReturnData,"");
           return true;
           }


       char *strData;
       strData =(char *) Data;
       int i, maxLength;
       maxLength = DataLength;
       if(maxLength >255) maxLength =255;
       for(i=0; i< maxLength;i++)
       { if(strData[i] >= 32 && strData[i] <127) // displayabe ACSII code
            ReturnData[i]= strData[i];
         else  ReturnData[i]= 0;
       }
       ReturnData[maxLength] = 0;
       return true;

    }
*/


bool DICOMDataObject::	SetElementProperities(DataElement *CurElement)
{ // search dictionary first and assign element data first
     int DicIndex;
     DicIndex = FindDicElement(CurElement->Group, CurElement->Element );
     if( DicIndex  != -1)
        {
        strcpy(CurElement->VRType ,TagDictionary[DicIndex].VRType);
        strcpy(CurElement->VM ,TagDictionary[DicIndex].VM);
        CurElement->shortVM = TagDictionary[DicIndex].shortVM;
        strcpy(CurElement->Description ,TagDictionary[DicIndex].Description);
        return true;
        }
     else
        {
        strcpy(CurElement->VRType ,"UN");
        strcpy(CurElement->VM ,"1");
        CurElement->shortVM = 1;
        strcpy(CurElement->Description ,"Unknown");
        return false;

        }

}

//int DICOMDataObject::	ReadDICOMPart10File(char *FileName)
int DICOMDataObject::	ReadDICOMPart10File()
{
    char			s[256];
    bool decodeRet =true;  //return value
   //	unsigned int		Offset, fileSize;
        unsigned int		Offset;
    //FILE	*fp;
  //  fp = fopen(FileName, "rb");
    fp = fopen(srcDCM, "rb");
    if(!fp)
        return false;

    fseek (fp, 0, SEEK_END);
    fileSize =ftell (fp);
    fileSize = fileSize -8;  // reserved file size is too small for a data element

    fseek(fp, 128, SEEK_SET);
    fread(s, 1, 4, fp);
    s[4] = '\0';
    if(strcmp(s, "DICM"))  // not equal
        {
        Offset = 0;
        //fseek(fp, 0, SEEK_SET);
        return false;
        }
    else  {		Offset = 128+4; }
    //EXPLICIT_LITTLE_ENDIAN;

    fseek(fp, Offset, SEEK_SET);


    int MaxOffset;
    MaxOffset = fileSize;
   //return DecodeDICOMObject(CurObjectIndex);
   while ( Offset < MaxOffset )
    {
    DataElement CurElement;
    CurElement.IsBeginOfObject =false;
    CurElement.IsEndOfObject = false;



    CurElement.ParentDICOMObjectID = 0; // Root    // AllElement.size();
    unsigned short tempShortInt;
    unsigned int Length;
    // construct point parameters for read data into  data element's proprieteries
    void *Ptr, * GPtr, *EPtr, *VRPtr;
    char  * VRType;
    GPtr = (void *) (&CurElement.Group);
    EPtr = (void *) (&CurElement.Element);
    VRPtr =(void *) CurElement.VRType;
    VRType = CurElement.VRType;  // set VRType point


     // For little ending only,
     fread (GPtr, 2, 1, fp);
     if(CurElement.Group >2) { EndOfP10HeaderOffset = Offset; //fseek(fp, Offset, SEEK_SET);  //not part 10 meta header group, should decode DICOM object by other function
                   break;	  }
     //else for Explict little endian only
     fread (EPtr, 2, 1, fp);

     SetElementProperities(&CurElement);

     Offset =  Offset + 4;


     fread (VRPtr, 2, 1, fp);  // Read VRType
     VRType[2] =0;

     Offset =  Offset + 2;

     if(strcmp(VRType, "OB") == 0 || strcmp(VRType, "OW") == 0 || strcmp(VRType, "OF") ==0|| strcmp(VRType, "SQ")==0  || strcmp(VRType, "UT")==0 || strcmp(VRType, "UN")==0)
            { Ptr = (void *) (& Length);
              fread (Ptr, 2, 1, fp);// Toss away 16 bits
              fread (Ptr, 4, 1, fp);
              Offset =  Offset + 6;
            }
         else
            { Ptr = (void *) (&tempShortInt);
              fread (Ptr, 2, 1, fp);
              Length = tempShortInt;
              Offset =  Offset + 2;
            }
     CurElement.Offset = Offset;
     CurElement.Length =Length;


     void  *Data;
    // char strData[256];

     Data = malloc(Length); //(void*) new[Length];
     fread (Data, Length, 1, fp);
     CurElement.Data =Data;
     Offset = Offset + Length;
     CurElement.ElementID = AllElement.size();
     DataToString(&CurElement);
     //DataToString(CurElement.Data,CurElement.Length,CurElement.VRType, CurElement.strData);
     AllElement.push_back(CurElement);
    // AllElement.push_back(CurElement);
    // char a[3]
    // char * strData;
    // if(CurElement.StrDataV.size()>0)
    //	 strData =CurElement.StrDataV[0].strData;

     printf("G = %04X E = %04X  %s VR = %s  Length = %d Offset= %d strData= %s \n", CurElement.Group,  CurElement.Element,CurElement.Description, CurElement.VRType, CurElement.Length, CurElement.Offset, CurElement.StrDataV[0].strData);
  //   printf("Next offset %d  ",Offset);
     /*
   Transfer Syntax UID	Transfer Syntax name
    1.2.840.10008.1.2	Implicit VR Endian: Default Transfer Syntax for DICOM
    1.2.840.10008.1.2.1	Explicit VR Little Endian
    1.2.840.10008.1.2.2	Explicit VR Big Endian
    others may not supported yet: http://www.dicomlibrary.com/dicom/transfer-syntax/

    */
     if(CurElement.Group ==  0x0002 && CurElement.Element == 0x0010)
     { if(strcmp(CurElement.StrDataV[0].strData, "1.2.840.10008.1.2") ==0)
          {
          IsExplicitVR = false;
          IsLittleEanian= true;
          }
       if(strcmp(CurElement.StrDataV[0].strData, "1.2.840.10008.1.2.1") ==0)
          {
          IsExplicitVR = true;
          IsLittleEanian= true;
          }
       if(strcmp(CurElement.StrDataV[0].strData, "1.2.840.10008.1.2.2") ==0)
          {
          IsExplicitVR = true;
          IsLittleEanian= false;
          }
     }

    }

    //fclose(fp);
    DICOMObjectIndex	CurObjectIndex;
    CurObjectIndex.IsBeginOfSequence =false;
    CurObjectIndex.IsEndOfSeqence = false;

    CurObjectIndex.ParentDataElementID =-1;
    CurObjectIndex.isKnowLength = false;   //Unknow object length
    CurObjectIndex.Length = -1;
    CurObjectIndex.DICOMObjectID =0;
    CurObjectIndex.Offset =  Offset;
    AllObject.push_back(CurObjectIndex);
    CurObjectIndex.ItemNumber = -1;  //not item number
    return DecodeDICOMObject(CurObjectIndex);


}
//bool  DICOMDataBuffer::ReadDICOMFileObject(unsigned int ParentDataElementID,unsigned int DDOOffset, bool inIsExplicitVR,bool inIsLittleEanian)
int DICOMDataObject::	ReadDICOMFileObject(char * FileName, unsigned int DDOOffset, bool inIsExplicitVR,bool inIsLittleEanian)
{

IsExplicitVR = inIsExplicitVR;
IsLittleEanian = inIsLittleEanian;
fp = fopen(FileName, "rb");
if(!fp)	 return false;
fseek (fp, 0, SEEK_END);
fileSize =ftell (fp);
fileSize = fileSize - 8;
//Offset = DDOOffset;
DICOMObjectIndex	CurObjectIndex;
CurObjectIndex.IsBeginOfSequence =false;
CurObjectIndex.IsEndOfSeqence = false;
CurObjectIndex.ParentDataElementID =-1;
CurObjectIndex.isKnowLength = false;   //Unknow object length
CurObjectIndex.Length = 0;  //Unknow object length
CurObjectIndex.DICOMObjectID =0;
CurObjectIndex.Offset =  DDOOffset;
CurObjectIndex.ItemNumber = -1;  //not item number
AllObject.push_back(CurObjectIndex);
return DecodeDICOMObject(CurObjectIndex);
//DecodeDICOMObject(unsigned int ParentDataElementID, unsigned int DDOOffset)
//return ReadDICOMObject( ParentDataElementID); //DDOOffset);

}
int  DICOMDataObject::	DecodeDICOMObject(DICOMObjectIndex MyObjectIndex)
    //;DecodeDICOMObject(unsigned int ParentDataElementID, unsigned int DDOOffset) //unsigned int DDOOffset)
{

    char			s[256];
    bool decodeRet = true;
    int MyDDOLength;
    MyDDOLength = -1;  // error
   //	unsigned int		Offset, fileSize;
    //unsigned int		fileSize;
    //FILE	*fp;
    unsigned int		Offset;
    Offset = MyObjectIndex.Offset;
    fseek(fp, Offset, SEEK_SET);

    int ElementCount;
    ElementCount =0;
    int LastElementID;
    int MaxOffset;
    if(MyObjectIndex.isKnowLength)   MaxOffset =MyObjectIndex.Offset +  MyObjectIndex.Length;
    else MaxOffset = fileSize;

    while ( Offset < MaxOffset  )
    {
    DataElement CurElement;
    CurElement.ParentDICOMObjectID = MyObjectIndex.DICOMObjectID ;
    CurElement.IsBeginOfObject =false;
    CurElement.IsEndOfObject =false;


    //unsigned short Group, Element , tempShortInt;
    unsigned short tempShortInt;
    unsigned int Length;
    void *Ptr, * GPtr, *EPtr, *VRPtr;

    GPtr = (void *) (&CurElement.Group);
    EPtr = (void *) (&CurElement.Element);
    VRPtr =(void *) CurElement.VRType;
    char  * VRType;
    VRType = CurElement.VRType;  // set VRType point


     fread (GPtr, 2, 1, fp);
     fread (EPtr, 2, 1, fp);

     if(IsLittleEanian == false)
         { // should reverse G , E first for check dictionary
         CurElement.Group = ReverseUnsignedShort(CurElement.Group  );
         CurElement.Element  = ReverseUnsignedShort(CurElement.Element );
         }
     Offset =  Offset + 4;
    if(CurElement.Group == 0xFFFE &&  CurElement.Element == 0xE00D)  //Sequence Item Delim. Tag (FFFE, E00D), end of current object
              {Ptr = (void *) (& Length);
               fread (Ptr, 4, 1, fp);
              // if(IsLittleEanian == false)  Length = ReverseUnsignedInt(Length );  // should be zero length
               Offset =  Offset + 4;
               break;
              }
    SetElementProperities(&CurElement);   // search dictionary and assign element VR, VM, and description
    if(IsExplicitVR == true)
         {fread (VRPtr, 2, 1, fp);  // Read VRType
          VRType[2] =0;
          Offset =  Offset + 2;
          if(strcmp(VRType, "OB") == 0 || strcmp(VRType, "OW") == 0 || strcmp(VRType, "OF") ==0|| strcmp(VRType, "SQ")==0  || strcmp(VRType, "UT")==0 || strcmp(VRType, "UN")==0)
            { Ptr = (void *) (& Length);
              fread (Ptr, 2, 1, fp);// Toss away 16 bits
              fread (Ptr, 4, 1, fp);
              Offset =  Offset + 6;
             if(IsLittleEanian == false)  Length = ReverseUnsignedInt(Length );

            }
          else
            { Ptr = (void *) (&tempShortInt);
              fread (Ptr, 2, 1, fp);
              if(IsLittleEanian == false)
                tempShortInt = ReverseUnsignedShort(tempShortInt );
              Length = tempShortInt;
              Offset =  Offset + 2;
            }
          }
     else
         {
            Ptr = (void *) (& Length);
            fread (Ptr, 4, 1, fp);
            if(IsLittleEanian == false)  Length = ReverseUnsignedInt(Length );
            Offset =  Offset + 4;  //should be 4 ?

         }
 /*	 if(IsLittleEanian == false)
         { // should reverse G , E first for check dictionary
         Length = ReverseUnsignedInt(Length );
         }
 */
     CurElement.Offset = Offset;
     CurElement.Length =Length;
     if(strcmp(VRType, "SQ")==0 )
           {
               if( ElementCount == 0 && MyObjectIndex.ParentDataElementID !=-1 )
                   CurElement.IsBeginOfObject = true;
                else CurElement.IsBeginOfObject = false;
                ElementCount ++;

               //Sequence element won't read and allocate data, should be decode  further by  ParseSQElement function (as following code)

               CurElement.ElementID = AllElement.size();
               LastElementID = CurElement.ElementID;
               AllElement.push_back(CurElement);
               printf("SQ data element G = %04X E = %04X  %s VR = %s  Length = %d Offset= %d  \n", CurElement.Group,  CurElement.Element,CurElement.Description, CurElement.VRType, CurElement.Length, CurElement.Offset);
               int SQLength;
               SQLength = ParseSQElement( CurElement);
               if(decodeRet  >0)    Offset = Offset +SQLength;
               else break; //error
            }
     else  {
           void  *Data;
           // char strData[256];
           Data = malloc(Length); //(void*) new[Length];
           fread (Data, Length, 1, fp);
           CurElement.Data =Data;
           Offset = Offset + Length;
           DataToString(&CurElement);
          // DataToString(CurElement.Data,CurElement.Length,CurElement.VRType, CurElement.strData);

           if( ElementCount == 0  && MyObjectIndex.ParentDataElementID !=-1 )
                 CurElement.IsBeginOfObject = true;
           else CurElement.IsBeginOfObject = false;
           ElementCount ++;

           CurElement.ElementID = AllElement.size();
           LastElementID = CurElement.ElementID;
           AllElement.push_back(CurElement);
           printf("G = %04X E = %04X  %s VR = %s  Length = %d Offset= %d strData= %s \n", CurElement.Group,  CurElement.Element,CurElement.Description, CurElement.VRType, CurElement.Length, CurElement.Offset, CurElement.StrDataV[0].strData);
          // printf("G = %04X E = %04X VR = %s  Length = %d Offset= %d str Data %s \n", CurElement.Group,  CurElement.Element, CurElement.VRType, CurElement.Length, CurElement.Offset, CurElement.strData);
          //  printf("Next offset %d  ",Offset);
         }

    }
    if(ElementCount >0)
         {


         AllElement[ LastElementID].IsEndOfObject = true;
        }
    //fclose(fp);
    MyDDOLength = Offset - MyObjectIndex.Offset;
    return MyDDOLength ;

}

// ftp://dicom.nema.org/medical/DICOM/2013/output/chtml/part05/sect_7.5.html
int  DICOMDataObject:: ParseSQElement(DataElement ParentElement)
 {bool isDefinedSQLength;
  bool isEndOfSeqence;
  int MySQLength;
  MySQLength =-1;
  isEndOfSeqence = false;
   bool decodeRet = true;
  unsigned int		Offset;

  int MaxOffset;
  if( ParentElement.Length == 0xFFFFFFFF)
         { isDefinedSQLength = 0; //unknow SQ Length
           MaxOffset = fileSize;
         }
  else  { isDefinedSQLength = 1;
         MaxOffset  =  ParentElement.Offset +  ParentElement.Length;
        }

    DataElement CurElement;
    unsigned short tempShortInt;
    unsigned int Length;
    void *Ptr, * GPtr, *EPtr, *VRPtr;

    GPtr = (void *) (&CurElement.Group);
    EPtr = (void *) (&CurElement.Element);
    Offset = ParentElement.Offset;
    fseek(fp, Offset, SEEK_SET);

    int SQItemCount;
    SQItemCount =0;
    int LastObjectID;


    while ( Offset < MaxOffset  )
    {
        fread (GPtr, 2, 1, fp);
        fread (EPtr, 2, 1, fp);
        if(IsLittleEanian == false)
           { // should reverse G , E first for check dictionary
           CurElement.Group = ReverseUnsignedShort(CurElement.Group  );
           CurElement.Element  = ReverseUnsignedShort(CurElement.Element );
           }
        Offset = Offset +4 ;
        //should be begin of SQ DICOM object tag :(FFFE, E000) , would be something worng other wise
        //(FFFE, E000) might add to data element as the begin of a sub data object, ending of the object should also be added  in this condiction
        if(CurElement.Group == 0xFFFE &&  CurElement.Element == 0xE000)
              {Ptr = (void *) (& Length);
               fread (Ptr, 4, 1, fp);
               if(IsLittleEanian == false)  Length = ReverseUnsignedInt(Length );
               Offset =  Offset + 4;
               if ( Length >0 || Length == 0xFFFFFFFF)
                    {DICOMObjectIndex	CurObjectIndex;
                    if(SQItemCount == 0) CurObjectIndex.IsBeginOfSequence = true;
                           else  CurObjectIndex.IsBeginOfSequence = false;
                     CurObjectIndex.IsEndOfSeqence = false;
                     CurObjectIndex.DICOMObjectID =AllObject.size();
                     LastObjectID = CurObjectIndex.DICOMObjectID;
                     CurObjectIndex.ParentDataElementID = ParentElement.ElementID;
                     CurObjectIndex.Length = Length;
                     if(CurObjectIndex.Length == 0xFFFFFFFF) CurObjectIndex.isKnowLength = false;   //Unknow object length
                           else  CurObjectIndex.isKnowLength = true;
                     CurObjectIndex.Offset =  Offset;
                     SQItemCount ++;
                     CurObjectIndex.ItemNumber = SQItemCount;
                     AllObject.push_back(CurObjectIndex);
                    int DDOLength;
                     DDOLength =  DecodeDICOMObject(CurObjectIndex);
                    if( DDOLength >0) Offset = Offset +  DDOLength;
                        else break;   // error
                    }
               // else,   CurObjectIndex.Length == 0, read other object
               }
        if(CurElement.Group == 0xFFFE &&  CurElement.Element == 0xE0DD)   //  Undefined Length sequence( CurObjectIndex.Length == 0xFFFFFFFF), Sequence Delimitation Item
              {Ptr = (void *) (& Length);
               fread (Ptr, 4, 1, fp);
              // if(IsLittleEanian == false)  Length = ReverseUnsignedInt(Length );  // should be zero length
               Offset =  Offset + 4;
               break;   // end of sequence
              }
    }

    if(SQItemCount >0)
         {

         AllObject[ LastObjectID].IsEndOfSeqence = true;
        }
    int LastElement;
     LastElement = AllElement.size()-1;
     AllElement[LastElement].SQEndings.push_back(ParentElement.ElementID);
    MySQLength = Offset -  ParentElement.Offset;
    return MySQLength ;


 }
//(K)、取代(D,Z,U)、刪除(X)、模糊化(C)
char  DICOMDataObject:: CheckDeanonymizationType(DataElement * DataElementP)
   {
    if(DataElementP->Group == 0X0002)  return 'K';
    else return 'D';

    }
bool  DICOMDataObject:: KeepDataElement(FILE *Retfp,  DataElement *CurElement)
   {
    void *Ptr, * GPtr, *EPtr, *VRPtr;

    GPtr = (void *) (&CurElement->Group);
    EPtr = (void *) (&CurElement->Element);
    VRPtr =(void *) CurElement->VRType;
    fwrite (GPtr, 2, 1, Retfp);
    fwrite (EPtr, 2, 1, Retfp);
    // only support explict VR
    fwrite (VRPtr, 2, 1, Retfp);
   }

unsigned short hex2int(char *hex) {
    unsigned short val = 0;
    while (*hex) {
        // get current character then increment
        unsigned char byte = *hex++;
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte >= '0' && byte <= '9') byte = byte - '0';
        else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;
        // shift 4 to make space for new digit, and add the 4 bits of the new digit
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}
void DICOMDataObject::SavePart10Header(FILE	*Retfp)
{   unsigned char  hdata[160];
    void * Data;

    void * p;

    int i, size;;
    for(i=0;i<128;i++) hdata[i]=0;
    hdata[128] ='D';   hdata[129] ='I';   hdata[130] ='C';   hdata[131] ='M';
    hdata[132] =2;     hdata[133] =0;   hdata[134] =0;   hdata[135] = 0; //little endian g:00020 e:000
    hdata[136] ='U';   hdata[137] ='L';
    hdata[138] =4;   hdata[139] =0;  // hdata[138] =0;   hdata[139] = 0; // length 4
    fwrite (hdata, 140, 1, Retfp);
    // shold add 4 byte group length here

   // 先存到暫存檔
    FILE	* tempfp;
    tempfp = fopen(tempPart10Elements, "wb");
   // if(!tempfp)      return false;

    QString strG,strE, strVR, strValue,strType;
    unsigned short g, e;
    int length;
    QByteArray baVR,baData;//= str1.toLocal8Bit();
    char * vr;
   // unsigned int uintData, Length;

    char charData[256]; // max 256
    char vrType[3];
    size = jsonTemp->rootJsonAarray.size();
  //  qDebug()<< size;
    for(i=0; i<size ;i++)
       {strG =jsonTemp->rootJsonAarray.at(i).toObject().value("g").toString();
        strE =jsonTemp->rootJsonAarray.at(i).toObject().value("e").toString();
        strVR=jsonTemp->rootJsonAarray.at(i).toObject().value("vr").toString();
        baVR =strVR.toLocal8Bit();
        vr = baVR.data();
        strValue = jsonTemp->rootJsonAarray.at(i).toObject().value("value").toString();
        QByteArray ba; //, ba2;
        ba = strG.toLocal8Bit();
        g = hex2int(ba.data());
        ba = strE.toLocal8Bit();
        e = hex2int(ba.data());
        //strG= "0X" + strG;
         //strE ="0X" + strE;
      //   g = strG.toUShort();              e= strE.toUShort();

 //  const char *c_str2 = ba.data();
      /*   QByteArray ba1, ba2;
         ba1 = strG.toLocal8Bit();
         sscanf(ba1.data(), "%x", &g);
         ba2 = strE.toLocal8Bit();
         sscanf(ba2.data(), "%x", &e);
         */
         if(g ==0x0002 && e >0x0000)
           {

            if(e ==0x0001)
                 {vrType[0] ='O';  vrType[1] ='B';  vrType[2] =0;
                  charData[0] =0; charData[1]=1;
                  length =2;
                  Data = (void *)charData;
                  }
            else {
                  strType = jsonTemp->rootJsonAarray.at(i).toObject().value("type").toString();
                  if(strType == "Replace")
                       {baData =  strValue.toLocal8Bit();
                        Data = baData.data();
                        length = strValue.length();
                       }
                 }
            SaveDataElement(tempfp, g, e, vr,length, Data );
           }
        }
    fclose(tempfp);


    unsigned char p10Str[2000];
    // 寫入 0002 0000 group length 及 0002 temp file 的資料
    tempfp = fopen(tempPart10Elements, "rb");
    unsigned fileLength;
    fseek(tempfp , 0, SEEK_END);
    fileLength = ftell(tempfp );
    fseek(tempfp , 0, SEEK_SET);
    fread( p10Str, 1,fileLength, tempfp);
    fwrite (&fileLength, 4, 1, Retfp);
    fwrite (p10Str, fileLength, 1, Retfp);
    fclose( tempfp );
 }

void DICOMDataObject::SetUsefulElements()
{   int i,TotalElement;
   // QString g ,e;
  //  char  charG[5], charE[5]; //, strData[128];
    TotalElement = AllElement.size();
    for(i=0;i< TotalElement;i ++)
    {if(AllElement[i].Group ==0x0020 && AllElement[i].Element == 0x000D)
                          //ufElements.studyUID = QString( (char *) AllElement[i].Data);
                           ufElements.studyUID = QString( AllElement[i].StrDataV[0].strData).trimmed();
     if(AllElement[i].Group ==0x0020 && AllElement[i].Element == 0x000E)
                          ufElements.seriesUID= QString( AllElement[i].StrDataV[0].strData).trimmed();
     if(AllElement[i].Group ==0x0020 && AllElement[i].Element == 0x0011)
                          ufElements.seriesNumber = QString( AllElement[i].StrDataV[0].strData).trimmed();
     if(AllElement[i].Group ==0x0008 && AllElement[i].Element == 0x0060)
                          ufElements.modality = QString( AllElement[i].StrDataV[0].strData).trimmed();
     if(AllElement[i].Group ==0x0008 && AllElement[i].Element == 0x0018)
                          ufElements.instanceUID = QString( AllElement[i].StrDataV[0].strData).trimmed();
     if(AllElement[i].Group ==0x0008 && AllElement[i].Element == 0x0016)
                          ufElements.sopClassUID = QString( AllElement[i].StrDataV[0].strData).trimmed();
     if(AllElement[i].Group ==0x0020 && AllElement[i].Element == 0x0013)
                          ufElements.instanceNumber = QString( AllElement[i].StrDataV[0].strData).trimmed();
    }
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



}

//QMap<QString, QString> DICOMDataObject::SetJSONElement(QMap<QString, QString> uidMap)
//{
//    QString strG,strE, strVR, strValue,strType;
//    int  i, size;
//    size = jsonTemp->rootJsonAarray.size();
//    QJsonArray newArray;
//    //  qDebug()<< size;

//    //generate current datetime for UID replacement
//    QDateTime now = now.currentDateTime();
//    QString dateTime = now.toString("yyyyMMddHHmmss");

//    for(i=0; i<size ;i++)
//    {
//        strG =jsonTemp->rootJsonAarray.at(i).toObject().value("g").toString();
//        strE =jsonTemp->rootJsonAarray.at(i).toObject().value("e").toString();
//        if(strG=="0002" && strE=="0003")    //MediaStorageSOPInstanceUID
//        {
//            QJsonValueRef ref = jsonTemp->rootJsonAarray[i];
//            QJsonObject o = ref.toObject();
//            QString val = o.value("value").toString();
//            if(uidMap.contains(val)){
//                o.insert("value", QString("1.3." + uidMap[val] + "." + ufElements.instanceNumber));
//            }
//            else {
//                QString str = "1.3." + dateTime + "." + ufElements.instanceNumber;
//                o.insert("value", str);
//                uidMap[val] = str;
//            }
//            newArray.push_back(o);
//        }
//        else if(strG=="0008" && strE=="0018")    //SOPInstanceUID
//        {
//            QJsonValueRef ref = jsonTemp->rootJsonAarray[i];
//            QJsonObject o = ref.toObject();
//            QString val = o.value("value").toString();
//            if(uidMap.contains(val)){
//                o.insert("value", QString("1.3." + dateTime + "." + ufElements.instanceNumber));
//            }
//            else {
//                QString str = "1.3." + dateTime + "." + ufElements.instanceNumber;
//                o.insert("value", str);
//                uidMap[val] = str;
//            }
//            newArray.push_back(o);
//        }
//        else if(strG=="0020" && strE=="000D")    //StudyInstanceUID
//        {
//            QJsonValueRef ref = jsonTemp->rootJsonAarray[i];
//            QJsonObject o = ref.toObject();
//            QString val = o.value("value").toString();
//            if(uidMap.contains(val)){
//                o.insert("value", QString("1.1." + dateTime + ".0001"));
//            }
//            else {
//                QString str = "1.1." + dateTime + ".0001";
//                o.insert("value", str);
//                uidMap[val] = str;
//            }
//            newArray.push_back(o);
//        }
//        else if(strG=="0020" && strE=="000E")    //SeriesInstanceUID
//        {
//            QJsonValueRef ref = jsonTemp->rootJsonAarray[i];
//            QJsonObject o = ref.toObject();
//            QString val = o.value("value").toString();
//            if(uidMap.contains(val)){
//                o.insert("value", QString("1.2." + dateTime + "." + ufElements.seriesNumber));
//            }
//            else {
//                QString str = "1.2." + dateTime + "." + ufElements.seriesNumber;
//                o.insert("value", str);
//                uidMap[val] = str;
//            }
//            newArray.push_back(o);
//        }
//        else
//        {
//            newArray.push_back(jsonTemp->rootJsonAarray.at(i));
//        }

//    }
//    jsonTemp->rootJsonAarray = newArray;
//    QJsonDocument doc(newArray);
//    qDebug()<< "start============================================";
//    qDebug()<< doc.toJson();
//    return uidMap;
//}

//void DICOMDataObject::SetJSONElement()
QMap<QString, QString> DICOMDataObject::SetJSONElement(QMap<QString, QString> uidMap)
{
    int i,TotalElement;
     QString g ,e;
     char  charG[5], charE[5]; //, strData[128];
     //QJsonArray newArray;
     TotalElement = AllElement.size();

     //generate current datetime for UID replacement
     QDateTime now = now.currentDateTime();
     QString dateTime = now.toString("yyyyMMddHHmmss");

     for(i=0;i< TotalElement;i ++)
     {
         if(AllElement[i].Group >0x0002)
         {
             sprintf(charG, "%04X", AllElement[i].Group);
             sprintf(charE, "%04X",  AllElement[i].Element);
             g = QString(charG);
             e = QString(charE);
             if(FindJSONElement(g, e) == true  )
             {
                 QJsonValueRef ref = jsonTemp->rootJsonAarray[elIdx];
                 QJsonObject o = ref.toObject();
                 QString val = QString((char*)AllElement[i].Data).trimmed();
                 if (FoundObj.value("type").toString()=="Keep" )
                 {
                     o.insert("value", val);
                 }
                 else if(g=="0008" && e=="0018")    //SOPInstanceUID
                 {
                     if(uidMap.contains(val)){
                         o.insert("value", QString(uidMap[val]));
                     }
                     else {
                         QString str = "1.3." + dateTime + "." + ufElements.instanceNumber;
                         o.insert("value", str);
                         uidMap[val] = str;
                     }
                 }
                 else if(g=="0020" && e=="000D")    //StudyInstanceUID
                 {
                     if(uidMap.contains(val)){
                         o.insert("value", QString(uidMap[val]));
                     }
                     else {
                         QString str = "1.1." + dateTime + ".0001";
                         o.insert("value", str);
                         uidMap[val] = str;
                     }
                 }
                 else if(g=="0020" && e=="000E")    //SeriesInstanceUID
                 {
                     if(uidMap.contains(val)){
                         o.insert("value", QString(uidMap[val]));
                     }
                     else {
                         QString str = "1.2." + dateTime + "." + ufElements.seriesNumber;
                         o.insert("value", str);
                         uidMap[val] = str;
                     }
                 }
                 ref = o;
             }
         }
         else if(AllElement[i].Group ==0x0002 && AllElement[i].Element == 0x0002)
         {
             if(FindJSONElement("0002", "0002") ==true)
             {
                 QJsonValueRef ref = jsonTemp->rootJsonAarray[elIdx];
                 QJsonObject o = ref.toObject();
                 QString val = QString((char*)AllElement[i].Data).trimmed();
                 o.insert("value", val);
                 ref=o;
             }
         }
         else if(AllElement[i].Group ==0x0002 && AllElement[i].Element == 0x0003) //MediaStorageSOPInstanceUID
         {
             if(FindJSONElement("0002", "0003") ==true)
             {
                 QJsonValueRef ref = jsonTemp->rootJsonAarray[elIdx];
                 QJsonObject o = ref.toObject();
                 QString val = QString((char*)AllElement[i].Data).trimmed();
                 if(uidMap.contains(val)){
                     o.insert("value", QString(uidMap[val]));
                 }
                 else {
                     QString str = "1.3." + dateTime + "." + ufElements.instanceNumber;
                     o.insert("value", str);
                     uidMap[val] = str;
                 }
                 ref=o;
             }
         }
     }
     //jsonTemp->rootJsonAarray = newArray;
     //QJsonDocument doc(newArray);
     //qDebug()<< "start============================================";
     //qDebug()<< doc.toJson();
     return uidMap;
}

bool DICOMDataObject::FindJSONElement(QString g, QString e)
{  QString strG,strE;
   int  i, size;
     size = jsonTemp->rootJsonAarray.size();
  //  qDebug()<< size;
    for(i=0; i<size ;i++)
       {strG =jsonTemp->rootJsonAarray.at(i).toObject().value("g").toString();
        strE =jsonTemp->rootJsonAarray.at(i).toObject().value("e").toString();
        if(g==strG && e == strE)
          { FoundObj = jsonTemp->rootJsonAarray.at(i).toObject();
            elIdx = i;
            return true;
           }
       }
    return  false;

}

void  DICOMDataObject::SaveDataElement(FILE	*Retfp, unsigned short g, unsigned short e, char * vr, unsigned int Length, void * Data )
{
 //  DDO.jsonTemp->getJSONData("0002","0001","type");
    void  * GPtr, *EPtr, *VRPtr , *LengthPtr;
    GPtr = (void *) (&g);
    EPtr = (void *) (&e);
    VRPtr =(void *) vr;
    fwrite (GPtr, 2, 1, Retfp);
    fwrite (EPtr, 2, 1, Retfp);
    // only support explict VR
    fwrite (VRPtr, 2, 1, Retfp);
    int isLengthOdd;
    isLengthOdd = Length % 2;
    if(isLengthOdd == 1) Length++;
    if(strcmp(vr, "OB") == 0 || strcmp(vr, "OW") == 0 || strcmp(vr, "OF") ==0|| strcmp(vr, "SQ")==0  || strcmp(vr, "UT")==0 || strcmp(vr, "UN")==0)
    { char tempData[3];
        void * vTempData;
        tempData[0]= 0;tempData[1]= 0;tempData[2]= 0;
        vTempData = (void *) tempData;
        fwrite (vTempData, 2, 1, Retfp);
        LengthPtr =(void *)(&Length);
        fwrite(LengthPtr, 4, 1, Retfp);

    }
    else
    { unsigned short shortLength;
        shortLength = Length; // 4 byte int to 2 byte int
        LengthPtr =(void *)(&shortLength);
        fwrite(LengthPtr, 2, 1, Retfp);
    }


    if(isLengthOdd == 1) {
        fwrite (Data ,Length-1, 1, Retfp);
        char pad[1];
        // http://dicom.nema.org/dicom/2013/output/chtml/part05/sect_6.2.html
        if(strcmp(vr, "OB") == 0 || strcmp(vr, "UI") == 0)     pad[0]=0;
        else   pad[0]=0;
        fwrite(pad, 1, 1, Retfp);
    }
    else fwrite (Data ,Length, 1, Retfp);

}

void  DICOMDataObject::SaveDataElement(FILE	*Retfp,  DataElement *CurElement)
{
    void  * GPtr, *EPtr, *VRPtr , *LengthPtr;
    GPtr = (void *) (&CurElement->Group);
    EPtr = (void *) (&CurElement->Element);
    VRPtr =(void *) CurElement->VRType;
    fwrite (GPtr, 2, 1, Retfp);
    fwrite (EPtr, 2, 1, Retfp);
    // only support explict VR
    fwrite (VRPtr, 2, 1, Retfp);

    char  * VRType;
    VRType = CurElement->VRType;
    if(strcmp(VRType, "OB") == 0 || strcmp(VRType, "OW") == 0 || strcmp(VRType, "OF") ==0|| strcmp(VRType, "SQ")==0  || strcmp(VRType, "UT")==0 || strcmp(VRType, "UN")==0)
      { char tempData[3];
        void * vTempData;
        tempData[0]= 0;tempData[1]= 0;tempData[2]= 0;
        vTempData = (void *) tempData;
        fwrite (vTempData, 2, 1, Retfp);
        LengthPtr =(void *)(&CurElement->Length);
        fwrite(LengthPtr, 4, 1, Retfp);

      }
    else
      {  unsigned short shortLength;
        shortLength = CurElement->Length; // 4 byte int to 2 byte int
        LengthPtr =(void *)(&shortLength);
        fwrite(LengthPtr, 2, 1, Retfp);
      }
    fwrite (CurElement->Data ,CurElement->Length, 1, Retfp);

}

//bool  DICOMDataObject:: SaveDICOM(char * FileName)
bool  DICOMDataObject:: SaveDICOM()
{
    FILE	*Retfp;
    Retfp = fopen(retDCM, "wb");

    if(!Retfp)      return false;

    SavePart10Header(Retfp);

   int i,TotalElement;
    QString g ,e;
    char  charG[5], charE[5];
    QString strG,strE, strVR, strValue,strType;
    QByteArray baData;
    void * Data;
    int length;
    TotalElement = AllElement.size();
    for(i=0;i< TotalElement;i ++)
    { // if(AllElement[i].Group== 0x7FE0 && AllElement[i].Element == 0x0010 )

    if(AllElement[i].Group >0x0002 && AllElement[i].Element != 0 )
          { if(strcmp(AllElement[i].VRType, "SQ") !=0)
               {if((AllElement[i].Group == 0x0028 || AllElement[i].Group == 0x0018) &&AllElement[i].Group != 0x0000  )
                        {SaveDataElement(Retfp,   &AllElement[i]); }
                if(AllElement[i].Group== 0x7FE0 && AllElement[i].Element == 0x0010 )
                         {SaveDataElement(Retfp,   &AllElement[i]); }

               sprintf(charG, "%04X", AllElement[i].Group);
               sprintf(charE, "%04X",  AllElement[i].Element);
               g = QString(charG);
               e = QString(charE);
               if(FindJSONElement(g, e) == true  )
                   {strType =FoundObj.value("type").toString();
                    strValue =FoundObj.value("value").toString();
                    if((AllElement[i].Group == 0x0028 && AllElement[i].Element != 0x0000) || strType == "Keep"  )
                      SaveDataElement(Retfp,   &AllElement[i]);
                    else if(strType == "Replace" || strType == "ZeroLength"  )
                     { baData =  strValue.toLocal8Bit();
                       Data = baData.data();
                       length = strValue.length();
                       SaveDataElement(Retfp, AllElement[i].Group,  AllElement[i].Element, AllElement[i].VRType,length, Data );
                      }

                   }

                }

          }
    }

    fclose(Retfp);
}
bool  DICOMDataObject::Deanonymization(char * FileName)
   {
       FILE	*Retfp;
       Retfp = fopen(FileName, "w");
       if(!Retfp)      return false;
       SavePart10Header(Retfp);
       int i,TotalElement;
       TotalElement = AllElement.size();
       /*

       bool  KeepDataElement(FILE	*Retfp,  DataElement *CurElement);
       bool  ReplaceZeroLength(FILE	*Retfp,  DataElement *CurElement);
       bool  ReplaceWithString(FILE	*Retfp,  DataElement *CurElement, unsigned short strLength, char * strData);

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

       char pType;
       for(i=0;i< TotalElement;i ++)
       { pType = CheckDeanonymizationType( &AllElement[i]);
         switch(pType) {
                   case 'K':
                       SaveDataElement(Retfp,   &AllElement[i]);
                       break;
                   case 'P':
                       break;

                   default:
                            break;
               }
       }
      fclose(Retfp);
}


bool DICOMDataObject::DecodeRetToXML(char * FileName)
{   FILE	*Retfp;
    Retfp = fopen(FileName, "w");

    if(!Retfp)
        return false;
    int i,TotalElement;
    TotalElement = AllElement.size();



    int MyObjectID;
    char EndOfSequence;
    fprintf(Retfp, "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n");

    fprintf(Retfp, "<NativeDicomModel>\n");
    for(i=0;i< TotalElement;i ++)
        {MyObjectID = AllElement[i].ParentDICOMObjectID;
         if(AllObject[ MyObjectID].IsEndOfSeqence == true) EndOfSequence ='T';
         else EndOfSequence ='F';
        if(AllElement[i].IsBeginOfObject == true)
            {fprintf(Retfp, "<Item  number=\"%d\"   >\n",AllObject[ MyObjectID].ItemNumber );  //<Item  Number=\"1\">
        //fprintf(Retfp, "\n<Item  Number=\"%d\" POis=\"%d\"  Offset=\"%d\"  Length=\"%d\" EndOfSQ=\"%c\"  >\n",AllObject[ MyObjectID].ItemNumber,MyObjectID, AllObject[ MyObjectID].Offset,AllObject[ MyObjectID].Length,EndOfSequence  );  //<Item  Number=\"1\">
        }
        fprintf(Retfp, "<DicomAttribute  ");
      //  fprintf(Retfp, "POid =\"%d\" Eid =\"%d\"  ",AllElement[i].ParentDICOMObjectID,AllElement[i].ElementID);
      //	fprintf(Retfp, "VM =\"%s\"  ",AllElement[i].VM);
        fprintf(Retfp, "tag=\"%04X%04X\"  ", AllElement[i].Group, AllElement[i].Element);
        fprintf(Retfp, "offset =\"%d\" length =\"%d\"  ", AllElement[i].Offset,AllElement[i].Length );
        fprintf(Retfp, "vr= \"%s\" keyword= \"%s\" ",AllElement[i].VRType,AllElement[i].Description);
        fprintf(Retfp, ">");
        char VRType[3];
        unsigned int j;
        for(j=0;j<3;j++)  VRType[j] =AllElement[i].VRType[j];
        if(AllElement[i].Length >0)
            {int j;
            for(j =0; j<AllElement[i].StrDataV.size();j++)
               {
            if(!(strcmp(VRType, "OB") == 0 || strcmp(VRType, "OW") == 0 || strcmp(VRType, "OF") ==0|| strcmp(VRType, "SQ")==0 )) // || strcmp(VRType, "UT")==0 ))
                  {//if(strcmp(VRType, "PN") != 0)
                    fprintf(Retfp, "<Value number=\"%d\" >%s</Value>",j+1,AllElement[i].StrDataV[j].strData);
                   }
               }
            }
        if(strcmp(VRType, "SQ") !=0 )
            { fprintf(Retfp, "</DicomAttribute>\n");
             if(AllElement[i].IsEndOfObject  == true && AllElement[i].ParentDICOMObjectID !=0) fprintf(Retfp, "</Item> \n");

             //if(AllObject[ MyObjectID].IsEndOfSeqence ==true)  fprintf(Retfp, "</DicomAttribute>\n");
             }
        int TheEelementSQEndingCount;
        TheEelementSQEndingCount =AllElement[i].SQEndings.size();
        for(j=0;j<TheEelementSQEndingCount ;j++)
            {fprintf(Retfp, "</DicomAttribute>");
             int SQElementID;
             SQElementID = AllElement[i].SQEndings[j];
             if(AllElement[SQElementID].IsEndOfObject == true && AllElement[SQElementID].ParentDICOMObjectID !=0)fprintf(Retfp, "</Item>\n");
            }
        }
    fprintf(Retfp, "</NativeDicomModel>");
    fclose(Retfp);
}

bool DICOMDataObject::DecodeSRToSVG(char * FileName)
{   FILE	*Retfp;
    Retfp = fopen(FileName, "w");

    if(!Retfp)
        return false;
    int i,TotalElement;
    TotalElement = AllElement.size();
    int MyObjectID;
    char EndOfSequence;
    void		*Data;
    float FL;
    int SL;
    unsigned int UL;
    fprintf(Retfp, "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n");
    fprintf(Retfp, "<SVG>\n");
    for(i=0;i< TotalElement;i ++)
      {
       if(strcmp(AllElement[i].Description, "ReferencedContentItemIdentifier" ) == 0)  // equal
        {
        fprintf(Retfp, "<Element tag=\"%04X%04X\"  ", AllElement[i].Group, AllElement[i].Element);
        fprintf(Retfp, "offset =\"%d\" length =\"%d\"  ", AllElement[i].Offset,AllElement[i].Length );
        fprintf(Retfp, "vr= \"%s\" keyword= \"%s\" ",AllElement[i].VRType,AllElement[i].Description);
        fprintf(Retfp, ">\n");
        if(strcmp(AllElement[i].VRType, "UL") == 0)
           { int j;
           for(j=0;j <AllElement[i].Length; j= j+4)
               { Data =(void*)((char*) AllElement[i].Data + j);
                UL=(*((unsigned int *) Data));
                if(IsLittleEanian == false)  UL = ReverseUnsignedInt(UL);
                if(j >0)   fprintf(Retfp,",");
                fprintf(Retfp,"%d",UL);
               }
            }
         fprintf(Retfp, "\n</Element>\n");
        }

       if(strcmp(AllElement[i].Description, "GraphicData") == 0)  // equal
        {
        fprintf(Retfp, "<Element tag=\"%04X%04X\"  ", AllElement[i].Group, AllElement[i].Element);
        fprintf(Retfp, "offset =\"%d\" length =\"%d\"  ", AllElement[i].Offset,AllElement[i].Length );
        fprintf(Retfp, "vr= \"%s\" keyword= \"%s\" ",AllElement[i].VRType,AllElement[i].Description);
        fprintf(Retfp, ">\n");
        if(strcmp(AllElement[i].VRType, "FL") == 0)
           { int j;
           fprintf(Retfp,"<polyline points=\"");
           for(j=0;j <AllElement[i].Length; j= j+4)
               { Data =(void*)((char*) AllElement[i].Data + j);
                FL=(*((float *) Data));
                if(IsLittleEanian == false)  FL = ReverseFloat(FL );
                SL =FL;
                if(j >0)   fprintf(Retfp,",");
                fprintf(Retfp,"%d",SL);
               }
            fprintf(Retfp, "\"\n");
            fprintf(Retfp," style=\"fill:none;stroke:black;stroke-width:1\" />\n");

            }
         fprintf(Retfp, "</Element>\n");
        }
       }
    fprintf(Retfp, "</SVG>");
    fclose(Retfp);
}
