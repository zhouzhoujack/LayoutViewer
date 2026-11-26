#include "layRulerObject.h"
#include "detail/utilsString.h"
#include "detail/utilsExpression.h"
#include "detail/utilsVariant.h"

#include "layRulerTemplate.h"
#include "layRulerConfig.h"

#include "tlString.h"
#include "tlExpression.h"

namespace lay
{

// -----------------------------------------------------------------------------
//  RulerObject implementation

RulerObject::RulerObject ()
  : m_p1 (), m_p2 (), m_id (-1),
    m_fmt_x ("$X"), m_fmt_y ("$Y"), m_fmt ("$D"),
    m_style (STY_ruler), m_outline (OL_diag),
    m_snap (true), m_angle_constraint (lay::AC_Global),
    m_main_position (POS_auto),
    m_main_xalign (AL_auto), m_main_yalign (AL_auto),
    m_xlabel_xalign (AL_auto), m_xlabel_yalign (AL_auto),
    m_ylabel_xalign (AL_auto), m_ylabel_yalign (AL_auto)
{
  //  .. nothing yet ..
}

RulerObject::RulerObject (const geo::DPoint &p1, const geo::DPoint &p2, int id, const std::string &fmt_x, const std::string &fmt_y, const std::string &fmt, style_type style, outline_type outline, bool snap, lay::angle_constraint_type angle_constraint)
  : m_p1 (p1), m_p2 (p2), m_id (id),
    m_fmt_x (fmt_x), m_fmt_y (fmt_y), m_fmt (fmt),
    m_style (style), m_outline (outline),
    m_snap (snap), m_angle_constraint (angle_constraint),
    m_main_position (POS_auto),
    m_main_xalign (AL_auto), m_main_yalign (AL_auto),
    m_xlabel_xalign (AL_auto), m_xlabel_yalign (AL_auto),
    m_ylabel_xalign (AL_auto), m_ylabel_yalign (AL_auto)
{
  //  .. nothing else ..
}

RulerObject::RulerObject (const geo::DPoint &p1, const geo::DPoint &p2, int id, const Template &t)
  : m_p1 (p1), m_p2 (p2), m_id (id),
    m_fmt_x (t.fmt_x ()), m_fmt_y (t.fmt_y ()), m_fmt (t.fmt ()),
    m_style (t.style ()), m_outline (t.outline ()),
    m_snap (t.snap ()), m_angle_constraint (t.angle_constraint ()),
    m_category (t.category ()),
    m_main_position (t.main_position ()),
    m_main_xalign (t.main_xalign ()), m_main_yalign (t.main_yalign ()),
    m_xlabel_xalign (t.xlabel_xalign ()), m_xlabel_yalign (t.xlabel_yalign ()),
    m_ylabel_xalign (t.ylabel_xalign ()), m_ylabel_yalign (t.ylabel_yalign ())
{
  //  .. nothing else ..
}

RulerObject::RulerObject (const RulerObject &d)
  : m_p1 (d.m_p1), m_p2 (d.m_p2), m_id (d.m_id),
    m_fmt_x (d.m_fmt_x), m_fmt_y (d.m_fmt_y), m_fmt (d.m_fmt),
    m_style (d.m_style), m_outline (d.m_outline),
    m_snap (d.m_snap), m_angle_constraint (d.m_angle_constraint),
    m_category (d.m_category),
    m_main_position (d.m_main_position),
    m_main_xalign (d.m_main_xalign), m_main_yalign (d.m_main_yalign),
    m_xlabel_xalign (d.m_xlabel_xalign), m_xlabel_yalign (d.m_xlabel_yalign),
    m_ylabel_xalign (d.m_ylabel_xalign), m_ylabel_yalign (d.m_ylabel_yalign)
{
  //  .. nothing else ..
}

RulerObject &
RulerObject::operator= (const RulerObject &d)
{
  if (this != &d) {
    m_p1 = d.m_p1;
    m_p2 = d.m_p2;
    m_id = d.m_id;
    m_fmt_x = d.m_fmt_x;
    m_fmt_y = d.m_fmt_y;
    m_fmt = d.m_fmt;
    m_style = d.m_style;
    m_outline = d.m_outline;
    m_snap = d.m_snap;
    m_angle_constraint = d.m_angle_constraint;
    m_category = d.m_category;
    m_main_position = d.m_main_position;
    m_main_xalign = d.m_main_xalign;
    m_main_yalign = d.m_main_yalign;
    m_xlabel_xalign = d.m_xlabel_xalign;
    m_xlabel_yalign = d.m_xlabel_yalign;
    m_ylabel_xalign = d.m_ylabel_xalign;
    m_ylabel_yalign = d.m_ylabel_yalign;
    property_changed ();
  }
  return *this;
}

bool 
RulerObject::operator< (const RulerObject &b) const
{
  if (m_id != b.m_id) {
    return m_id < b.m_id;
  }
  if (m_p1 != b.m_p1) {
    return m_p1 < b.m_p1;
  }
  if (m_p2 != b.m_p2) {
    return m_p2 < b.m_p2;
  }
  if (m_fmt_x != b.m_fmt_x) {
    return m_fmt_x < b.m_fmt_x;
  }
  if (m_fmt_y != b.m_fmt_y) {
    return m_fmt_y < b.m_fmt_y;
  }
  if (m_fmt != b.m_fmt) {
    return m_fmt < b.m_fmt;
  }
  if (m_style != b.m_style) {
    return m_style < b.m_style;
  }
  if (m_outline != b.m_outline) {
    return m_outline < b.m_outline;
  }
  if (m_snap != b.m_snap) {
    return m_snap < b.m_snap;
  }
  if (m_angle_constraint != b.m_angle_constraint) {
    return m_angle_constraint < b.m_angle_constraint;
  }
  if (m_category != b.m_category) {
    return m_category < b.m_category;
  }
  if (m_main_position != b.m_main_position) {
    return m_main_position < b.m_main_position;
  }
  if (m_main_xalign != b.m_main_xalign) {
    return m_main_xalign < b.m_main_xalign;
  }
  if (m_main_yalign != b.m_main_yalign) {
    return m_main_yalign < b.m_main_yalign;
  }
  if (m_xlabel_xalign != b.m_xlabel_xalign) {
    return m_xlabel_xalign < b.m_xlabel_xalign;
  }
  if (m_xlabel_yalign != b.m_xlabel_yalign) {
    return m_xlabel_yalign < b.m_xlabel_yalign;
  }
  if (m_ylabel_xalign != b.m_ylabel_xalign) {
    return m_ylabel_xalign < b.m_ylabel_xalign;
  }
  if (m_ylabel_yalign != b.m_ylabel_yalign) {
    return m_ylabel_yalign < b.m_ylabel_yalign;
  }
  return false;
}

bool 
RulerObject::equals (const geo::DUserObjectBase *d) const
{
  const RulerObject *ruler = dynamic_cast<const RulerObject *> (d);
  if (ruler) {
    return *this == *ruler;
  } else {
    return false;
  }
}

bool 
RulerObject::operator== (const RulerObject &d) const
{
  return m_p1 == d.m_p1 && m_p2 == d.m_p2 && m_id == d.m_id && 
         m_fmt_x == d.m_fmt_x && m_fmt_y == d.m_fmt_y && m_fmt == d.m_fmt &&
         m_style == d.m_style && m_outline == d.m_outline && 
         m_snap == d.m_snap && m_angle_constraint == d.m_angle_constraint &&
         m_category == d.m_category &&
         m_main_position == d.m_main_position &&
         m_main_xalign == d.m_main_xalign && m_main_yalign == d.m_main_yalign &&
         m_xlabel_xalign == d.m_xlabel_xalign && m_xlabel_yalign == d.m_xlabel_yalign &&
         m_ylabel_xalign == d.m_ylabel_xalign && m_ylabel_yalign == d.m_ylabel_yalign
    ;
}

bool 
RulerObject::less (const geo::DUserObjectBase *d) const
{
  const RulerObject *ruler = dynamic_cast<const RulerObject *> (d);
  if (ruler) {
    return *this < *ruler;
  } else {
    return class_id () < d->class_id ();
  }
}

unsigned int 
RulerObject::class_id () const
{
  static unsigned int cid = geo::get_unique_user_object_class_id ();
  return cid;
}

geo::DUserObjectBase *
RulerObject::clone () const
{
  return new RulerObject (*this);
}

geo::DBox 
RulerObject::box () const
{
  return geo::DBox (m_p1, m_p2);
}

class AnnotationEval
  : public geo::Eval
{
public:
  AnnotationEval (const RulerObject &obj, const geo::DFTrans &t)
    : m_obj (obj), m_trans (t)
  { }

  const RulerObject &obj () const { return m_obj; }
  const geo::DFTrans &trans () const { return m_trans; }

private:
  const RulerObject &m_obj;
  geo::DFTrans m_trans;
};

static double 
delta_x (const RulerObject &obj, const geo::DFTrans &t)
{
  double dx = ((t * obj.p2 ()).x () - (t * obj.p1 ()).x ());

  //  avoid "almost 0" outputs
  if (fabs (dx) < 1e-5 /*micron*/) {
    dx = 0;
  }

  return dx;
}

static double 
delta_y (const RulerObject &obj, const geo::DFTrans &t)
{
  double dy = ((t * obj.p2 ()).y () - (t * obj.p1 ()).y ());

  //  avoid "almost 0" outputs
  if (fabs (dy) < 1e-5 /*micron*/) {
    dy = 0;
  }

  return dy;
}

class AnnotationEvalFunction
  : public geo::EvalFunction
{
public:
  AnnotationEvalFunction (char function, const AnnotationEval *eval)
    : m_function (function), mp_eval (eval)
  {
    // .. nothing yet ..
  }

  void execute (const geo::ExpressionParserContext &context, geo::Variant &out, const std::vector <geo::Variant> &vv) const
  {
    if (vv.size () != 0) {
      throw geo::EvalError (geo::to_string (QObject::tr ("Annotation function must not have arguments")), context);
    }

    const RulerObject &obj = mp_eval->obj ();
    const geo::DFTrans &trans = mp_eval->trans ();

    if (m_function == 'L') {
      out = fabs (delta_x (obj, trans)) + fabs (delta_y (obj, trans));
    } else if (m_function == 'D') {
      out = sqrt (delta_x (obj, trans) * delta_x (obj, trans) + delta_y (obj, trans) * delta_y (obj, trans));
    } else if (m_function == 'A') {
      out = delta_x (obj, trans) * delta_y (obj, trans) * 1e-6;
    } else if (m_function == 'X') {
      out = delta_x (obj, trans);
    } else if (m_function == 'Y') {
      out = delta_y (obj, trans);
    } else if (m_function == 'U') {
      out = (trans * obj.p1 ()).x ();
    } else if (m_function == 'V') {
      out = (trans * obj.p1 ()).y ();
    } else if (m_function == 'P') {
      out = (trans * obj.p2 ()).x ();
    } else if (m_function == 'Q') {
      out = (trans * obj.p2 ()).y ();
    } else {
      out = geo::Variant ();
    }
  }

private:
  char m_function;
  const AnnotationEval *mp_eval;
};

std::string 
RulerObject::formatted (const std::string &fmt, const geo::DFTrans &t) const
{
  AnnotationEval eval (*this, t);
  eval.define_function ("L", new AnnotationEvalFunction('L', &eval)); // manhattan length
  eval.define_function ("D", new AnnotationEvalFunction('D', &eval)); // euclidian distance
  eval.define_function ("X", new AnnotationEvalFunction('X', &eval)); // x delta
  eval.define_function ("Y", new AnnotationEvalFunction('Y', &eval)); // y delta
  eval.define_function ("U", new AnnotationEvalFunction('U', &eval)); // p1.x
  eval.define_function ("V", new AnnotationEvalFunction('V', &eval)); // p1.y
  eval.define_function ("P", new AnnotationEvalFunction('P', &eval)); // p2.x
  eval.define_function ("Q", new AnnotationEvalFunction('Q', &eval)); // p2.y
  eval.define_function ("A", new AnnotationEvalFunction('A', &eval)); // area mm2
  return eval.interpolate (fmt);
}

const char *
RulerObject::class_name () const
{
  return "RulerObject";
}

void 
RulerObject::from_string (const char *s, const char * /*base_dir*/)
{
  geo::Extractor ex (s);
  while (! ex.at_end ()) {

    if (ex.test ("id=")) {

      int i = 0;
      ex.read (i);
      id (i);

    } else if (ex.test ("category=")) {

      std::string s;
      ex.read_word_or_quoted (s);
      set_category (s);

    } else if (ex.test ("fmt=")) {

      std::string s;
      ex.read_word_or_quoted (s);
      fmt (s);

    } else if (ex.test ("fmt_x=")) {

      std::string s;
      ex.read_word_or_quoted (s);
      fmt_x (s);

    } else if (ex.test ("fmt_y=")) {

      std::string s;
      ex.read_word_or_quoted (s);
      fmt_y (s);

    } else if (ex.test ("x1=")) {

      double q = 0;
      ex.read (q);
      geo::DPoint p (p1 ());
      p.set_x (q);
      p1 (p);

    } else if (ex.test ("y1=")) {

      double q = 0;
      ex.read (q);
      geo::DPoint p (p1 ());
      p.set_y (q);
      p1 (p);

    } else if (ex.test ("x2=")) {

      double q = 0;
      ex.read (q);
      geo::DPoint p (p2 ());
      p.set_x (q);
      p2 (p);

    } else if (ex.test ("y2=")) {

      double q = 0;
      ex.read (q);
      geo::DPoint p (p2 ());
      p.set_y (q);
      p2 (p);

    } else if (ex.test ("position=")) {

      std::string s;
      ex.read_word (s);
      lay::PositionConverter pc;
      RulerObject::position_type pos;
      pc.from_string (s, pos);
      set_main_position (pos);
      ex.test (",");

    } else if (ex.test ("xalign=")) {

      std::string s;
      ex.read_word (s);
      lay::AlignmentConverter ac;
      RulerObject::alignment_type a;
      ac.from_string (s, a);
      set_main_xalign (a);
      ex.test (",");

    } else if (ex.test ("yalign=")) {

      std::string s;
      ex.read_word (s);
      lay::AlignmentConverter ac;
      RulerObject::alignment_type a;
      ac.from_string (s, a);
      set_main_yalign (a);
      ex.test (",");

    } else if (ex.test ("xlabel_xalign=")) {

      std::string s;
      ex.read_word (s);
      lay::AlignmentConverter ac;
      RulerObject::alignment_type a;
      ac.from_string (s, a);
      set_xlabel_xalign (a);
      ex.test (",");

    } else if (ex.test ("xlabel_yalign=")) {

      std::string s;
      ex.read_word (s);
      lay::AlignmentConverter ac;
      RulerObject::alignment_type a;
      ac.from_string (s, a);
      set_xlabel_yalign (a);
      ex.test (",");

    } else if (ex.test ("ylabel_xalign=")) {

      std::string s;
      ex.read_word (s);
      lay::AlignmentConverter ac;
      RulerObject::alignment_type a;
      ac.from_string (s, a);
      set_ylabel_xalign (a);
      ex.test (",");

    } else if (ex.test ("ylabel_yalign=")) {

      std::string s;
      ex.read_word (s);
      lay::AlignmentConverter ac;
      RulerObject::alignment_type a;
      ac.from_string (s, a);
      set_ylabel_yalign (a);
      ex.test (",");

    } else if (ex.test ("style=")) {

      std::string s;
      ex.read_word (s);
      lay::StyleConverter sc;
      RulerObject::style_type st;
      sc.from_string (s, st);
      style (st);

    } else if (ex.test ("outline=")) {

      std::string s;
      ex.read_word (s);
      lay::OutlineConverter oc;
      RulerObject::outline_type ot;
      oc.from_string (s, ot);
      outline (ot);

    } else if (ex.test ("snap=")) {

      bool f = false;
      ex.read (f);
      snap (f);

    } else if (ex.test ("angle_constraint=")) {

      std::string s;
      ex.read_word (s);
      lay::ACConverter sc;
      lay::angle_constraint_type sm;
      sc.from_string (s, sm);
      angle_constraint (sm);

    } else {
      break;
    }

    ex.test (",");

  }
}

std::string 
RulerObject::to_string () const
{
  std::string r;

  r += "id=";
  r += geo::to_string (id ());
  r += ",";

  r += "x1=";
  r += geo::to_string (p1 ().x ());
  r += ",";
  r += "y1=";
  r += geo::to_string (p1 ().y ());
  r += ",";
  r += "x2=";
  r += geo::to_string (p2 ().x ());
  r += ",";
  r += "y2=";
  r += geo::to_string (p2 ().y ());
  r += ",";

  r += "category=";
  r += geo::to_word_or_quoted_string (category ());
  r += ",";

  r += "fmt=";
  r += geo::to_word_or_quoted_string (fmt ());
  r += ",";
  r += "fmt_x=";
  r += geo::to_word_or_quoted_string (fmt_x ());
  r += ",";
  r += "fmt_y=";
  r += geo::to_word_or_quoted_string (fmt_y ());
  r += ",";
  
  r += "position=";
  lay::PositionConverter pc;
  r += pc.to_string (main_position ());
  r += ",";

  lay::AlignmentConverter ac;
  r += "xalign=";
  r += ac.to_string (main_xalign ());
  r += ",";
  r += "yalign=";
  r += ac.to_string (main_yalign ());
  r += ",";
  r += "xlabel_xalign=";
  r += ac.to_string (xlabel_xalign ());
  r += ",";
  r += "xlabel_yalign=";
  r += ac.to_string (xlabel_yalign ());
  r += ",";
  r += "ylabel_xalign=";
  r += ac.to_string (ylabel_xalign ());
  r += ",";
  r += "ylabel_yalign=";
  r += ac.to_string (ylabel_yalign ());
  r += ",";

  r += "style=";
  lay::StyleConverter sc;
  r += sc.to_string (style ());
  r += ",";
  
  r += "outline=";
  lay::OutlineConverter oc;
  r += oc.to_string (outline ());
  r += ",";
  
  r += "snap=";
  r += geo::to_string (snap ());
  r += ",";

  r += "angle_constraint=";
  lay::ACConverter acc;
  r += acc.to_string (angle_constraint ());

  return r;
}

void
RulerObject::property_changed ()
{
  //  .. nothing yet ..
}

/**
 *  @brief Registration of the RulerObject class in the DUserObject space
 */
static geo::DUserObjectDeclaration class_registrar (new geo::user_object_factory_impl<RulerObject, geo::DCoord> ("RulerObject"));

} // namespace lay

