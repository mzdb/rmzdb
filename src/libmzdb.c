#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <math.h>
#include <time.h>

#include "libmzdb.h"

/**
 * `asprintf.c' - asprintf
 *
 * Source: https://github.com/littlstar/asprintf.c/blob/master/asprintf.c
 *
 * copyright (c) 2014 joseph werle <joseph.werle@gmail.com>
 */

#ifndef HAVE_ASPRINTF

int
asprintf (char **str, const char *fmt, ...)
{
  int size = 0;
  va_list args;

  // init variadic arguments
  va_start(args, fmt);

  // format and get size
  size = vasprintf(str, fmt, args);

  // toss args
  va_end(args);

  return size;
}

int
vasprintf (char **str, const char *fmt, va_list args)
{
  int size = 0;
  va_list tmpa;

  // copy
  va_copy(tmpa, args);

  // apply variadic arguments to
  // sprintf with format to get size
  size = vsnprintf(NULL, size, fmt, tmpa);

  // toss args
  va_end(tmpa);

  // return -1 to be compliant if
  // size is less than 0
  if (size < 0) { return -1; }

  // alloc with size plus 1 for `\0'
  *str = (char *) malloc(size + 1);

  // return -1 to be compliant
  // if pointer is `NULL'
  if (NULL == *str) { return -1; }

  // format string with original
  // variadic arguments and set new size
  size = vsprintf(*str, fmt, args);
  return size;
}

#endif

log_level_enum get_log_level(void)
{
    return _LIBMZDB_LOGGING_DATA.min_log_level;
}

void set_log_level(log_level_enum lvl)
{
    _LIBMZDB_LOGGING_DATA.min_log_level = lvl;
}

FILE* get_log_file()
{
    return _LIBMZDB_LOGGING_DATA.file;
}

void set_log_file(FILE* file)
{
    _LIBMZDB_LOGGING_DATA.file = file;
}

char* time_to_str()
{
    /*const size_t str_time_size = 256;
    char* str_time = (char*) malloc(sizeof(char)*str_time_size);
    time_t now = time(NULL);
    strftime(str_time, str_time_size, "%c", localtime(&now));*/

    char str_time[1024];
    time_t now = time(NULL);
    strftime(str_time, 1024, "%c", localtime(&now));

    return str_time;
}

// Source: http://stackoverflow.com/questions/6508461/logging-library-for-c
// TODO: implement log_gile function?
// Source: https://github.com/rxi/log.c/blob/master/src/log.c
int flog(log_level_enum lvl, const char* message) {
    if(lvl <= _LIBMZDB_LOGGING_DATA.min_log_level || !_LIBMZDB_LOGGING_DATA.file) return 0;

    char* str_time = time_to_str();
    if(_LIBMZDB_LOGGING_DATA.file == stdout && lvl >= LOG_WARN)
    {
        return fprintf(stderr, LOG_TEMPLATE, LOG_LEVELS[lvl], str_time, message, "\n");
    }
    else
    {
        int rc = fprintf(_LIBMZDB_LOGGING_DATA.file, LOG_TEMPLATE, LOG_LEVELS[lvl], str_time, message, "\n");
        fflush(_LIBMZDB_LOGGING_DATA.file);

        return rc;
    }
}

int flogf(log_level_enum lvl, const char* msgfmt, ...)
{
  // Return if logging level is too low
  if(lvl <=  _LIBMZDB_LOGGING_DATA.min_log_level || !_LIBMZDB_LOGGING_DATA.file) return 0;

  int size = 0;
  va_list args;
  char* message;

  // init variadic arguments
  va_start(args, msgfmt);

  // format and get size
  vasprintf(&message, msgfmt, args);

  // toss args
  va_end(args);

  // log message
  size = flog(lvl, message);

  // free memory
  if (size >= 0)
  {
    free(message);
  }

  return size;
}


/* OLD IMPLEMENTATIONS
int flog(log_level_enum lvl, const char* message)
{
  // Return if logging level is too low
  if (lvl <= MIN_LOGGING_LEVEL) return 0;

  ///time_t now;
  //time(&now);
  //printf(LOG_TEMPLATE, LOG_LEVELS[lvl], ctime(&now), message);

  char timestr[1000];
  time_t now = time(NULL);
  struct tm * p = localtime(&now);
  strftime(timestr, 1000, "%c", p);

  if (lvl >= LOG_WARN)
  {
    return fprintf(stderr,LOG_TEMPLATE, LOG_LEVELS[lvl], timestr, message,"\n");
  }
  else
  {
    int rc = fprintf(stdout,LOG_TEMPLATE, LOG_LEVELS[lvl], timestr, message,"\n");
    fflush(stdout); // we have to flush stdout because it is buffered (note that stderr is unbuffered)
    return rc;
  }
}

*/


/**
 * `asprintf.c' - asprintf
 *
 * Source: https://github.com/littlstar/asprintf.c/blob/master/asprintf.c
 *
 * copyright (c) 2014 joseph werle <joseph.werle@gmail.com>
 */

char * libmzdb_strdup(const char * str_src)
{
    char * str_dest = (char *) malloc(sizeof(char) * strlen(str_src));
    strcpy(str_dest, str_src);
    return str_dest;
}

//TODO
/**
 * @brief strs_concat concatenate n char*
 * @param count the number of char*
 * @return a char * or NULL if an error occured
 */
const char* concat_fast(int count, ...)
{
    va_list va;
    if (count<1) return NULL; //no char* to concatenate

    int* arg_pos=(int *) malloc(sizeof(int)*(count+1)); //the array of each arg position in the new char *

    va_start(va, count);
    arg_pos[0]= 0;
    for(int i=1; i<count; ++i)arg_pos[i]= arg_pos[i-1]+strlen(va_arg(va, char*));
    va_end(va);

    //copy the arg in the variable out at the right position
    char* out =(char *) calloc(sizeof(char), arg_pos[count]+1);
    va_start(va, count);
    for(int i=0; i<count; ++i) strcpy(out+arg_pos[i], va_arg(va, const char*));
    va_end(va);
    free(arg_pos);
    return out;
}

/**
 * @brief strrep replace each occurance of targer with by
 * @param str the str to edit
 * @param target the char to replace
 * @param by the new char
 * @return the replaced str
 */
char* strrep(char* str, char target, char by)
{
    int len = strlen(str);
    for(int i=0; i<len; ++i) if (str[i]=target) str[i]=by;
    return str;
}

/**
 * @brief atob char* to c-boolean
 * @param str the string
 * @return return 0 if the str equals to "true" else false
 */
int atob(char* str)
{
    return strcmp(str, "true") == 0;
}

/*int _btoi(char* bytes)
{
    int out = bytes[0] | ( (int)bytes[1] << 8 ) | ( (int)bytes[2] << 16 ) | ( (int)bytes[3] << 24 );
    return out;
}*/

// Source: http://stackoverflow.com/questions/8465006/how-to-concatenate-2-strings-in-c
const char* concat(int count, ...)
{
    va_list ap;
    int i;
    int len;
    const char *merged;
    int null_pos;

    if (count > MAX_CONCAT_STR_COUNT)
    {
        fputs("concat (util.c): too many strings to concat !", stderr);
        return NULL;
    }

    // Find required length to store merged string
    len = 1;   // room for NULL
    va_start(ap, count);
    for (i = 0; i < count; i++)
    {
        len += strlen(va_arg(ap, char*));
    }
    va_end(ap);

    // Allocate memory to concat strings
    merged = (const char*)calloc(sizeof(char), len);
    null_pos = 0;

    // Actually concatenate strings
    va_start(ap, count);
    char* s;
    for (i = 0; i < count; i++)
    {
        s = va_arg(ap, char*);
        strcpy(merged + null_pos, s);
        null_pos += strlen(s);
    }
    va_end(ap);

    return merged;
}

char *chngChar(char *str, char oldChar, char newChar)
{
    char *strPtr = str;

    while ((strPtr = strchr(strPtr, oldChar)) != NULL)
        *strPtr++ = newChar;

    return str;
}

//duplicates a string with memory allocation
/*void strmemdup(char** dest, const char *str) {
    size_t len;

    len = strlen(str) + 1;
    if (!(*dest = malloc(sizeof(char) * len)))
        return;

    memcpy(*dest, str, len);
   }*/



void xml_browse_rec(nodec_ptr_t node, int level, void (* callback) (nodec_ptr_t node, int level, void *), void* user_param)
{
  if(node&&callback) callback(node, level, user_param);
  if(node->next) xml_browse_rec(node->next, level, callback, user_param);
  if(node->numchildren) xml_browse_rec(node->firstchild, level+1, callback, user_param );
}

void xml_browse(nodec_ptr_t root, void (* callback) (nodec_ptr_t node, int level, void *), void* user_param)
{
  if(root) xml_browse_rec(root, 0, callback, user_param);
}

int get_attribute_value_rec(attc_ptr_t attribute, char* attribute_name, char attribute_value[2048])
{
  if(!attribute) return 0;
  if(!strncmp(attribute_name, attribute->name, attribute->namelen))
  {
    strncpy(attribute_value, attribute->name, attribute->namelen);
    attribute_value[attribute->vallen]= 0;
    return 1;
  }
  return get_attribute_value_rec(attribute->next, attribute_name, attribute_value);
}

char* get_attribute_value(nodec_ptr_t node, char* attribute_name)
{
  static char attribute_value[2048];
  if (!node || !node->firstatt ) return NULL;
  if (get_attribute_value_rec(node->firstatt, attribute_name, attribute_value)) return attribute_value;
  return NULL;
}

int get_attribute_value_as_float(nodec_ptr_t node, char* attribute_name, float* result)
{
  char* attr = get_attribute_value(node, attribute_name);
  if(!attr) return -1;
  *result = atof(attr);
  return 0;
}

int get_attribute_value_as_int(nodec_ptr_t node, char* attribute_name, int* result)
{
  char* attr = get_attribute_value(node, attribute_name);
  if(!attr) return -1;
  *result = atoi(attr);
  return 0;
}

void _add_brothers_rec(nodec_ptr_t** children, nodec_ptr_t node, int children_count)
{
  if(node->next)
  {
    (*children)[children_count] = node->next;
    _add_brothers_rec(children, node->next, children_count+1);
  }
}

nodec_ptr_t* get_children(nodec_ptr_t node)
{
  if(!node || !node->numchildren) return NULL;
  nodec_ptr_t* children = (nodec_ptr_t*) malloc(sizeof(nodec_ptr_t)*node->numchildren);
  children[0] = node->firstchild;
  _add_brothers_rec(&children, node->firstchild, 1);
  return children;
}

void _add_attributes_brother_rec(attc_ptr_t** attributes, attc_ptr_t attr, int attribute_count)
{
  if(attr->next)
  {
    (*attributes)[attribute_count] = attr->next;
    _add_attributes_brother_rec(attributes, attr->next, attribute_count+1);
  }
}

attc_ptr_t* get_attribute(nodec_ptr_t node)
{
  if(!node || !node->numatt) return NULL;
  attc_ptr_t* attrs = (attc_ptr_t*) malloc(sizeof(attc_ptr_t)*node->numatt);
  attrs[0] = node->firstatt;
  _add_attributes_brother_rec(&attrs, node->firstatt, 1);
  return attrs;
}

void fprintf_attribute(FILE* f, attc_ptr_t attr, int level)
{
  fprintf(f, "%.*s************Attribute*******\n", level * 2, "                                                                    ");
  fprintf(f, "%.*s", level * 2, "                                                                    ");
  fprintf(f, "name : '%.*s'\n", attr->namelen, attr->name);
  fprintf(f, "%.*s", level * 2, "                                                                    ");
  fprintf(f, "value : '%.*s'\n", attr->vallen, attr->value);
}


void fprintf_node_rec(FILE *f, nodec_ptr_t node, int level)
{
  nodec_ptr_t *children;
  attc_ptr_t *attributes;
  if (node)
  {
    fprintf(f, "%.*s------------------------------------------------------------------\n", level * 2, "                                                                    ");
    fprintf(f, "%.*s", level * 2, "                                                                    ");
    fprintf(f, "name : '%.*s'\n", node->namelen, node->name);
    fprintf(f, "%.*s", level * 2, "                                                                    ");
    fprintf(f, "comment : '%.*s'\n", node->comlen, node->comment);
    fprintf(f, "%.*s", level * 2, "                                                                    ");
//    fprintf(f, "curchild : %X\n", node->curchild);

    //    fprintf(f,"%.*s", level*2, "                                                                    ");
    //    fprintf(f,"firstchild : %X   lastchild : %X\n", node->firstchild, node->lastchild);
    //    fprintf(f,"diff : %ld   struct : %d\n", ((long)( node->lastchild))- ((long)(node->firstchild)), sizeof(struct nodec));

    fprintf(f, "%.*s", level * 2, "                                                                    ");
    fprintf(f, "nb attrs : %d\n", node->numatt);

    fprintf(f, "%.*s", level * 2, "                                                                    ");
    fprintf(f, "nb vals : %d\n", node->numvals);

    fprintf(f, "%.*s", level * 2, "                                                                    ");
    fprintf(f, "value : '%.*s'\n", node->vallen, node->value);

    fprintf(f, "%.*s", level * 2, "                                                                    ");
    fprintf(f, "nb children : %d\n", node->numchildren);

    fprintf(f, "%.*s", level * 2, "                                                                    ");
    fprintf(f, "nb comments : %d\n", node->numcoms);

//    fprintf(f, "%.*s", level * 2, "                                                                    ");
//    fprintf(f, "next : %X\n", node->next);


    attributes = get_attribute(node);
    for (int n = 0; n < node->numatt; ++n) fprintf_attribute(f, attributes[n], level);
    free(attributes);

    children = get_children(node);
    for (int n = 0; n < node->numchildren; ++n) fprintf_node_rec(f, children[n], level+1);
    free(children);
  }
}

void fprintf_node (FILE* f, nodec_ptr_t node)
{
  fprintf_node_rec(f, node, 0);
}

void print_xml_bare(char* text)
{
  struct parserc parser;
  int err = parserc_parse(&parser, text); //err = 0 if no error
  if(!err) fprintf_node(stdout, parser.rootnode);
}


int xml_parse(char* text, nodec_ptr_t * rootnode)
{
  struct parserc parser;
  int err = parserc_parse(&parser, text);
  if(err)
  {
    flogf(LOG_ERROR, "something wrong happened (error code=%d) while parsing XML chunk '%s", err, text);
    return -1;
  }
  *rootnode = parser.rootnode;
  return 0;
}

void xml_browse_callback_mzdb_param_tree(nodec_ptr_t node, int level, void* user_param)
{
  libmzdb_mzdb_param_tree_t* tree = (libmzdb_mzdb_param_tree_t*) user_param;
  if (strncmp(node->name, "userParam", node->namelen)) return;

  char* value = get_attribute_value(node, "name");
  if (!value) return;

  if      (!strcmp(value, "ms1_bb_mz_width"))   tree->ms1_bb_mz_width     = atof(get_attribute_value(node, "value"));
  else if (!strcmp(value, "msn_bb_mz_width"))   tree->msn_bb_mz_width     = atof(get_attribute_value(node, "value"));
  else if (!strcmp(value, "ms1_bb_time_width")) tree->ms1_bb_time_width   = atof(get_attribute_value(node, "value"));
  else if (!strcmp(value, "msn_bb_time_width")) tree->msn_bb_time_width   = atof(get_attribute_value(node, "value"));
  else if (!strcmp(value, "is_lossless"))       tree->is_loss_less       = (atob(get_attribute_value(node, "value"))) ? 1 : 0; //see utils.h
  else if (!strcmp(value, "origin_file_format"))tree->origin_file_format = get_attribute_value(node, "value");
}

























































///** recursive xml traversal with callback on all nodes */
//static void xmlBrowseRec( nodecPtr node, int level, void (*callback)(nodecPtr node, int level, void *), void *userParam) {
////    printf("browse node %.*s\n", node->namelen, node->name);   fflush(stdout);

//    if ( node && callback ) callback( node, level, userParam);
//    if ( node->next ) {
//        xmlBrowseRec( node->next, level, callback, userParam);
//    }
//    if ( node->numchildren) {
//        xmlBrowseRec(node->firstchild,level+1, callback, userParam);
//    }
//}


