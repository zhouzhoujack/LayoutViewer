#include "layLayoutStatisticsForm.h"
#include "tlException.h"
#include "detail/utilsString.h"
#include "detail/utilsInternational.h"
#include "tlLog.h"
#include "detail/utilsProgress.h"
#include "detail/utilsExpression.h"
#include "detail/utilsVariant.h"
#include "layLayoutView.h"
#include "tlInternational.h"
#include "tlString.h"
#include "tlExpression.h"
#include "tlTimer.h"
#include "detail/geoLayoutQuery.h"
#include "detail/geoCellGraphUtils.h"

#include <set>
#include <sstream>

#include <QBuffer>
#include <QResource>
#include <QFileInfo>
#include <QDomElement>
#include <QDomDocument>
#include <QXmlStreamWriter>
#include <QTextStream>
#if QT_VERSION >= 0x050000
#  include <QUrlQuery>
#endif

namespace lay
{

/**
 *  @brief A comparison operator to sort layers by layer and datatype and then by name
 */
struct CompareLDName 
{
  CompareLDName (const geo::Layout &l) 
    : mp_layout (&l)
  {
    //  .. nothing yet ..
  }

  bool operator() (unsigned int a, unsigned int b)
  {
    if (mp_layout->is_valid_layer (a) && mp_layout->is_valid_layer (b)) {
      const geo::LayerProperties &lp_a = mp_layout->get_properties (a);
      const geo::LayerProperties &lp_b = mp_layout->get_properties (b);
      if (lp_a.layer != lp_b.layer) {
        return lp_a.layer < lp_b.layer;
      }
      if (lp_a.datatype != lp_b.datatype) {
        return lp_a.datatype < lp_b.datatype;
      }
      if (lp_a.name != lp_b.name) {
        return lp_a.name < lp_b.name;
      }
    }
    return false;
  }

private:
  const geo::Layout *mp_layout;
};

/**
 *  @brief A comparison operator to sort layers by name and then by layer and datatype 
 */
struct CompareNameLD 
{
  CompareNameLD (const geo::Layout &l) 
    : mp_layout (&l)
  {
    //  .. nothing yet ..
  }

  bool operator() (unsigned int a, unsigned int b)
  {
    if (mp_layout->is_valid_layer (a) && mp_layout->is_valid_layer (b)) {
      const geo::LayerProperties &lp_a = mp_layout->get_properties (a);
      const geo::LayerProperties &lp_b = mp_layout->get_properties (b);
      if (lp_a.name != lp_b.name) {
        return lp_a.name < lp_b.name;
      }
      if (lp_a.layer != lp_b.layer) {
        return lp_a.layer < lp_b.layer;
      }
      if (lp_a.datatype != lp_b.datatype) {
        return lp_a.datatype < lp_b.datatype;
      }
    }
    return false;
  }

private:
  const geo::Layout *mp_layout;
};

static std::string format_tech_name (const std::string &s)
{
  if (s.empty ()) {
    return s;
  } else {
    return " ('" + s + "')";
  }
}

// ------------------------------------------------------------

/**
 *  @brief A template processor for creating HTML pages from a template
 *
 *  TODO: this is just a first step and far from being complete.
 *  The template processor is used from the browser page by using an extension .stxml.
 *  It reads a XML file from a resource path ":/st/<path>" and converts it into HTML.
 */
class StatisticsTemplateProcessor 
{
public:
  StatisticsTemplateProcessor (const QUrl &url, const geo::Layout *layout)
    : mp_layout (layout)
  {
    QResource res (QString::fromUtf8 (":/st/") + url.path ());
    if (res.compressionAlgorithm () != QResource::NoCompression) {
      m_temp = qUncompress ((const unsigned char *)res.data (), (int)res.size ());
    } else {
      m_temp = QByteArray ((const char *)res.data (), (int)res.size ());
    }

#if QT_VERSION >= 0x050000
    QList<QPair<QString, QString> > queryItems = QUrlQuery (url.query ()).queryItems ();
#else
    QList<QPair<QString, QString> > queryItems = url.queryItems ();
#endif
    for (QList<QPair<QString, QString> >::const_iterator q = queryItems.begin (); q != queryItems.end (); ++q) {
      m_top_eval.set_var (geo::to_string (q->first), geo::to_string (q->second));
    }
  }

