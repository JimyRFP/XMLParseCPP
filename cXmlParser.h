#ifndef CXMLPARSER_H
#define CXMLPARSER_H
#define CXMLPARSER_OPENTAG_INIT '<'
#define CXMLPARSER_CLOSETAG_STRING "</"
#define CXMLPARSER_CLOSETAG_BAR '/'
#define CXMLPARSER_ENDTAG '>'
#define CXMLPARSER_PARAM_OPENCLOSE  '\"'
#define CXMLPARSER_SPACE ' '
#define CXMLPARSER_EQUAL '='
//#define  CXMLPARSER_DEBUG

#include <stdio.h>
#include "..\StringFunctions\StringFunctions.h"
struct xml_StructInfo{
 char *name;
 char *value;
 char **attributes;
 char **attributesValue;
 int nAttributes;
 xml_StructInfo*prev;
 xml_StructInfo*next;
 xml_StructInfo*father;
 bool isclose;
};
struct xml_StringArray{
  char **val;
  size_t size;

};


class cXmlParser{
 public:
    xml_StructInfo* parser(const char*);
    void Print(const xml_StructInfo*);
    xml_StructInfo* getRefByAttributeName(const xml_StructInfo*,const char *);
    xml_StringArray getAttributeValue(const xml_StructInfo*,const char*);
 private:
    void zeroMemory(xml_StructInfo*);
    char * getTagName(const char*,int *);
    int  getAttributes(const char*,int*,char***,char***);
    char* getTagValue(const char*,int*);
    void getTagEnd(xml_StructInfo*,const char*,int*);
    void sendDebugMessage(const char*,const char*,const char*);
    bool copyStr(const xml_StructInfo*,xml_StructInfo**);
    //OBJ
   // bool
    c_StringFunctions m_StrF;
};

#endif
