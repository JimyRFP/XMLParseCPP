#ifndef CXMLPARSER_CPP
#define CXMLPARSER_CPP
#include "cXmlParser.h"
#include "..\FileControl\getFileName.h"
int main(){
 char *teste=(char*)"<getta att1=\"val1\" att2=\"val2\" att3=\"12354><h3 alt=\"teset\">nome</h3><h2>value</h2><nova1><nova2></nova2></nova1></getta>";
 cXmlParser par;
 xml_StructInfo *info=par.parser(teste);
 par.Print(info);
 return 0;
}

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
 xml_StructInfo* ret,*base,*prev=NULL;

 ret=(xml_StructInfo*)malloc(1*sizeof(xml_StructInfo));
 base=ret;
 zeroMemory(ret);
 while(data[ind]!=STRING_END){
  while(data[ind]!=CXMLPARSER_OPENTAG_INIT){
    ind++;
    if(data[ind]==STRING_END)break;;
  }
  if(data[ind]==STRING_END)break;
  if(m_StrF.strCompare((mystr)&(data[ind]),(mystr)CXMLPARSER_CLOSETAG_STRING,2)){
    ind+=m_StrF.getStringLen((mystr)CXMLPARSER_CLOSETAG_STRING);
    getTagEnd(base,data,&ind);
    continue;
  }
  ind+=1;
  if(nTags>0){
  base->next=(xml_StructInfo*)malloc(1*sizeof(xml_StructInfo));
  if(base->next==NULL)return NULL;
  prev=base;
  base=base->next;
  zeroMemory(base);
  base->prev=prev;
  }
  base->name=getTagName(data,&ind);
  if(base->name==NULL)break;
  base->nAttributes=getAttributes(data,&ind,&(base->attributes),&(base->attributesValue));
  base->value=getTagValue(data,&ind);
   nTags++;
 }
 if(nTags==0){
  free(ret);
  return NULL;
 }
 return ret;
};

char* cXmlParser::getTagName(const char* data,int*ind){
 char *ret=NULL;
 while(data[*ind]==CXMLPARSER_SPACE)(*ind)++;
 while(data[*ind]!=CXMLPARSER_SPACE && data[*ind]!=CXMLPARSER_ENDTAG && data[*ind]!=STRING_END){
    m_StrF.strAdd((mystr*)&ret,(mystr)&(data[*ind]),0,1);
    (*ind)++;
 }
 sendDebugMessage(__func__,"TagName",ret);
 return ret;
}

int cXmlParser::getAttributes(const char*data,int*ind,char***retParam,char***retParamValue){
 int natt=0;
 char *tempAtt=NULL;
 bool haveparam;
 while(data[*ind]!=STRING_END && data[*ind]!=CXMLPARSER_ENDTAG){
  haveparam=false;
  //GETPARAM NAME
  while(data[*ind]!=STRING_END && data[*ind]!=CXMLPARSER_ENDTAG){
    if(data[*ind]==CXMLPARSER_EQUAL){
     if(tempAtt==NULL)break;
     (*ind)++;
     if(data[*ind]!=CXMLPARSER_PARAM_OPENCLOSE){
       free(tempAtt);
       tempAtt=NULL;
       break;
     }
     natt++;
     if(natt==1){
       *retParam=(char**)malloc(natt*sizeof(char**));
       *retParamValue=(char**)malloc(natt*sizeof(char**));
     }else{
       *retParam=(char**)realloc(*retParam,(natt)*sizeof(char**));
       *retParamValue=(char**)realloc(*retParamValue,(natt)*sizeof(char**));
     }
     (*retParam)[natt-1]=tempAtt;
     (*retParamValue)[natt-1]=NULL;
     sendDebugMessage(__func__,"AttName",tempAtt);
     tempAtt=NULL;
     haveparam=true;
     break;
     }
    m_StrF.strAdd((mystr*)&tempAtt,(mystr)&data[*ind],0,1);
    (*ind)++;
  }

  if(data[*ind]==STRING_END || data[*ind]==CXMLPARSER_ENDTAG)return natt;
  (*ind)++;
  if(!haveparam)continue;

  while(data[*ind]!=STRING_END && data[*ind]!=CXMLPARSER_ENDTAG){

    if(data[*ind]==CXMLPARSER_PARAM_OPENCLOSE){
      if(tempAtt==NULL)break;
      (*retParamValue)[natt-1]=tempAtt;
      sendDebugMessage(__func__,"AttValue",tempAtt);
      tempAtt=NULL;
      (*ind)++;
      break;
    }
    m_StrF.strAdd((mystr*)&tempAtt,(mystr)&data[*ind],0,1);
    (*ind)++;
  }

 }
 if(tempAtt!=NULL)free(tempAtt);
 return natt;
}

char *cXmlParser::getTagValue(const char *data,int *ind){
  char *ret=NULL;
  if(data[*ind]==CXMLPARSER_ENDTAG)(*ind)++;
  while(data[*ind]!=STRING_END && data[*ind]!=CXMLPARSER_OPENTAG_INIT){
    m_StrF.strAdd((mystr*)&ret,(mystr)&(data[*ind]),0,1);
  (*ind)++;
  }
  sendDebugMessage(__func__,"TagValue",ret);
  return ret;
}

void cXmlParser::getTagEnd(xml_StructInfo*strRet,const char *data,int*ind){
  char *closetag=NULL;
  xml_StructInfo*infoRef,*fatherRef;
  while(data[*ind]!=STRING_END && data[*ind]!=CXMLPARSER_ENDTAG){
    m_StrF.strAdd((mystr*)(&closetag),(mystr)&(data[*ind]),0,1);
    (*ind)++;
  }
  infoRef=strRet;
  while(infoRef!=NULL){
    if(m_StrF.strCompare(infoRef->name,closetag)){
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

 }


void cXmlParser::Print(const xml_StructInfo*info){
  if(info==NULL)return;
  xml_StructInfo *base=(xml_StructInfo *)info;
  while(1){
    printf("TagName: %s\n",base->name);
    printf("nAttributes %d\n",base->nAttributes);
    for(int i=0;i<base->nAttributes;i++){
      printf("AttName:%s AttValue:%s",base->attributes[i],base->attributesValue[i]);
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

void cXmlParser::sendDebugMessage(const char*fName,const char*message1=NULL,const char*message2=NULL){
  #ifndef CXMLPARSER_DEBUG
   return;
  #endif
  printf("Function Name: %s Message1: %s Message2: %s\n",fName,message1,message2);

}
#endif
