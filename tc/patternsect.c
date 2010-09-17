#include "patternsect.h"
#include "typecheck.h"
#include "../ast/node.h"
#include "../debug.h"

#include <string.h>

//FIXME: check whether input was already defined
static void archi_ipattern_typecheck( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( st && n && n->node_type == NT_IPATTERN ) ;

  archi_symtab_idlist *tf ;
  archi_symtab_idlist *tl = archi_symtab_idlist_fill( NULL, st, NT_JVINSTRDEF ) ;

  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    DEBUG_ASSERT( c->node_type == NT_NODEDEF ) ;
    tf = archi_variabledef_typecheck( st, c, c->attr.nt_nodedef.id, tl ) ; 

    if( tf ){
      archi_ast_node *l = archi_symtab_lookup( st, archi_symtab_idlist_id(tf) ) ;

      int32_t niex = l->attr.nt_jvinstrdef.ninputs ;
      int32_t nig = c->attr.nt_nodedef.input->attr.nt_input.nchildren ;
      int32_t noex = l->attr.nt_jvinstrdef.noutputs ;
      int32_t nog = c->attr.nt_nodedef.output->attr.nt_output.nchildren ;
      if( niex ==  nig ){
        archi_ast_node *cc ;
        FOREACH_CHILD( c->attr.nt_nodedef.input, cc ){
          l = archi_symtab_lookup( st, cc->attr.nt_id.id ) ;
          
          if( l == NULL ){
            EMSG_MISSING_ID( cc, cc->attr.nt_id.id ) ;
            continue ;
          }

          archi_ast_node *p = archi_ast_node_parent_get( l, 1 ) ;
          archi_ast_node *pp = archi_ast_node_parent_get( l, 2 ) ;

          if( p != NULL && pp != NULL && (p->node_type != NT_INPUT || pp->node_type != NT_MATCHDEF) &&
              (p->node_type != NT_OUTPUT || pp->node_type != NT_NODEDEF) ){
            archi_add_emsg( cc, "%s is not of the right type", cc->attr.nt_id.id ) ;
          } 
        } 
      }
      else archi_add_emsg( c, "expected %d inputs, got %d inputs", niex, nig ) ;
      if( noex == nog  ){
        archi_ast_node *cc ;
        FOREACH_CHILD( c->attr.nt_nodedef.output, cc ){
          l = archi_symtab_lookup( st, cc->attr.nt_id.id ) ;
         
          if( l != NULL ){
            archi_ast_node *p = archi_ast_node_parent_get( l, 1 ) ;
            archi_ast_node *pp = archi_ast_node_parent_get( l, 2 ) ;
            if( p != NULL && pp != NULL && (p->node_type != NT_OUTPUT || pp->node_type !=NT_MATCHDEF) )
              archi_add_emsg( cc, "%s is not of the right type", cc->attr.nt_id.id ) ;
          }
          else archi_id_def_check( st, cc->attr.nt_id.id, cc ) ;
        }
      }
      else archi_add_emsg( c, "expected %d outputs, got %d outputs", noex, nog ) ;
    }
  } 

  TALLOC_FREE( tl ) ;
}

