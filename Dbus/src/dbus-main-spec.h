/* Generated by dbus-binding-tool; do not edit! */


#ifndef __dbus_glib_marshal_cd_dbus_main_MARSHAL_H__
#define __dbus_glib_marshal_cd_dbus_main_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

#ifdef G_ENABLE_DEBUG
#define g_marshal_value_peek_boolean(v)  g_value_get_boolean (v)
#define g_marshal_value_peek_char(v)     g_value_get_char (v)
#define g_marshal_value_peek_uchar(v)    g_value_get_uchar (v)
#define g_marshal_value_peek_int(v)      g_value_get_int (v)
#define g_marshal_value_peek_uint(v)     g_value_get_uint (v)
#define g_marshal_value_peek_long(v)     g_value_get_long (v)
#define g_marshal_value_peek_ulong(v)    g_value_get_ulong (v)
#define g_marshal_value_peek_int64(v)    g_value_get_int64 (v)
#define g_marshal_value_peek_uint64(v)   g_value_get_uint64 (v)
#define g_marshal_value_peek_enum(v)     g_value_get_enum (v)
#define g_marshal_value_peek_flags(v)    g_value_get_flags (v)
#define g_marshal_value_peek_float(v)    g_value_get_float (v)
#define g_marshal_value_peek_double(v)   g_value_get_double (v)
#define g_marshal_value_peek_string(v)   (char*) g_value_get_string (v)
#define g_marshal_value_peek_param(v)    g_value_get_param (v)
#define g_marshal_value_peek_boxed(v)    g_value_get_boxed (v)
#define g_marshal_value_peek_pointer(v)  g_value_get_pointer (v)
#define g_marshal_value_peek_object(v)   g_value_get_object (v)
#define g_marshal_value_peek_variant(v)  g_value_get_variant (v)
#else /* !G_ENABLE_DEBUG */
/* WARNING: This code accesses GValues directly, which is UNSUPPORTED API.
 *          Do not access GValues directly in your code. Instead, use the
 *          g_value_get_*() functions
 */
#define g_marshal_value_peek_boolean(v)  (v)->data[0].v_int
#define g_marshal_value_peek_char(v)     (v)->data[0].v_int
#define g_marshal_value_peek_uchar(v)    (v)->data[0].v_uint
#define g_marshal_value_peek_int(v)      (v)->data[0].v_int
#define g_marshal_value_peek_uint(v)     (v)->data[0].v_uint
#define g_marshal_value_peek_long(v)     (v)->data[0].v_long
#define g_marshal_value_peek_ulong(v)    (v)->data[0].v_ulong
#define g_marshal_value_peek_int64(v)    (v)->data[0].v_int64
#define g_marshal_value_peek_uint64(v)   (v)->data[0].v_uint64
#define g_marshal_value_peek_enum(v)     (v)->data[0].v_long
#define g_marshal_value_peek_flags(v)    (v)->data[0].v_ulong
#define g_marshal_value_peek_float(v)    (v)->data[0].v_float
#define g_marshal_value_peek_double(v)   (v)->data[0].v_double
#define g_marshal_value_peek_string(v)   (v)->data[0].v_pointer
#define g_marshal_value_peek_param(v)    (v)->data[0].v_pointer
#define g_marshal_value_peek_boxed(v)    (v)->data[0].v_pointer
#define g_marshal_value_peek_pointer(v)  (v)->data[0].v_pointer
#define g_marshal_value_peek_object(v)   (v)->data[0].v_pointer
#define g_marshal_value_peek_variant(v)  (v)->data[0].v_pointer
#endif /* !G_ENABLE_DEBUG */


/* BOOLEAN:STRING,STRING,STRING,STRING,POINTER */
extern void dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_STRING_STRING_STRING_POINTER (GClosure     *closure,
                                                                                         GValue       *return_value,
                                                                                         guint         n_param_values,
                                                                                         const GValue *param_values,
                                                                                         gpointer      invocation_hint,
                                                                                         gpointer      marshal_data);
