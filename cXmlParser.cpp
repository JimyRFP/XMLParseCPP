#ifndef CXMLPARSER_CPP
#define CXMLPARSER_CPP
#include "cXmlParser.h"

void cXmlParser::zeroMemory(xml_StructInfo* structToZero){
 structToZero->attributes=NULL;
 structToZero->attributesValue=NULL;
 structToZero->father=NULL;
 structToZero->name=NULL;
 structToZero->nAttributes=0;
 structToZero->next=NULL;
 structToZero->prev=NULL;
 structToZero->value=NULL;
 structToZero->isclose=false;
};

xml_StructInfo* cXmlParser::parser(const char* dataToParser){
 int numberOfTags=0;
 int indData=0;
 char *tagName;
 xml_StructInfo* returnStructRef,*currentStructRef,*prevStructRef=NULL;

 returnStructRef=createXmlStruct();
 currentStructRef=returnStructRef;
 while(dataToParser[indData]!=STRING_END){
  while(dataToParser[indData]!=CXMLPARSER_OPENTAG_INIT && dataToParser[indData]!=STRING_END)indData++;
  if(dataToParser[indData]==STRING_END)break;
  if(m_StrF.strCompare((mystr)&(dataToParser[indData]),(mystr)CXMLPARSER_CLOSETAG_STRING,2)){
    indData+=CXMLPARSER_CLOSETAG_LEN;
    setTagEnd(currentStructRef,dataToParser,&indData);
    continue;
  }
  tagName=getTagName(dataToParser,&indData);
  if(tagName==NULL){
    #ifdef CXMLPARSER_DEBUG
      sendDebugMessage(__func__,"Null TagName","");
    #endif
    continue;
  }
  if(numberOfTags>0){
    prevStructRef=currentStructRef;
    currentStructRef->next=createXmlStruct();
    if(currentStructRef->next==NULL){
      #ifdef CXMLPARSER_DEBUG
        sendDebugMessage(__func__,"Alloc Erro","");
      #endif
      freeMemory(&returnStructRef);
      return NULL;
     }
    currentStructRef=currentStructRef->next;
    currentStructRef->prev=prevStructRef;
  }
  currentStructRef->name=tagName;
  currentStructRef->nAttributes=getAttsNameAndValue(dataToParser,&indData,&(currentStructRef->attributes),&(currentStructRef->attributesValue));
  currentStructRef->value=getTagValue(dataToParser,&indData);
  numberOfTags++;
 }
 if(numberOfTags==0){
  freeMemory(&returnStructRef);
  return NULL;
 }
 return returnStructRef;
};

char* cXmlParser::getTagName(const char* data,int*dataInd){
 mystr tagName=NULL;
 while(data[*dataInd]==CXMLPARSER_OPENTAG_INIT)(*dataInd)++;
 while(data[*dataInd]==CXMLPARSER_SPACE)(*dataInd)++;
 (*dataInd)+=getStrUntilStopChar(&tagName,(const mystr)&data[*dataInd],(const mystr)CXMLPARSER_TAGNAME_STOPCHARS);
 #ifdef CXMLPARSER_DEBUG
   sendDebugMessage(__func__,"TagName",tagName);
 #endif
 return (char*)tagName;
}

