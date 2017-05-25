#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QDate>
#include <QString>
#include <QSqlDatabase>
#include <QSqlError>

#include "context.h"

const int IdFieldWidth = 5;

class DBContext
{
    int context_id;
    Context context;
};

class DatabaseManager : public QObject
{
public:
    typedef enum
    {
        Result_Ok,
        Result_MissingFile,
        Result_CannotOpen,
        Result_OperationError,
        Result_QueryError,
    } Result;

public:
    Result          get_result() const;
    QString         get_result_text() const;

    void            set_database(const QString& path);
    QString         get_database();

    bool            open(bool create_if_necessary=false);
    bool            remove();

    bool            compact();

    QSqlError       lastError();

    unsigned        next_id() const { return next_employee_id; }
    //QString         id_to_string(unsigned id);

    // problem domain
    bool            get_employees(EmployeesMap& list);
    bool            get_hours_scheduled(unsigned id, IntervalList& list, const QDateTime& from = QDateTime(), const QDateTime& to = QDateTime());
    bool            get_hours_completed(unsigned id, IntervalList& list, const QDateTime& from = QDateTime(), const QDateTime& to = QDateTime());

    bool            add_employee(const Employee& e);
    bool            delete_employee(const Employee& e);
    bool            restore_employee(const QString& name, Employee& e);
    bool            update_employee(const Employee& e);

    bool            add_hours_scheduled(unsigned id, const TimeInterval& i);
    bool            delete_hours_scheduled(unsigned id, const QDate& on = QDate());
    bool            update_hours_scheduled(unsigned id, const IntervalList& i);

    bool            add_hours_completed(unsigned id, const TimeInterval& i);
    bool            delete_hours_completed(unsigned id, const QDate& on = QDate());
    bool            update_hours_completed(unsigned id, const IntervalList& i);
    bool            update_hours_completed(unsigned id, const TimeInterval& old_entry, const TimeInterval& new_entry);

private:
    DatabaseManager(QObject *parent = 0);
    ~DatabaseManager();

    void            initialize_db(QSqlDatabase& db);

    // problem domain
    bool            archive_employee(const Employee& e);
    void            get_employee(QSqlQuery& query, Employee& e);
    bool            insert_employee(QSqlQuery& query, const Employee& e);
    bool            insert_completed(QSqlQuery& query, unsigned id, const TimeInterval& i);
    bool            insert_scheduled(QSqlQuery& query, unsigned id, const TimeInterval& i);

    friend class Database;

private:
    QSqlDatabase    db;
    QString         database_path;
    QString         database_name;

    Result          last_result;
    QString         last_result_text;

    // problem domain
    unsigned        next_context_id;
};

class Database
{
public:
    static  void        initialize();
    static  void        shutdown();
    static  bool        is_valid();

    static  bool        unlock();
    static  bool        lock();

    static  DatabaseManager* db() { return manager; }

private:
    static  DatabaseManager* manager;
};

#endif      // DATABASE_H