void
dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_STRING_STRING_STRING_POINTER (GClosure     *closure,
                                                                             GValue       *return_value G_GNUC_UNUSED,
                                                                             guint         n_param_values,
                                                                             const GValue *param_values,
                                                                             gpointer      invocation_hint G_GNUC_UNUSED,
                                                                             gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__STRING_STRING_STRING_STRING_POINTER) (gpointer     data1,
                                                                                 gpointer     arg_1,
                                                                                 gpointer     arg_2,
                                                                                 gpointer     arg_3,
                                                                                 gpointer     arg_4,
                                                                                 gpointer     arg_5,
                                                                                 gpointer     data2);
  register GMarshalFunc_BOOLEAN__STRING_STRING_STRING_STRING_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 6);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__STRING_STRING_STRING_STRING_POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_string (param_values + 1),
                       g_marshal_value_peek_string (param_values + 2),
                       g_marshal_value_peek_string (param_values + 3),
                       g_marshal_value_peek_string (param_values + 4),
                       g_marshal_value_peek_pointer (param_values + 5),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

/* BOOLEAN:BOXED,POINTER */
extern void dbus_glib_marshal_cd_dbus_main_BOOLEAN__BOXED_POINTER (GClosure     *closure,
                                                                   GValue       *return_value,
                                                                   guint         n_param_values,
                                                                   const GValue *param_values,
                                                                   gpointer      invocation_hint,
                                                                   gpointer      marshal_data);
void
dbus_glib_marshal_cd_dbus_main_BOOLEAN__BOXED_POINTER (GClosure     *closure,
                                                       GValue       *return_value G_GNUC_UNUSED,
                                                       guint         n_param_values,
                                                       const GValue *param_values,
                                                       gpointer      invocation_hint G_GNUC_UNUSED,
                                                       gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__BOXED_POINTER) (gpointer     data1,
                                                           gpointer     arg_1,
                                                           gpointer     arg_2,
                                                           gpointer     data2);
  register GMarshalFunc_BOOLEAN__BOXED_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 3);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__BOXED_POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_boxed (param_values + 1),
                       g_marshal_value_peek_pointer (param_values + 2),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

/* BOOLEAN:BOOLEAN,POINTER */
extern void dbus_glib_marshal_cd_dbus_main_BOOLEAN__BOOLEAN_POINTER (GClosure     *closure,
                                                                     GValue       *return_value,
                                                                     guint         n_param_values,
                                                                     const GValue *param_values,
                                                                     gpointer      invocation_hint,
                                                                     gpointer      marshal_data);
void
dbus_glib_marshal_cd_dbus_main_BOOLEAN__BOOLEAN_POINTER (GClosure     *closure,
                                                         GValue       *return_value G_GNUC_UNUSED,
                                                         guint         n_param_values,
                                                         const GValue *param_values,
                                                         gpointer      invocation_hint G_GNUC_UNUSED,
                                                         gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__BOOLEAN_POINTER) (gpointer     data1,
                                                             gboolean     arg_1,
                                                             gpointer     arg_2,
                                                             gpointer     data2);
  register GMarshalFunc_BOOLEAN__BOOLEAN_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 3);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__BOOLEAN_POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_boolean (param_values + 1),
                       g_marshal_value_peek_pointer (param_values + 2),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

/* BOOLEAN:BOOLEAN,STRING,STRING,POINTER */
extern void dbus_glib_marshal_cd_dbus_main_BOOLEAN__BOOLEAN_STRING_STRING_POINTER (GClosure     *closure,
                                                                                   GValue       *return_value,
                                                                                   guint         n_param_values,
                                                                                   const GValue *param_values,
                                                                                   gpointer      invocation_hint,
                                                                                   gpointer      marshal_data);
void
dbus_glib_marshal_cd_dbus_main_BOOLEAN__BOOLEAN_STRING_STRING_POINTER (GClosure     *closure,
                                                                       GValue       *return_value G_GNUC_UNUSED,
                                                                       guint         n_param_values,
                                                                       const GValue *param_values,
                                                                       gpointer      invocation_hint G_GNUC_UNUSED,
                                                                       gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__BOOLEAN_STRING_STRING_POINTER) (gpointer     data1,
                                                                           gboolean     arg_1,
                                                                           gpointer     arg_2,
                                                                           gpointer     arg_3,
                                                                           gpointer     arg_4,
                                                                           gpointer     data2);
  register GMarshalFunc_BOOLEAN__BOOLEAN_STRING_STRING_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 5);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__BOOLEAN_STRING_STRING_POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_boolean (param_values + 1),
                       g_marshal_value_peek_string (param_values + 2),
                       g_marshal_value_peek_string (param_values + 3),
                       g_marshal_value_peek_pointer (param_values + 4),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

/* BOOLEAN:POINTER */
extern void dbus_glib_marshal_cd_dbus_main_BOOLEAN__POINTER (GClosure     *closure,
                                                             GValue       *return_value,
                                                             guint         n_param_values,
                                                             const GValue *param_values,
                                                             gpointer      invocation_hint,
                                                             gpointer      marshal_data);
