#ifndef RAPIDXML_EXT_H
#define RAPIDXML_EXT_H

#include "rapidxml.hpp"
#include <fstream>  // For std::ofstream
#include <iterator> // For std::back_inserter

// Adding declarations to make it compatible with gcc 4.7 and greater
namespace rapidxml
{
    namespace internal
    {
        // Forward declarations of internal print functions (from rapidxml_print.hpp)
        template <class OutIt, class Ch>
        inline OutIt print_children(OutIt out, const xml_node<Ch> *node, int flags, int indent);

        template <class OutIt, class Ch>
        inline OutIt print_attributes(OutIt out, const xml_node<Ch> *node, int flags);

        template <class OutIt, class Ch>
        inline OutIt print_data_node(OutIt out, const xml_node<Ch> *node, int flags, int indent);

        template <class OutIt, class Ch>
        inline OutIt print_cdata_node(OutIt out, const xml_node<Ch> *node, int flags, int indent);

        template <class OutIt, class Ch>
        inline OutIt print_element_node(OutIt out, const xml_node<Ch> *node, int flags, int indent);

        template <class OutIt, class Ch>
        inline OutIt print_declaration_node(OutIt out, const xml_node<Ch> *node, int flags, int indent);

        template <class OutIt, class Ch>
        inline OutIt print_comment_node(OutIt out, const xml_node<Ch> *node, int flags, int indent);

        template <class OutIt, class Ch>
        inline OutIt print_doctype_node(OutIt out, const xml_node<Ch> *node, int flags, int indent);

        template <class OutIt, class Ch>
        inline OutIt print_pi_node(OutIt out, const xml_node<Ch> *node, int flags, int indent);
    }
}

#include "rapidxml_print.hpp"

// Helper function to write XML document to file using a buffer
static const int buf_len = 2048;
static char buf[buf_len] = {0};

// Fixed function signature: use template for character type and const reference for filename
template<class Ch = char>
void xml2file(const std::string& filename, rapidxml::xml_node<Ch>& doc)
{
    std::ofstream outfile(filename, std::ios::out);
    if (outfile)
    {
        char* end = rapidxml::print(buf, doc, 0);
        *end = 0;
        outfile << buf;
        outfile.close();
    }
}

// Helper: Convert string to XML char (rapidxml modifies strings during parsing)
inline char *allocate_xml_string(rapidxml::xml_document<char> &doc, const std::string &s)
{
    return doc.allocate_string(s.c_str());
}

// Helper: Write a node with text content
template <typename T>
inline void write_node(rapidxml::xml_node<char> *parent, const char *name, const T &value)
{
    std::ostringstream oss;
    oss << std::setprecision(10) << value;
    std::string str = oss.str();
    rapidxml::xml_node<char> *node = parent->document()->allocate_node(
        rapidxml::node_element, name);
    node->value(allocate_xml_string(*parent->document(), str));
    parent->append_node(node);
}

// Helper: Write a node with string content
inline void write_node(rapidxml::xml_node<char> *parent, const char *name, const std::string &value)
{
    rapidxml::xml_node<char> *node = parent->document()->allocate_node(
        rapidxml::node_element, name);
    node->value(allocate_xml_string(*parent->document(), value));
    parent->append_node(node);
}

// Helper: Read text content from node as string
inline std::string read_string(const rapidxml::xml_node<char> *node, const char *name)
{
    auto child = node->first_node(name);
    return child ? child->value() : "";
}

// Helper: Read text content from node as double
inline double read_double(const rapidxml::xml_node<char> *node, const char *name, double defaultValue = 0.0)
{
    auto child = node->first_node(name);
    if (!child)
        return defaultValue;
    try
    {
        return std::stod(child->value());
    }
    catch (...)
    {
        return defaultValue;
    }
}

// Helper: Read text content from node as int
inline int read_int(const rapidxml::xml_node<char> *node, const char *name, int defaultValue = 0)
{
    auto child = node->first_node(name);
    if (!child)
        return defaultValue;
    try
    {
        return std::stoi(child->value());
    }
    catch (...)
    {
        return defaultValue;
    }
}

// Helper: Read text content from node as bool
inline bool read_bool(const rapidxml::xml_node<char> *node, const char *name, bool defaultValue = false)
{
    auto child = node->first_node(name);
    if (!child)
        return defaultValue;
    std::string val = child->value();
    return val == "true" || val == "1";
}

// Helper: Read attribute value from node as double
inline double read_attr_double(const rapidxml::xml_node<char>* node, const char* name, double defaultValue = 0.0)
{
    auto attr = node->first_attribute(name);
    if (!attr) return defaultValue;
    try { return std::stod(attr->value()); } catch (...) { return defaultValue; }
}


#endif // RAPIDXML_EXT_H