static void archi_opattern_typecheck( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( st && n && n->node_type == NT_OPATTERN ) ;

  archi_symtab_idlist *tf ;
  archi_symtab_idlist *tl = archi_symtab_idlist_fill( NULL, st, NT_INSTRDEF ) ;

  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    DEBUG_ASSERT( c->node_type == NT_NODEDEF ) ;
    tf = archi_variabledef_typecheck( st, c, c->attr.nt_nodedef.id, tl ) ;
    
    if( tf ){
      archi_ast_node *l = archi_symtab_lookup( st, archi_symtab_idlist_id(tf) ) ;

      int32_t niex = l->attr.nt_instrdef.input->attr.nt_input.nchildren ;
      int32_t nig = c->attr.nt_nodedef.input->attr.nt_input.nchildren ;
      int32_t noex = l->attr.nt_instrdef.output->attr.nt_output.nchildren ;
      int32_t nog = c->attr.nt_nodedef.output->attr.nt_output.nchildren ;
      if( niex == nig ){
        archi_ast_node *cc ;
        archi_ast_node *instr_input = l->attr.nt_instrdef.input->first_child ;
        FOREACH_CHILD( c->attr.nt_nodedef.input, cc ){
          archi_ast_node *ll = archi_symtab_lookup( st, cc->attr.nt_id.id ) ;

          if( l == NULL ){
            EMSG_MISSING_ID( cc, cc->attr.nt_id.id ) ;
            continue ;
          }
        
          archi_ast_node *p = archi_ast_node_parent_get( ll, 1 ) ;
          archi_ast_node *pp = archi_ast_node_parent_get( ll, 2 ) ;

          if( p != NULL && pp != NULL && (p->node_type != NT_INPUT || pp->node_type != NT_MATCHDEF) &&
              (p->node_type != NT_OUTPUT || pp->node_type != NT_NODEDEF) ){
            archi_add_emsg( cc, "%s is not of the right type", cc->attr.nt_id.id ) ;
          }
          else{
            archi_ast_node *ll = archi_symtab_lookup( st, cc->attr.nt_id.id ) ;
            if( strcmp(ll->data_type, instr_input->data_type) )
              EMSG_TYPE_MISMATCH( cc, ll->data_type, instr_input->data_type ) ;
            archi_ast_node_data_type_set( cc, instr_input->data_type ) ;
          }
        
          instr_input = instr_input->next_sibling ;
        }
      } 
      else archi_add_emsg( c, "expected %d inputs, got %d inputs", niex, nig ) ;

      if( noex == nog  ){
        archi_ast_node *cc ;
        archi_ast_node *instr_output = l->attr.nt_instrdef.output->first_child ;
        FOREACH_CHILD( c->attr.nt_nodedef.output, cc ){
          l = archi_symtab_lookup( st, cc->attr.nt_id.id ) ;
         
          if( l != NULL ){
            archi_ast_node *p = archi_ast_node_parent_get( l, 1 ) ;
            archi_ast_node *pp = archi_ast_node_parent_get( l, 2 ) ;
            if( p != NULL && pp != NULL && (p->node_type != NT_OUTPUT || pp->node_type != NT_MATCHDEF) )
              archi_add_emsg( cc, "%s is not of the right type", cc->attr.nt_id.id ) ;
            else{
              archi_ast_node *ll = archi_symtab_lookup( st, cc->attr.nt_id.id ) ;
              if( strcmp(ll->data_type, instr_output->data_type) )
                EMSG_TYPE_MISMATCH( cc, ll->data_type, instr_output->data_type ) ;
            }
          }
          else archi_id_def_check( st, cc->attr.nt_id.id, cc ) ;
  
          archi_ast_node_data_type_set( cc, instr_output->data_type ) ;
          instr_output = instr_output->next_sibling ;
        }
      }
      else archi_add_emsg( c, "expected %d outputs, got %d outputs", noex, nog ) ;
    }
  }

  TALLOC_FREE( tl ) ;
}

//FIXME: check whether input/outpt ids have been used
static void archi_matchdef_typecheck( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( st && n && n->node_type == NT_MATCHDEF ) ;

  if( n->attr.nt_matchdef.input != NULL ){
    archi_symtab_idlist *l = archi_symtab_idlist_fill( NULL, st, NT_REGCLDEF ) ;
    archi_ast_node *c ;
    FOREACH_CHILD( n->attr.nt_matchdef.input, c )
      archi_variabledef_typecheck( st, c, c->attr.nt_tid.id, l ) ;
    TALLOC_FREE( l ) ;
  }
  else EMSG_MISSING_ATTRIBUTE( n, "input" ) ;

  if( n->attr.nt_matchdef.output != NULL ){
    archi_symtab_idlist *l = archi_symtab_idlist_fill( NULL, st, NT_REGCLDEF ) ;
    archi_ast_node *c ;
    FOREACH_CHILD( n->attr.nt_matchdef.output, c )
      archi_variabledef_typecheck( st, c, c->attr.nt_tid.id, l ) ;
    TALLOC_FREE( l ) ;
  }
  else EMSG_MISSING_ATTRIBUTE( n, "output" ) ;

  if( n->attr.nt_matchdef.ipattern != NULL )
    archi_ipattern_typecheck( st, n->attr.nt_matchdef.ipattern ) ;
  else EMSG_MISSING_ATTRIBUTE( n, "ipattern" ) ;

  if( n->attr.nt_matchdef.opattern != NULL )
    archi_opattern_typecheck( st, n->attr.nt_matchdef.opattern ) ;
  else EMSG_MISSING_ATTRIBUTE( n, "opattern" ) ;
}

void archi_patternsect_typecheck( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( st && n && n->node_type == NT_PATTERNSECT ) ;
  
  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_MATCHDEF ){
      archi_symtab_push_scope( st ) ;
      archi_matchdef_typecheck( st, c ) ;
      archi_symtab_pop_scope( st ) ;
    }
    else DEBUG_ASSERT(0) ;
  }
}