///** xml trversal with callback on all nodes */
//void xmlBrowse( nodecPtr root, void (*callback)(nodecPtr, int, void *), void *userParam) {
//    if ( !root ) return;
//    xmlBrowseRec( root, 0, callback, userParam);
//}

///** get the attribute value matching attributeName */
//static int getAttributeValueRec(attcPtr attribute, char *attributeName, char attributeValue[2048]) {
//    if ( !attribute) return 0;

//    if ( !strncmp(attributeName, attribute->name, attribute->namelen) ) {
//        strncpy(attributeValue,attribute->value, attribute->vallen);
//        attributeValue[attribute->vallen] = 0;
//        return 1;
//    }
//    return getAttributeValueRec(attribute->next, attributeName, attributeValue);
//}

///** get the attribute value matching attributeName. Static char returned: Do not need to free memory */
//char *getAttributeValue(nodecPtr node, char *attributeName) {
//    static char attributeValue[2048];
//    if ( !node ) return NULL;
//    if ( !node->firstatt )  return NULL;
//    if ( getAttributeValueRec(node->firstatt, attributeName, attributeValue) ) return attributeValue;
//    return NULL;
//}

///** get the attribute value matching attributeName as float */
////TODO: gestion d'erreurs ...
//float getAttributeValueAsFloat(nodecPtr node, char *attributeName) {
//    char *attributeValue = getAttributeValue(node, attributeName);
//    if (attributeValue) return atof( attributeValue );
//}

//int getAttributeAsInt(nodecPtr node, char *attributeName){
//    char *attributeValue = getAttributeValue(node, attributeName);
//    if(attributeValue) return atoi( attributeValue );
//}

///* fill the children array */
//void _addBrothersRec(nodecPtr **children, nodecPtr node, int nbChildren)
//{
//  if (!node->next)
//    {
//      return;
//    }
//  (*children)[nbChildren] = node->next;
//  _addBrothersRec(children, node->next, nbChildren + 1);
//}

///* get an array containing all children (and allocate array memory to be freed)*/
//nodecPtr* getChildren(nodecPtr node)
//{
//  nodecPtr *children;

//  if (!node || !node->numchildren)
//    return NULL;
//  children = (nodecPtr*)malloc(sizeof(nodecPtr) * node->numchildren);   // allocate children
//  children[0] = node->firstchild;
//  _addBrothersRec(&children, node->firstchild, 1);
//  return children;
//}

///* fill the attributes array */
//void _addAttributesBrotherRec(attcPtr **attributes, attcPtr attr, int nbAttributes)
//{
//  if (!attr->next)
//    {
//      return;
//    }
//  (*attributes)[nbAttributes] = attr->next;
//  _addAttributesBrotherRec(attributes, attr->next, nbAttributes + 1);
//}

///* get an array containing all attributes (and allocate array memory to be freed)*/
//attcPtr* getAttributes(nodecPtr node)
//{
//  attcPtr *attributes;

//  if (!node || !node->numatt)
//    return NULL;
//  attributes = (attcPtr*)malloc(sizeof(attcPtr) * node->numatt);   // allocate attributes
//  attributes[0] = node->firstatt;
//  _addAttributesBrotherRec(&attributes, node->firstatt, 1);
//  return attributes;
//}






//void fprintfAttribute(FILE *f, attcPtr attr, int level)
//{
//  fprintf(f, "%.*s************Attribute*******\n", level * 2, "                                                                    ");
//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "name : '%.*s'\n", attr->namelen, attr->name);
//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "value : '%.*s'\n", attr->vallen, attr->value);
//}

//void fprintfNodeRec(FILE *f, nodecPtr node, int level)
//{
//  int n = 0;
//  nodecPtr *children;
//  attcPtr *attributes;

//  if (!node)
//    return;

//  fprintf(f, "%.*s------------------------------------------------------------------\n", level * 2, "                                                                    ");
//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "name : '%.*s'\n", node->namelen, node->name);
//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "comment : '%.*s'\n", node->comlen, node->comment);
//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "curchild : %X\n", node->curchild);

//  //    fprintf(f,"%.*s", level*2, "                                                                    ");
//  //    fprintf(f,"firstchild : %X   lastchild : %X\n", node->firstchild, node->lastchild);
//  //    fprintf(f,"diff : %ld   struct : %d\n", ((long)( node->lastchild))- ((long)(node->firstchild)), sizeof(struct nodec));

//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "nb attrs : %d\n", node->numatt);

//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "nb vals : %d\n", node->numvals);

//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "value : '%.*s'\n", node->vallen, node->value);

//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "nb children : %d\n", node->numchildren);

//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "nb comments : %d\n", node->numcoms);

//  fprintf(f, "%.*s", level * 2, "                                                                    ");
//  fprintf(f, "next : %X\n", node->next);


//  attributes = getAttributes(node);
//  for (n = 0; n < node->numatt; n++)
//    {
//      fprintfAttribute(f, attributes[n], level);
//    }
//  free(attributes);

//  children = getChildren(node);
//  for (n = 0; n < node->numchildren; n++)
//    {
//      fprintfNodeRec(f, children[n], level + 1);
//    }
//  free(children);
//}



//void fprintfNode(FILE *f, struct nodec *node)
//{
//  fprintfNodeRec(f, node, 0);
//}

////char* text= "<CATALOG><CD><TITLE>Empire Burlesque</TITLE><ARTIST>Bob Dylan</ARTIST><COUNTRY>USA</COUNTRY>";

/////TODO: change name (display ?)
//void testXmlBare(char* text)
//{
//  struct parserc *parser = (struct parserc *)calloc(1, sizeof(struct parserc));
//  int err;

//  err = parserc_parse(parser, text);

//  fprintfNode(stdout, parser->rootnode);

//  /* Below printfs are only temp vars
//     printf( "attname: %s\n", parser->attname );
//     printf( "attname len: %d\n", parser->attname_len );
//     printf( "attval: %s\n", parser->attval );
//     printf( "attval len: %d\n", parser->attval_len );
//     printf( "att_has val: %d\n", parser->att_has_val );
//     //printf( "curatt: %d\n", parser->curatt );
//     //printf( "curnode: %d\n", parser->curnode );
//     printf( "err: %d\n", parser->err );
//     printf( "last state: %d\n", parser->last_state );
//     //printf( "rootnode: %d\n", parser->rootnode );
//     printf( "rootpos: %s\n", parser->rootpos );
//     printf( "tagname: %s\n", parser->tagname );
//     printf( "tagname len: %d\n", parser->tagname_len );
//     //printf( "last state: %s\n", parser->rootnode-> );
//   */

//  free(parser);
//}



//nodecPtr xmlParse(char* text)
//{
//  struct parserc *parser = (struct parserc *)calloc(1, sizeof(struct parserc));
//  nodecPtr root = NULL;
//  int errCode;

//  errCode = parserc_parse(parser, text);
//  //TODO: Check error code
//  if (parser == NULL)
//    {
//      _logf(LOG_ERROR,"something wrong happened (error code=%d) while parsing XML chunk '%s'", errCode, text);

//      return NULL;
//    }


//  root = parser->rootnode;
//  free(parser);
//  return root;
//}

//void xmlBrowseCallbackMzdbParamTree(nodecPtr node, int level, void *userParam)
//{
//  float value;
//  char *result;
//  char * attributeValue;

//  MzdbParamTree *mzdbParamTree = (MzdbParamTree*)userParam;

//  if (!strncmp(node->name, "userParam", node->namelen))
//    {
//      attributeValue = getAttributeValue(node, "name");    // ms1_bb_mz_width
//      if (attributeValue && !strcmp(attributeValue, "ms1_bb_mz_width"))
//        {
//          value = getAttributeValueAsFloat(node, "value");
//          //printf("Attribute Value ms1_bb_mz_width = %f type : %s\n", value , getAttributeValue(node, "type" ) );
//          mzdbParamTree->ms1BbMzWidth = getAttributeValueAsFloat(node, "value");
//        }
//      else
//        {
//          if (attributeValue && !strcmp(attributeValue, "msn_bb_mz_width"))
//            {
//              value = getAttributeValueAsFloat(node, "value");
//              //printf("Attribute Value msn_bb_mz_width = %f type : %s\n", value , getAttributeValue(node, "type" ) );
//              mzdbParamTree->msnBbMzWidth = getAttributeValueAsFloat(node, "value");
//            }
//          else
//            {
//              if (attributeValue && !strcmp(attributeValue, "ms1_bb_time_width"))
//                {
//                  value = getAttributeValueAsFloat(node, "value");
//                  //printf("Attribute Value ms1_bb_time_width = %f type : %s\n", value , getAttributeValue(node, "type" ) );
//                  mzdbParamTree->ms1BbTimeWidth = getAttributeValueAsFloat(node, "value");
//                }
//              else
//                {
//                  if (attributeValue && !strcmp(attributeValue, "msn_bb_time_width"))
//                    {
//                      value = getAttributeValueAsFloat(node, "value");
//                      // printf("Attribute Value msn_bb_time_width = %f type : %s\n", value , getAttributeValue(node, "type" ) );
//                      mzdbParamTree->msnBbTimeWidth = getAttributeValueAsFloat(node, "value");
//                    }
//                  else
//                    {
//                      if (attributeValue && !strcmp(attributeValue, "is_lossless"))
//                        {
//                          //printf("Attribute Value isLossLess = %s\n", getAttributeValue(node,"value"));
//                          result = getAttributeValue(node, "value");
//                          if (strcmp(result, "true") == 0)
//                            {
//                              mzdbParamTree->isLossless = 1;
//                            }
//                          else
//                            {
//                              mzdbParamTree->isLossless = 0;
//                            }
//                        }
//                      else
//                        {
//                          if (attributeValue && !strcmp(attributeValue, "origin_file_format"))
//                            {
//                              //printf("Attribute Value originFileFormat = %s\n", getAttributeValue(node,"value"));
//                              mzdbParamTree->originFileFormat = getAttributeValue(node, "value");
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
//}




/*TODO*/

/*************** PREPARED STATEMENTS EXECUTION ******************/

int libmzdb_execute_prepared_stmt(sqlite3 *db, sqlite3_stmt *stmt, int index, void(*callback)(sqlite3_stmt*, int, long, void*), long size_in_bytes, void *result, char **err_msg)
{
    int stmt_step_res; //step result state
    while ( (stmt_step_res = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        callback(stmt, index, size_in_bytes, result);
    }

    if (stmt_step_res != SQLITE_DONE) *err_msg = (char*)sqlite3_errmsg(db);

    return stmt_step_res;
}

int libmzdb_execute_select_stmt_for_all_types(sqlite3 *db, const char *sql, int index, void(*callback)(sqlite3_stmt*, int, long, void*), long size_in_bytes, void **result, char **err_msg)
{
    sqlite3_stmt *stmt;
    int rc /*sql result state*/, stmt_step_res /*step result state*/;

    //*err_msg = "Error (libmzdb_execute_select_stmt_for_all_types): sql prepare failed";

    if ((rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL)) != SQLITE_OK)
    {
        //        printf("%d, %d", rc, SQLITE_OK);
        //        printf("%s", sqlite3_errmsg(db));
        *err_msg = (char*)sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        return rc;
    }

    stmt_step_res = libmzdb_execute_prepared_stmt(db, stmt, index, callback, size_in_bytes, result, err_msg);

    sqlite3_finalize(stmt);

    return (stmt_step_res != SQLITE_DONE) ? stmt_step_res : rc;
}

int libmzdb_execute_select_stmt(sqlite3 *db, const char *sql, int index, void (*callback)(sqlite3_stmt*, int, long, void*), void**result, char**err_msg)
{
    return libmzdb_execute_select_stmt_for_all_types(db, sql, index, callback, 1, result, err_msg);
}

int libmzdb_execute_select_first_field_stmt(sqlite3* db, const char* sql, void (*callback)(sqlite3_stmt*, int, long, void*), void*result, char** err_msg)
{
    return libmzdb_execute_select_stmt_for_all_types(db, sql, 0, callback, 1, result, err_msg);
}

/*************** CALLBACKS ******************/

void libmzdb_get_string_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result) //size in bytes unused
{
    const char * str = libmzdb_strdup((const char *) sqlite3_column_text(stmt, index));
    *((void**)result) = (void*) str;
}

void libmzdb_get_double_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result)
{
    *(double*)result = sqlite3_column_double(stmt, index);
}

void libmzdb_get_float_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result)
{
    double d = sqlite3_column_double(stmt, index);
    if (d > FLT_MAX)
    {
        printf("libmzdb error: floating overflow !");
        exit(EXIT_FAILURE);
    }
    *(float*) result = (float)d;
}

void libmzdb_get_int_cb(sqlite3_stmt *stmt, int index, long size_in_bytes, void *result)
{
    *(int*)result = sqlite3_column_int(stmt, index);
}


void libmzdb_get_n_int_cb(sqlite3_stmt *stmt, int index, long size_in_bytes, void *result)
{
    int* arr= (int*) malloc(sizeof(int)*index);
    for (int i= 0; i<index; ++i) arr[i]= sqlite3_column_int(stmt, i);
    result= (void *) arr;
}


/* The pointer returned by sqlite3_column_blob() points to memory
** that is owned by the statement handle (pStmt). It is only good
** until the next call to an sqlite3_XXX() function (e.g. the
** sqlite3_finalize() below) that involves the statement handle.
** So we need to make a copy of the blob into memory obtained from
** malloc() to return to the caller.
*/
// FIXME: WTF!
void libmzdb_get_blob_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result)
{
    memcpy( *(void**)result, sqlite3_column_blob(stmt, index), size_in_bytes);
}

/*void get_blob_cb(sqlite3_stmt* stmt, int index, void* result)
{
    TODO: malloc a blob_result_t
    memcpy( *(void**)result, sqlite3_column_blob(stmt, index), sqlite3_column_bytes(stmt, index));
}*/

void libmzdb_get_int_64_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result)
{
    *(sqlite3_int64*)result = sqlite3_column_int64(stmt, index);
}

/*************** UTILS ******************/

void libmzdb_die_on_sqlite_error(int rc, char** err_msg)
{
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Exiting program because of SQLite error: %s\n", *err_msg);
        sqlite3_free(err_msg);
        exit(EXIT_FAILURE);
    }
}

/*end TODO*/

void libmzdb_dump_blob_content(unsigned char* blob, int blob_size)
{
    printf("blob content: ");
    for (int i=0; i<blob_size; ++i)
      printf("%d ",blob[i]);

    printf("\n");
}
















///*************** PREPARED STATEMENTS EXECUTION ******************/

///*

//int executeSelectFirstFieldStmt(
//    sqlite3 *db,
//    const char *sql,
//    void (*resultCallback)(sqlite3_stmt*, int, long, void*),
//    void *cbRes,
//    char **errMsg
//    )
//{
//  return executeSelectNStmt(
//        db,
//        sql,
//        0,
//        resultCallback,
//        1,
//        cbRes,
//        errMsg
//        );
//}

////Execute the statement previously prepared
//int executeSelectStmt(
//    sqlite3 *db,
//    const char *sql,
//    int index,
//    void (*resultCallback)(sqlite3_stmt*, int, long, void*),
//    void *cbRes,
//    char **errMsg
//    )
//{
//  return executeSelectNStmt(
//        db,
//        sql,
//        index,
//        resultCallback,
//        1,
//        cbRes,
//        errMsg
//        );
//}



////Prepare and check is the required statement is OK
//int executeSelectNStmt(
//    sqlite3 *db,
//    const char *sql,
//    int index,
//    void (*resultCallback)(sqlite3_stmt*, int, long, void*),
//    long sizeInBytes,
//    void *cbRes,
//    char **errMsg
//    )
//{
//  sqlite3_stmt *stmt;

//  // Prepare SQL query
//  int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

//  int stmtStepRc = 0;

//  // Check prepare is OK
//  if (rc == SQLITE_OK)
//  {
//    // Bind stmt here if required
//    //Example: rc = sqlite3_bind_int(stmt, 1, id);

//    // Execute prepared statement
//    stmtStepRc = executePreparedStmt(
//          db,
//          stmt,
//          index,
//          resultCallback,
//          sizeInBytes,
//          cbRes,
//          errMsg
//          );

//    if (stmtStepRc != SQLITE_DONE)
//    {
//      sqlite3_finalize(stmt);
//      return stmtStepRc;
//    }
//  }

//  sqlite3_finalize(stmt);

