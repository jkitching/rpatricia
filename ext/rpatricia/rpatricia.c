/*
 *   rpatricia.c - Ruby wrapper of Net::Patricia
 *   Tatsuya Mori <mori.tatsuya@gmail.com>
 */

#include "ruby.h"
#include <stdlib.h>
#include "patricia.h"
#include <assert.h>

static VALUE cPatricia, cNode;

static void dummy(void) {}

static VALUE
p_destroy (VALUE self)
{
  patricia_tree_t *tree;

  Data_Get_Struct(self, patricia_tree_t, tree);
  Clear_Patricia(tree, dummy);
  tree->head = NULL; /* Clear_Patricia() should do this, actually */

  return Qtrue;
}

static void
p_node_mark (void *ptr)
{
  patricia_node_t *node = ptr;

  rb_gc_mark((VALUE)node->data);
}

static VALUE
wrap_node(patricia_node_t *node)
{
  /* node will be freed when parent is freed */
  return Data_Wrap_Struct(cNode, p_node_mark, 0, node);
}

static prefix_t *
my_ascii2prefix(int family, VALUE str)
{
  char *cstr = StringValuePtr(str);
  prefix_t *prefix = ascii2prefix(family, cstr);

  if (!prefix)
    rb_raise(rb_eArgError, "invalid prefix: %s", cstr);

  return prefix;
}

static VALUE
p_add (int argc, VALUE *argv, VALUE self)
{
  VALUE user_data;
  patricia_tree_t *tree;
  patricia_node_t *node;
  prefix_t *prefix;

  if (argc > 2 || argc < 1) 
    return Qnil;

  Data_Get_Struct(self, patricia_tree_t, tree);
  prefix = my_ascii2prefix(AF_INET, argv[0]);
  node = patricia_lookup(tree, prefix);
  Deref_Prefix(prefix);

  if (argc == 2) {
    user_data = argv[1];

    /* for backwards compatibility, we always dup and return new strings */
    if (TYPE(user_data) == T_STRING)
      user_data = rb_str_dup(user_data);
  } else {
    user_data = rb_str_new(NULL, 0);
  }
  PATRICIA_DATA_SET(node, user_data);

  /* node will be freed when parent is freed */
  return wrap_node(node);
}

static VALUE
p_remove (VALUE self, VALUE r_key)
{
  patricia_tree_t *tree;
  patricia_node_t *node;
  prefix_t *prefix;

  Data_Get_Struct(self, patricia_tree_t, tree);
  prefix = my_ascii2prefix (AF_INET, r_key);
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
  prefix = my_ascii2prefix (AF_INET, r_key);
  node = patricia_search_best(tree, prefix);
  Deref_Prefix (prefix);

  return node ? wrap_node(node) : Qnil;
}

static VALUE
p_include (VALUE self, VALUE r_key)
{
  patricia_tree_t *tree;
  patricia_node_t *node;
  prefix_t *prefix;

  Data_Get_Struct(self, patricia_tree_t, tree);
  prefix = my_ascii2prefix (AF_INET, r_key);
  node = patricia_search_best(tree, prefix);
  Deref_Prefix (prefix);

  return node ? Qtrue : Qfalse;
}

static VALUE
p_match_exact (VALUE self, VALUE r_key)
{
  patricia_tree_t *tree;
  patricia_node_t *node;
  prefix_t *prefix;

  Data_Get_Struct(self, patricia_tree_t, tree);
  prefix = my_ascii2prefix (AF_INET, r_key);
  node = patricia_search_exact(tree, prefix);
  Deref_Prefix (prefix);

  return node ? wrap_node(node) : Qnil;
}

static VALUE
p_num_nodes (VALUE self)
{
  int n;
  patricia_tree_t *tree;

  Data_Get_Struct(self, patricia_tree_t, tree);
  n = tree->head ? patricia_walk_inorder(tree->head, dummy) : 0;

  return INT2NUM(n);
}

/* needed for Ruby 1.8.6, in 1.8.7 and later */
#ifndef HAVE_RB_STR_SET_LEN
static void
rb_str_set_len(VALUE str, long len)
{
  RSTRING(str)->len = len;
  RSTRING(str)->ptr[len] = '\0';
}
#endif