  bool process ();

  const QByteArray &get () const
  {
    return m_output.buffer ();
  }

private:
  QByteArray m_temp;
  QBuffer m_output;
  geo::Eval m_top_eval;
  const geo::Layout *mp_layout;

  void process (const QDomElement &element, geo::Eval &eval, QXmlStreamWriter &writer);
  void process_child_nodes (const QDomElement &element, geo::Eval &eval, QXmlStreamWriter &writer);
};

bool 
StatisticsTemplateProcessor::process ()
{
  m_output.open (QIODevice::WriteOnly);

  bool ret_value = false;

  try
  {
    tl::SelfTimer timer (toolkit::logkit::verbosity () > 21, "StatisticsForm: create content");

    QDomDocument doc;
    doc.setContent (m_temp, true);

    QXmlStreamWriter writer (&m_output);
    writer.writeStartDocument (QString::fromUtf8 ("1.0"));
    process (doc.documentElement (), m_top_eval, writer);
    writer.writeEndDocument ();

    ret_value = true;

  } catch (tl::Exception &ex) {
    LOGE << ex.msg ();
    QTextStream writer (&m_output);
    writer << geo::to_string (QObject::tr ("ERROR: evaluating template: ")).c_str () << ex.msg ().c_str ();
  }

  m_output.close ();
  return ret_value;
}

void
StatisticsTemplateProcessor::process_child_nodes (const QDomElement &element, geo::Eval &eval, QXmlStreamWriter &writer)
{
  if (element.isNull ()) {
    return;
  }

  for (QDomNode n = element.firstChild (); ! n.isNull (); n = n.nextSibling ()) {

    if (n.isElement ()) {

      process (n.toElement (), eval, writer);

    } else if (n.isCDATASection ()) {

      writer.writeCDATA (geo::to_qstring (eval.interpolate (geo::to_string (n.toCDATASection ().data ()))));

    } else if (n.isCharacterData ()) {

      QString t; 
      QTextStream s (&t);

      while (true) {

        s << n.toCharacterData ().data ();
        QDomNode nn = n.nextSibling ();
        if (nn.isNull () || ! nn.isCharacterData ()) {
          break;
        }
        n = nn;

      }

      writer.writeCharacters (geo::to_qstring (eval.interpolate (geo::to_string (t))));

    }

  }
}

void
StatisticsTemplateProcessor::process (const QDomElement &element, geo::Eval &eval, QXmlStreamWriter &writer)
{
  static QString template_namespace_uri = QString::fromUtf8 ("www.klayout.org/layout-statistics-template");
  static QString template_cmd_if = QString::fromUtf8 ("if");
  static QString template_cmd_true = QString::fromUtf8 ("true");
  static QString template_cmd_false = QString::fromUtf8 ("false");
  static QString template_cmd_eval = QString::fromUtf8 ("eval");
  static QString template_cmd_query = QString::fromUtf8 ("query");
  static QString template_cmd_begin = QString::fromUtf8 ("begin");
  static QString template_cmd_end = QString::fromUtf8 ("end");
  static QString template_cmd_max = QString::fromUtf8 ("max");
  static QString template_cmd_each = QString::fromUtf8 ("each");
  static QString template_value_true = QString::fromUtf8 ("true");
  static QString template_value_empty_query = QString::fromUtf8 ("");
  static QString template_name_expr = QString::fromUtf8 ("expr");
  static QString template_name_max = QString::fromUtf8 ("max");

  if (element.namespaceURI () == template_namespace_uri) {

    if (element.localName () == template_cmd_eval) {

      geo::Expression expr;
      eval.parse (expr, geo::to_string (element.attribute (template_name_expr, template_value_true)));
      expr.execute ();

    } else if (element.localName () == template_cmd_if) {

      QDomElement true_node, false_node;

      for (QDomNode n = element.firstChild (); ! n.isNull (); n = n.nextSibling ()) {
        QDomElement e = n.toElement ();
        if (! e.isNull () && e.namespaceURI () == template_namespace_uri) {
          if (e.localName () == template_cmd_true) {
            true_node = e;
          } else if (e.localName () == template_cmd_false) {
            false_node = e;
          }
        }
      }

      if (true_node.isNull () && false_node.isNull ()) {
        true_node = element;
      }

      geo::Expression expr;
      eval.parse (expr, geo::to_string (element.attribute (template_name_expr, template_value_true)));
      geo::Variant value = expr.execute ();

      if (value.to_bool ()) {
        if (! true_node.isNull ()) {
          process (true_node, eval, writer);
        } 
      } else {
        if (! false_node.isNull ()) {
          process (false_node, eval, writer);
        } 
      }

    } else if (element.localName () == template_cmd_query) {

      QDomElement begin_node, end_node, each_node, max_node;

      for (QDomNode n = element.firstChild (); ! n.isNull (); n = n.nextSibling ()) {
        QDomElement e = n.toElement ();
        if (! e.isNull () && e.namespaceURI () == template_namespace_uri) {
          if (e.localName () == template_cmd_begin) {
            begin_node = e;
          } else if (e.localName () == template_cmd_end) {
            end_node = e;
          } else if (e.localName () == template_cmd_max) {
            max_node = e;
          } else if (e.localName () == template_cmd_each) {
            each_node = e;
          }
        }
      }

      if (begin_node.isNull () && end_node.isNull () && max_node.isNull () && each_node.isNull ()) {
        each_node = element;
      }

      unsigned long max_count = std::numeric_limits<unsigned long>::max ();
      QString max_attr = element.attribute (template_name_max, QString ());
      if (! max_attr.isNull ()) {
        geo::Expression expr;
        eval.parse (expr, geo::to_string (max_attr));
        geo::Variant max = expr.execute ();
        if (max.can_convert_to_ulong ()) {
          max_count = max.to_ulong ();
        }
      }

#if defined(LAY_HAS_LAYOUT_QUERY)
      geo::LayoutQuery q (geo::to_string (element.attribute (template_name_expr, template_value_empty_query)));

      geo::LayoutQueryIterator qi (q, mp_layout, &eval);

      process_child_nodes (begin_node, qi.eval (), writer);

      for ( ; !qi.at_end (); ++qi) {
        if (max_count == 0) {
          process_child_nodes (max_node, qi.eval (), writer);
          break;
        } else {
          --max_count;
          process_child_nodes (each_node, qi.eval (), writer);
        }
      }

      process_child_nodes (end_node, qi.eval (), writer);
#else
      // TODO(layout-core): layout-core 的预编译库里没有导出 geo::LayoutQuery /
      // geo::LayoutQueryIterator，统计模板的 <query> 指令暂不支持。
      LOGW << tl::to_string (QObject::tr ("Layout statistics: query templates are not available in this build")) << "\n";
#endif

    }

  } else {

    writer.writeStartElement (element.nodeName ());

    if (element.hasAttributes ()) {
      //  Hint: attribute nodes are not children of the elements ..
      QDomNamedNodeMap attributes = element.attributes ();
      for (int i = 0; i < attributes.count (); ++i) {
        QDomAttr a = attributes.item (i).toAttr ();
        if (! a.isNull ()) {
          writer.writeAttribute (a.nodeName (), geo::to_qstring (eval.interpolate (geo::to_string (a.value ()))));
        }
      }
    }

    process_child_nodes (element, eval, writer);

    writer.writeEndElement ();

  }
}

// ------------------------------------------------------------

class ShapeStatistics
{
public:
  ShapeStatistics ()
  {
    // .. nothing yet ..
  }

