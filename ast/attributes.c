#include "attributes.h"
#include "../debug.h"

#include <stdlib.h>

void archi_nt_regdef_attributes_init( archi_nt_regdef_attributes *attr )
{
  DEBUG_ASSERT( attr ) ;

  attr->code = -1 ;
  attr->id = NULL ;
  attr->regcl = NULL ;
}

void archi_nt_archdef_attributes_init( archi_nt_archdef_attributes *attr )
{
  DEBUG_ASSERT( attr ) ;

  attr->regsect = NULL ;
  attr->instrsect = NULL ;
  attr->patternsect = NULL ;
}

void archi_nt_regcldef_attributes_init( archi_nt_regcldef_attributes *attr )
{
  DEBUG_ASSERT( attr ) ;   

  attr->bits = -1 ;
  attr->regs = NULL ;
  attr->pregcl = NULL ;
  attr->id = NULL ;
}

void archi_nt_instrdef_attributes_init( archi_nt_instrdef_attributes *attr )
{
  DEBUG_ASSERT( attr ) ;

  attr->id = NULL ;
  attr->input = NULL ;
  attr->output = NULL ;
  attr->encoding = NULL ;
  attr->flags = NULL ;
}

void archi_nt_matchdef_attributes_init( archi_nt_matchdef_attributes *attr )
{
  DEBUG_ASSERT( attr ) ;
  
  attr->id = NULL ;
  attr->input = NULL ;
  attr->output = NULL ;
  attr->ipattern = NULL ;
  attr->opattern = NULL ; 
}

void archi_nt_bstr_attributes_init( archi_nt_bstr_attributes *attr )
{
  DEBUG_ASSERT( attr ) ;

  attr->bstr = NULL ;
  attr->len = 0 ;
}

void archi_nt_str_attributes_init( archi_nt_str_attributes *attr )
{
  DEBUG_ASSERT( attr ) ;
  
  attr->str = NULL ;
  attr->length = -1 ;
}

void archi_nt_bslc_attributes_init( archi_nt_bslc_attributes *attr )
{
  DEBUG_ASSERT( attr ) ;

  attr->start = -1 ;
  attr->length = -1 ; 
}

void archi_nt_ifthenelse_attributes_init( archi_nt_ifthenelse_attributes *attr )
{
  DEBUG_ASSERT( attr ) ;

  attr->pred = NULL ;
  attr->cthen = NULL ;
  attr->celse = NULL ;
}