void
dbus_glib_marshal_cd_dbus_main_BOOLEAN__POINTER (GClosure     *closure,
                                                 GValue       *return_value G_GNUC_UNUSED,
                                                 guint         n_param_values,
                                                 const GValue *param_values,
                                                 gpointer      invocation_hint G_GNUC_UNUSED,
                                                 gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__POINTER) (gpointer     data1,
                                                     gpointer     arg_1,
                                                     gpointer     data2);
  register GMarshalFunc_BOOLEAN__POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 2);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_pointer (param_values + 1),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

/* BOOLEAN:STRING,STRING,POINTER */
extern void dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_STRING_POINTER (GClosure     *closure,
                                                                           GValue       *return_value,
                                                                           guint         n_param_values,
                                                                           const GValue *param_values,
                                                                           gpointer      invocation_hint,
                                                                           gpointer      marshal_data);
void
dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_STRING_POINTER (GClosure     *closure,
                                                               GValue       *return_value G_GNUC_UNUSED,
                                                               guint         n_param_values,
                                                               const GValue *param_values,
                                                               gpointer      invocation_hint G_GNUC_UNUSED,
                                                               gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__STRING_STRING_POINTER) (gpointer     data1,
                                                                   gpointer     arg_1,
                                                                   gpointer     arg_2,
                                                                   gpointer     arg_3,
                                                                   gpointer     data2);
  register GMarshalFunc_BOOLEAN__STRING_STRING_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 4);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__STRING_STRING_POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_string (param_values + 1),
                       g_marshal_value_peek_string (param_values + 2),
                       g_marshal_value_peek_pointer (param_values + 3),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

/* BOOLEAN:STRING,POINTER,POINTER */
extern void dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_POINTER_POINTER (GClosure     *closure,
                                                                            GValue       *return_value,
                                                                            guint         n_param_values,
                                                                            const GValue *param_values,
                                                                            gpointer      invocation_hint,
                                                                            gpointer      marshal_data);
void
dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_POINTER_POINTER (GClosure     *closure,
                                                                GValue       *return_value G_GNUC_UNUSED,
                                                                guint         n_param_values,
                                                                const GValue *param_values,
                                                                gpointer      invocation_hint G_GNUC_UNUSED,
                                                                gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__STRING_POINTER_POINTER) (gpointer     data1,
                                                                    gpointer     arg_1,
                                                                    gpointer     arg_2,
                                                                    gpointer     arg_3,
                                                                    gpointer     data2);
  register GMarshalFunc_BOOLEAN__STRING_POINTER_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 4);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__STRING_POINTER_POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_string (param_values + 1),
                       g_marshal_value_peek_pointer (param_values + 2),
                       g_marshal_value_peek_pointer (param_values + 3),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

/* BOOLEAN:STRING,INT,STRING,POINTER */
extern void dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_INT_STRING_POINTER (GClosure     *closure,
                                                                               GValue       *return_value,
                                                                               guint         n_param_values,
                                                                               const GValue *param_values,
                                                                               gpointer      invocation_hint,
                                                                               gpointer      marshal_data);
void
dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_INT_STRING_POINTER (GClosure     *closure,
                                                                   GValue       *return_value G_GNUC_UNUSED,
                                                                   guint         n_param_values,
                                                                   const GValue *param_values,
                                                                   gpointer      invocation_hint G_GNUC_UNUSED,
                                                                   gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__STRING_INT_STRING_POINTER) (gpointer     data1,
                                                                       gpointer     arg_1,
                                                                       gint         arg_2,
                                                                       gpointer     arg_3,
                                                                       gpointer     arg_4,
                                                                       gpointer     data2);
  register GMarshalFunc_BOOLEAN__STRING_INT_STRING_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 5);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__STRING_INT_STRING_POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_string (param_values + 1),
                       g_marshal_value_peek_int (param_values + 2),
                       g_marshal_value_peek_string (param_values + 3),
                       g_marshal_value_peek_pointer (param_values + 4),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

/* BOOLEAN:STRING,DOUBLE,STRING,POINTER,POINTER */
extern void dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_DOUBLE_STRING_POINTER_POINTER (GClosure     *closure,
                                                                                          GValue       *return_value,
                                                                                          guint         n_param_values,
                                                                                          const GValue *param_values,
                                                                                          gpointer      invocation_hint,
                                                                                          gpointer      marshal_data);