  void compute (const geo::Shapes &shapes)
  {
    for (geo::Shapes::shape_iterator i = shapes.begin (geo::ShapeIterator::All); ! i.at_end (); ++i) {
      size_t n = 1;
      if (i.in_array ()) {
        n = i.array ().array_size ();
        m_count [i.array ().type ()] += 1;
        i.finish_array ();
      }
      m_count [i->type ()] += n;
    }
  }

  void operator*= (size_t f)
  {
    for (std::map<geo::Shape::object_type, size_t>::iterator c = m_count.begin (); c != m_count.end (); ++c) {
      c->second *= f;
    }
  }

  void operator+= (const ShapeStatistics &other)
  {
    for (std::map<geo::Shape::object_type, size_t>::const_iterator c = other.m_count.begin (); c != other.m_count.end (); ++c) {
      m_count [c->first] += c->second;
    }
  }

  size_t count (geo::Shape::object_type t) const
  {
    std::map<geo::Shape::object_type, size_t>::const_iterator c = m_count.find (t);
    if (c != m_count.end ()) {
      return c->second;
    } else {
      return 0;
    }
  }

  size_t box_total () const
  {
    return count (geo::Shape::Box) + count (geo::Shape::BoxArrayMember) + count (geo::Shape::ShortBox) + count (geo::Shape::ShortBoxArrayMember);
  }