static VALUE
p_print_nodes (int argc, VALUE *argv, VALUE self)
{
  ID id_printf = rb_intern("printf");
  VALUE fmt = rb_str_new2("node: %s\n");
  VALUE buf = rb_str_buf_new(128);
  char *cbuf;
  patricia_tree_t *tree;
  patricia_node_t *node;
  VALUE out;
  Data_Get_Struct(self, patricia_tree_t, tree);

  rb_scan_args(argc, argv, "01", &out);
  if (NIL_P(out))
    out = rb_stdout;

  if (tree->head) {
    PATRICIA_WALK(tree->head, node) {
      rb_str_resize(buf, 128);
      cbuf = RSTRING_PTR(buf);
      prefix_toa2x(node->prefix, cbuf, 1);
      rb_str_set_len(buf, strlen(cbuf));
      rb_funcall(out, id_printf, 2, fmt, buf);
    } PATRICIA_WALK_END;
  }
  return Qtrue;
}

static VALUE
p_data (VALUE self)
{
  VALUE user_data;
  patricia_node_t *node;
  Data_Get_Struct(self, patricia_node_t, node);

  user_data = (VALUE)node->data;

  /* for backwards compatibility, we always dup and return new strings */
  if (TYPE(user_data) == T_STRING)
    user_data = rb_str_dup(user_data);

  return user_data;
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

/* called during GC for each node->data attached to a Patricia tree */
static void
p_tree_mark_each(prefix_t *prefix, void *data)
{
  VALUE user_data = (VALUE)data;

  rb_gc_mark(user_data);
}

static void
p_tree_mark (void *ptr)
{
  patricia_tree_t *tree = ptr;

  patricia_process(tree, p_tree_mark_each);
}

static void
p_tree_free (void *ptr)
{
  patricia_tree_t *tree = ptr;

  /* no need to explicitly free each node->data, GC will do it for us */
  Destroy_Patricia(tree, NULL);
}

static VALUE
p_alloc(VALUE klass)
{
  patricia_tree_t *tree;
  tree = New_Patricia(32); /* assuming only IPv4 */

  return Data_Wrap_Struct(klass, p_tree_mark, p_tree_free, tree);
}

static VALUE
p_init_copy(VALUE self, VALUE orig)
{
  patricia_tree_t *orig_tree;

  Data_Get_Struct(orig, patricia_tree_t, orig_tree);
  if (orig_tree->head) {
    patricia_tree_t *tree;
    patricia_node_t *orig_node, *node;
    prefix_t prefix;
    VALUE user_data;

    Data_Get_Struct(self, patricia_tree_t, tree);
    PATRICIA_WALK(orig_tree->head, orig_node) {
      node = patricia_lookup(tree, orig_node->prefix);
      assert(node->prefix == orig_node->prefix);

      user_data = (VALUE)(orig_node->data);
      if (T_STRING == TYPE(user_data))
        user_data = rb_str_dup(user_data);
      PATRICIA_DATA_SET(node, user_data);
    } PATRICIA_WALK_END;
  }
}

void
Init_rpatricia (void)
{
  cPatricia = rb_define_class("Patricia", rb_cObject);
  cNode = rb_define_class_under(cPatricia, "Node", rb_cObject);

  /* allocate new Patricia object, called before initialize  */
  rb_define_alloc_func(cPatricia, p_alloc);
  rb_define_method(cPatricia, "initialize_copy", p_init_copy, 1);

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

  /* check existence */
  rb_define_method(cPatricia, "include?", p_include, 1);

  /* removal */
  rb_define_method(cPatricia, "remove", p_remove, 1); 
  rb_define_method(cPatricia, "remove_node", p_remove, 1); 

  /* derivatives of climb */
  rb_define_method(cPatricia, "num_nodes", p_num_nodes, 0);
  rb_define_method(cPatricia, "show_nodes", p_print_nodes, -1);

  /* destroy tree */
  rb_define_method(cPatricia, "destroy", p_destroy, 0); 
  rb_define_method(cPatricia, "clear", p_destroy, 0); 

  /*---------- methods to node ----------*/
  rb_define_method(cNode, "data", p_data, 0);
  rb_define_method(cNode, "show_data", p_data, 0);
  rb_define_method(cNode, "network", p_network, 0);
  rb_define_method(cNode, "prefix", p_prefix, 0);
  rb_define_method(cNode, "prefixlen", p_prefixlen, 0);
  //  rb_define_method(cPatricia, "family", p_family, 0); 

}