//  return rc;
//}





//// The return value should SQLITE_DONE if no error occured
//int executePreparedStmt(
//    sqlite3 *db,
//    sqlite3_stmt *stmt,
//    int index,
//    void (*resultCallback)(sqlite3_stmt*, int, long, void*),
//    long sizeInBytes,
//    void *cbRes,
//    char **errMsg
//    )
//{
//  int stmtStepRc = 0;

//  // While we obtain rows
//  while (1)
//  {
//    // Execute SQL statement
//    stmtStepRc = sqlite3_step(stmt);

//    // Check we obtain a row
//    if (stmtStepRc == SQLITE_ROW)
//    {
//      resultCallback(stmt, index, sizeInBytes, cbRes);
//      // Break if no more row and last call to step has completed normally
//    }
//    else if (stmtStepRc == SQLITE_DONE)
//    {
//      break;
//      // Else get last error message from DB
//    }
//    else
//    {
//      *errMsg = (char*)sqlite3_errmsg(db);
//      return stmtStepRc;
//    }
//  }

//  return stmtStepRc;
//}




////Statement Test On Bind That We May Need To Use Later
///*sqlite3_stmt *stmt;

//   // Prepare SQL query
//   int rc = sqlite3_prepare_v2(db, sqlString, -1, &stmt, NULL);

//   int stmtStepRc = 0;

//   // Check prepare is OK
//   if (rc == SQLITE_OK) {

//    // Bind msLevel here
//    rc = sqlite3_bind_int(stmt, 1, msLevel);

//    if (rc == SQLITE_OK) {

//        // Execute prepared statement
//        stmtStepRc = executePreparedStmt(
//            db,
//            stmt,
//            resultCallback,
//            cbRes,
//            errMsg
//        );

//        if (stmtStepRc != SQLITE_DONE) {
//            sqlite3_finalize(stmt);
//            return stmtStepRc;
//        }
//    }
//   }

//   sqlite3_finalize(stmt);

//*/

///*************** CALLBACKS ******************/

///*
//// Source: http://stackoverflow.com/questions/12984248/sqlite-in-c-c-sqlite3-exec-parameter-set-in-callback-function-is-pointing-to
//// Note: free must be called on result string by the caller
//void getStringCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
//{
//  *((char**)cbRes) = strdup(sqlite3_column_text(stmt, index));
//  //strmemdup((void **)cbRes, sqlite3_column_text(stmt, index));
//}

////return the first callback from the statement (case : we're working with double type)
//void getDoubleCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
//{
//  *((double*)cbRes) = sqlite3_column_double(stmt, index);
//}

////return the first callback from the statement (case : we're working with Float type)
//void getFloatCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
//{
//  double d = sqlite3_column_double(stmt, index);

//  if (d > FLT_MAX)
//  {
//    printf("libmzdb error: floating overflow !");
//    exit(EXIT_FAILURE);
//  }
//  *((float*)cbRes) = (float)d;
//}

////Get the first callback from the statement (case : we're working with Int type)
//void getIntCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
//{
//  int test = sqlite3_column_int(stmt, index);

//  *((int*)cbRes) = test;
//}
///*static void getIntCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
//{
//  int test = sqlite3_column_int(stmt, index);

//  *((int*)cbRes) = test;
//}

//*//*

//void getNIntCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
//{
//  int i;
//  int *test;

//  test = malloc(index * sizeof(int));
//  for (i = 0; i < index; i++)
//  {
//    test[i] = sqlite3_column_int(stmt, i);
//    *((int*)cbRes) = test[i];
//  }
//}

////return the first callback from the statement (case : we're working with blob type)
//void getBlobCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
//{
//  memcpy(*((void**)cbRes), sqlite3_column_blob(stmt, index), sizeInBytes);
//}

////return the first callback from the statement (case : we're working with int64 type)
//static void getInt64Cb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
//{
//  *((sqlite3_int64*)cbRes) = sqlite3_column_int64(stmt, index);
//}

///*static int sqliteQueryPrintfCallback(void *data, int argc, char **argv, char **azColName) {
//    int i;
//    fprintf(stderr, "%s: ", (const char*)data);
//    for(i=0; i<argc; i++){
//        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//    }
//    printf("\n");
//    return 0;
//   }
//*/

///*************** UTILS ******************/

///*
//void dieOnSqliteError(int rc, char *errMsg)
//{
//  if (rc != SQLITE_OK)
//  {
//    fprintf(stderr, "Exiting program because of SQLite error: %s\n", errMsg);
//    sqlite3_free(errMsg);
//    exit(EXIT_FAILURE);
//  }
//}
//*/
//#define _CRT_SECURE_NO_WARNINGS // TODO: comment the define


#define NO_ERR_MSG ""


int libmzdb_get_model_version(sqlite3* db, char** model_version, char** err_msg)
{
    return libmzdb_execute_select_stmt(db, "SELECT version FROM mzdb LIMIT 1", 0, &libmzdb_get_string_cb, (void **) model_version, err_msg);
}

char* libmzdb_get_model_version_or_die(sqlite3* db)
{
    char* version;
    char* err_msg;
    int rc = libmzdb_get_model_version(db, &version, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);

    return version;
}

int libmzdb_get_param_tree_chromatogram(sqlite3* db, char** param_tree_chro, char** err_msg)
{
    return libmzdb_execute_select_stmt(db, "SELECT param_tree FROM chromatogram", 0, &libmzdb_get_string_cb, (void **) param_tree_chro, err_msg);
}

char* libmzdb_get_param_tree_chromatogram_or_die(sqlite3* db)
{
    char* param_tree, *err_msg;
    int rc = libmzdb_get_param_tree_chromatogram(db, &param_tree, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return param_tree;
}

int libmzdb_get_param_tree_spectrum(sqlite3* db, int param_tree_id, char** param_tree_spectrum, char**err_msg)
{
    char* sql_str = sqlite3_mprintf("SELECT param_tree FROM spectrum WHERE id = %d", param_tree_id);
    int rc = libmzdb_execute_select_stmt(db, sql_str, 0, &libmzdb_get_string_cb, (void **) param_tree_spectrum, err_msg);
    sqlite3_free(sql_str);
    return rc;
}

char* libmzdb_get_param_tree_spectrum_or_die(sqlite3* db, int param_tree_id)
{
    char* param_tree_spectrum, *err_msg;
    int rc = libmzdb_get_param_tree_spectrum(db, param_tree_id, &param_tree_spectrum, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return param_tree_spectrum;
}


int libmzdb_get_param_tree_mzdb(sqlite3* db, libmzdb_mzdb_param_tree_t* param_tree_mzdb, char** err_msg)
{
    char* param_tree; int rc;
    if( (rc = libmzdb_execute_select_stmt(db, "SELECT param_tree FROM mzdb", 0, &libmzdb_get_string_cb, (void **) &param_tree, err_msg))!=SQLITE_OK) return rc;
    nodec_ptr_t xml;
    if( xml_parse(param_tree, &xml) == 0) return -1;
    memset((void *) param_tree_mzdb, 0, sizeof(libmzdb_mzdb_param_tree_t)); //reset the param tree mzdb
    xml_browse(xml, xml_browse_callback_mzdb_param_tree, (void *)param_tree_mzdb);
    return rc;
}


libmzdb_mzdb_param_tree_t libmzdb_get_param_tree_mzdb_or_die(sqlite3 *db)
{
    libmzdb_mzdb_param_tree_t param_tree_mzdb;
    char * err_msg;
    int rc = libmzdb_get_param_tree_mzdb(db, &param_tree_mzdb, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return param_tree_mzdb;
}

int libmzdb_get_pwiz_mzdb_version(sqlite3* db, char** mzdb_version, char** err_msg)
{
    return libmzdb_execute_select_stmt(db, "SELECT version FROM software WHERE name LIKE '%mzDB'", 0, &libmzdb_get_string_cb, (void **) mzdb_version, err_msg);
}

char* libmzdb_get_pwiz_mzdb_version_or_die(sqlite3 *db)
{
    char* version, *err_msg;
    libmzdb_die_on_sqlite_error(libmzdb_get_pwiz_mzdb_version(db, &version, &err_msg), &err_msg);
    return version;
}

int libmzdb_get_last_time(sqlite3* db, double* last_time, char** err_msg)
{
    return libmzdb_execute_select_stmt(db, "SELECT time FROM spectrum ORDER BY id DESC LIMIT 1", 0, &libmzdb_get_double_cb, (void **)last_time, err_msg);
}

double libmzdb_get_last_time_or_die(sqlite3* db)
{
    double last_time;
    char* err_msg;
    libmzdb_die_on_sqlite_error(libmzdb_get_last_time(db, &last_time, &err_msg), &err_msg);
    return last_time;
}

int libmzdb_get_max_ms_level(sqlite3* db, int* max_ms_level, char** err_msg)
{
    return libmzdb_execute_select_stmt(db, "SELECT max(ms_level) FROM run_slice", 0, &libmzdb_get_int_cb, (void **) max_ms_level, err_msg);
}

int libmzdb_get_max_ms_level_or_die(sqlite3 *db)
{
    int max_ms_level;
    char* err_msg;
    int rc = libmzdb_get_max_ms_level(db, &max_ms_level, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return max_ms_level;
}



//Uses the method get_table_records_count to specifically return the Table Records Count from the bounding_box table
int libmzdb_get_bounding_boxes_count_from_sequence(sqlite3 *db,int *bb_count,char **err_msg)
{
    return libmzdb_get_table_records_count(db, "bounding_box", bb_count, err_msg);
}

int libmzdb_get_bounding_boxes_count_from_sequence_or_die(sqlite3 *db)
{
    int bb_count;
    char *err_msg;
    int rc = libmzdb_get_bounding_boxes_count_from_sequence(db, &bb_count, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return bb_count;
}

//Return the Mz Range depending on the ms_level
int libmzdb_get_mz_range(sqlite3 *db, int ms_level,int **min_max_mz,char **err_msg)
{
    int min_mz;
    int max_mz;
    int rc;
    char *z_sql = sqlite3_mprintf("SELECT min(begin_mz), max(end_mz) FROM run_slice WHERE ms_level=%d", ms_level);

    if ((rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_int_cb, (void **) &min_mz, err_msg))!= SQLITE_OK)
    {
        sqlite3_free(z_sql);
        return rc;
    }

    if ((rc = libmzdb_execute_select_stmt(db, z_sql, 1, &libmzdb_get_int_cb, (void **) &max_mz, err_msg))!= SQLITE_OK)
    {
        sqlite3_free(z_sql);
        return rc;
    }
    sqlite3_free(z_sql);

    static int mz_range[2];
    mz_range[0] = min_mz;
    mz_range[1] = max_mz;
    *min_max_mz = mz_range;
    return SQLITE_OK;
}

int *libmzdb_get_mz_range_or_die(sqlite3 *db, int ms_level)
{
    int *min_max_mz;
    char *err_msg;
    libmzdb_die_on_sqlite_error(libmzdb_get_mz_range(db, ms_level, &min_max_mz, &err_msg), &err_msg);
    return min_max_mz;
}

int libmzdb_get_bounding_boxes_count(sqlite3 *db, int bb_run_slice_id, int *bb_count, char **err_msg)
{
    char *z_sql = sqlite3_mprintf("SELECT count(*) FROM bounding_box WHERE bounding_box.run_slice_id = %d",bb_run_slice_id);
    int rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_int_cb, (void **) bb_count, err_msg);
    sqlite3_free(z_sql);
    return rc;
}

int libmzdb_get_bounding_boxes_count_or_die(sqlite3 *db, int bb_run_slice_id)
{
    int bb_count;
    char *err_msg;
    libmzdb_die_on_sqlite_error(libmzdb_get_bounding_boxes_count(db,bb_run_slice_id, &bb_count, &err_msg), &err_msg);
    return bb_count;
}

int libmzdb_get_cycles_count(sqlite3 *db, int *cycles_count, char **err_msg)
{
    return libmzdb_execute_select_stmt(db, "SELECT cycle FROM spectrum ORDER BY id DESC LIMIT 1", 0, &libmzdb_get_int_cb, (void **) cycles_count, err_msg);
}

int libmzdb_get_cycles_count_or_die(sqlite3 *db)
{
    int cycles_count;
    char *err_msg;
    libmzdb_die_on_sqlite_error(libmzdb_get_cycles_count(db, &cycles_count, &err_msg), &err_msg);
    return cycles_count;
}

int libmzdb_get_data_encodings_count_from_sequence(sqlite3 *db, int *count,char **err_msg)
{
    return libmzdb_get_table_records_count(db, "data_encoding", count, err_msg);
}

int libmzdb_get_data_encodings_count_from_sequence_or_die(sqlite3 *db)
{
    int count;
    char *err_msg;
    libmzdb_die_on_sqlite_error(libmzdb_get_data_encodings_count_from_sequence(db, &count, &err_msg), &err_msg);
    return count;
}

int libmzdb_get_spectra_count_from_sequence(sqlite3 *db, int *spectra_count, char **err_msg)
{
    return libmzdb_get_table_records_count(db, "spectrum", spectra_count, err_msg);
}

int libmzdb_get_spectra_count_from_sequence_or_die(sqlite3 *db)
{
    int spectra_count;
    char *err_msg;
    int rc = libmzdb_get_spectra_count_from_sequence(db, &spectra_count, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return spectra_count;
}

int libmzdb_get_spectra_count(sqlite3 *db, int ms_level, int *spectra_count, char **err_msg)
{
    char *z_sql = sqlite3_mprintf("SELECT count(id) FROM spectrum WHERE ms_level = %d", ms_level);
    int rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_int_cb, (void **) spectra_count, err_msg);
    sqlite3_free(z_sql);
    return rc;
}

int libmzdb_get_spectra_count_or_die(sqlite3 *db, int ms_level)
{
    int spectra_count;
    char *err_msg;
    int rc = libmzdb_get_spectra_count(db, ms_level, &spectra_count, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return spectra_count;
}

int libmzdb_get_run_slices_count_from_sequence(sqlite3 *db, int *run_slices_count, char **err_msg)
{
    return libmzdb_get_table_records_count(db, "run_slice", run_slices_count, err_msg);
}

int libmzdb_get_run_slices_count_from_sequence_or_die(sqlite3 *db)
{
    int run_slices_count;
    char *err_msg;
    int rc = libmzdb_get_run_slices_count_from_sequence(db, &run_slices_count, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return run_slices_count;
}

int libmzdb_get_table_records_count(sqlite3 *db, char *table_name, int *table_records_count, char **err_msg)
{
    char *z_sql = sqlite3_mprintf("SELECT seq FROM sqlite_sequence WHERE name = '%s'", table_name);
    int rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_int_cb, (void **) table_records_count, err_msg);
    sqlite3_free(z_sql);
    return rc;
}

int libmzdb_get_table_records_count_or_die(sqlite3 *db, char *table_name)
{
    int table_records_count;
    char *err_msg;
    libmzdb_die_on_sqlite_error(libmzdb_get_table_records_count(db, table_name, &table_records_count, &err_msg), &err_msg);
    return table_records_count;
}


int libmzdb_get_bounding_box_data(sqlite3 *db, int bb_id, byte **blob, int *blob_length, char **err_msg)
{
    sqlite3_blob *blob_handle;
    int rc;
    *err_msg = "Error : get bbox data : cannot open the blob";
    if ((rc = sqlite3_blob_open(db, "main", "bounding_box", "data", bb_id, 0, &blob_handle)) != SQLITE_OK) return rc;

    *blob_length = sqlite3_blob_bytes(blob_handle);
    *blob = (byte*)calloc(*blob_length, sizeof(byte));
    rc = sqlite3_blob_read(blob_handle, *blob, *blob_length, 0);
    sqlite3_blob_close(blob_handle);
    *err_msg = NO_ERR_MSG;
    return rc;
}

byte *libmzdb_get_bounding_box_data_or_die(sqlite3 *db, int bb_id, int *blob_length)
{
    byte *blob = NULL;
    char *err_msg = NULL;
    libmzdb_die_on_sqlite_error(libmzdb_get_bounding_box_data(db, bb_id, &blob, blob_length, &err_msg), &err_msg);
    return blob;
}

int libmzdb_get_chromatogram_data_points(sqlite3 *db, int c_id, libmzdb_data_points_32_32_t **data, int *data_points_count, char **err_msg)
{
    byte* blob = NULL;
    char* z_sql = sqlite3_mprintf("SELECT data_points FROM chromatogram WHERE chromatogram.id = %d", c_id);
    int rc = libmzdb_execute_select_stmt(db, z_sql, 3, &libmzdb_get_blob_cb, (void **) blob, err_msg);

    *data_points_count = sizeof(byte) / sizeof(libmzdb_data_points_32_32_t);
    *data = (libmzdb_data_points_32_32_t*) calloc((*data_points_count), sizeof(libmzdb_data_points_32_32_t));
    *data = (libmzdb_data_points_32_32_t*) blob; //FIXME

    sqlite3_free(z_sql);
    return rc;
}

int libmzdb_get_chromatogram_data_points_or_die(sqlite3 *db, libmzdb_data_points_32_32_t** data_points,  int c_id)
{
    char *err_msg = NULL;
    int point_count;
    libmzdb_die_on_sqlite_error(libmzdb_get_chromatogram_data_points(db, c_id, data_points, &point_count, &err_msg), &err_msg);
    return point_count;
}

int libmzdb_get_bounding_box_first_spectrum_id(sqlite3 *db, int first_id, long *bb_first_spectrum_id, char **err_msg)
{
    char *z_sql = sqlite3_mprintf("SELECT bb_first_spectrum_id FROM spectrum WHERE id = %d", first_id);
    int rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_int_cb, (void **) bb_first_spectrum_id, err_msg);
    sqlite3_free(z_sql);
    return rc;
}

long libmzdb_get_bounding_box_first_spectrum_id_or_die(sqlite3 *db, int first_id)
{
    long bb_first_spectrum_id;
    char *err_msg = NULL;
    libmzdb_die_on_sqlite_error(libmzdb_get_bounding_box_first_spectrum_id(db, first_id, &bb_first_spectrum_id, &err_msg), &err_msg);
    return bb_first_spectrum_id;
}

int libmzdb_get_bounding_box_min_mz(sqlite3 *db, int bb_r_tree_id, float *bb_min_mz, char **err_msg)
{
    char* z_sql = sqlite3_mprintf("SELECT min_mz FROM bounding_box_rtree WHERE bounding_box_rtree.id = %d", bb_r_tree_id);
    int rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_float_cb, (void **) bb_min_mz, err_msg);
    sqlite3_free(z_sql);
    return rc;
}

float libmzdb_get_bounding_box_min_mz_or_die(sqlite3 *db, int bb_r_tree_id)
{
    float bbmin_mz;
    char *err_msg = NULL;
    libmzdb_die_on_sqlite_error(libmzdb_get_bounding_box_min_mz(db, bb_r_tree_id, &bbmin_mz, &err_msg), &err_msg);
    return bbmin_mz;
}

int libmzdb_get_bounding_box_min_time(sqlite3 *db, int bb_r_tree_id, float *bb_min_time, char **err_msg)
{
    char *z_sql = sqlite3_mprintf("SELECT min_time FROM bounding_box_rtree WHERE bounding_box_rtree.id = %d", bb_r_tree_id);
    int rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_float_cb, (void **) bb_min_time, err_msg);
    sqlite3_free(z_sql);
    return rc;
}

float libmzdb_get_bounding_box_min_time_or_die(sqlite3 *db, int bb_r_tree_id)
{
    float bb_min_time;
    char *err_msg = NULL;
    libmzdb_die_on_sqlite_error(libmzdb_get_bounding_box_min_time(db, bb_r_tree_id, &bb_min_time, &err_msg), &err_msg);
    return bb_min_time;
}

int libmzdb_get_run_slice_id(sqlite3 *db, int bb_id, int *run_slice_id, char **err_msg)
{
    char *z_sql = sqlite3_mprintf("SELECT run_slice_id FROM bounding_box WHERE id = %d", bb_id);
    int rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_int_cb, (void **) run_slice_id, err_msg);
    sqlite3_free(z_sql);
    return rc;
}

