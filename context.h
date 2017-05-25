#pragma once

#include <QtCore/QString>
#include <QtCore/QRegExp>
#include <QtCore/QList>

#include "types.h"

/// @class Context
/// @brief Manages a collection of Notes for a given Context
///
/// Each unique context added to WindowNotes is represented by this
/// class, and it manages the database of Notes that are associated with
/// that context.

struct Context
{
public:
    QString         context_id;
    QRegExp         context_expr;
    bool            id_is_expression;

    QDomDocument*   my_doc;     // unfortunately, needed for createElement() and such
    QDomElement     my_node;

    Context(QDomDocument* doc, QDomElement node);
    ~Context();

    int     note_count() const;

    bool    match(const QString& id);   // performs regexp match if id_is_expression
    int     remove_note(QDomNode note);
    void    clear_notes();
    void    add_note(const NoteData& data);

    QDomElement get_note(int index);
    QDomElement get_node() const   { return my_node; }
    int         get_icon() const;

    bool operator==(const QString& id) const { return context_id.compare(id) == 0; }
};

typedef QList<Context*>         ContextsList;
typedef ContextsList::iterator  ContextsListIter;