  size_t box_single () const
  {
    return count (geo::Shape::Box) + count (geo::Shape::ShortBox);
  }

  size_t box_array () const
  {
    return count (geo::Shape::BoxArray) + count (geo::Shape::ShortBoxArray);
  }

  size_t polygon_total () const
  {
    return count (geo::Shape::Polygon) + count (geo::Shape::PolygonRef) + count (geo::Shape::PolygonPtrArrayMember)
         + count (geo::Shape::SimplePolygon) + count (geo::Shape::SimplePolygonRef) + count (geo::Shape::SimplePolygonPtrArrayMember);
  }

  size_t polygon_single () const
  {
    return count (geo::Shape::Polygon) + count (geo::Shape::PolygonRef)
         + count (geo::Shape::SimplePolygon) + count (geo::Shape::SimplePolygonRef);
  }

  size_t polygon_array () const
  {
    return count (geo::Shape::PolygonPtrArray) + count (geo::Shape::SimplePolygonPtrArray);
  }

  size_t path_total () const
  {
    return count (geo::Shape::Path) + count (geo::Shape::PathRef) + count (geo::Shape::PathPtrArrayMember);
  }

  size_t path_single () const
  {
    return count (geo::Shape::Path) + count (geo::Shape::PathRef);
  }

  size_t path_array () const
  {
    return count (geo::Shape::PathPtrArray);
  }

  size_t text_total () const
  {
    return count (geo::Shape::Text) + count (geo::Shape::TextRef) + count (geo::Shape::TextPtrArrayMember);
  }

  size_t text_single () const
  {
    return count (geo::Shape::Text) + count (geo::Shape::TextRef);
  }

  size_t text_array () const
  {
    return count (geo::Shape::TextPtrArray);
  }

  size_t edge_total () const
  {
    return count (geo::Shape::Edge);
  }

