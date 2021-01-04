#ifndef CXMLPARSER_CPP
#define CXMLPARSER_CPP
#include "cXmlParser.h"

void cXmlParser::zeroMemory(xml_StructInfo*str){
 str->attributes=NULL;
 str->attributesValue=NULL;
 str->father=NULL;
 str->name=NULL;
 str->nAttributes=0;
 str->next=NULL;
 str->prev=NULL;
 str->value=NULL;
 str->isclose=false;
};

xml_StructInfo* cXmlParser::parser(const char* data){
 int nTags=0;
 int ind=0;
 char *mot=NULL;
 char *tagName;
 xml_StructInfo* ret,*base,*prev=NULL;

 ret=createStr();
 base=ret;
 while(data[ind]!=STRING_END){
  while(data[ind]!=CXMLPARSER_OPENTAG_INIT){
    ind++;
    if(data[ind]==STRING_END)break;
  }
  if(data[ind]==STRING_END)break;
  if(m_StrF.strCompare((mystr)&(data[ind]),(mystr)CXMLPARSER_CLOSETAG_STRING,2)){
    ind+=m_StrF.getStringLen((mystr)CXMLPARSER_CLOSETAG_STRING);
    getTagEnd(base,data,&ind);
    continue;
  }
  tagName=getTagName(data,&ind);
  if(tagName==NULL){

    m_StrF.strAdd((mystr*)&mot,(mystr)"StopReason Char: ");
    m_StrF.strAdd((mystr*)&mot,(mystr)&data[ind]);
    m_StrF.strAdd((mystr*)&mot,(mystr)"Asc2: ");
    m_StrF.strAdd((mystr*)&mot,m_StrF.integer2String(data[ind]));
    sendDebugMessage(__func__,"Null TagName",mot);
    m_StrF.freeStr(&mot);
    continue;
  }
  if(nTags>0){
  base->next=createStr();
  if(base->next==NULL){
    sendDebugMessage(__func__,"Alloc Erro","");
    freeMemory(&ret);
    return NULL;
  }
  prev=base;
  base=base->next;
  base->prev=prev;
  }
  base->name=tagName;
  base->nAttributes=getAttributes(data,&ind,&(base->attributes),&(base->attributesValue));
  base->value=getTagValue(data,&ind);
  nTags++;
 }
 if(nTags==0){
  freeMemory(&ret);
  return NULL;
 }
 return ret;
};

char* cXmlParser::getTagName(const char* data,int*ind){
 char *ret=NULL;
 while(data[*ind]==CXMLPARSER_SPACE)(*ind)++;
 int initBase=(*ind);
 while(data[*ind]!=CXMLPARSER_SPACE && data[*ind]!=CXMLPARSER_ENDTAG && data[*ind]!=STRING_END)(*ind)++;
 if((*ind)-initBase>0){
 m_StrF.strAdd((mystr*)&ret,(mystr)&(data[initBase]),(*ind)-initBase);
 ret=m_StrF.trimFree(ret);
 }
 sendDebugMessage(__func__,"TagName",ret);
 return ret;
}

int cXmlParser::getAttributes(const char*data,int*ind,char***retParam,char***retParamValue){
 int natt=0;
 char *tempAtt=NULL;
 bool haveparam;
 int initInd;
 while(data[*ind]!=STRING_END && data[*ind]!=CXMLPARSER_ENDTAG){
  haveparam=false;
  initInd=(*ind);
  //GETPARAM NAME
  while(data[*ind]!=STRING_END && data[*ind]!=CXMLPARSER_ENDTAG){
    if(data[*ind]==CXMLPARSER_EQUAL){
     if((*ind)-initInd>0){
       m_StrF.strAdd((mystr*)&tempAtt,(mystr)&data[initInd],(*ind)-initInd);
       tempAtt=(mystr)m_StrF.trimFree((mystr)tempAtt);
     }
     if(tempAtt==NULL)break;
     (*ind)++;
     if(data[*ind]!=CXMLPARSER_PARAM_OPENCLOSE){
       m_StrF.freeStr(&tempAtt);
       break;
     }
     natt++;
     if(natt==1){
       *retParam=(char**)malloc(natt*sizeof(char*));
       *retParamValue=(char**)malloc(natt*sizeof(char*));
     }else{
       *retParam=(char**)realloc(*retParam,(natt)*sizeof(char*));
       *retParamValue=(char**)realloc(*retParamValue,(natt)*sizeof(char*));
     }
     if(*retParam==NULL || *retParamValue==NULL){
        return 0;
     }
     (*retParam)[natt-1]=tempAtt;
     (*retParamValue)[natt-1]=NULL;
     sendDebugMessage(__func__,"AttName",tempAtt);
     tempAtt=NULL;
     haveparam=true;
     break;
     }
    (*ind)++;
  }

  if(data[*ind]==STRING_END || data[*ind]==CXMLPARSER_ENDTAG)return natt;
  (*ind)++;
  if(!haveparam)continue;
  initInd=(*ind);
  while(data[*ind]!=STRING_END && data[*ind]!=CXMLPARSER_ENDTAG){

    if(data[*ind]==CXMLPARSER_PARAM_OPENCLOSE){
      if((*ind)-initInd>0){
        m_StrF.strAdd((mystr*)&tempAtt,(mystr)&data[initInd],(*ind)-initInd);
        tempAtt=(mystr)m_StrF.trimFree((mystr)tempAtt);
      }
      if(tempAtt==NULL){
         break;
      }
      (*retParamValue)[natt-1]=tempAtt;
      sendDebugMessage(__func__,"AttValue",tempAtt);
      tempAtt=NULL;
      (*ind)++;
      break;
    }

    (*ind)++;
  }

 }
 if(tempAtt!=NULL)m_StrF.freeStr(&tempAtt);
 return natt;
}