int libmzdb_get_run_slice_id_or_die(sqlite3 *db, int bb_id)
{
    int run_slice_id;
    char *err_msg = NULL;
    libmzdb_die_on_sqlite_error(libmzdb_get_run_slice_id(db, bb_id, &run_slice_id, &err_msg), &err_msg);
    return run_slice_id;
}

int libmzdb_get_ms_level_from_run_slice_id_manually(sqlite3 *db, int run_slice_id, int *ms_level, char **err_msg)
{
    char *z_sql = sqlite3_mprintf("SELECT ms_level FROM run_slice WHERE run_slice.id = %d", run_slice_id);
    int rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_int_cb, (void **) ms_level, err_msg);
    sqlite3_free(z_sql);
    return rc;
}

int libmzdb_get_ms_level_from_run_slice_id_manually_or_die(sqlite3 *db, int run_slice_id)
{
    int ms_level;
    char *err_msg = NULL;
    int rc = libmzdb_get_ms_level_from_run_slice_id_manually(db, run_slice_id, &ms_level, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return ms_level;
}

int libmzdb_get_bounding_box_ms_level(sqlite3 *db, int bb_id, int *result_ms_level,char **err_msg)
{
    int *first_result = 0;
    char *z_sql = sqlite3_mprintf("SELECT run_slice_id FROM bounding_box WHERE id = %d", bb_id);
    int rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_int_cb, (void **) &first_result, err_msg);
    sqlite3_free(z_sql); //mem leak?
    if(rc != SQLITE_OK) return rc;

    z_sql = sqlite3_mprintf("SELECT ms_level FROM run_slice WHERE run_slice.id = %d", first_result);
    rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_int_cb, (void **) result_ms_level, err_msg);
    sqlite3_free(z_sql);
    return SQLITE_OK;
}