  size_t user_total () const
  {
    return count (geo::Shape::UserObject);
  }

private:
  std::map<geo::Shape::object_type, size_t> m_count;
};

// ------------------------------------------------------------

class StatisticsSource
  : public lay::BrowserSource
{
public:
  StatisticsSource (const lay::LayoutHandleRef &h)
    : m_h (h)
  {
    //  .. nothing yet ..
  }

  std::string get (const std::string &url);

private:
  const lay::LayoutHandleRef m_h;
};

std::string
StatisticsSource::get (const std::string &url)
{
  static QString s_per_layer_stat_path_ld = QString::fromUtf8 ("per-layer-stat-ld");
  static QString s_per_layer_stat_path_name = QString::fromUtf8 ("per-layer-stat-name");

  QUrl qurl (geo::to_qstring (url));
  QFileInfo fi (qurl.path ());

  if (fi.suffix () == QString::fromUtf8 ("stxml")) {

    StatisticsTemplateProcessor tp (qurl, &m_h->layout ());
    tp.process ();
    std::string r = tp.get ().constData ();
    return r;

  } else if (fi.baseName () == s_per_layer_stat_path_ld || fi.baseName () == s_per_layer_stat_path_name) {

    //  This is the default top level page
    //  TODO: handle other input as well

    const geo::Layout &layout = m_h->layout ();

    std::ostringstream os;
    os.imbue (std::locale ("C"));

    std::vector <unsigned int> layers;
    for (unsigned int i = 0; i < layout.layers (); ++i) {
      if (layout.is_valid_layer (i)) {
        layers.push_back (i);
      }
    }

    if (fi.baseName () == s_per_layer_stat_path_ld) {
      std::sort (layers.begin (), layers.end (), CompareLDName (layout));
    } else {
      std::sort (layers.begin (), layers.end (), CompareNameLD (layout));
    }

    os << "<html>" << std::endl
       <<   "<body>" << std::endl
       <<     "<h2>" << geo::to_string (QObject::tr ("Detailed Layer Statistics for '")) << m_h->name () << "'</h2>" << std::endl

       <<     "<p>" << std::endl
       <<     "<table cellspacing=\"5\" cellpadding=\"5\">" << std::endl

       <<       "<tr>" << std::endl
       <<         "<th bgcolor=\"#f0f0f0\">" << geo::to_string (QObject::tr ("Layer")) << "</th>" << std::endl
       <<         "<th colspan=\"3\" bgcolor=\"#f0f0f0\">" << geo::to_string (QObject::tr ("Boxes")) << "</th>" << std::endl
       <<         "<th colspan=\"3\" bgcolor=\"#f0f0f0\">" << geo::to_string (QObject::tr ("Polygons")) << "</th>" << std::endl
       <<         "<th colspan=\"3\" bgcolor=\"#f0f0f0\">" << geo::to_string (QObject::tr ("Paths")) << "</th>" << std::endl
       <<         "<th colspan=\"3\" bgcolor=\"#f0f0f0\">" << geo::to_string (QObject::tr ("Texts")) << "</th>" << std::endl
       <<         "<th bgcolor=\"#f0f0f0\">" << geo::to_string (QObject::tr ("Edges")) << "</th>" << std::endl
       <<         "<th bgcolor=\"#f0f0f0\">" << geo::to_string (QObject::tr ("User objects")) << "</th>" << std::endl
       <<       "</tr>" << std::endl

       <<       "<tr>" << std::endl
       <<         "<th></th>" << std::endl
       <<         "<th>" << geo::to_string (QObject::tr ("(total)")) << "</th><th>" << geo::to_string (QObject::tr ("(single)")) << "</th><th>" << geo::to_string (QObject::tr ("(arrays)")) << "</th>" << std::endl
       <<         "<th>" << geo::to_string (QObject::tr ("(total)")) << "</th><th>" << geo::to_string (QObject::tr ("(single)")) << "</th><th>" << geo::to_string (QObject::tr ("(arrays)")) << "</th>" << std::endl
       <<         "<th>" << geo::to_string (QObject::tr ("(total)")) << "</th><th>" << geo::to_string (QObject::tr ("(single)")) << "</th><th>" << geo::to_string (QObject::tr ("(arrays)")) << "</th>" << std::endl
       <<         "<th>" << geo::to_string (QObject::tr ("(total)")) << "</th><th>" << geo::to_string (QObject::tr ("(single)")) << "</th><th>" << geo::to_string (QObject::tr ("(arrays)")) << "</th>" << std::endl
       <<         "<th>" << geo::to_string (QObject::tr ("(total)")) << "</th>" << std::endl
       <<         "<th>" << geo::to_string (QObject::tr ("(total)")) << "</th>" << std::endl
       <<       "</tr>" << std::endl
      ;

    geo::CellCounter cc (&layout);

    geo::RelativeProgress progress (geo::to_string (QObject::tr ("Collecting statistics")), layers.size () * layout.cells (), 100000);
    for (std::vector <unsigned int>::const_iterator l = layers.begin (); l != layers.end (); ++l) {

      ShapeStatistics st_hier;
      ShapeStatistics st_flat;

      for (geo::Layout::top_down_const_iterator c = layout.begin_top_down (); c != layout.end_top_down (); ++c) {

        ShapeStatistics st;
        st.compute (layout.cell (*c).shapes (*l));

        st_hier += st;
        st *= cc.weight (*c);
        st_flat += st;

        ++progress;

      }

      os <<       "<tr>" << std::endl
         <<         "<td>" << layout.get_properties (*l).to_string () << "</td>" << std::endl
         // Boxes (total, single, array)
         <<         "<td>" << st_hier.box_total () << "<br></br>" << st_flat.box_total () << "</td>" << std::endl
         <<         "<td>" << st_hier.box_single () << "<br></br>" << st_flat.box_single () << "</td>" << std::endl
         <<         "<td>" << st_hier.box_array () << "<br></br>" << st_flat.box_array () << "</td>" << std::endl
         // Polygons (total, single, array)
         <<         "<td>" << st_hier.polygon_total () << "<br></br>" << st_flat.polygon_total () << "</td>" << std::endl
         <<         "<td>" << st_hier.polygon_single () << "<br></br>" << st_flat.polygon_single () << "</td>" << std::endl
         <<         "<td>" << st_hier.polygon_array () << "<br></br>" << st_flat.polygon_array () << "</td>" << std::endl
         // Paths (total, single, array)
         <<         "<td>" << st_hier.path_total () << "<br></br>" << st_flat.path_total () << "</td>" << std::endl
         <<         "<td>" << st_hier.path_single () << "<br></br>" << st_flat.path_single () << "</td>" << std::endl
         <<         "<td>" << st_hier.path_array () << "<br></br>" << st_flat.path_array () << "</td>" << std::endl
         // Texts (total, single, array)
         <<         "<td>" << st_hier.text_total () << "<br></br>" << st_flat.text_total () << "</td>" << std::endl
         <<         "<td>" << st_hier.text_single () << "<br></br>" << st_flat.text_single () << "</td>" << std::endl
         <<         "<td>" << st_hier.text_array () << "<br></br>" << st_flat.text_array () << "</td>" << std::endl
         // Edges (total)
         <<         "<td>" << st_hier.edge_total () << "<br></br>" << st_flat.edge_total () << "</td>" << std::endl
         // User objects (total)
         <<         "<td>" << st_hier.user_total () << "<br></br>" << st_flat.user_total () << "</td>" << std::endl
         // ...
         <<         "<td>" << geo::to_string (QObject::tr ("(hier)")) << "<br></br>" << geo::to_string (QObject::tr ("(flat)")) << "</td>" << std::endl
         <<       "</tr>" << std::endl
        ;

    }

    os <<     "</table>" << std::endl
       <<     "</p>" << std::endl
       <<     geo::to_string (QObject::tr ("<h4>Note</h4>"
                                          "<p>"
                                          "\"(hier)\" is the object count where each cell counts once. "
                                          "\"(flat)\" is the \"as if flat\" count where the cells count as many times as they are seen from the top cells."
                                          "</p>"
                                          "<p>"
                                          "\"(total)\" is the effective number of shapes. \"(single)\" are the single shapes. "
                                          "\"(arrays)\" is the number of shape arrays where each array counts as one, but contributes many individual shapes to \"(total)\"."
                                          "</p>"
                                         ))
       <<   "</body>" << std::endl
       << "</html>";

    return os.str ();

  } else {

    //  This is the default top level page
    //  TODO: handle other input as well

    const geo::Layout &layout = m_h->layout ();

    std::ostringstream os;
    os.imbue (std::locale ("C"));

    size_t num_cells = layout.cells ();

    size_t num_layers = 0;
    for (unsigned int i = 0; i < layout.layers (); ++i) {
      if (layout.is_valid_layer (i)) {
        ++num_layers;
      }
    }

    os << "<html>" << std::endl
       <<   "<body>" << std::endl
       <<     "<h2>" << geo::to_string (QObject::tr ("Common Statistics For '")) << m_h->name () << "'</h2>" << std::endl
       <<     "<p>" << std::endl
       <<     "<table>" << std::endl
       <<       "<tr>"
       <<         "<td>" << geo::to_string (QObject::tr ("Path")) << ":&nbsp;</td><td>" << m_h->filename () << "</td>"
       <<       "</tr>" << std::endl
       <<       "<tr>" 
       <<         "<td>" << geo::to_string (QObject::tr ("Technology")) << ":&nbsp;</td><td>" << m_h->technology ()->description () << format_tech_name (m_h->tech_name ()) << "</td>"
       <<       "</tr>" << std::endl
       <<       "<tr>"
       <<         "<td>" << geo::to_string (QObject::tr ("Database unit")) << ":&nbsp;</td><td>" << geo::sprintf ("%.12g ", layout.dbu ()) << geo::to_string (QObject::tr ("micron")) << "</td>"
       <<       "</tr>" << std::endl
       <<       "<tr>"
       <<         "<td>" << geo::to_string (QObject::tr ("Number of cells")) << ":&nbsp;</td><td>" << num_cells << "</td>"
       <<       "</tr>" << std::endl
       <<       "<tr>"
       <<         "<td>" << geo::to_string (QObject::tr ("Number of layers")) << ":&nbsp;</td><td>" << num_layers << "</td>"
       <<       "</tr>" << std::endl;
    for (geo::Layout::meta_info_iterator meta = layout.begin_meta (); meta != layout.end_meta (); ++meta) {
      os <<     "<tr><td>" << meta->description << "</td><td>" << meta->value << "</td></tr>" << std::endl;
    }
    os <<     "</table>" << std::endl
       <<     "<h2>" << geo::to_string (QObject::tr ("Top Cells")) << "</h2>" << std::endl
       <<     "<table>" << std::endl;
    for (geo::Layout::top_down_const_iterator tc = layout.begin_top_down (); tc != layout.end_top_cells (); ++tc) {
      os <<     "<tr><td>" << layout.cell_name (*tc) << "</td></tr>" << std::endl;
    }
    os <<     "</table>" << std::endl;
    os <<     "</p>" << std::endl;

    std::vector <unsigned int> layers_with_oasis_names;

    std::vector <unsigned int> layers_sorted_by_ld;
    layers_sorted_by_ld.reserve (num_layers);
    for (unsigned int i = 0; i < layout.layers (); ++i) {
      if (layout.is_valid_layer (i)) {
        layers_sorted_by_ld.push_back (i);
        const geo::LayerProperties &lp = layout.get_properties (i);
        if (! lp.name.empty ()) {
          layers_with_oasis_names.push_back (i);
        }
      }
    }

    std::sort (layers_sorted_by_ld.begin (), layers_sorted_by_ld.end (), CompareLDName (layout));
    std::sort (layers_with_oasis_names.begin (), layers_with_oasis_names.end (), CompareNameLD (layout));
     
    if (! layers_sorted_by_ld.empty ()) {

      os <<     "<h2>" << geo::to_string (QObject::tr ("Layers (sorted by layer and datatype)")) << "</h2>" << std::endl
         <<     "<p><a href=\"" << geo::to_string (s_per_layer_stat_path_ld) << "\">Detailed layer statistics</a></p>" << std::endl
         <<     "<p>" << std::endl
         <<     "<table>" << std::endl
         <<     "<tr><td><b>" << geo::to_string (QObject::tr ("Layer/Datatype")) << "</b>&nbsp;&nbsp;</td>";
      if (! layers_with_oasis_names.empty ()) {
        os << "<td><b>" << geo::to_string (QObject::tr ("Layer name")) << "</b></td>";
      }
      os << "</tr>" << std::endl;

      for (std::vector <unsigned int>::const_iterator i = layers_sorted_by_ld.begin (); i != layers_sorted_by_ld.end (); ++i) {
        if (layout.is_valid_layer (*i)) {
          const geo::LayerProperties &lp = layout.get_properties (*i);
          os << "<tr>"
             <<   "<td>" << geo::sprintf ("%d/%d", lp.layer, lp.datatype) << "</td>";
          if (! layers_with_oasis_names.empty ()) {
            os <<   "<td>" << lp.name << "</td>";
          }
          os << "</tr>" << std::endl;
        }
      }

      os <<     "</table>" << std::endl;
      os <<     "</p>" << std::endl;

    }

    if (! layers_with_oasis_names.empty ()) {

      os <<     "<h2>" << geo::to_string (QObject::tr ("Layers (sorted by layer names)")) << "</h2>" << std::endl
         <<     "<p><a href=\"" << geo::to_string (s_per_layer_stat_path_name) << "\">Detailed layer statistics</a></p>" << std::endl
         <<     "<p>" << std::endl
         <<     "<table>" << std::endl
         <<     "<tr><td><b>" << geo::to_string (QObject::tr ("Layer name")) << "</b>&nbsp;&nbsp;</td><td><b>" << geo::to_string (QObject::tr ("Layer/Datatype")) << "</b></td></tr>" << std::endl;
      
      for (std::vector <unsigned int>::const_iterator i = layers_with_oasis_names.begin (); i != layers_with_oasis_names.end (); ++i) {
        if (layout.is_valid_layer (*i)) {
          const geo::LayerProperties &lp = layout.get_properties (*i);
          if (! lp.name.empty ()) {
            os << "<tr>"
               <<   "<td>" << lp.name << "</td>"
               <<   "<td>" << geo::sprintf ("%d/%d", lp.layer, lp.datatype) << "</td>"
               << "</tr>" << std::endl;
          }
        }
      }

      os <<     "</table>" << std::endl;
      os <<     "</p>" << std::endl;

    }

    os <<   "</body>" << std::endl
       << "</html>" << std::endl;
       ;

    return os.str ();

  }
}

// ------------------------------------------------------------

// -----------------------------------------------------------------------------
//  LayoutStatisticsForm implementation

LayoutStatisticsForm::LayoutStatisticsForm (QWidget *parent, lay::LayoutView *view, const char *name)
  : QDialog (parent), Ui::LayoutStatisticsForm (), mp_source (0)
{
  setObjectName (QString::fromUtf8 (name));

  Ui::LayoutStatisticsForm::setupUi (this);

  //  collect the distinct layout handles 
  std::set <lay::LayoutHandle *> handles;
  for (unsigned int n = 0; n < view->cellviews (); ++n) {
    handles.insert (view->cellview (n).operator-> ());
  }
  
  m_handles.reserve (handles.size ());
  for (unsigned int n = 0; n < view->cellviews (); ++n) {
    lay::LayoutHandle *h = view->cellview (n).operator-> ();
    if (handles.find (h) != handles.end ()) {
      m_handles.push_back (h);
      handles.erase (h);
      layout_cbx->addItem (geo::to_qstring (h->name ()));
    }
  }

  layout_cbx->setCurrentIndex (view->active_cellview_index ());

  connect (layout_cbx, SIGNAL (activated (int)), this, SLOT (layout_selected (int)));

  layout_selected (layout_cbx->currentIndex ());
}

LayoutStatisticsForm::~LayoutStatisticsForm ()
{
  browser->set_source (0);
  if (mp_source != 0) {
    delete mp_source;
    mp_source = 0;
  }
}

void
LayoutStatisticsForm::layout_selected (int index)
{
  if (index >= int (m_handles.size ()) || index < 0) {
    return;
  }

  browser->set_source (0);
  if (mp_source != 0) {
    delete mp_source;
  }

  mp_source = new StatisticsSource (m_handles [index]);
  browser->set_source (mp_source);
  browser->set_home ("int:index");
  browser->home ();
}

}


