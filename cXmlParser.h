#ifndef CXMLPARSER_H
#define CXMLPARSER_H
#define CXMLPARSER_OPENTAG_INIT '<'
#define CXMLPARSER_CLOSETAG_STRING "</"
#define CXMLPARSER_CLOSETAG_LEN 2
#define CXMLPARSER_CLOSETAG_BAR '/'
#define CXMLPARSER_ENDTAG '>'
#define CXMLPARSER_PARAM_OPENCLOSE  '\"'
#define CXMLPARSER_SPACE ' '
#define CXMLPARSER_EQUAL '='
#define CXMLPARSER_TAGNAME_STOPCHARS " >"
#define CXMLPARSER_TAGVALUE_STOPCHARS "<"
#define CXMLPARSER_TAGEND_STOPCHARS ">"
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
    xml_StructInfo* getRefByTagName(const xml_StructInfo*,const char *);
    void freeMemory(xml_StructInfo**);

 private:
    xml_StructInfo* createXmlStruct();
    void zeroMemory(xml_StructInfo*);
    char * getTagName(const char*,int *);
    int  getAttsNameAndValue(const char*,int*,char***,char***);
    char* getTagValue(const char*,int*);
    void setTagEnd(xml_StructInfo*,const char*,int*);
    void sendDebugMessage(const char*,const char*,const char*);
    bool copyStr(const xml_StructInfo*,xml_StructInfo**);
    inline int getStrUntilStopChar(mystr*returnStr,const mystr source,const mystr stopCharList);
    //OBJ
    c_StringFunctions m_StrF;
};

#endif