int cXmlParser::getAttsNameAndValue(const char*data,int*dataInd,char***retAttName,char***retAttValue){
 int numberOfAttributes=0;
 char *tempStr=NULL;
 bool attHaveParamName;
 int initTempStrInd;
 while(data[*dataInd]!=STRING_END && data[*dataInd]!=CXMLPARSER_ENDTAG){
  attHaveParamName=false;
  initTempStrInd=(*dataInd);
  //GETPARAM NAME
  while(data[*dataInd]!=STRING_END && data[*dataInd]!=CXMLPARSER_ENDTAG){
    if(data[*dataInd]==CXMLPARSER_EQUAL){
     if((*dataInd)-initTempStrInd>0){
       m_StrF.strAdd((mystr*)&tempStr,(mystr)&data[initTempStrInd],(*dataInd)-initTempStrInd);
       tempStr=(mystr)m_StrF.trimFree((mystr)tempStr);
     }
     if(tempStr==NULL)break;
     (*dataInd)++;
     if(data[*dataInd]!=CXMLPARSER_PARAM_OPENCLOSE){
       m_StrF.freeStr(&tempStr);
       break;
     }
     numberOfAttributes++;
     if(numberOfAttributes==1){
       *retAttName=(char**)malloc(numberOfAttributes*sizeof(char*));
       *retAttValue=(char**)malloc(numberOfAttributes*sizeof(char*));
     }else{
       *retAttName=(char**)realloc(*retAttName,(numberOfAttributes)*sizeof(char*));
       *retAttValue=(char**)realloc(*retAttValue,(numberOfAttributes)*sizeof(char*));
     }
     if(*retAttName==NULL || *retAttValue==NULL){
        m_StrF.freeStrArray(retAttName,numberOfAttributes);
        m_StrF.freeStrArray(retAttValue,numberOfAttributes);
        return 0;
     }
     (*retAttName)[numberOfAttributes-1]=tempStr;
     (*retAttValue)[numberOfAttributes-1]=NULL;
     #ifdef CXMLPARSER_DEBUG
       sendDebugMessage(__func__,"AttName",tempStr);
     #endif
     tempStr=NULL;
     attHaveParamName=true;
     break;
     }
    (*dataInd)++;
  }

  if(data[*dataInd]==STRING_END || data[*dataInd]==CXMLPARSER_ENDTAG)return numberOfAttributes;
  (*dataInd)++;
  if(!attHaveParamName)continue;
  initTempStrInd=(*dataInd);
  while(data[*dataInd]!=STRING_END && data[*dataInd]!=CXMLPARSER_ENDTAG){

    if(data[*dataInd]==CXMLPARSER_PARAM_OPENCLOSE){
      if((*dataInd)-initTempStrInd>0){
        m_StrF.strAdd((mystr*)&tempStr,(mystr)&data[initTempStrInd],(*dataInd)-initTempStrInd);
        tempStr=(mystr)m_StrF.trimFree((mystr)tempStr);
      }
      if(tempStr==NULL){
         break;
      }
      (*retAttValue)[numberOfAttributes-1]=tempStr;
      #ifdef CXMLPARSER_DEBUG
        sendDebugMessage(__func__,"AttValue",tempStr);
      #endif
      tempStr=NULL;
      (*dataInd)++;
      break;
    }

    (*dataInd)++;
  }

 }
 if(tempStr!=NULL)m_StrF.freeStr(&tempStr);
 return numberOfAttributes;
}

char *cXmlParser::getTagValue(const char *data,int *dataInd){
  mystr tagValueStr=NULL;
  while(data[*dataInd]==CXMLPARSER_ENDTAG)(*dataInd)++;
  (*dataInd)+=getStrUntilStopChar(&tagValueStr,(const mystr)&(data[*dataInd]),(const mystr)CXMLPARSER_TAGVALUE_STOPCHARS);
  #ifdef CXMLPARSER_DEBUG
    sendDebugMessage(__func__,"TagValue",tagValueStr);
  #endif
  return tagValueStr;
}

void cXmlParser::setTagEnd(xml_StructInfo*strRet,const char *data,int*dataInd){
  mystr tagEndStr=NULL;
  xml_StructInfo*infoRef,*fatherRef;
  (*dataInd)+=getStrUntilStopChar(&tagEndStr,(const mystr)&(data[*dataInd]),(const mystr)CXMLPARSER_TAGEND_STOPCHARS);
  if(tagEndStr==NULL)return;
  infoRef=strRet;
  while(infoRef!=NULL){
    if(m_StrF.strCompare(infoRef->name,tagEndStr) && !infoRef->isclose){
      infoRef->isclose=true;
      #ifdef CXMLPARSER_DEBUG
        sendDebugMessage(__func__,"Closed",infoRef->name);
      #endif
      fatherRef=infoRef->prev;
      while(fatherRef!=NULL){
        if(!fatherRef->isclose){
          infoRef->father=fatherRef;
          #ifdef CXMLPARSER_DEBUG
            sendDebugMessage(__func__,"FatherName",fatherRef->name);
          #endif
          break;
        }
        fatherRef=fatherRef->prev;
      }
       break;
    }
   infoRef=infoRef->prev;
  }
  m_StrF.freeStr(&tagEndStr);
}


void cXmlParser::Print(const xml_StructInfo*info){
  if(info==NULL)return;
  xml_StructInfo *base=(xml_StructInfo *)info;
  while(1){
    printf("TagName: %s\n",base->name);
    printf("nAttributes %d\n",base->nAttributes);
    for(int i=0;i<base->nAttributes;i++){
      printf("%s=\"%s\" ",base->attributes[i],base->attributesValue[i]);
    }
    if(base->nAttributes>0)printf("\n");
    if(base->value!=NULL)
      printf("TagValue: %s\n",base->value);
    printf("TagIsClose: %d\n",base->isclose);
    if(base->father!=NULL){
       printf("FatherTagName %s\n",base->father->name);
    }
    printf("\n\n\n\n");
    if(base->next==NULL)break;
    base=base->next;

  }

}

