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
}

void archi_nt_input_attributes_init( archi_nt_input_attributes *attr )
{
  DEBUG_ASSERT( attr ) ;

  attr->nints = -1 ;
  attr->nregs = -1 ; 
}

void archi_nt_bstr_attributes_init( archi_nt_bstr_attributes *attr )
{
  DEBUG_ASSERT( attr ) ;

  attr->bstr = NULL ;
  attr->len = -1 ;
}

void archi_nt_bslc_attributes_init( archi_nt_bslc_attributes *attr )
{
  DEBUG_ASSERT( attr ) ;

  attr->start = -1 ;
  attr->length = -1 ; 
}