char *cXmlParser::getTagValue(const char *data,int *ind){
  char *ret=NULL;
  if(data[*ind]==CXMLPARSER_ENDTAG)(*ind)++;
  int initInd=(*ind);
  while(data[*ind]!=STRING_END && data[*ind]!=CXMLPARSER_OPENTAG_INIT)(*ind)++;
  if((*ind)-initInd>0){
   m_StrF.strAdd((mystr*)&ret,(mystr)&(data[initInd]),(*ind)-initInd);
   ret=m_StrF.trimFree(ret);
  }
  sendDebugMessage(__func__,"TagValue",ret);
  return ret;
}

void cXmlParser::getTagEnd(xml_StructInfo*strRet,const char *data,int*ind){
  char *closetag=NULL;
  xml_StructInfo*infoRef,*fatherRef;
  int initInd=(*ind);
  while(data[*ind]!=STRING_END && data[*ind]!=CXMLPARSER_ENDTAG)(*ind)++;
  if((*ind)-initInd>0){
    m_StrF.strAdd((mystr*)(&closetag),(mystr)&(data[initInd]),(*ind)-initInd);
    closetag=m_StrF.trimFree(closetag);
  }
  infoRef=strRet;
  while(infoRef!=NULL){
    if(m_StrF.strCompare(infoRef->name,closetag) && !infoRef->isclose){
      infoRef->isclose=true;
      sendDebugMessage(__func__,"Closed",infoRef->name);
      fatherRef=infoRef->prev;
      while(fatherRef!=NULL){
        if(!fatherRef->isclose){
          infoRef->father=fatherRef;
          sendDebugMessage(__func__,"FatherName",fatherRef->name);
          break;
        }
        fatherRef=fatherRef->prev;
      }
       break;
    }
   infoRef=infoRef->prev;
  }
  m_StrF.freeStr(&closetag);
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

xml_StructInfo* cXmlParser::getRefByAttributeName(const xml_StructInfo*baseRef,const char *name){
  xml_StructInfo*readInfo,*ret=NULL,*lastRef=NULL;
  readInfo=(xml_StructInfo*)baseRef;
  bool hasVal;
  bool refSet=false;
  while(readInfo!=NULL){
    hasVal=false;
    for(int i=0;i<readInfo->nAttributes;i++){
      if(m_StrF.strCompare((mystr)name,(mystr)readInfo->attributes[i],m_StrF.getStringLen((mystr)name))){
         hasVal=true;
        break;
      }
    }
    if(hasVal){
      if(!refSet){
        if(!copyStr(readInfo,&ret))return NULL;
        ret->prev=NULL;
        refSet=true;
        lastRef=ret;

      }else{
      if(!copyStr(readInfo,&lastRef->next)){
         freeMemory(&ret);
         return NULL;
      }
      lastRef=lastRef->next;
      }
    }
    readInfo=readInfo->next;
  }
  if(lastRef!=NULL)lastRef->next=NULL;
  return ret;
}

bool cXmlParser::copyStr(const xml_StructInfo*base,xml_StructInfo**dst){
  *dst=createStr();
  if(*dst==NULL)return false;
  (*dst)->attributes=m_StrF.copyStrArray(base->attributes,base->nAttributes);
  (*dst)->attributesValue=m_StrF.copyStrArray(base->attributesValue,base->nAttributes);
  (*dst)->father=base->father;
  (*dst)->isclose=base->isclose;
  (*dst)->name=m_StrF.copyStr(base->name);
  (*dst)->nAttributes=base->nAttributes;
  (*dst)->next=base->next;
  (*dst)->prev=base->prev;
  (*dst)->value=m_StrF.copyStr(base->value);
  return true;
}

void cXmlParser::freeMemory(xml_StructInfo**info){
  xml_StructInfo *next,*base=*info;
  *info=NULL;
  while(base!=NULL){
    m_StrF.freeStrArray(&base->attributes,base->nAttributes);
    m_StrF.freeStrArray(&base->attributesValue,base->nAttributes);
    m_StrF.freeStr(&base->value);
    m_StrF.freeStr(&base->name);
    next=base->next;
    free(base);
    base=next;
  }
}
xml_StructInfo* cXmlParser::createStr(){
  xml_StructInfo*ret=(xml_StructInfo*)malloc(sizeof(xml_StructInfo));
  if(ret==NULL)return NULL;
  zeroMemory(ret);
  return ret;
}

void cXmlParser::sendDebugMessage(const char*fName,const char*message1=NULL,const char*message2=NULL){
  #ifndef CXMLPARSER_DEBUG
   return;
  #endif
  printf("Function Name: %s Message1: %s Message2: %s\n",fName,message1,message2);

}
#endif