int get_bounding_box_ms_level_or_die(sqlite3 *db, int bb_id)
{
    int result_ms_level;
    char *err_msg = NULL;
    int rc = libmzdb_get_bounding_box_ms_level(db, bb_id, &result_ms_level, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return result_ms_level;
}

int libmzdb_get_data_encoding_id(sqlite3 *db, int bounding_box_id, int *data_encoding_id, char **err_msg)
{
    char *sql_str_encoding_id = sqlite3_mprintf("SELECT s.data_encoding_id FROM spectrum s, bounding_box b WHERE b.id = %d AND b.first_spectrum_id = s.id", bounding_box_id);
    int rc = libmzdb_execute_select_stmt(db, sql_str_encoding_id, 0, &libmzdb_get_int_cb, (void **) data_encoding_id, err_msg);
    sqlite3_free(sql_str_encoding_id);

    return rc;
}

int libmzdb_get_data_encoding_id_or_die(sqlite3 *db, int bounding_box_id)
{
    int data_encoding_id;
    char *err_msg = NULL;
    int rc = libmzdb_get_data_encoding_id(db, bounding_box_id, &data_encoding_id, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return data_encoding_id;
}

/**
 * @brief _get_data_enconding_count private function : get the number of data encoding
 * @param db the database on which the SQL executes
 * @return the data encoding count
 */
int _get_data_enconding_count(sqlite3 *db)
{
    int rc;
    int stmt_step_res;
    sqlite3_stmt* stmt;

    if ((rc= sqlite3_prepare_v2(db, "SELECT count(id) FROM data_encoding", -1, &stmt, NULL))!=SQLITE_OK) return -1;
    if ((stmt_step_res= sqlite3_step(stmt))!=SQLITE_ROW) return -1;
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt); //delete the stmt*/

    // FIXME: should work
    /*rc = execute_select_first_field_stmt(db, "SELECT count(id) FROM data_encoding", &get_int_cb, &count, NULL);
    if (rc!=SQLITE_OK) return -1;*/

    return count;
}

/**
 * @brief get_data_encodings get an array of each data encoding
 * @param db the database on which the SQL executes
 * @param data_encodings an array which contains each data encoding
 * @param de_count the number of data encoding
 * @return SQLITE_OK if no error occured
 */
int get_data_encodings(sqlite3 *db, libmzdb_data_encoding_t **data_encodings, int *de_count)
{
    int rc;
    int stmt_step_res;
    sqlite3_stmt* stmt;

    *de_count = _get_data_enconding_count(db);
    if(*de_count < 0) return SQLITE_ERROR;

    libmzdb_data_encoding_t* de = (libmzdb_data_encoding_t*) malloc(sizeof(libmzdb_data_encoding_t)* (*de_count));

    const unsigned char *mode, *byte_order; //*compression => was set and not used
    int mz_precision, intensity_precision, id;

    if ((rc= sqlite3_prepare_v2(db, "SELECT * FROM data_encoding", -1, &stmt, NULL))!=SQLITE_OK) return rc;

    int index = -1;
    while((stmt_step_res = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        id = sqlite3_column_int(stmt, 0);
        mode = sqlite3_column_text(stmt, 1);
        //compression = sqlite3_column_text(stmt, 2);
        byte_order = sqlite3_column_text(stmt, 3);
        mz_precision = sqlite3_column_int(stmt, 4);
        intensity_precision = sqlite3_column_int(stmt, 5);

        index++;
        de[index].id = id;

        //printf("de mode str from loader: %s \n " , mode);

        if (strcmp((const char *) mode, "fitted") == 0) de[index].mode = FITTED;
        else if (strcmp((const char *) mode, "centroid") == 0) de[index].mode = CENTROID;
        else de[index].mode = PROFILE;

        //printf("de id from loader: %d \n " , de[index].id);
        //printf("de mode  from loader: %d \n " , de[index].mode);

        de[index].byte_order = (strcmp((const char *) byte_order, "little_endian") == 0) ? LITTLE_ENDIAN : BIG_ENDIAN;

        if (mz_precision == 32) de[index].peak_encoding = LOW_RES_PEAK;
        else if (intensity_precision == 32) de[index].peak_encoding = HIGH_RES_PEAK;
        else de[index].peak_encoding = NO_LOSS_PEAK;
    }

    *data_encodings = de;

    return SQLITE_OK;
}

int _get_spectra_data_encoding_ids(sqlite3 *db, int **spectra_data_encoding_ids)
{
    int rc;
    int stmt_step_res;
    sqlite3_stmt* stmt;
    int spectrum_id;
    int de_id;

    int spectra_count = libmzdb_get_spectra_count_from_sequence_or_die(db);
    int* local_spectra_data_encoding_ids = (int*) malloc(sizeof(int) * (1 + spectra_count) );
    memset(local_spectra_data_encoding_ids, -1, (1 + spectra_count) * sizeof(int)); // Set all values to -1

    if ((rc= sqlite3_prepare_v2(db, "SELECT id, data_encoding_id FROM spectrum", -1, &stmt, NULL))!=SQLITE_OK) {
        free(local_spectra_data_encoding_ids);
        return rc;
    }

    while((stmt_step_res = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        spectrum_id = sqlite3_column_int(stmt, 0);
        de_id = sqlite3_column_int(stmt, 1);
        local_spectra_data_encoding_ids[spectrum_id] = de_id;
    }

    *spectra_data_encoding_ids = local_spectra_data_encoding_ids;

    return SQLITE_OK;
}

int libmzdb_get_data_encodings_cache(sqlite3 *db, libmzdb_data_encodings_cache_t** data_encodings_cache)
{
    int rc;
    libmzdb_data_encodings_cache_t* cache = (libmzdb_data_encodings_cache_t*) malloc(sizeof(libmzdb_data_encodings_cache_t));

    if ((rc = get_data_encodings(db, &cache->data_encodings, &cache->de_size))!=SQLITE_OK) {
        free(cache);
        return rc;
    }

    int de_size = cache->de_size;
    libmzdb_data_encoding_t last_de = cache->data_encodings[de_size - 1];
    int mapping_size = last_de.id + 1;
    cache->mapping_size = mapping_size;

    int* data_encoding_id_mapping = (int*) malloc(sizeof(int) * mapping_size);
    memset(data_encoding_id_mapping, -1, mapping_size * sizeof(int)); // Set all values to -1

    for(int i=0; i<de_size; ++i) data_encoding_id_mapping[cache->data_encodings[i].id] = i;

    cache->data_encoding_id_mapping = data_encoding_id_mapping;

    int* spectra_data_encoding_ids;
    _get_spectra_data_encoding_ids(db, &spectra_data_encoding_ids);

    cache->spectrum_id_to_data_encoding_id = spectra_data_encoding_ids;

    *data_encodings_cache = cache;

    return SQLITE_OK;
}

libmzdb_data_encoding_t* libmzdb_get_data_encoding_from_cache(libmzdb_data_encodings_cache_t* data_encodings_cache, int data_encoding_id)
{
    if (data_encoding_id < 1 || data_encoding_id >= data_encodings_cache->mapping_size) return NULL;

    int de_index = data_encodings_cache->data_encoding_id_mapping[data_encoding_id];
    return &data_encodings_cache->data_encodings[de_index];

}

libmzdb_spectrum_data_t* libmzdb_read_spectrum_slice_data(
        const byte* bb_bytes,
        const int peaks_start_pos,
        const int peaks_count,
        const libmzdb_data_encoding_t de,
        const double min_mz,
        const double max_mz
        ) {
    const libmzdb_data_mode_t data_mode = de.mode;
    const libmzdb_peak_encoding_t pe = de.peak_encoding;
    const int peak_size = data_mode == FITTED ? pe + 8 : pe;
    const int peaks_bytes_length = peaks_count * peak_size;

    int filtered_peaks_count = 0;
    int filtered_peaks_start_idx = 0;

    // If no m/z range is provided
    if (min_mz < 0 && max_mz < 0)
    {
        // Compute the peaks count for the whole spectrum slice
        filtered_peaks_count = peaks_count;
        // Set peaks_start_idx value to spectrum_slice_start_pos
        filtered_peaks_start_idx = peaks_start_pos;
        // Else determine the peaks_start_idx and peaks_count corresponding to provided m/z filters
    }
    else
    {
        // Determine the max m/z threshold to use
        double max_mz_threshold = (max_mz < 0) ? DBL_MAX : max_mz;
        //        int peaks_start_pos_as_ints = peaks_start_pos * 0.25;

        for (int i = 0; i < peaks_bytes_length; i += peak_size)
        {
            const int peak_start_pos = peaks_start_pos + i;
            double mz = 0.0;

            switch (pe)
            {
            case LOW_RES_PEAK:
                memcpy( &mz, &bb_bytes[peak_start_pos], 4); // sizeof(float)
                break;
            default:
                memcpy( &mz, &bb_bytes[peak_start_pos], 8); // sizeof(double)
                break;
            }

            // Check if we are in the desired m/z range
            if (mz >= min_mz && mz <= max_mz_threshold)
            {
                // Increment the number of peaks to read
                filtered_peaks_count++;
                // Determine the peaks start idx
                if (mz >= min_mz && filtered_peaks_start_idx == 0) filtered_peaks_start_idx = peak_start_pos;
            }
        }
    }

    // Create new arrays of primitives
    void* mz_array;
    void* intensity_array;
    float* lwhm_array = NULL;
    float* rwhm_array = NULL;

    // Allocate memory for arrays
    switch (pe)
    {
    case HIGH_RES_PEAK:
        mz_array = malloc(sizeof(double) * filtered_peaks_count);
        intensity_array = malloc(sizeof(float) * filtered_peaks_count);
        break;
    case LOW_RES_PEAK:
        mz_array = malloc(sizeof(float) * filtered_peaks_count);
        intensity_array = malloc(sizeof(float) * filtered_peaks_count);
        break;
    default : //case NO_LOSS_PEAK:
        mz_array = malloc(sizeof(double) * filtered_peaks_count);
        intensity_array = malloc(sizeof(double) * filtered_peaks_count);
        break;
    }

    if (data_mode == FITTED) {
        int size_of_hwhms = sizeof(float) * filtered_peaks_count;
        lwhm_array = (float*) malloc(size_of_hwhms);
        rwhm_array = (float*) malloc(size_of_hwhms);
    }


    for (int peak_idx = 0; peak_idx < filtered_peaks_count; peak_idx++ ) {

        int peak_bytes_index = filtered_peaks_start_idx + peak_idx * peak_size;

        // Read m/z and intensity values
        switch (pe)
        {
        case HIGH_RES_PEAK:
            memcpy( &((double*)mz_array)[peak_idx], &bb_bytes[peak_bytes_index], 8); // sizeof(double)
            memcpy( &((float*)intensity_array)[peak_idx], &bb_bytes[peak_bytes_index + 8], 4); // sizeof(float)
            break;
        case LOW_RES_PEAK:
            memcpy( &((float*)mz_array)[peak_idx], &bb_bytes[peak_bytes_index], 4); // sizeof(float)
            memcpy( &((float*)intensity_array)[peak_idx], &bb_bytes[peak_bytes_index + 4], 4); // sizeof(float)
            break;
        case NO_LOSS_PEAK:
            memcpy( &((double*)mz_array)[peak_idx], &bb_bytes[peak_bytes_index], 8); // sizeof(double)
            memcpy( &((double*)intensity_array)[peak_idx], &bb_bytes[peak_bytes_index + 8], 8); // sizeof(double)
            break;
        }

        // Read left and right HWHMs if needed
        if (data_mode == FITTED) {
            memcpy( &lwhm_array[peak_idx], &bb_bytes[peak_bytes_index + pe], 4); // sizeof(float)
            memcpy( &rwhm_array[peak_idx], &bb_bytes[peak_bytes_index + pe + 4], 4); // sizeof(float)
        }
    }

    libmzdb_spectrum_data_t* sd = (libmzdb_spectrum_data_t *) malloc( sizeof(libmzdb_spectrum_data_t) );
    //sd->data_encoding = (data_encoding_t*) malloc(sizeof(data_encoding_t));
    sd->data_encoding = de;


    switch (pe)
    {
    case HIGH_RES_PEAK:
        sd->mz_array_as_doubles = mz_array;
        sd->intensity_array_as_floats = intensity_array;
        break;
    case LOW_RES_PEAK:
        sd->mz_array_as_floats = mz_array;
        sd->intensity_array_as_floats = intensity_array;
        break;
    case NO_LOSS_PEAK:
        sd->mz_array_as_doubles = mz_array;
        sd->intensity_array_as_doubles = intensity_array;
        break;
    }
    if (data_mode == FITTED) {
        sd->lwhm_array = lwhm_array;
        sd->rwhm_array = rwhm_array;
    }

    sd->peak_count = peaks_count;
    // return the newly created spectrum data
    return sd;
}

libmzdb_spectrum_data_t* libmzdb_read_spectrum_slice_data_at(
        const libmzdb_indexed_bounding_box_t indexed_bbox,
        libmzdb_data_encodings_cache_t cache,
        const int spectrum_slice_idx,
        const long spectrum_id,
        const double min_mz,
        const double max_mz
        ) {

    // Determine peak size in bytes
    const libmzdb_data_encoding_t de = *libmzdb_get_data_encoding_from_cache(&cache, cache.spectrum_id_to_data_encoding_id[spectrum_id]);

    // Retrieve the number of peaks
    const int peaks_count = indexed_bbox.peaks_counts[spectrum_slice_idx];

    // Skip spectrum id and peaks count (two integers)
    const int peaks_start_pos = indexed_bbox.slices_indexes[spectrum_slice_idx] + 8;

    // Instantiate a new SpectrumData for the corresponding spectrum slice
    return libmzdb_read_spectrum_slice_data(indexed_bbox.bb.blob, peaks_start_pos, peaks_count, de, min_mz, max_mz);
}

libmzdb_bounding_box_t libmzdb_build_bbox(sqlite3_stmt* stmt, int do_malloc)
{
    libmzdb_bounding_box_t bbox;
    //cur_bb.first_spectrum_id = first_spec_id; //first spectrum id

    libmzdb_get_int_cb(stmt, 0, 1, &(bbox.id)); //id
    libmzdb_get_int_cb(stmt, 2, 1, &(bbox.run_slice_id)); //run slice id
    libmzdb_get_int_cb(stmt, 3, 1, &(bbox.first_spectrum_id)); //first spectrum id : to check
    libmzdb_get_int_cb(stmt, 4, 1, &(bbox.last_spectrum_id)); //last spectrum id

    // PREVIOUS CODE //
    if (!do_malloc) {
        const void * blob = sqlite3_column_blob(stmt, 1); // get the blob's data
        bbox.blob = (byte*) blob;
        bbox.blob_size = sqlite3_column_bytes(stmt, 1); // get the blob size
    }
    // NEW CODE //
    else {
        /* The pointer returned by sqlite3_column_blob() points to memory
        ** that is owned by the statement handle (pStmt). It is only good
        ** until the next call to an sqlite3_XXX() function (e.g. the
        ** sqlite3_finalize() below) that involves the statement handle.
        ** So we need to make a copy of the blob into memory obtained from
        ** malloc() to return to the caller.
        */
        bbox.blob_size = sqlite3_column_bytes(stmt, 1); // get the blob size

        bbox.blob = (unsigned char*) malloc(bbox.blob_size);
        memcpy(bbox.blob, sqlite3_column_blob(stmt, 1), bbox.blob_size);
    }

    return bbox;
}

libmzdb_indexed_bounding_box_t libmzdb_index_bbox(libmzdb_bounding_box_t bbox, libmzdb_data_encodings_cache_t cache)
{
    libmzdb_indexed_bounding_box_t indexed_bbox;

    int* spectrum_id_to_data_encoding_id = cache.spectrum_id_to_data_encoding_id;
    int estimated_slices_count = 1 + bbox.last_spectrum_id - bbox.first_spectrum_id; //will always be a greater value than the slices count

    int size_of_array = sizeof(int) * estimated_slices_count;
    int* slice_indexes = (int*) malloc(size_of_array);
    int* spectra_ids = (int*) malloc(size_of_array);
    int* peaks_counts = (int*) malloc(size_of_array);

    int bytes_idx = 0; //loop index
    int peak_size, peak_count, spectrum_id;
    int slice_count = 0;
    libmzdb_data_encoding_t de;

    byte* blob = bbox.blob;
    // FIXME: avoid this? => see: https://stackoverflow.com/questions/8703047/how-to-convert-a-byte-array-into-double-in-c
    int* blob_as_ints = (int*)blob; //use to access to the spectrum id and the peak count, a blob will always be a multiple of 4
    int bytes_as_ints_offset = 0;
    int blob_size = bbox.blob_size;

    while (bytes_idx < blob_size) //for each spectrum slice store in the blob
    {
        spectrum_id = blob_as_ints[bytes_as_ints_offset]; //get the current spectrum id in the blob
        spectra_ids[slice_count] = spectrum_id; //stock the spectrum id
        peak_count = blob_as_ints[bytes_as_ints_offset + 1]; //get the current peak count in the blob
        peaks_counts[slice_count] = peak_count; //stock the peak count

        int de_id = spectrum_id_to_data_encoding_id[spectrum_id]; //get the data encoding id which match with the spectrum id in the cache
        de = *libmzdb_get_data_encoding_from_cache(&cache, de_id); //get the data encoding from the cache with its id

        peak_size = (de.mode == FITTED) ? de.peak_encoding + 8 : de.peak_encoding; //increase the peak size if the peaks are fitted
        slice_indexes[slice_count] = bytes_idx; //store the last byte index
        bytes_idx += 8 + (peak_size * peak_count); //shift the byte index with the size (in byte) of the current spectrum slice
        bytes_as_ints_offset = (bytes_idx / 4); //convert the byte index into integer index
        slice_count++;
    }

    // Set fields of indexed_bbox struct
    indexed_bbox.spectrum_slices_count = slice_count;
    indexed_bbox.spectra_ids = spectra_ids;
    indexed_bbox.slices_indexes = slice_indexes;
    indexed_bbox.peaks_counts = peaks_counts;
    indexed_bbox.bb = bbox;

    return indexed_bbox;
}

libmzdb_spectrum_data_t libmzdb_merge_spectrum_slices(libmzdb_spectrum_data_t* sd_slices, int bb_count, int peak_count)
{
    libmzdb_spectrum_data_t merged_sd;

    merged_sd.data_encoding = sd_slices[0].data_encoding;
    merged_sd.peak_count = peak_count;

    switch (merged_sd.data_encoding.peak_encoding)
    {
    case HIGH_RES_PEAK:
        merged_sd.mz_array_as_doubles = (double*) malloc(sizeof(double)*peak_count);
        merged_sd.intensity_array_as_floats = (float*) malloc(sizeof(float)*peak_count);
        break;
    case LOW_RES_PEAK:
        merged_sd.mz_array_as_floats = (float*) malloc(sizeof(float)*peak_count);
        merged_sd.intensity_array_as_floats = (float*) malloc(sizeof(float)*peak_count);
        break;
    case NO_LOSS_PEAK:
        merged_sd.mz_array_as_doubles = (double*) malloc(sizeof(double)*peak_count);
        merged_sd.intensity_array_as_doubles = (double*) malloc(sizeof(double)*peak_count);
        break;
    }
    if (merged_sd.data_encoding.mode == FITTED)
    {
        merged_sd.lwhm_array = (float*) malloc(sizeof(float)*peak_count);
        merged_sd.rwhm_array = (float*) malloc(sizeof(float)*peak_count);
    }

    libmzdb_spectrum_data_t curr_slice;
    int merged_sd_idx = 0;
    for (int i=0; i<bb_count; ++i)
    {
        curr_slice = sd_slices[i];
        switch (merged_sd.data_encoding.peak_encoding)
        {
        case HIGH_RES_PEAK:
            memcpy(&merged_sd.mz_array_as_doubles[merged_sd_idx], curr_slice.mz_array_as_doubles, curr_slice.peak_count * sizeof(double));
            memcpy(&merged_sd.intensity_array_as_floats[merged_sd_idx], curr_slice.intensity_array_as_floats, curr_slice.peak_count * sizeof(float));
            break;
        case LOW_RES_PEAK:
            memcpy(&merged_sd.mz_array_as_floats[merged_sd_idx], curr_slice.mz_array_as_floats, curr_slice.peak_count * sizeof(float));
            memcpy(&merged_sd.intensity_array_as_floats[merged_sd_idx], curr_slice.intensity_array_as_floats, curr_slice.peak_count * sizeof(float));
            break;
        case NO_LOSS_PEAK:
            memcpy(&merged_sd.mz_array_as_doubles[merged_sd_idx], curr_slice.mz_array_as_doubles, curr_slice.peak_count * sizeof(double));
            memcpy(&merged_sd.intensity_array_as_doubles[merged_sd_idx], curr_slice.intensity_array_as_doubles, curr_slice.peak_count * sizeof(double));
            break;
        }
        if (merged_sd.data_encoding.mode == FITTED)
        {
            memcpy(&merged_sd.lwhm_array[merged_sd_idx], curr_slice.lwhm_array, curr_slice.peak_count * sizeof(float));
            memcpy(&merged_sd.rwhm_array[merged_sd_idx], curr_slice.rwhm_array, curr_slice.peak_count * sizeof(float));
        }
        merged_sd_idx += curr_slice.peak_count;
    }
    return merged_sd;
}


int libmzdb_get_spectrum(sqlite3* db, long spectrum_id, libmzdb_entity_cache_t entity_cache, libmzdb_spectrum_t** result)
{
    sqlite3_stmt *stmt;
    int rc, stmt_step_res;
    int bb_count;
    char *err_msg;

    long first_spec_id = libmzdb_get_bounding_box_first_spectrum_id_or_die(db, spectrum_id);

    // Count the number of BBs to be loaded
    char *count_sql_str = sqlite3_mprintf("SELECT count(id) FROM bounding_box WHERE bounding_box.first_spectrum_id = %d", first_spec_id);
    rc = libmzdb_execute_select_stmt(db, count_sql_str, 0, &libmzdb_get_int_cb, (void **) &bb_count, &err_msg);
    sqlite3_free(count_sql_str);
    if (rc != SQLITE_OK) return rc;

    // Load BBs from the DB
    libmzdb_spectrum_data_t* sd_slices = (libmzdb_spectrum_data_t*) malloc(sizeof(libmzdb_spectrum_data_t) * bb_count); //Allocate memory for BBs
    char *sql_str = sqlite3_mprintf("SELECT * FROM bounding_box WHERE bounding_box.first_spectrum_id = %d", first_spec_id);
    rc = sqlite3_prepare_v2(db, sql_str, -1, &stmt, NULL);
    sqlite3_free(sql_str);
    if (rc != SQLITE_OK) return rc;

    libmzdb_data_encodings_cache_t de_cache = entity_cache.data_encodings_cache;
    const int min_mz = -1;
    const int max_mz = -1;
    int target_slice_idx = -1;
    int bb_index=0;

    while ((stmt_step_res=sqlite3_step(stmt)) == SQLITE_ROW) //for each bounding box, will collect the data of the spectrum
    {
        libmzdb_bounding_box_t cur_bb = libmzdb_build_bbox(stmt, 0);
        libmzdb_indexed_bounding_box_t indexed_bb = libmzdb_index_bbox(cur_bb, de_cache);

        // Search for slice index corresponding to spectrum_id
        if (target_slice_idx == -1)
            for (int cur_slice_idx=0; cur_slice_idx<indexed_bb.spectrum_slices_count; ++cur_slice_idx) //add a break ?
                if (indexed_bb.spectra_ids[cur_slice_idx] == spectrum_id)
                {
                    target_slice_idx = cur_slice_idx;
                    break;
                }
        // TODO: add log ERROR
        if (target_slice_idx == -1) return -1; //no target slice index has been found => ERROR

        //printf("spectrum_idx=%d\n",target_slice_idx);
        //printf("spectrum_id=%d\n",spectrum_id);

        /*printf("blob content: ");
        for (int i=0; i<indexed_bb.bb.blob_size; ++i)
          printf("%d ",indexed_bb.bb.blob[i]);

        printf("\n");*/

        // Add sd to sd_slices
        libmzdb_spectrum_data_t* buffer = libmzdb_read_spectrum_slice_data_at(indexed_bb, de_cache, target_slice_idx, spectrum_id, min_mz, max_mz);
        sd_slices[bb_index] = *buffer;
        free(buffer); //the buffer is not needed, but the content must not be free

        //free the indexed bbox content
        free(indexed_bb.peaks_counts);
        free(indexed_bb.slices_indexes);
        free(indexed_bb.spectra_ids);

        bb_index++;
    }
    sqlite3_finalize(stmt);

    //calculate the number of peak of the computed spectrum
    int peak_count = 0;
    for (int i=0; i<bb_count; ++i) peak_count += sd_slices[i].peak_count;

    libmzdb_spectrum_data_t sdata = libmzdb_merge_spectrum_slices(sd_slices, bb_count, peak_count);

	//printf("first peak m/z =%f \n", sdata.mz_array_as_doubles[0]);

    //free the spectrum data slices buffer

    for (int i=0; i<bb_count; ++i) libmzdb_free_spectrum_data_content(sd_slices[i]);
    free(sd_slices);

    // Create spectrum and return it
    libmzdb_spectrum_t* spectrum = (libmzdb_spectrum_t*) malloc(sizeof(libmzdb_spectrum_t));
    spectrum->header    = entity_cache.spectrum_headers[spectrum_id - 1];
    spectrum->data      = sdata;
    *result = spectrum; //store the spectrum in the result pointer

    return SQLITE_OK;
}




void libmzdb_free_spectrum_data_content(libmzdb_spectrum_data_t spectrum_data)
{
    switch (spectrum_data.data_encoding.peak_encoding)
    {
    case HIGH_RES_PEAK:
        if (spectrum_data.intensity_array_as_floats) free(spectrum_data.intensity_array_as_floats);
        if (spectrum_data.mz_array_as_doubles) free(spectrum_data.mz_array_as_doubles);
        break;
    case LOW_RES_PEAK:
        if (spectrum_data.intensity_array_as_floats) free(spectrum_data.intensity_array_as_floats);
        if (spectrum_data.mz_array_as_floats) free(spectrum_data.mz_array_as_floats);
        break;
    case NO_LOSS_PEAK:
        if (spectrum_data.intensity_array_as_doubles) free(spectrum_data.intensity_array_as_doubles);
        if (spectrum_data.mz_array_as_doubles) free(spectrum_data.mz_array_as_doubles);
        break;
    }

    if (spectrum_data.data_encoding.mode == FITTED)
    {
        if (spectrum_data.lwhm_array) free(spectrum_data.lwhm_array);
        if (spectrum_data.rwhm_array) free(spectrum_data.rwhm_array);
    }
}

void libmzdb_free_spectrum_header_content(libmzdb_spectrum_header_t spectrum_header)
{
    if(spectrum_header.title) free(spectrum_header.title);
    if(spectrum_header.activation_type) free(spectrum_header.activation_type);
    if(spectrum_header.scan_list_str) free(spectrum_header.scan_list_str);
    if(spectrum_header.precursor_list_str) free(spectrum_header.precursor_list_str);
    if(spectrum_header.product_list_str) free(spectrum_header.precursor_list_str);
    if(spectrum_header.param_tree_str) free(spectrum_header.param_tree_str); //TODO: Error
}

void libmzdb_free_spectrum(libmzdb_spectrum_t* spectrum)
{
    libmzdb_free_spectrum_data_content(spectrum->data);
    //free_spectrum_header_content(spectrum->header); => spectrum headers are saved in the entity cache
    free(spectrum);
}



















































































///**
// * @brief fill_peaks64_32_callback Callback for getAllFromBoundingBox. Specific to 64_32 peaks
// * @param blob
// * @param spectrum_data
// */
//void fill_peaks64_32_callback(byte *blob, spectrum_peaks_t *spectrum_data)
//{
//    int peak_count;
//    data_point_64_32_t *peaks64_32 = NULL;

//    spectrum_data->data_precision = DATA_PRECISION_64_32;
//    peak_count = ((int*)blob)[1];

//    spectrum_data->peak_count = peak_count;
//    spectrum_data->peaks64_32 = (data_point_64_32_t*) malloc(sizeof(data_point_64_32_t) * peak_count);
//    peaks64_32 = (data_point_64_32_t*)((int*)blob + 2);

//    memcpy(spectrum_data->peaks64_32, peaks64_32, peak_count);
//    /*
//  for (i = 0; i < peak_count; i++)
//  {
//    spectrum_data->peaks64_32[i].x = peaks64_32[i].x;
//    spectrum_data->peaks64_32[i].y = peaks64_32[i].y;
//  }
//  */
//}

////Not tested!
///**
// * @brief fill_peaks64_64_callback Callback for getAllFromBoundingBox. Specific to 64_64 peaks
// * @param blob
// * @param spectrum_data
// */
//void fill_peaks64_64_callback(byte *blob, spectrum_peaks_t *spectrum_data)
//{
//    int peak_count;
//    data_point_64_64_t *peaks64_64 = NULL;

//    spectrum_data->data_precision = DATA_PRECISION_64_64;

//    peak_count = ((int*)blob)[1];
//    // convert blob to 64_64 peaks
//    // id = ((int*)blob)[0]); // [unused]
//    spectrum_data->peak_count = peak_count;
//    spectrum_data->peaks64_64 = (data_point_64_64_t*)malloc(sizeof(data_point_64_64_t) * peak_count);
//    peaks64_64 = (data_point_64_64_t*)((int*)blob + 2);

//    memcpy(spectrum_data->peaks64_64, peaks64_64, peak_count);
//    /*
//  for (i = 0; i < peak_count; i++)
//  {
//    spectrum_data->peaks64_64[i].x = peaks64_64[i].x;
//    spectrum_data->peaks64_64[i].y = peaks64_64[i].y;
//  }
//  */
//}

///**
// * @brief fill_peaks32_32_callback Callback for getAllFromBoundingBox. Specific to 32_32 peaks
// * @param blob
// * @param spectrum_data
// */
//void fill_peaks32_32_callback(byte *blob, spectrum_peaks_t *spectrum_data)
//{
//    int peak_count;
//    data_point_32_32_t *peaks32_32;

//    spectrum_data->data_precision = DATA_PRECISION_32_32;
//    peak_count = ((int*)blob)[1];

//    spectrum_data->peak_count = peak_count;
//    spectrum_data->peaks32_32 = (data_point_32_32_t*)malloc(sizeof(data_point_32_32_t) * peak_count);
//    peaks32_32 = (data_point_32_32_t*)((int*)blob + 2);

//    memcpy(spectrum_data->peaks32_32, peaks32_32, peak_count);
//    /*
//  for (i = 0; i < peak_count; i++)
//  {
//    spectrum_data->peaks32_32[i].x = peaks32_32[i].x;
//    spectrum_data->peaks32_32[i].y = peaks32_32[i].y;
//  }
//  */
//}

////Not tested
////TO DO : more callback for other data_encoding types when they are used
///**
// * @brief fill_fitted_peaks64_32_callback Callback for getAllFromBoundingBox. Specific to fitted 64_32 peaks
// * @param blob
// * @param spectrum_data
// */
//void fill_fitted_peaks64_32_callback(byte *blob, spectrum_peaks_t *spectrum_data)
//{
//    int i;
//    int peak_count;
//    fitted_peak_t *fitted_peaks;

//    spectrum_data->data_precision = DATA_PRECISION_64_32;
//    peak_count = ((int*)blob)[1];

//    spectrum_data->peak_count = peak_count;
//    spectrum_data->fitted_peaks = (fitted_peak_t*) malloc(sizeof(fitted_peak_t) * peak_count);
//    fitted_peaks = (fitted_peak_t*)((int*)blob + 2);

//    memcpy(spectrum_data->fitted_peaks, fitted_peaks, peak_count);
//    /*
//  for (i = 0; i < peak_count; i++)
//  {
//    spectrum_data->peaksFitted64_32[i].x = fittedPeaks64_32[i].x;
//    spectrum_data->peaksFitted64_32[i].y = fittedPeaks64_32[i].y;
//  }
//  */
//}

////return NULL if DataEncoding is not managed
////TO DO : Create cache to avoid sqlQuerie at each call
///**
// * @brief get_fill_peaks_callback Function returning a callback pointer depending on the Data Encoding Id
// * @param data_precision
// * @return
// */
//fill_peaks_callback_ptr get_fill_peaks_callback(data_precision_enum data_precision)
//{
//    switch (data_precision)
//    {
//    case DATA_PRECISION_32_32: return fill_peaks32_32_callback;
//    case DATA_PRECISION_64_32: return fill_peaks64_32_callback;
//    case DATA_PRECISION_64_64: return fill_peaks64_64_callback;
//    }
//    return NULL;
//}

//int _get_data_point_memory_size(data_precision_enum data_precision)
//{
//    switch (data_precision)
//    {
//    case DATA_PRECISION_32_32:return sizeof(data_point_32_32_t);
//    case DATA_PRECISION_64_32:return sizeof(data_point_64_32_t);
//    case DATA_PRECISION_64_64:return sizeof(data_point_64_64_t);
//    case DATA_PRECISION_FITTED_64_32:return sizeof(fitted_peak_t);
//    }
//    return 0;
//}


//double get_x_value(spectrum_peaks_t spectrum_data, int index)
//{
//    switch (spectrum_data.data_precision)
//    {
//    case DATA_PRECISION_32_32:return spectrum_data.peaks32_32[index].x;
//    case DATA_PRECISION_64_32:return spectrum_data.peaks64_32[index].x;
//    case DATA_PRECISION_64_64:return spectrum_data.peaks64_64[index].x;
//    }
//    return 0;
//}

//double get_y_value(spectrum_peaks_t spectrum_data, int index)
//{
//    switch (spectrum_data.data_precision)
//    {
//    case DATA_PRECISION_32_32:return spectrum_data.peaks32_32[index].y;
//    case DATA_PRECISION_64_32:return spectrum_data.peaks64_32[index].y;
//    case DATA_PRECISION_64_64:return spectrum_data.peaks64_64[index].y;
//    }
//    return 0;
//}

///**
// * @brief get_left_hwhm_value get left hwhm value value from spectrum_data_t depending of its DATA_PRECISION (only needed in fitted mode)
// * @param spectrum_data
// * @param index
// * @param data_precision
// * @return
// */
//// TODO: private
//double get_left_hwhm_value(spectrum_peaks_t spectrum_data, int index, data_precision_enum data_precision)
//{
//    return (data_precision==DATA_PRECISION_FITTED_64_32) ? spectrum_data.fitted_peaks[index].left_hwhm : 0;
//}

///**
// * @brief get_right_hwhm_value get RightHwhmValue value from spectrum_data_t depending of its DATA_PRECISION (only needed in fitted mode)
// * @param spectrum_data
// * @param index
// * @param data_precision
// * @return
// */
//// TODO: private
//double get_right_hwhm_value(spectrum_peaks_t spectrum_data, int index, data_precision_enum data_precision)
//{
//    return (data_precision==DATA_PRECISION_FITTED_64_32) ? spectrum_data.fitted_peaks[index].right_hwhm : 0;
//}


//int get_bounding_box_data_v1(sqlite3 *db, int bb_id, byte **blob, int *blob_length, char **err_msg)
//{
//    sqlite3_blob *blob_handle;
//    int rc;
//    if ((rc = sqlite3_blob_open(db, "main", "bounding_box", "data", bb_id, 0, &blob_handle)) != SQLITE_OK) return rc;

//    *blob_length = sqlite3_blob_bytes(blob_handle);
//    *blob = (byte*)calloc(*blob_length, sizeof(byte));
//    rc = sqlite3_blob_read(blob_handle, *blob, *blob_length, 0);
//    sqlite3_blob_close(blob_handle);
//    return rc;
//}

//int get_data_encoding_id_(sqlite3 *db, int spectrum_id, int *data_encoding_id, char **err_msg)
//{
//    char *sql_str_encoding_id = sqlite3_mprintf("SELECT data_encoding_id FROM spectrum WHERE id=%d", spectrum_id);
//    int rc = execute_select_stmt(db, sql_str_encoding_id, 0, &get_int_cb, data_encoding_id, err_msg);
//    sqlite3_free(sql_str_encoding_id);
//    return rc;
//}

//int get_data_encoding_id_or_die_(sqlite3 *db, int spectrum_id)
//{
//    char* err_msg = NULL;
//    int res;
//    int rc = get_data_encoding_id(db, spectrum_id, &res, &err_msg);
//    die_on_sqlite_error(rc, err_msg);
//    return res;
//}

//int _get_bbox_count(sqlite3* db, int* bb_count)
//{
//    sqlite3_stmt* stmt;
//    int rc, stmt_step_res;

//    if ((rc= sqlite3_prepare_v2(db, "SELECT count(id) FROM bounding_box", -1, &stmt, NULL))!=SQLITE_OK) return rc;
//    if ((stmt_step_res= sqlite3_step(stmt))!=SQLITE_ROW) return rc;

//    *bb_count = sqlite3_column_int(stmt, 0);
//    sqlite3_finalize(stmt); //delete the stmt

//    return rc;
//}

//indexed_bounding_box_t index_bbox_old(bounding_box_t bbox, data_encodings_cache_t de_cache)
//{
//    indexed_bounding_box_t indexed_bb;
//    _index_each_blob_id(bbox, &indexed_bb, de_cache); //calculate the mapping
//    return indexed_bb;
//}







//int _index_each_blob_id(const bounding_box_t bbox, indexed_bounding_box_t *indexed_bbox, const data_encodings_cache_t cache)
//{
//    int* spectrum_id_to_data_encoding_id = cache.spectrum_id_to_data_encoding_id;
//    int estimated_slices_count = 1 + bbox.last_spectrum_id - bbox.first_spectrum_id; //will always be a greater value than the slices count

//    int size_of_array = sizeof(int) * estimated_slices_count;
//    int* slice_indexes = (int*) malloc(size_of_array);
//    int* spectra_ids = (int*) malloc(size_of_array);
//    int* peaks_counts = (int*) malloc(size_of_array);

//    int bytes_idx = 0; //loop index
//    int peak_size, peak_count, spectrum_id;
//    int slice_count = 0;
//    data_encoding_t de;

//    byte* blob = bbox.blob;
//    // FIXME: avoid this? => see: https://stackoverflow.com/questions/8703047/how-to-convert-a-byte-array-into-double-in-c
//    int* blob_as_ints = (int*)blob; //use to access to the spectrum id and the peak count, a blob will always be a multiple of 4
//    int bytes_as_ints_offset = 0;
//    int blob_size = bbox.blob_size;

//    while (bytes_idx < blob_size) //for each spectrum slice store in the blob
//    {
//        spectrum_id = blob_as_ints[bytes_as_ints_offset]; //get the current spectrum id in the blob
//        spectra_ids[slice_count] = spectrum_id; //stock the spectrum id
//        peak_count = blob_as_ints[bytes_as_ints_offset + 1]; //get the current peak count in the blob
//        peaks_counts[slice_count] = peak_count; //stock the peak count

//        int de_id = spectrum_id_to_data_encoding_id[spectrum_id]; //get the data encoding id which match with the spectrum id in the cache
//        de = *get_data_encoding_from_cache(&cache, de_id); //get the data encoding from the cache with its id

//        peak_size = (de.mode == FITTED) ? de.peak_encoding + 8 : de.peak_encoding; //increase the peak size if the peaks are fitted
//        slice_indexes[slice_count] = bytes_idx; //store the last byte index
//        bytes_idx += 8 + (peak_size * peak_count); //shift the byte index with the size (in byte) of the current spectrum slice
//        bytes_as_ints_offset = (bytes_idx / 4); //convert the byte index into integer index
//        slice_count++;
//    }

//    // Set fields of indexed_bbox struct
//    indexed_bbox->spectrum_slices_count = slice_count;
//    indexed_bbox->spectra_ids = spectra_ids;
//    indexed_bbox->slices_indexes = slice_indexes;
//    indexed_bbox->peaks_counts = peaks_counts;
//    indexed_bbox->bb = bbox;

//    // return OK
//    return 0;
//}


#define WAIT_HAS_NEXT   0
#define WAIT_NEXT       1

#define BUFFER_MAX_SIZE 1024 //

//#include <windows.h>
//#include <psapi.h>

//// TODO: move to util.c
//void libmzdb_memdisp(char* msg)
//{
//    PROCESS_MEMORY_COUNTERS_EX pmc;
//    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
//    SIZE_T mem = pmc.PrivateUsage;
//    printf("%s => mem: %llu\n", msg, mem/1024);
//    //Sleep(100);
//}


void meminit_null_callback(sqlite3_stmt* stmt, void* arg, void** res){} //this callback will be use in the meminit, has no effect

//this function is use to init the memory
void libmzdb_meminit(sqlite3* db) //FIXME: SegFault // rename: init_io_cache
{
    // TODO: only read bytes from disk
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT * FROM bounding_box", -1, &stmt, NULL);
    libmzdb_stmt_iterator_for_each(stmt, &meminit_null_callback, NULL, NULL);
    sqlite3_finalize(stmt);
}

/*==============================STMT ITERATOR=======================================*/

libmzdb_stmt_iterator_t* libmzdb_stmt_iterator_create(sqlite3_stmt *stmt)
{
    libmzdb_stmt_iterator_t* stmt_it = (libmzdb_stmt_iterator_t*) malloc(sizeof(libmzdb_stmt_iterator_t));
    stmt_it->stmt = stmt;
    stmt_it->is_iterator_checked = WAIT_HAS_NEXT; //wait only a has next, next will be ignored
    return stmt_it;
}

int libmzdb_stmt_iterator_dispose(libmzdb_stmt_iterator_t *it)
{
    free(it);
    return IT_DONE;
}

int libmzdb_stmt_iterator_has_next(libmzdb_stmt_iterator_t *it)
{
    if (it->is_iterator_checked == WAIT_NEXT) return it->last_rc; //use as a barrier, lock the "has_next" while "next" has not been called
    else it->is_iterator_checked = WAIT_NEXT; //lock the has_next and wait for a next

    it->last_rc = sqlite3_step(it->stmt);
    switch (it->last_rc)
    {
    case SQLITE_ROW:    return IT_STEP;
    case SQLITE_DONE:   return IT_DONE;
    default:            return IT_ERROR;
    }
}

sqlite3_stmt* libmzdb_stmt_iterator_next(libmzdb_stmt_iterator_t *it)
{
    if (it->is_iterator_checked == WAIT_HAS_NEXT)
        if (libmzdb_stmt_iterator_has_next(it) == IT_ERROR) return NULL; //if the checker wait a "has_next", call the has_next

    it->is_iterator_checked = WAIT_HAS_NEXT; // reset the checker

    return it->stmt; // return current stmt
}


int libmzdb_stmt_iterator_for_each(sqlite3_stmt *stmt, void (*callback) (sqlite3_stmt*, void*, void**), void *arg, void **result)
{
    libmzdb_stmt_iterator_t* it = libmzdb_stmt_iterator_create(stmt);
    int rc = IT_DONE;
    while((rc = libmzdb_stmt_iterator_has_next(it)) == IT_STEP)
    {
        if(!(libmzdb_stmt_iterator_next(it))) return IT_ERROR;
        callback(it->stmt, arg, result);
    }
    libmzdb_stmt_iterator_dispose(it);
    return rc;
}


/*==============================BBOX ITERATOR====================================*/

libmzdb_bbox_iterator_t* libmzdb_bbox_iterator_create(sqlite3_stmt* stmt, libmzdb_entity_cache_t entity_cache)
{
    libmzdb_bbox_iterator_t* bb_it = (libmzdb_bbox_iterator_t*) malloc(sizeof(libmzdb_bbox_iterator_t));
    bb_it->stmt_it = libmzdb_stmt_iterator_create(stmt);
    bb_it->entity_cache = entity_cache;
    return bb_it;
}

void libmzdb_bbox_iterator_dispose(libmzdb_bbox_iterator_t* bb_it)
{
    libmzdb_stmt_iterator_dispose(bb_it->stmt_it);
    free(bb_it);
}

int libmzdb_bbox_iterator_has_next(libmzdb_bbox_iterator_t* bbox_it)
{
    libmzdb_stmt_iterator_t* stmt_it = bbox_it->stmt_it;
    return libmzdb_stmt_iterator_has_next(stmt_it);
}

int libmzdb_bbox_iterator_next(libmzdb_bbox_iterator_t* bb_it, libmzdb_bounding_box_t* bbox)
{
    sqlite3_stmt* stmt = libmzdb_stmt_iterator_next(bb_it->stmt_it);
    if(!stmt) return IT_ERROR;

    //build the next bbox
    *bbox = libmzdb_build_bbox(stmt, 1);

    return IT_OK;
}

int libmzdb_bbox_iterator_for_each(sqlite3_stmt* stmt, libmzdb_entity_cache_t entity_cache, void (*callback) (libmzdb_bounding_box_t, void*, void**), void* arg, void **result)
{
    libmzdb_bbox_iterator_t* it = libmzdb_bbox_iterator_create(stmt, entity_cache);

    int rc = IT_DONE;
    libmzdb_bounding_box_t current;
    //current.blob = NULL;
    while((rc = libmzdb_bbox_iterator_has_next(it)) == IT_STEP)
    {
        if(libmzdb_bbox_iterator_next(it, &current) != IT_OK) return IT_ERROR;
        callback(current, arg, result);
        //current.blob is free with the sqlite3_step, sqlite3_finalize ...
    }
    libmzdb_bbox_iterator_dispose(it);
    return rc;
}

/*==============================SPECTRUM ITERATOR====================================*/

int _spectrum_iterator_load_bb_row(libmzdb_spectrum_iterator_t* it, libmzdb_indexed_bounding_box_t** indexed_bb_row)
{
    //printf("bb id=%d\n", it->first_bb->id);

    libmzdb_entity_cache_t entity_cache = it->bbox_it->entity_cache;
    libmzdb_indexed_bounding_box_t indexed_first_bb = libmzdb_index_bbox(*it->first_bb, entity_cache.data_encodings_cache);

    // Allocate memory for BBs
    libmzdb_indexed_bounding_box_t* indexed_bbs = (libmzdb_indexed_bounding_box_t*) malloc(sizeof(libmzdb_indexed_bounding_box_t) * it->run_slices_count);
    indexed_bbs[0] = indexed_first_bb;

    // Load all BBs having the same first spectrum id (<=> load a row of BBs)
    libmzdb_bounding_box_t current_bbox;
    int current_first_spectrum_id = indexed_first_bb.spectra_ids[0];
    int bb_count = 1;

    int is_first_spectrum_id_modified = 0;

    while ( (!is_first_spectrum_id_modified) && (it->has_next_bb = libmzdb_bbox_iterator_has_next(it->bbox_it) == IT_STEP) )
    {

        libmzdb_bbox_iterator_next(it->bbox_it, &current_bbox); //TODO: check bbox validity
        libmzdb_indexed_bounding_box_t cur_indexed_bb = libmzdb_index_bbox(current_bbox, entity_cache.data_encodings_cache);

        if (cur_indexed_bb.spectra_ids[0] != current_first_spectrum_id)
        {
            free(it->first_bb); //free the previous first bb

            //create and init the next first bb
            it->first_bb = (libmzdb_bounding_box_t*) malloc(sizeof(libmzdb_bounding_box_t));
            *(it->first_bb) = current_bbox;

            current_first_spectrum_id = cur_indexed_bb.spectra_ids[0];

            is_first_spectrum_id_modified = 1;
        }
        else indexed_bbs[bb_count++] = cur_indexed_bb;
    }

    //printf("load bb row stop with %d && %d \n", (!is_first_spectrum_id_modified), it->has_next_bb);

    *indexed_bb_row = indexed_bbs;

    return bb_count;
}

int _spectrum_iterator_bb_row_to_spectrum_buffer(libmzdb_spectrum_iterator_t* it, libmzdb_indexed_bounding_box_t* indexed_bb_row, int bb_count, libmzdb_spectrum_t* spectrum_buffer, int sb_offset)
{
    libmzdb_entity_cache_t entity_cache = it->bbox_it->entity_cache;

    // Compute the peak_count sum for each spectrum
    int spectra_count = indexed_bb_row[0].spectrum_slices_count;
    int* spectrum_peak_counts = (int *) calloc(spectra_count, sizeof(int));

    //for each spectrum store in each bbox, sum the result of their peak count
    for (int bb_idx=0; bb_idx<bb_count; ++bb_idx)
        for (int spectra_idx=0; spectra_idx<spectra_count; ++spectra_idx)
            spectrum_peak_counts[spectra_idx] += indexed_bb_row[bb_idx].peaks_counts[spectra_idx];

    //create a new 2D array of (spectrum, bbox) => for each spectrum idx, there is an array of spectrum slice
    //spectrum_data_t** bbs_sd_slices = (spectrum_data_t**) malloc(sizeof(spectrum_data_t*) * spectra_count);
    //for(int i=0; i<spectra_count; ++i) bbs_sd_slices[i] = (spectrum_data_t*) malloc(sizeof(spectrum_data_t) * bb_count);

    libmzdb_data_encodings_cache_t de_cache = entity_cache.data_encodings_cache;

    int spectrum_buffer_index;
    //initialise the array of spectrum and build the spectrum by merging each spectrum slice
    for (int spectrum_idx=0; spectrum_idx<spectra_count; ++spectrum_idx)
    {
        libmzdb_spectrum_data_t* spectrum_slices = (libmzdb_spectrum_data_t*) malloc(sizeof(libmzdb_spectrum_data_t) * bb_count);

        //for each bounding box, get the spectrum slice of the current spectrum index
        for (int bb_idx=0; bb_idx<bb_count; ++bb_idx)
        {
            libmzdb_indexed_bounding_box_t indexed_bb = indexed_bb_row[bb_idx];
            int spectrum_id = indexed_bb.spectra_ids[spectrum_idx];
            //printf("spectrum_idx=%d\n",spectrum_idx);
            //printf("spectrum_id=%d\n",spectrum_id);

            //const libmzdb_data_encoding_t de = *libmzdb_get_data_encoding_from_cache(&de_cache, de_cache.spectrum_id_to_data_encoding_id[spectrum_id]);
            //printf("de=%d\n", de.peak_encoding);

            // TODO: use the stack luke

            libmzdb_spectrum_data_t* sd_ptr = libmzdb_read_spectrum_slice_data_at(indexed_bb, de_cache, spectrum_idx, spectrum_id, -1, -1);
            //bbs_sd_slices[spectrum_idx][bb_idx] = *sd_ptr;
            spectrum_slices[bb_idx] = *sd_ptr;

            //printf("first m/z=%f\n",sd_ptr->mz_array_as_doubles[0]);

            free(sd_ptr); //free the pointer but not the content set in the array
        }

        //printf("before merge\n");

        spectrum_buffer_index = spectrum_idx + sb_offset; //move to the next spectrum index
        spectrum_buffer[spectrum_buffer_index].data = libmzdb_merge_spectrum_slices(spectrum_slices, bb_count, spectrum_peak_counts[spectrum_idx]);
        spectrum_buffer[spectrum_buffer_index].header = entity_cache.spectrum_headers[indexed_bb_row->spectra_ids[spectrum_idx] - 1];

        //printf("after merge %f\n", spectrum_buffer[spectrum_buffer_index].data.intensity_array_as_floats[0]);

        //free all the data in the array of spectrum slices, each slice has been copied and merged
        for(int j=0; j<bb_count; ++j) libmzdb_free_spectrum_data_content(spectrum_slices[j]);
        free(spectrum_slices);
    }

    //printf("end of _spectrum_iterator_bb_row_to_spectrum_buffer\n");

//    //build each spectrum by merging each spectrum slice
//    int spectrum_buffer_index;
//    for (int spectrum_idx=0; spectrum_idx<spectra_count; ++spectrum_idx)
//    {
//        spectrum_buffer_index = spectrum_idx + sb_offset; //move to the next spectrum index
//        spectrum_buffer[spectrum_buffer_index].data = merge_spectrum_slices(bbs_sd_slices[spectrum_idx], bb_count, spectrum_peak_counts[spectrum_idx]);
//        spectrum_buffer[spectrum_buffer_index].header = entity_cache.spectrum_headers[indexed_bb_row->spectra_ids[spectrum_idx] - 1];
//    }

    //free all the data in the 2D array, each spectrum has been copied and merged
    /*for(int i=0; i<spectra_count; ++i)
    {
        for(int j=0; j<bb_count; ++j) free_spectrum_data_content(bbs_sd_slices[i][j]);
        free(bbs_sd_slices[i]);
    }
    free(bbs_sd_slices);*/

    return spectra_count;
}

int _spectrum_id_comparator(const void * a, const void * b) {
   return ( ((libmzdb_spectrum_t*)a)->header.id - ((libmzdb_spectrum_t*)b)->header.id );
}


void _spectrum_iterator_fill_spectrum_buffer(libmzdb_spectrum_iterator_t* it)
{
//    printf("\nStart to fill the buffer\n\n");
    libmzdb_entity_cache_t entity_cache = it->bbox_it->entity_cache;
    libmzdb_spectrum_t* spectrum_buffer = (libmzdb_spectrum_t*) malloc(sizeof(libmzdb_spectrum_t) * BUFFER_MAX_SIZE); // aribitray MAX size (should be < ~50)
    libmzdb_indexed_bounding_box_t* indexed_bbs;
    int spectrum_count = 0;
    int bb_count;
    int next_ms_level;
    int ms1_row_count = 0;

    //int spectrum_count = entity_cache.spectrum_header_count;

    //the loop will stop if the next ms level is a ms level 1 and if a ms level 1 has already been processed
    //=> will collect one ms level 1 and each ms level > 1 (before or after the ms level 1)
    // note: this loop is required to sort MS1 and MS2 spectra and thus iterate them in the right order
    int previous_first_spectrum_id = -1;
    while (ms1_row_count <= 1 && previous_first_spectrum_id != it->first_bb->first_spectrum_id) {

        previous_first_spectrum_id = it->first_bb->first_spectrum_id;

        //this will load a bb row and convert each of them into spectrum data
        bb_count = _spectrum_iterator_load_bb_row(it, &indexed_bbs); //load each bb in the row, will update the it->first_bb
        //printf("Spectrum cout before bb row to spectrum buff: %d \n", spectrum_count);
        spectrum_count += _spectrum_iterator_bb_row_to_spectrum_buffer(it, indexed_bbs, bb_count, spectrum_buffer, spectrum_count);
        //printf("Spectrum cout after bb row to spectrum buff: %d \n", spectrum_count);

        //printf("before free each indexed bbox ms1_row_count=%d\n", ms1_row_count);

//        printf("after _spectrum_iterator_bb_row_to_spectrum_buffer %f\n", spectrum_buffer[spectrum_count - 1].data.intensity_array_as_floats[0]);

        //free each indexed bbox
        for(int i=0; i<bb_count; ++i)
        {
            free(indexed_bbs[i].peaks_counts);
            free(indexed_bbs[i].slices_indexes);
            free(indexed_bbs[i].spectra_ids);
            free(indexed_bbs[i].bb.blob);
        }
        free(indexed_bbs);

        next_ms_level = entity_cache.spectrum_headers[it->first_bb->first_spectrum_id - 1].ms_level;

        if (next_ms_level == 1) ms1_row_count += 1;
        if (it->is_single_ms_level_mode) break; //if the iterator loads only one ms level, it needs to only go through the loop once
    }

    //printf("before qsort %f\n", spectrum_buffer[spectrum_count - 1].data.intensity_array_as_floats[0]);

    //sort the array with the spectrum id
    qsort(spectrum_buffer, spectrum_count, sizeof(libmzdb_spectrum_t), _spectrum_id_comparator);

    //printf("after qsort %f\n", spectrum_buffer[spectrum_count - 1].data.intensity_array_as_floats[0]);

    //for each spectrum in the previous buffer, free it
    for (int i=0; i<it->spectrum_count; ++i) libmzdb_free_spectrum_data_content(it->spectrum_buffer[i].data);

    free(it->spectrum_buffer); //free the buffer

    it->spectrum_buffer = spectrum_buffer; //replace the freed buffer
    it->spectrum_count = spectrum_count;
    it->spectrum_idx = 0; //reset the current spectrum index use by the next function
}

#define SQLQUERY_ALLMSLEVELS "SELECT bounding_box.* FROM bounding_box, spectrum WHERE spectrum.id = bounding_box.first_spectrum_id"
#define SQLQUERY_SINGLEMSLEVEL "SELECT bounding_box.* FROM bounding_box, spectrum WHERE spectrum.id = bounding_box.first_spectrum_id AND spectrum.ms_level= %d"

libmzdb_spectrum_iterator_t* libmzdb_spectrum_iterator_create(sqlite3* db, int ms_level, libmzdb_entity_cache_t entity_cache)
{
    sqlite3_stmt* stmt;

    //initialise the query (all ms level or only one), and preprare it for the iterator
    char* sql_str = (ms_level <= 0) ? SQLQUERY_ALLMSLEVELS: sqlite3_mprintf(SQLQUERY_SINGLEMSLEVEL, ms_level);

    int rc = sqlite3_prepare_v2(db, sql_str, -1, &stmt, NULL);

    if(ms_level > 0) sqlite3_free((void *)sql_str);

    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        return NULL;
    }

    libmzdb_spectrum_iterator_t* it = (libmzdb_spectrum_iterator_t*) malloc(sizeof(libmzdb_spectrum_iterator_t));
    it->bbox_it = libmzdb_bbox_iterator_create(stmt, entity_cache);

	if (libmzdb_bbox_iterator_has_next(it->bbox_it) != IT_STEP) //if there isn't bounding to process, return NULL
	{
		libmzdb_spectrum_iterator_dispose(it);
		return NULL;
	}

	// init the first bb
	it->first_bb = (libmzdb_bounding_box_t*)malloc(sizeof(libmzdb_bounding_box_t));
	if (libmzdb_bbox_iterator_next(it->bbox_it, it->first_bb) != IT_OK) //if next failed, retrun NULL
	{
		libmzdb_spectrum_iterator_dispose(it);
		return NULL;
	}

	// Init iterator members
	it->is_single_ms_level_mode = (ms_level > 0);
	it->is_iterator_checked = WAIT_HAS_NEXT;  // wait only a has next, next will be ignored
	it->last_has_next_value = IT_ERROR;
	it->spectrum_idx = 0;
	it->spectrum_count = -1;
	it->has_next_bb = 1;

    //create and fill the spectrum buffer
    it->spectrum_buffer = NULL;
    it->run_slices_count = libmzdb_get_run_slices_count_from_sequence_or_die(db);

    //_spectrum_iterator_fill_spectrum_buffer(it);

    return it;
}

void libmzdb_spectrum_iterator_dispose(libmzdb_spectrum_iterator_t* spectrum_it)
{
    //printf("before spectrum_iterator_dispose\n");

    //free the first bb and its content
    free(spectrum_it->first_bb);

    //free the spectrum buffer and its content
    for(int i=0; i<spectrum_it->spectrum_count; ++i)
        libmzdb_free_spectrum_data_content(spectrum_it->spectrum_buffer[i].data); //free only the data, headers depend on the entity cache
    free(spectrum_it->spectrum_buffer);

    //free the bbox iterator
    libmzdb_bbox_iterator_dispose(spectrum_it->bbox_it);

    free(spectrum_it);
}

/*int _libmzdb_spectrum_iterator_next(libmzdb_spectrum_iterator_t* it, libmzdb_spectrum_t* result) {
    //int spectrum_count = (it->has_next_bb) ? it->spectrum_count : it->spectrum_count - 1; //FIXME: spectra count is incorrect in the last fill buffer
    int spectrum_count = it->spectrum_count;

    if (it->spectrum_idx >= spectrum_count) //if no more spectrum, fill the next bounding box
    {
        if (it->has_next_bb) _spectrum_iterator_fill_spectrum_buffer(it);
        else //if there isn't next bouding box, free the first bb
        {
            free(it->first_bb);
            it->first_bb = NULL; //has next will return false if first bb is NULL
        }
    }

    *result = it->spectrum_buffer[it->spectrum_idx++]; //get the spectrum at the saved idx
    return IT_OK;
}*/

int libmzdb_spectrum_iterator_has_next(libmzdb_spectrum_iterator_t* it)
{
    //printf("libmzdb_spectrum_iterator_has_next: is_iterator_checked %d \n ", it->is_iterator_checked);

    // If next has not be called before (i.e. two consecutive has_next calls)
    if (it->is_iterator_checked == WAIT_NEXT) return it->last_has_next_value; // use as a barrier, lock the "has_next" while "next" has not been called
    else it->is_iterator_checked = WAIT_NEXT; // lock the has_next and wait for a next

    //printf("after libmzdb_spectrum_iterator_has_next: is_iterator_checked %d \n ", it->is_iterator_checked);

    // Fill spectrum buffer if needed
    int has_next_spectrum = IT_STEP;
    if (it->spectrum_idx >= it->spectrum_count) // if no more spectrum, fill the next bounding box
    {
		//printf("after libmzdb_spectrum_iterator_has_next: has_next_b %d \n ", it->has_next_bb);

        if (it->has_next_bb) _spectrum_iterator_fill_spectrum_buffer(it);
        else // if there isn't next bouding box, free the first bb
        {
            free(it->first_bb);
            it->first_bb = NULL; // has next will return false if first bb is NULL
            has_next_spectrum = IT_DONE;
        }
    }

    it->last_has_next_value = has_next_spectrum;

    //int has_more_spectra = (it->has_next_bb) ? 1 : 0; // it->spectrum_idx < it->spectrum_count &&
    //return it->first_bb ? 1 : 0;
    //return it->has_next_bb;
    return has_next_spectrum;
}

int libmzdb_spectrum_iterator_next(libmzdb_spectrum_iterator_t* it, libmzdb_spectrum_t* result)
{
    //printf("libmzdb_spectrum_iterator_next: is_iterator_checked %d \n ", it->is_iterator_checked);

    if (it->is_iterator_checked == WAIT_HAS_NEXT)
        if (!libmzdb_spectrum_iterator_has_next(it)) {
            it->is_iterator_checked = WAIT_HAS_NEXT; // reset the checker
            return IT_DONE; //if the checker wait a "has_next", call the has_next
        }

    it->is_iterator_checked = WAIT_HAS_NEXT; // reset the checker

    //printf("after libmzdb_spectrum_iterator_next: is_iterator_checked %d \n ", it->is_iterator_checked);

	//printf("after libmzdb_spectrum_iterator_next: spectrum_idx %d \n ", it->spectrum_idx);

    *result = it->spectrum_buffer[it->spectrum_idx++]; //get the spectrum at the saved idx

    return IT_OK;
}

int libmzdb_spectrum_iterator_for_each(sqlite3* db, int ms_level, libmzdb_entity_cache_t entity_cache, void (*callback) (libmzdb_spectrum_t, void*, void**), void* arg, void **result)
{
    libmzdb_spectrum_iterator_t* it = libmzdb_spectrum_iterator_create(db, ms_level, entity_cache);
    int rc = IT_DONE;
    libmzdb_spectrum_t current_spectrum;

//    libmzdb_spectrum_iterator_has_next(it);
//    if(libmzdb_spectrum_iterator_next(it, &current_spectrum) != IT_OK) return IT_ERROR;
//    callback(current_spectrum, arg, result);

//    int n = 0;

    while((rc = libmzdb_spectrum_iterator_has_next(it)) == IT_STEP)
    {
        if(libmzdb_spectrum_iterator_next(it, &current_spectrum) != IT_OK) return IT_ERROR;
        callback(current_spectrum, arg, result);
//        n++;
    }

    libmzdb_spectrum_iterator_dispose(it);

//    printf("read %d spectra\n", n);

    return rc;
}














































//void _spectrum_iterator_init_spectrum_slices_buffer(spectrum_iterator_t* it)
//{
//    entity_cache_t entity_cache = it->bbox_it->entity_cache;
//    indexed_bounding_box_t indexed_first_bb = index_bbox(*it->first_bb, entity_cache.data_encodings_cache);

//    if (it->first_spectrum_slices != NULL)
//    {
//        it->spectrum_slice_buffer = it->first_spectrum_slices;
//        it->spectrum_slice_buffer_count = it->first_spectrum_slices_count;
//    }
//    else
//    {
//        it->spectrum_slice_buffer = bbox_to_slices(indexed_first_bb, entity_cache);
//        it->spectrum_slice_buffer_count = indexed_first_bb.spectrum_slices_count;
//    }

//    it->spectrum_slice_idx = 0;

//    bounding_box_t current_bbox;
//    indexed_bounding_box_t indexed_current_bb;
//    spectrum_t* s_slices;
//    int s_slices_count;

//    while((it->bb_has_next = bbox_iterator_has_next(it->bbox_it)) == IT_STEP)
//    {
//        bbox_iterator_next(it->bbox_it, &current_bbox); //to check
//        indexed_current_bb = index_bbox(current_bbox, entity_cache.data_encodings_cache);

//        s_slices = bbox_to_slices(indexed_current_bb, entity_cache);
//        s_slices_count = indexed_current_bb.spectrum_slices_count;

//        if (s_slices[0].header.id == it->spectrum_slice_buffer[0].header.id)
//        {
//            for(int i=0; i<s_slices_count; ++i)
//            {
//                it->spectrum_slice_buffer[i].header = s_slices[i].header; //set the header

//                //printf("header: %d \n", s_slices[i].header.id);

//                spectrum_data_t* merged_sd = (spectrum_data_t*) malloc(sizeof(spectrum_data_t));

//                spectrum_data_t slice_data_at_i = s_slices[i].data;
//                spectrum_data_t buffered_slice_data_at_i = it->spectrum_slice_buffer[i].data;

//                merged_sd->data_encoding = slice_data_at_i.data_encoding;
//                merged_sd->peak_count = slice_data_at_i.peak_count + buffered_slice_data_at_i.peak_count;

//                malloc_spectrum_data_peaks(merged_sd);

//                // TODO: factorize me with code of get_spectrum
//                switch (merged_sd->data_encoding->peak_encoding)
//                {
//                case HIGH_RES_PEAK:



//memcpy(merged_sd->mz_array_as_doubles, buffered_slice_data_at_i.mz_array_as_doubles, buffered_slice_data_at_i.peak_count * sizeof(double));
//                    memcpy(&merged_sd->mz_array_as_doubles[buffered_slice_data_at_i.peak_count], slice_data_at_i.mz_array_as_doubles, slice_data_at_i.peak_count * sizeof(double));

//                    memcpy(merged_sd->intensity_array_as_floats, buffered_slice_data_at_i.intensity_array_as_floats, buffered_slice_data_at_i.peak_count * sizeof(float));
//                    memcpy(&merged_sd->intensity_array_as_floats[buffered_slice_data_at_i.peak_count], slice_data_at_i.intensity_array_as_floats, slice_data_at_i.peak_count * sizeof(float));

//                    break;

//                case LOW_RES_PEAK:

//                    memcpy(merged_sd->mz_array_as_floats, buffered_slice_data_at_i.mz_array_as_floats, buffered_slice_data_at_i.peak_count * sizeof(float));
//                    memcpy(&merged_sd->mz_array_as_floats[buffered_slice_data_at_i.peak_count], slice_data_at_i.mz_array_as_floats, slice_data_at_i.peak_count * sizeof(float));

//                    memcpy(merged_sd->intensity_array_as_floats, buffered_slice_data_at_i.intensity_array_as_floats, buffered_slice_data_at_i.peak_count * sizeof(float));
//                    memcpy(&merged_sd->intensity_array_as_floats[buffered_slice_data_at_i.peak_count], slice_data_at_i.intensity_array_as_floats, slice_data_at_i.peak_count * sizeof(float));

//                    break;

//                case NO_LOSS_PEAK:

//                    memcpy(merged_sd->mz_array_as_doubles, buffered_slice_data_at_i.mz_array_as_doubles, buffered_slice_data_at_i.peak_count * sizeof(double));
//                    memcpy(&merged_sd->mz_array_as_doubles[buffered_slice_data_at_i.peak_count], slice_data_at_i.mz_array_as_doubles, slice_data_at_i.peak_count * sizeof(double));

//                    memcpy(merged_sd->intensity_array_as_doubles, buffered_slice_data_at_i.intensity_array_as_doubles, buffered_slice_data_at_i.peak_count * sizeof(double));
//                    memcpy(&merged_sd->intensity_array_as_doubles[buffered_slice_data_at_i.peak_count], slice_data_at_i.intensity_array_as_doubles, slice_data_at_i.peak_count * sizeof(double));

//                    break;
//                }

//                if (merged_sd->data_encoding->mode == FITTED)
//                {
//                    memcpy(merged_sd->lwhm_array, buffered_slice_data_at_i.lwhm_array, buffered_slice_data_at_i.peak_count * sizeof(float));
//                    memcpy(&merged_sd->lwhm_array[buffered_slice_data_at_i.peak_count], slice_data_at_i.lwhm_array, slice_data_at_i.peak_count * sizeof(float));

//                    memcpy(merged_sd->rwhm_array, buffered_slice_data_at_i.rwhm_array, buffered_slice_data_at_i.peak_count * sizeof(float));
//                    memcpy(&merged_sd->rwhm_array[buffered_slice_data_at_i.peak_count], slice_data_at_i.rwhm_array, slice_data_at_i.peak_count * sizeof(float));
//                }

//                it->spectrum_slice_buffer[i].data = *merged_sd;
//            }
//        }
//        else
//        {
//            *(it->first_bb) = current_bbox;


//            //free(it->first_spectrum_slices);
//            it->first_spectrum_slices = s_slices;
//            //indexed need to be free ?
//            break;
//        }
//    }

//}

// TODO

int libmzdb_open_mzdb_file(const char* filename, sqlite3** pp_db)
{
    // TODO: use int32_t instead of int/long everywhere in the project
    // See: https://stackoverflow.com/questions/11438794/is-the-size-of-c-int-2-bytes-or-4-bytes/11438838
    assert(sizeof(int) == 4); // check that mzDB files can be decoded on this platform
    return sqlite3_open_v2(filename, pp_db, SQLITE_OPEN_READONLY, NULL);
}

int libmzdb_close_mzdb_file(sqlite3* pp_db)
{
    return sqlite3_close(pp_db);
}

int libmzdb_create_spectrum_header(sqlite3_stmt* stmt, libmzdb_spectrum_header_t* spectrum_header, char** err_msg)
{
    if(!spectrum_header) return -1; //add warning log
    libmzdb_spectrum_header_t* h = spectrum_header; //esthetic variable
    //init the header
    h->id = sqlite3_column_int(stmt, 0);
    h->initial_id = sqlite3_column_int(stmt, 1);
    h->title = strdup((const char *) sqlite3_column_text(stmt, 2));
    h->cycle = sqlite3_column_int(stmt, 3);
    h->time = (float)sqlite3_column_double(stmt, 4);
    h->ms_level = sqlite3_column_int(stmt, 5);
    h->activation_type = strdup( (const char *) sqlite3_column_text(stmt, 6));
    h->tic = (float)sqlite3_column_double(stmt, 7);
    h->base_peak_mz = sqlite3_column_double(stmt, 8);
    h->base_peak_intensity = (float)sqlite3_column_double(stmt, 9);
    h->precursor_mz = sqlite3_column_double(stmt, 10);
    h->precursor_charge = sqlite3_column_int(stmt, 11);
    h->peaks_count = sqlite3_column_int(stmt, 12);
    h->param_tree_str = strdup((const char *)sqlite3_column_text(stmt, 13)); //TODO: check!
    h->scan_list_str = strdup((const char *)sqlite3_column_text(stmt, 14));
    h->precursor_list_str = strdup((const char *)sqlite3_column_text(stmt, 15));
    h->product_list_str = strdup((const char *)sqlite3_column_text(stmt, 16));
    h->shared_param_tree_id = sqlite3_column_int(stmt, 17);
    h->instrument_configuration_id = sqlite3_column_int(stmt, 18);
    h->source_file_id = sqlite3_column_int(stmt, 19);
    h->run_id = sqlite3_column_int(stmt, 20);
    h->data_processing_id = sqlite3_column_int(stmt, 21);
    h->data_encoding_id = sqlite3_column_int(stmt, 22);
    h->bb_first_spectrum_id = sqlite3_column_int(stmt, 23);
    return SQLITE_OK;
}

int libmzdb_get_spectrum_headers(sqlite3* db, libmzdb_spectrum_header_t** spectrum_headers, int* spectrum_header_count, char** err_msg)
{
    sqlite3_stmt* stmt;
    int rc /* sql result state */, stmt_step_res /* step result state */;

    //get the length of the spectrum_headers array

    //*err_msg = "Error: get spectrum failed headers (count query)"; //to improve
    if ((rc= sqlite3_prepare_v2(db, "SELECT count(id) FROM spectrum", -1, &stmt, NULL))!=SQLITE_OK) return rc;
    if ((stmt_step_res= sqlite3_step(stmt))!=SQLITE_ROW) return rc;



    const int count = sqlite3_column_int(stmt, 0); //the length of the spectrum_headers array
    sqlite3_finalize(stmt); //delete the stmt



    //*err_msg = "Error: get spectrum headers failed (* query)";
    libmzdb_spectrum_header_t* h_arr = (libmzdb_spectrum_header_t *) malloc(count * sizeof(libmzdb_spectrum_header_t));
    if ((rc= sqlite3_prepare_v2(db, "SELECT * FROM spectrum", -1, &stmt, NULL))!=SQLITE_OK) {
        sqlite3_finalize(stmt);
        free(h_arr);
        return rc;
    }

    //*err_msg = "Error: get spectrum headers failed (init of each headers)";

    int index=-1;
    while((stmt_step_res=sqlite3_step(stmt))==SQLITE_ROW)
    {
        libmzdb_create_spectrum_header(stmt, &h_arr[++index], err_msg); //init the current header
    }

    sqlite3_finalize(stmt); //delete the stmt
    if (stmt_step_res != SQLITE_DONE) return stmt_step_res;

    //*err_msg = "";
    *spectrum_headers = h_arr;
    *spectrum_header_count = count;

    return SQLITE_OK;
}

int libmzdb_create_entity_cache(sqlite3 *db, libmzdb_entity_cache_t** entity_cache, char **err_msg)
{
    int rc;

    //printf("Before get_data_encodings_cache\n");

    libmzdb_data_encodings_cache_t* de_cache;
    if ((rc = libmzdb_get_data_encodings_cache(db, &de_cache))!=SQLITE_OK) {
        *err_msg = "Error: get_data_encodings_cache failed";
        return rc;
    }

    //printf("Before get_spectrum_headers\n");

    libmzdb_spectrum_header_t *headers;
    int header_count;
    if ((rc = libmzdb_get_spectrum_headers(db, &headers, &header_count, err_msg)) != SQLITE_OK) return rc;

    //printf("Before malloc\n");

    libmzdb_entity_cache_t* cache = (libmzdb_entity_cache_t*) malloc(sizeof(libmzdb_entity_cache_t));
    *entity_cache = cache;

    cache->data_encodings_cache = *de_cache;
    cache->spectrum_headers = headers;
    cache->spectrum_header_count = header_count;

    return SQLITE_OK;
}