xml_StructInfo* cXmlParser::getRefByAttributeName(const xml_StructInfo*structRef,const char *attName){
  xml_StructInfo*currentReadRef,*returnRef=NULL,*addStructRef=NULL;
  currentReadRef=(xml_StructInfo*)structRef;
  bool hasAttName;
  bool refIsSet=false;
  while(currentReadRef!=NULL){
    hasAttName=false;
    for(int i=0;i<currentReadRef->nAttributes;i++){
      if(m_StrF.strCompare((mystr)attName,(mystr)currentReadRef->attributes[i])){
         hasAttName=true;
        break;
      }
    }
    if(hasAttName){
      if(!refIsSet){
        if(!copyStr(currentReadRef,&returnRef))return NULL;
        returnRef->prev=NULL;
        refIsSet=true;
        addStructRef=returnRef;

      }else{
      if(!copyStr(currentReadRef,&addStructRef->next)){
         freeMemory(&returnRef);
         return NULL;
      }
      addStructRef=addStructRef->next;
      }
    }
    currentReadRef=currentReadRef->next;
  }
  if(addStructRef!=NULL)addStructRef->next=NULL;
  return returnRef;
}

xml_StructInfo* cXmlParser::getRefByTagName(const xml_StructInfo *base,const char *tagName){
  if(tagName==NULL)return NULL;
  xml_StructInfo*returnRef=NULL;
  xml_StructInfo*addStructRef=NULL;
  xml_StructInfo*currentReadRef=(xml_StructInfo*)base;
  while(currentReadRef!=NULL){
    if(m_StrF.strCompare(currentReadRef->name,(mystr)tagName)){
       if(returnRef==NULL){
        copyStr(currentReadRef,&returnRef);
        if(returnRef==NULL)return NULL;
        returnRef->next=NULL;
        addStructRef=returnRef;
       }else{
        copyStr(currentReadRef,&addStructRef->next);
        if(addStructRef->next==NULL){
         freeMemory(&returnRef);
         return NULL;
        }
        addStructRef=returnRef->next;
        addStructRef->next=NULL;
       }
    }
    currentReadRef=currentReadRef->next;
  }

  return returnRef;
}

bool cXmlParser::copyStr(const xml_StructInfo*source,xml_StructInfo**destination){
  if(source==NULL)return false;
  *destination=createXmlStruct();
  if(*destination==NULL)return false;
  (*destination)->attributes=m_StrF.copyStrArray(source->attributes,source->nAttributes);
  (*destination)->attributesValue=m_StrF.copyStrArray(source->attributesValue,source->nAttributes);
  (*destination)->father=source->father;
  (*destination)->isclose=source->isclose;
  (*destination)->name=m_StrF.copyStr(source->name);
  (*destination)->nAttributes=source->nAttributes;
  (*destination)->next=source->next;
  (*destination)->prev=source->prev;
  (*destination)->value=m_StrF.copyStr(source->value);
  return true;
}

void cXmlParser::freeMemory(xml_StructInfo**structToFree){
  xml_StructInfo *nextRefToFree,*currentRefToFree=*structToFree;
  if(currentRefToFree==NULL)return;
  while(currentRefToFree!=NULL){
    m_StrF.freeStrArray(&currentRefToFree->attributes,currentRefToFree->nAttributes);
    m_StrF.freeStrArray(&currentRefToFree->attributesValue,currentRefToFree->nAttributes);
    m_StrF.freeStr(&currentRefToFree->value);
    m_StrF.freeStr(&currentRefToFree->name);
    nextRefToFree=currentRefToFree->next;
    free(currentRefToFree);
    currentRefToFree=nextRefToFree;
  }
  *structToFree=NULL;
}

xml_StructInfo* cXmlParser::createXmlStruct(){
  xml_StructInfo*structRef=(xml_StructInfo*)malloc(sizeof(xml_StructInfo));
  if(structRef==NULL)return NULL;
  zeroMemory(structRef);
  return structRef;
}

void cXmlParser::sendDebugMessage(const char*fName,const char*message1=NULL,const char*message2=NULL){
  #ifndef CXMLPARSER_DEBUG
   return;
  #endif
  printf("Function Name: %s Message1: %s Message2: %s\n",fName,message1,message2);

}


inline int cXmlParser::getStrUntilStopChar(mystr*returnStr,const mystr source,const mystr stopCharList){
 int sourceInd=0;
 int stopCharListInd;
 bool stopCharFound;
 while(source[sourceInd]!=STRING_END){
   stopCharListInd=0;
   stopCharFound=false;
   while(stopCharList[stopCharListInd]!=STRING_END){
     if(stopCharList[stopCharListInd]==source[sourceInd]){
        stopCharFound=true;
        break;
     }
     stopCharListInd++;
   }
   if(stopCharFound)break;
   sourceInd++;
 }
 m_StrF.strAdd(returnStr,source,sourceInd);
 *returnStr=m_StrF.trimFree(*returnStr);
 if(*returnStr==NULL)return 0;
 return sourceInd;
};
#endif
