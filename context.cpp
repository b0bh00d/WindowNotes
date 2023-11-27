#include    "context.h"

Context::Context(QDomDocument* doc, QDomElement node)
    : my_doc(doc),
      my_node(node)
{
    if(!my_node.isNull() && my_node.hasAttribute("id"))
    {
        context_id = my_node.attribute("id");
        if(my_node.hasAttribute("is_regexp") && my_node.attribute("is_regexp").toInt())
        {
            id_is_expression = true;
            context_expr.setPattern(context_id);
            if(!context_expr.isValid())
                id_is_expression = false;
        }
    }
}

Context::~Context()
{
    clear_notes();
}

bool Context::match(const QString& id)
{
    if(id_is_expression)
        return (context_expr.indexIn(id) != -1);
    else
        return (QString::compare(context_id, id, Qt::CaseInsensitive) == 0);
}

int Context::note_count() const
{
    if(!my_node.isNull())
        return my_node.childNodes().length();
    return 0;
}

QDomElement Context::get_note(int index)
{
    if(my_node.isNull())
        return QDomElement();

    return my_node.childNodes().at(index).toElement();
}

int Context::get_icon() const
{
    if(my_node.isNull() || !my_node.hasAttribute("tab_icon"))
        return 0;
    return my_node.attribute("tab_icon").toInt();
}

void Context::add_note(const NoteData& data)
{
    QDomElement child = my_doc->createElement("Note");
    QDomText child_text = my_doc->createTextNode(data.content);
    child.appendChild(child_text);
    my_node.appendChild(child);
}

int Context::remove_note(QDomNode note)
{
    my_node.removeChild(note);
    return my_node.childNodes().length();
}

void Context::clear_notes()
{
    if(my_node.isNull())
        return;

    while(my_node.childNodes().length())
        my_node.removeChild(my_node.firstChild());
}
