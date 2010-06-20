#include "attributes.h"

#include <stdlib.h>

void archi_regdef_attributes_init( archi_regdef_attributes *attr )
{
  attr->code = ARCHI_REG_CODE_NOT_DEFINED ;
  attr->id = NULL ;
}

void archi_regcldef_attributes_init( archi_regcldef_attributes *attr )
{
  attr->bits = ARCHI_REGCL_BITS_NOT_DEFINED ;
  attr->regs = ARCHI_REGCL_REGS_NOT_DEFINED ;
  attr->id = NULL ;
}
