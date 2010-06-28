#include "attributes.h"

#include <stdlib.h>

void archi_regdef_attributes_init( archi_regdef_attributes *attr )
{
  attr->code = -1 ;
  attr->id = NULL ;
  attr->regcl = NULL ;
}

void archi_regcldef_attributes_init( archi_regcldef_attributes *attr )
{
  attr->bits = -1 ;
  attr->regs = NULL ;
  attr->pregcl = NULL ;
  attr->id = NULL ;
}
