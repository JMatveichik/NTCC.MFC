#include "stdafx.h"


//Copyright (C) 1998-2010 MKTMK Moscow, Russia
//e-mail: mktmk<at>narod.ru
//interpreter Open Basic (OB).

#include "ob.h"

#ifdef OB_WCHAR
namespace ob_wcharspace{
#else
namespace ob_charspace{
#endif

//here is function depend on OB_WCHAR flag

#ifdef OB_WCHAR

void wchar2char(ob_type_string_char_small* sfn,ob_type_char* name){
ob_type_stringsize l=ob_strlen(name);
sfn->recount(l);
if(wcstombs(sfn->c_str(),name,l)==-1) OB_ERR(ob_err82)
}

void char2wchar(ob_type_string* sfn,ob_type_small_char* name){
ob_type_stringsize l=ob_strlen_small(name);
sfn->recount(l);
if(mbstowcs(sfn->c_str(),name,l)==-1) OB_ERR(ob_err83)
}

#endif

ob_input_file::ob_input_file(
ob_type_char* name,
ios::openmode mode,
ob_type_filenum num,
ob_modestream ms):ob_file_base(num){

#ifndef OB_WCHAR

file.open(name,mode);

#else

ob_type_string_char_small sfn;
wchar2char(&sfn,name);
file.open(sfn.c_str(),mode);

#endif

iss.set_stream_with_startposition(&file);
*((ob_modestream*)(&iss))=ms;
}

ob_output_file::ob_output_file(
ob_type_char* name,
ios::openmode mode,
ob_type_filenum num,
ob_modestream ms):ob_file_base(num){

#ifndef OB_WCHAR

file.open(name,mode);

#else

ob_type_string_char_small sfn;
wchar2char(&sfn,name);
file.open(sfn.c_str(),mode);

#endif

oss.set_stream_with_startposition(&file);
*((ob_modestream*)(&oss))=ms;
}

//for version 1.90
//ob_iostream class hierarchy
//for support UTF-8 and UNICODE input-output stream

const ob_type_small_char ob_stream_base::utf8_codeprefix[]={
(const ob_type_small_char)0x00,
(const ob_type_small_char)0xc0,
(const ob_type_small_char)0xe0,
(const ob_type_small_char)0xf0,
(const ob_type_small_char)0xf8,
(const ob_type_small_char)0xfc
};

const ob_type_small_char ob_stream_base::utf8_codemask[]={
~0x7f,~0x1f,~0x0f,~0x07,~0x03,~0x01};

const ob_type_small_char ob_stream_base::utf8_codemask_inverse[]={
 0x7f, 0x1f, 0x0f, 0x07, 0x03, 0x01};

const ob_type_small_char ob_stream_base::add_code_prefix=(const ob_type_small_char)0x80;
const ob_type_small_char ob_stream_base::add_code_mask=(const ob_type_small_char)0xc0;
const ob_type_small_char ob_stream_base::add_code_mask_inverse=(const ob_type_small_char)(~0xc0);

const ob_type_small_char ob_stream_base::utf8_shiftvalue=6;

const ob_type_wide_char* ob_stream_base::code_bound=
ob_wide_char_mask<sizeof(ob_type_wide_char)>::array;

//conversion wchar stream to char stream for wchar version
//for example create wchar name and load it to ob_lex as input stream
//or for ob_obasic::load

void ob_wstream_to_stream(ob_shortstrstream* datastream,ob_standartstring* st){

ob_type_data_operator_length l=sizeof(ob_type_char)*st->length();

const
ob_type_small_char* ptr_small_char=(const ob_type_small_char*)(st->c_str());

ob_type_data_operator_length j;
for(j=0;j!=l;++j){
*datastream<<ptr_small_char[j];
}//for

}

//for version 1.90
ob_modestream ob_obasic::get_modestream(namestream ns){

ob_modestream ms;

switch(ns){
case STREAM_BASE           :ms=*((ob_modestream*)baselex.get_input_stream());break;
case STREAM_READ           :ms=*((ob_modestream*)readlex.get_input_stream());break;
case STREAM_READ_FROM_FILE :ms=*((ob_modestream*)read_from_file_lex.get_input_stream());break;
case STREAM_DATA           :ms=*((ob_modestream*)datalex.get_input_stream());break;
case STREAM_VAR_ACCESS     :ms=(ob_modestream)modestream_for_var_access_lex;break;
case STREAM_OUT            :ms=(ob_modestream)out;break;
}//switch

return ms;
}

void ob_obasic::set_modestream(namestream ns,ob_modestream ms){

switch(ns){
case STREAM_BASE           :*((ob_modestream*)baselex.get_input_stream())=ms;break;
case STREAM_READ           :*((ob_modestream*)readlex.get_input_stream())=ms;break;
case STREAM_READ_FROM_FILE :*((ob_modestream*)read_from_file_lex.get_input_stream())=ms;break;
case STREAM_DATA           :*((ob_modestream*)datalex.get_input_stream())=ms;break;
case STREAM_VAR_ACCESS     :(ob_modestream)modestream_for_var_access_lex=ms;break;
case STREAM_OUT            :(ob_modestream)out=ms;break;
}//switch

}

#ifdef OB_WCHAR
}//end namespace ob_wcharspace
#else
}//end namespace ob_charspace
#endif