void
dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_DOUBLE_STRING_POINTER_POINTER (GClosure     *closure,
                                                                              GValue       *return_value G_GNUC_UNUSED,
                                                                              guint         n_param_values,
                                                                              const GValue *param_values,
                                                                              gpointer      invocation_hint G_GNUC_UNUSED,
                                                                              gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__STRING_DOUBLE_STRING_POINTER_POINTER) (gpointer     data1,
                                                                                  gpointer     arg_1,
                                                                                  gdouble      arg_2,
                                                                                  gpointer     arg_3,
                                                                                  gpointer     arg_4,
                                                                                  gpointer     arg_5,
                                                                                  gpointer     data2);
  register GMarshalFunc_BOOLEAN__STRING_DOUBLE_STRING_POINTER_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 6);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__STRING_DOUBLE_STRING_POINTER_POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_string (param_values + 1),
                       g_marshal_value_peek_double (param_values + 2),
                       g_marshal_value_peek_string (param_values + 3),
                       g_marshal_value_peek_pointer (param_values + 4),
                       g_marshal_value_peek_pointer (param_values + 5),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

/* BOOLEAN:STRING,POINTER */
extern void dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_POINTER (GClosure     *closure,
                                                                    GValue       *return_value,
                                                                    guint         n_param_values,
                                                                    const GValue *param_values,
                                                                    gpointer      invocation_hint,
                                                                    gpointer      marshal_data);
void
dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_POINTER (GClosure     *closure,
                                                        GValue       *return_value G_GNUC_UNUSED,
                                                        guint         n_param_values,
                                                        const GValue *param_values,
                                                        gpointer      invocation_hint G_GNUC_UNUSED,
                                                        gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__STRING_POINTER) (gpointer     data1,
                                                            gpointer     arg_1,
                                                            gpointer     arg_2,
                                                            gpointer     data2);
  register GMarshalFunc_BOOLEAN__STRING_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 3);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__STRING_POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_string (param_values + 1),
                       g_marshal_value_peek_pointer (param_values + 2),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

/* BOOLEAN:STRING,BOOLEAN,POINTER */
extern void dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_BOOLEAN_POINTER (GClosure     *closure,
                                                                            GValue       *return_value,
                                                                            guint         n_param_values,
                                                                            const GValue *param_values,
                                                                            gpointer      invocation_hint,
                                                                            gpointer      marshal_data);
void
dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_BOOLEAN_POINTER (GClosure     *closure,
                                                                GValue       *return_value G_GNUC_UNUSED,
                                                                guint         n_param_values,
                                                                const GValue *param_values,
                                                                gpointer      invocation_hint G_GNUC_UNUSED,
                                                                gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__STRING_BOOLEAN_POINTER) (gpointer     data1,
                                                                    gpointer     arg_1,
                                                                    gboolean     arg_2,
                                                                    gpointer     arg_3,
                                                                    gpointer     data2);
  register GMarshalFunc_BOOLEAN__STRING_BOOLEAN_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 4);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__STRING_BOOLEAN_POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_marshal_value_peek_string (param_values + 1),
                       g_marshal_value_peek_boolean (param_values + 2),
                       g_marshal_value_peek_pointer (param_values + 3),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

G_END_DECLS

#endif /* __dbus_glib_marshal_cd_dbus_main_MARSHAL_H__ */

