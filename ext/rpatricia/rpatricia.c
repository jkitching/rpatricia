/*
 *   rpatricia.c - Ruby wrapper of Net::Patricia
 *   Tatsuya Mori <mori.tatsuya@gmail.com>
 */

#include "ruby.h"
#include <stdio.h> /* printf */
#include <stdlib.h>
#include "patricia.h"

static VALUE cPatricia;

static void dummy(void) {}

static VALUE
p_destroy (VALUE self)
{
  patricia_tree_t *tree;
  Data_Get_Struct(self, patricia_tree_t, tree);
  Destroy_Patricia(tree, free);
  return Qtrue;
}

static VALUE
p_add (int argc, VALUE *argv, VALUE self)
{
  int str_len;
  char *user_data;
  patricia_tree_t *tree;
  patricia_node_t *node;
  prefix_t *prefix;

  if (argc > 2 || argc < 1) 
    return Qnil;

  Data_Get_Struct(self, patricia_tree_t, tree);
  prefix = ascii2prefix(AF_INET, STR2CSTR(argv[0]));
  node = patricia_lookup(tree, prefix);
  Deref_Prefix(prefix);

  if (argc == 2) {
    user_data = STR2CSTR(argv[1]);
    str_len = strlen(user_data);
    node->data = (char *) malloc((str_len + 1) * sizeof(char));
    sprintf((char *)node->data, user_data);
  } else {
    node->data = (char *) malloc(sizeof(char));
    sprintf((char *)node->data, "");
  }
  return Data_Wrap_Struct(cPatricia, 0, 0, node);
}

static VALUE
p_remove (VALUE self, VALUE r_key)
{
  char *c_key;
  patricia_tree_t *tree;
  patricia_node_t *node;
  prefix_t *prefix;

  Data_Get_Struct(self, patricia_tree_t, tree);
  c_key = STR2CSTR(r_key);
  prefix = ascii2prefix (AF_INET, c_key);
  node = patricia_search_exact(tree, prefix);
  Deref_Prefix (prefix);

  if (node) {
    patricia_remove (tree, node);
    return Qtrue;
  } else {
    return Qfalse;
  }
}

static VALUE
p_match (VALUE self, VALUE r_key)
{
  patricia_tree_t *tree;
  patricia_node_t *node;
  prefix_t *prefix;
  
  Data_Get_Struct(self, patricia_tree_t, tree);
  prefix = ascii2prefix (AF_INET, STR2CSTR(r_key));
  node = patricia_search_best(tree, prefix);
  Deref_Prefix (prefix);

  if (node)
    return Data_Wrap_Struct(cPatricia, 0, 0, node);
  else 
    return Qfalse;

}

static VALUE
p_match_exact (VALUE self, VALUE r_key)
{
  patricia_tree_t *tree;
  patricia_node_t *node;
  prefix_t *prefix;

  Data_Get_Struct(self, patricia_tree_t, tree);
  prefix = ascii2prefix (AF_INET, STR2CSTR(r_key));
  node = patricia_search_exact(tree, prefix);
  Deref_Prefix (prefix);

  if (node)
    return Data_Wrap_Struct(cPatricia, 0, 0, node);
  else 
    return Qfalse;
}

static VALUE
p_num_nodes (VALUE self)
{
  int n;
  patricia_tree_t *tree;

  Data_Get_Struct(self, patricia_tree_t, tree);
  n = patricia_walk_inorder(tree->head, dummy);

  return INT2NUM(n);
}

static VALUE
p_print_nodes (VALUE self)
{
  int n;
  char buff[32];
  patricia_tree_t *tree;
  patricia_node_t *node;
  Data_Get_Struct(self, patricia_tree_t, tree);

  PATRICIA_WALK(tree->head, node) {
    prefix_toa2x(node->prefix, buff, 1);
    printf("node: %s\n", buff);
  } PATRICIA_WALK_END;
  return Qtrue;
}

static VALUE
p_data (VALUE self)
{
  patricia_node_t *node;
  Data_Get_Struct(self, patricia_node_t, node);
  return rb_str_new2((char *)node->data);
}

static VALUE
p_network (VALUE self)
{
  char buff[32];
  patricia_node_t *node;
  Data_Get_Struct(self, patricia_node_t, node);
  prefix_toa2x (node->prefix, buff, 0);
  return rb_str_new2(buff);
}

static VALUE
p_prefix (VALUE self)
{
  char buff[32];
  patricia_node_t *node;
  Data_Get_Struct(self, patricia_node_t, node);
  prefix_toa2 (node->prefix, buff);
  return rb_str_new2(buff);
}

static VALUE
p_prefixlen (VALUE self)
{
  patricia_node_t *node;
  Data_Get_Struct(self, patricia_node_t, node);
  return INT2NUM(node->prefix->bitlen);
}

static VALUE
p_new (VALUE self) 
{
  patricia_tree_t *tree;
  tree = New_Patricia(32); /* assuming only IPv4 */
  return Data_Wrap_Struct(cPatricia, 0, 0, tree);
}

void
Init_rpatricia (void)
{
  cPatricia = rb_define_class("Patricia", rb_cObject);

  /* create new Patricia object */
  rb_define_singleton_method(cPatricia, "new", p_new, 0);

  /*---------- methods to tree ----------*/
  /* add string */
  rb_define_method(cPatricia, "add", p_add, -1);
  rb_define_method(cPatricia, "add_node", p_add, -1);

  /* match prefix */
  rb_define_method(cPatricia, "match_best", p_match, 1); 
  rb_define_method(cPatricia, "search_best", p_match, 1); 

  /* exact match  */
  rb_define_method(cPatricia, "match_exact", p_match_exact, 1); 
  rb_define_method(cPatricia, "search_exact", p_match_exact, 1); 

  /* removal */
  rb_define_method(cPatricia, "remove", p_remove, 1); 
  rb_define_method(cPatricia, "remove_node", p_remove, 1); 

  /* derivatives of climb */
  rb_define_method(cPatricia, "num_nodes", p_num_nodes, 0);
  rb_define_method(cPatricia, "show_nodes", p_print_nodes, 0); 

  /* destroy tree */
  rb_define_method(cPatricia, "destroy", p_destroy, 0); 
  rb_define_method(cPatricia, "clear", p_destroy, 0); 

  /*---------- methods to node ----------*/
  rb_define_method(cPatricia, "data", p_data, 0);
  rb_define_method(cPatricia, "show_data", p_data, 0);
  rb_define_method(cPatricia, "network", p_network, 0);
  rb_define_method(cPatricia, "prefix", p_prefix, 0);
  rb_define_method(cPatricia, "prefixlen", p_prefixlen, 0);
  //  rb_define_method(cPatricia, "family", p_family, 0); 

}