#include <dbus/dbus-glib.h>
static const DBusGMethodInfo dbus_glib_cd_dbus_main_methods[] = {
  { (GCallback) cd_dbus_main_reboot, dbus_glib_marshal_cd_dbus_main_BOOLEAN__POINTER, 0 },
  { (GCallback) cd_dbus_main_quit, dbus_glib_marshal_cd_dbus_main_BOOLEAN__POINTER, 34 },
  { (GCallback) cd_dbus_main_show_dock, dbus_glib_marshal_cd_dbus_main_BOOLEAN__BOOLEAN_POINTER, 66 },
  { (GCallback) cd_dbus_main_show_desklet, dbus_glib_marshal_cd_dbus_main_BOOLEAN__BOOLEAN_POINTER, 111 },
  { (GCallback) cd_dbus_main_reload_module, dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_POINTER, 166 },
  { (GCallback) cd_dbus_main_activate_module, dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_BOOLEAN_POINTER, 222 },
  { (GCallback) cd_dbus_main_get_icon_properties, dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_POINTER_POINTER, 294 },
  { (GCallback) cd_dbus_main_add_launcher, dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_DOUBLE_STRING_POINTER_POINTER, 383 },
  { (GCallback) cd_dbus_main_add_temporary_icon, dbus_glib_marshal_cd_dbus_main_BOOLEAN__BOXED_POINTER, 490 },
  { (GCallback) cd_dbus_main_reload_launcher, dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_POINTER, 558 },
  { (GCallback) cd_dbus_main_remove_launcher, dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_POINTER, 618 },
  { (GCallback) cd_dbus_main_set_quick_info, dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_STRING_POINTER, 678 },
  { (GCallback) cd_dbus_main_set_label, dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_STRING_POINTER, 748 },
  { (GCallback) cd_dbus_main_set_icon, dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_STRING_POINTER, 810 },
  { (GCallback) cd_dbus_main_set_emblem, dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_INT_STRING_POINTER, 871 },
  { (GCallback) cd_dbus_main_animate, dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_INT_STRING_POINTER, 948 },
  { (GCallback) cd_dbus_main_demands_attention, dbus_glib_marshal_cd_dbus_main_BOOLEAN__BOOLEAN_STRING_STRING_POINTER, 1027 },
  { (GCallback) cd_dbus_main_show_dialog, dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_INT_STRING_POINTER, 1112 },
  { (GCallback) cd_dbus_main_create_launcher_from_scratch, dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_STRING_STRING_STRING_POINTER, 1191 },
  { (GCallback) cd_dbus_main_load_launcher_from_file, dbus_glib_marshal_cd_dbus_main_BOOLEAN__STRING_POINTER, 1302 },
};

const DBusGObjectInfo dbus_glib_cd_dbus_main_object_info = {  1,
  dbus_glib_cd_dbus_main_methods,
  20,
"org.cairodock.CairoDock\0Reboot\0S\0\0org.cairodock.CairoDock\0Quit\0S\0\0org.cairodock.CairoDock\0ShowDock\0S\0show\0I\0b\0\0org.cairodock.CairoDock\0ShowDesklet\0S\0widgetLayer\0I\0b\0\0org.cairodock.CairoDock\0ReloadModule\0S\0cModuleName\0I\0s\0\0org.cairodock.CairoDock\0ActivateModule\0S\0cModuleName\0I\0s\0bActivate\0I\0b\0\0org.cairodock.CairoDock\0GetIconProperties\0S\0cIconQuery\0I\0s\0pIconProperties\0O\0F\0N\0aa{sv}\0\0org.cairodock.CairoDock\0AddLauncher\0S\0cDesktopFilePath\0I\0s\0fOrder\0I\0d\0cDockName\0I\0s\0cLauncherFile\0O\0F\0N\0s\0\0org.cairodock.CairoDock\0AddTemporaryIcon\0S\0hIconProperties\0I\0a{sv}\0\0org.cairodock.CairoDock\0ReloadLauncher\0S\0cLauncherFile\0I\0s\0\0org.cairodock.CairoDock\0RemoveLauncher\0S\0cLauncherFile\0I\0s\0\0org.cairodock.CairoDock\0SetQuickInfo\0S\0cQuickInfo\0I\0s\0cIconQuery\0I\0s\0\0org.cairodock.CairoDock\0SetLabel\0S\0cLabel\0I\0s\0cIconQuery\0I\0s\0\0org.cairodock.CairoDock\0SetIcon\0S\0cImage\0I\0s\0cIconQuery\0I\0s\0\0org.cairodock.CairoDock\0SetEmblem\0S\0cImage\0I\0s\0iPosition\0I\0i\0cIconQuery\0I\0s\0\0org.cairodock.CairoDock\0Animate\0S\0cAnimation\0I\0s\0iNbRounds\0I\0i\0cIconQuery\0I\0s\0\0org.cairodock.CairoDock\0DemandsAttention\0S\0bStart\0I\0b\0cAnimation\0I\0s\0cIconQuery\0I\0s\0\0org.cairodock.CairoDock\0ShowDialog\0S\0message\0I\0s\0iDuration\0I\0i\0cIconQuery\0I\0s\0\0org.cairodock.CairoDock\0CreateLauncherFromScratch\0S\0cIconFile\0I\0s\0cLabel\0I\0s\0cCommand\0I\0s\0cParentDockName\0I\0s\0\0org.cairodock.CairoDock\0LoadLauncherFromFile\0S\0cDesktopFile\0I\0s\0\0\0",
"\0",
"\0"
};

