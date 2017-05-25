#ifdef QT_WIN
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#include <QCryptographicHash>
#include <QCoreApplication>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QTreeWidget>
#include <QFile>
#include <QDir>

#include "database.h"

static Database*     db_instance = 0;
DatabaseManager* Database::manager = 0;

const int DBVersion = 1;

void Database::initialize()
{
    if(!db_instance)
    {
        db_instance = new Database();
        db_instance->manager = new DatabaseManager(qApp);
    }
}

void Database::shutdown()
{
    if(!db_instance)
        return;

    delete db_instance->manager;
    delete db_instance;
    db_instance = (Database*)0;
}

bool Database::is_valid()
{
    if(!db_instance)
        initialize();
    return db_instance->manager->db.isOpen();
}

bool Database::unlock()
{
    return true;
}

bool Database::lock()
{
    return true;
}

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent)
{
    // we defer opening/creating the database until it is first
    // accessed.  this gives us a window within which to set the
    // file path, if desired.

    database_name = QString("default_%1.sqlite").arg(DBVersion);
}

DatabaseManager::~DatabaseManager()
{
    if(db.isOpen())
        db.close();
}

DatabaseManager::Result DatabaseManager::get_result() const
{
    return last_result;
}

QString DatabaseManager::get_result_text() const
{
    return last_result_text;
}

void DatabaseManager::set_database(const QString& path)
{
    database_path = path;
    QFileInfo file_info(database_path);
    if(file_info.isDir())
    {
        database_path = QDir::toNativeSeparators(QString("%1/%2")
                        .arg(database_path)
                        .arg(database_name));
    }

    if(db.isOpen())
        db.close();
}

QString DatabaseManager::get_database()
{
    return database_path;
}

void DatabaseManager::initialize_db(QSqlDatabase& db)
{
    QSqlQuery q("", db);

    q.exec("drop table Contexts");
    q.exec("drop table Notes");

    q.exec("create table Contexts (ContextId integer primary key, "
                                   "Context varchar, "
                                   "IsRegexp integer)");

    q.exec("create table Notes (ContextId integer, "
                                "Contents varchar, "
                                "UseMask integer, "
                                "Mask varchar)");
}

bool DatabaseManager::open(bool create_if_necessary)
{
//#ifdef Q_OS_LINUX
//    // NOTE: We have to store database file into user home folder in Linux
//    QString path(QDir::home().path());
//    path.append(QDir::separator()).append("my.db.sqlite");
//    path = QDir::toNativeSeparators(path);
//    database_name = path;
//#else
//    // NOTE: File exists in the application private folder, in Symbian Qt implementation
//    //database_name = "ht.sqlite";
//#endif

    bool init_database = false;
    if(database_path == ":memory:")
        init_database = true;
    else
    {
        if(database_path.isEmpty())
            database_path = QDir::toNativeSeparators(QString("%1/%2")
                            .arg(Options::get("settings_path").toString())
                            .arg(database_name));
        //else if(!QFile::exists(database_path))
        //    database_path = QDir::toNativeSeparators(QString("%1%2%3")
        //                    .arg(database_path)
        //                    .arg(QDir::separator())
        //                    .arg(database_name));

        init_database = !QFile::exists(database_path);
        if(init_database && !create_if_necessary)
        {
            last_result = Result_MissingFile;
            last_result_text.clear();
            return false;
        }

    // Find QSLite driver
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(database_path);
    if(!db.open())
    {
        last_result = Result_CannotOpen;
        last_result_text = db.lastError().text();
        return false;
        //QString message = QString(tr("An error occurred while opening the connection: %1")).arg(db.lastError().text());
        //QMessageBox::warning(0, tr("Unable to open database"), message);
    }

    if(init_database)
    {
        initialize_db(db);
        next_context_id = 1;
    }
    else
    {
        QSqlQuery q("", db);
        q.exec("select max(ContextId) from Contexts");
        while(q.next())
            next_context_id = q.value(0).toInt();
        ++next_context_id;
    }

    last_result = Result_Ok;
    last_result_text.clear();

    return true;
}

QSqlError DatabaseManager::lastError()
{
    return db.lastError();
}

//QString DatabaseManager::id_to_string(unsigned id)
//{
//    return QString("%1").arg(id, IdFieldWidth, 10, QChar('0'));
//}

bool DatabaseManager::remove()
{
    if(db.isOpen())
        db.close();

    if(database_path != ":memory:")
        return QFile::remove(database_path);

    QSqlDatabase::removeDatabase("QSQLITE");

    last_result = Result_Ok;
    last_result_text.clear();

    return true;
}

bool DatabaseManager::compact()
{
    if(!db.isOpen())
    {
        if(!open())
            return false;
    }

    QSqlQuery query(db);
    query.prepare("VACUUM;");
    if(!query.exec())
    {
        last_result = Result_OperationError;
        last_result_text = db.lastError().text();
        //QString message = QString(tr("An error occurred while querying the database: %1")).arg(db.lastError().text());
        //QMessageBox::warning(0, tr("Unable to compact database"), message);
        return false;
    }

    last_result = Result_Ok;
    last_result_text.clear();

    return true;
}

// problem domain

void DatabaseManager::get_notes(const DBContext& dbcontext, Context* context)
{
    QString qstring = "SELECT * FROM Notes where ContextId=:id";

    QSqlQuery query(db);
    query.prepare(qstring);
    query.bindValue(":id", dbcontext.context_id);

    query.prepare();
    if(!query.exec())
    {
        last_result = Result_QueryError;
        last_result_text = db.lastError().text();
        //QString message = QString(tr("An error occurred while querying the database: %1")).arg(db.lastError().text());
        //QMessageBox::warning(0, tr("Unable to retrieve records"), message);
        return false;
    }

    while(query.next())
    {
        QString contents = query.value(query.record().indexOf("Content")).toString();
        QString mask = query.value(query.record().indexOf("Mask")).toString();
        bool use_mask = query.value(query.record().indexOf("UseMask")).toUInt() != 0;

        context->notes.push_back(new Note(contents, use_mask, mask));
    }
}

void DatabaseManager::get_context(QSqlQuery& query, DBContext& dbcontext)
{
    dbcontext.context_id = query.value(query.record().indexOf("ContextId")).toUInt();
    dbcontext.context.context_id = query.value(query.record().indexOf("Context")).toString();
    dbcontext.context.id_is_expression = query.value(query.record().indexOf("IsRegexp")).toUInt();
}

bool DatabaseManager::get_contexts(ContextsList& list)
{
    list.clear();

    if(!db.isOpen())
    {
        if(!open())
            return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT * FROM Contexts");
    if(!query.exec())
    {
        last_result = Result_QueryError;
        last_result_text = db.lastError().text();
        //QString message = QString(tr("An error occurred while querying the database: %1")).arg(db.lastError().text());
        //QMessageBox::warning(0, tr("Unable to retrieve records"), message);
        return false;
    }

    while(query.next())
    {
        dbcontext c;
        get_context(query, c);

        list.push_back(new Context(dbcontext.context.context_id, dbcontext.context.id_is_expression))
        Context* context = list.back();

        get_notes(dbcontext, context);
    }

    last_result = Result_Ok;
    last_result_text.clear();

    return true;
}

bool DatabaseManager::insert_employee(QSqlQuery& query, const Employee& e)
{
    query.prepare("insert into Employees "
        "(EmployeeId, "
         "First, "
         "Last, "
         "Middle, "
         "Sex, "
         "DOB, "
         "Address1, "
         "Address2, "
         "City, "
         "State, "
         "Zip, "
         "Phone1, "
         "Phone1Type, "
         "Phone2, "
         "Phone2Type, "
         "SSN, "

         "DOE, "
         "DOT, "
         "Manager, "
         "PayRate, "
         "PayInterval, "
         "Password, "

         "Avatar, "

         "OnTheClock, "
         "ClockedIn, "

         "VacationTime, "
         "SickTime)"

        " values ("

        ":id, "
        ":first, "
        ":last, "
        ":middle, "
        ":sex, "
        ":dob, "
        ":address1, "
        ":address2, "
        ":city, "
        ":state, "
        ":zip, "
        ":phone1, "
        ":phone1type, "
        ":phone2, "
        ":phone2type, "
        ":ssn, "

        ":doe, "
        ":dot, "
        ":manager, "
        ":payrate, "
        ":payinterval, "
        ":password, "

        ":avatar, "

        ":ontheclock, "
        ":clockedin, "

        ":vacationtime, "
        ":sicktime)");

    query.bindValue(":id", e.id);
    query.bindValue(":first", e.first_name);
    query.bindValue(":last", e.last_name);
    query.bindValue(":middle", e.middle_initial);
    query.bindValue(":sex", static_cast<int>(e.sex));
    query.bindValue(":dob", e.DOB.toString());
    query.bindValue(":address1", e.address1);
    query.bindValue(":address2", e.address2);
    query.bindValue(":city", e.city);
    query.bindValue(":state", e.state);
    query.bindValue(":zip", e.zip);
    query.bindValue(":phone1", e.phone1);
    query.bindValue(":phone1type", static_cast<int>(e.phone1_type));
    query.bindValue(":phone2", e.phone2);
    query.bindValue(":phone2type", static_cast<int>(e.phone2_type));
    query.bindValue(":ssn", e.SSN);

    query.bindValue(":doe", e.DOE.toString());
    query.bindValue(":dot", e.DOT.toString());
    query.bindValue(":manager", e.manager);
    query.bindValue(":payrate", e.pay_rate);
    query.bindValue(":payinterval", static_cast<int>(e.pay_interval));
    query.bindValue(":password", e.password);

    query.bindValue(":avatar", e.avatar_file);

    query.bindValue(":ontheclock", e.on_the_clock ? 1 : 0);
    query.bindValue(":clockedin", e.clocked_in.toString());

    query.bindValue(":vacationtime", (int)e.vacation_time);
    query.bindValue(":sicktime", (int)e.sick_time);

    return query.exec();
}

bool DatabaseManager::add_employee(const Employee& e)
{
    if(!db.isOpen())
    {
        if(!open())
            return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT * FROM Employees where EmployeeId=:id");
    query.bindValue(":id", e.id);
    if(!query.exec())
    {
        last_result = Result_QueryError;
        last_result_text = db.lastError().text();
        //QString message = QString(tr("An error occurred while querying the database: %1")).arg(db.lastError().text());
        //QMessageBox::warning(0, tr("Unable to retrieve records"), message);
        return false;
    }

    if(query.next())
        return update_employee(e);

    if(!insert_employee(query, e))
    {
        last_result = Result_QueryError;
        last_result_text = db.lastError().text();
        //QString message = QString(tr("An error occurred while updating the database: %1")).arg(db.lastError().text());
        //QMessageBox::warning(0, tr("Unable to add record"), message);
        return false;
    }

    query.exec("select max(EmployeeId) from Employees");
    while(query.next())
        next_context_id = query.value(0).toInt();
    ++next_context_id;

    last_result = Result_Ok;
    last_result_text.clear();

    return true;
}

bool DatabaseManager::archive_employee(const Employee& e)
{
    if(!db.isOpen())
    {
        if(!open())
            return false;
    }

    QString archive_name = QString("%1_%2.sqlite").arg(e.full_name()).arg(DBVersion);
    QString archive_path = Options::get("employee_archive_folder").toString();

    if(!QFile::exists(archive_path))
    {
        int result = 0;
#ifdef QT_WIN
        result = _mkdir(archive_path.toAscii().constData());
#else
        result = mkdir(archive_path.toAscii().constData(), S_IRWXU);
#endif
        if(result)
        {
            last_result = Result_OperationError;
            last_result_text = QString(tr("Failed to create archive folder:\n%1")).arg(archive_path);
            //QString message = QString(tr("Failed to create archive folder:\n%1")).arg(archive_path);
            //QMessageBox::warning(0, tr("Unable to remove employee"), message);
            return false;
        }
    }

    QString archive_db_file = QDir::toNativeSeparators(QString("%1/%2")
                                    .arg(QDir::tempPath())
                                    .arg(archive_name));

    QSqlDatabase archive_db = QSqlDatabase::addDatabase("QSQLITE", e.full_name());
    archive_db.setDatabaseName(archive_db_file);
    if(!archive_db.open())
    {
        last_result = Result_CannotOpen;
        last_result_text = db.lastError().text();
        //QString message = QString(tr("An error occurred while opening the connection: %1")).arg(db.lastError().text());
        //QMessageBox::warning(0, tr("Unable to create database"), message);
        return false;
    }

    initialize_db(archive_db);

    {
        QSqlQuery archive_query(archive_db);
        if(!insert_employee(archive_query, e))
        {
            last_result = Result_QueryError;
            last_result_text = db.lastError().text();
            //QString message = QString(tr("An error occurred while updating the database: %1")).arg(db.lastError().text());
            //QMessageBox::warning(0, tr("Unable to add record"), message);
            return false;
        }
    }

    IntervalList i;

    // scheduled intervals will be useless if we are ever restored
    //if(!get_hours_scheduled(e.id, i))
    //    return false;
    //for(int j = 0;j < (int)i.size();j++)
    //{
    //    QSqlQuery archive_query(archive_db);
    //    if(!insert_scheduled(archive_query, e.id, i[j]))
    //    {
    //        QString message = QString(tr("An error occurred while updating the database: %1")).arg(db.lastError().text());
    //        QMessageBox::warning(0, tr("Unable to add record"), message);
    //        return false;
    //    }
    //}

    if(!get_hours_completed(e.id, i))
        return false;
    for(int j = 0;j < (int)i.size();j++)
    {
        QSqlQuery archive_query(archive_db);
        if(!insert_completed(archive_query, e.id, i[j]))
        {
            last_result = Result_QueryError;
            last_result_text = db.lastError().text();
            //QString message = QString(tr("An error occurred while updating the database: %1")).arg(db.lastError().text());
            //QMessageBox::warning(0, tr("Unable to add record"), message);
            return false;
        }
    }

    archive_db.close();
    QSqlDatabase::removeDatabase(e.full_name());

    QString avatar_file_name;
    QByteArray ba_archive_file_data;
    QByteArray ba_avatar_file_data;

    {
        QFile f(archive_db_file);
        if(!f.open(QIODevice::ReadOnly))
        {
            last_result = Result_CannotOpen;
            last_result_text = QString(tr("Failed to open file for reading: %1")).arg(archive_db_file);
            //QString message = QString(tr("An error occurred while updating the database: %1")).arg(db.lastError().text());
            //QMessageBox::warning(0, tr("Unable to add record"), message);
            return false;
        }

        QByteArray local = f.readAll();
        f.close();
        ba_archive_file_data = qCompress(local, -1);
    }

    // is there a valid avatar?  pull it in, if so...
    if(e.avatar)
    {
        QString avatar_path = QDir::toNativeSeparators(QString("%1/%2").arg(Options::get("avatar_path").toString()).arg(e.avatar_file));
        avatar_file_name = e.avatar_file;

        QFile f(avatar_path);
        if(!f.open(QIODevice::ReadOnly))
        {
            last_result = Result_CannotOpen;
            last_result_text = QString(tr("Failed to open file for reading: %1")).arg(e.avatar_file);
            //QString message = QString(tr("An error occurred while updating the database: %1")).arg(db.lastError().text());
            //QMessageBox::warning(0, tr("Unable to add record"), message);
            return false;
        }

        QByteArray local = f.readAll();
        f.close();
        ba_avatar_file_data = qCompress(local, -1);

        QFile::remove(avatar_path);
    }

    {
        QString target = QDir::toNativeSeparators(QString("%1/%2.bin")
                                    .arg(archive_path)
                                    .arg(e.full_name()));

        QFile f(target);
        if(!f.open(QIODevice::WriteOnly))
        {
            last_result = Result_CannotOpen;
            last_result_text = QString(tr("Failed to open file for writing: %1")).arg(target);
            //QString message = QString(tr("An error occurred while updating the database: %1")).arg(db.lastError().text());
            //QMessageBox::warning(0, tr("Unable to add record"), message);
            return false;
        }

        QDataStream stream(&f);

        stream << DBVersion;
        stream << ba_archive_file_data;
        stream << avatar_file_name;
        stream << ba_avatar_file_data;

        f.close();
    }

    QFile::remove(archive_db_file);

    last_result = Result_Ok;
    last_result_text.clear();

    return true;
}

bool DatabaseManager::restore_employee(const QString& name, Employee& e)
{
    if(!db.isOpen())
    {
        if(!open())
            return false;
    }

    QString bin_path = QDir::toNativeSeparators(QString("%1/%2.bin")
                                .arg(Options::get("employee_archive_folder").toString())
                                .arg(name));

    int db_version;
    QString avatar_file_name;
    QByteArray ba_archive_file_data;
    QByteArray ba_avatar_file_data;

    {
        QFile f(bin_path);
        if(!f.open(QIODevice::ReadOnly))
        {
            last_result = Result_CannotOpen;
            last_result_text = QString(tr("Failed to open file for reading: %1")).arg(bin_path);
            //QString message = QString(tr("An error occurred while recovering employee: %1")).arg(name);
            //QMessageBox::warning(0, tr("Unable to open archive"), message);
            return false;
        }

        QDataStream ds(&f);

        ds >> db_version;
        ds >> ba_archive_file_data;
        ds >> avatar_file_name;
        ds >> ba_avatar_file_data;

        f.close();
    }

    QString archive_name = QString("%1_%2.sqlite").arg(name).arg(db_version);
    QString archive_db_file = QDir::toNativeSeparators(QString("%1/%2")
                                    .arg(QDir::tempPath())
                                    .arg(archive_name));

    {
        QFile f(archive_db_file);
        if(!f.open(QIODevice::WriteOnly))
        {
            last_result = Result_CannotOpen;
            last_result_text = QString(tr("Failed to open file for reading: %1")).arg(archive_db_file);
            //QString message = QString(tr("An error occurred while recovering employee: %1")).arg(name);
            //QMessageBox::warning(0, tr("Unable to restore database"), message);
            return false;
        }

        QByteArray local = qUncompress(ba_archive_file_data);
        f.write(local.constData(), local.size());
        f.close();
    }

    QSqlDatabase archive_db = QSqlDatabase::addDatabase("QSQLITE", name);
    archive_db.setDatabaseName(archive_db_file);
    if(!archive_db.open())
    {
        last_result = Result_CannotOpen;
        last_result_text = db.lastError().text();
        //QString message = QString(tr("An error occurred while opening the connection: %1")).arg(db.lastError().text());
        //QMessageBox::warning(0, tr("Unable to open database"), message);
        QSqlDatabase::removeDatabase(name);
        return false;
    }

    {
        QSqlQuery archive_query(archive_db);
        archive_query.prepare("SELECT * FROM Employees");
        if(!archive_query.exec())
        {
            last_result = Result_QueryError;
            last_result_text = db.lastError().text();
            //QString message = QString(tr("An error occurred while querying the database: %1")).arg(db.lastError().text());
            //QMessageBox::warning(0, tr("Unable to query database"), message);
            archive_db.close();
            QSqlDatabase::removeDatabase(name);
            return false;
        }

        if(!archive_query.next())
        {
            last_result = Result_QueryError;
            last_result_text = db.lastError().text();
            //QString message = QString(tr("An error occurred while querying the database: %1")).arg(db.lastError().text());
            //QMessageBox::warning(0, tr("Unable to query database"), message);
            archive_db.close();
            QSqlDatabase::removeDatabase(name);
            return false;
        }

        get_employee(archive_query, e);
    }

    QString avatar_file = QDir::toNativeSeparators(QString("%1/%2")
                                    .arg(Options::get("avatar_folder").toString())
                                    .arg(avatar_file_name));

    if(!QFile::exists(avatar_file))
    {
        QFile f(avatar_file);
        if(!f.open(QIODevice::WriteOnly))
        {
            last_result = Result_CannotOpen;
            last_result_text = QString(tr("Failed to open file for writing: %1")).arg(avatar_file);
            //QString message = QString(tr("An error occurred while recovering employee: %1")).arg(name);
            //QMessageBox::warning(0, tr("Unable to restore database"), message);
            return false;
        }

        QByteArray local = qUncompress(ba_avatar_file_data);
        f.write(local.constData(), local.size());
        f.close();

        e.avatar_file = avatar_file_name;
    }

    {
        QSqlQuery query(db);
        query.prepare("SELECT * FROM Employees where EmployeeId=:id");
        query.bindValue(":id", e.id);
        if(!query.exec())
        {
            last_result = Result_QueryError;
            last_result_text = db.lastError().text();
            //QString message = QString(tr("An error occurred while querying the database: %1")).arg(db.lastError().text());
            //QMessageBox::warning(0, tr("Unable to retrieve records"), message);
            return false;
        }

        if(query.next())
            e.id = next_context_id++;
    }

    if(!add_employee(e))
    {
        archive_db.close();
        QSqlDatabase::removeDatabase(name);
        return false;
    }

    {
        QSqlQuery archive_query(archive_db);
        archive_query.prepare("SELECT * FROM HoursCompleted");
        if(!archive_query.exec())
        {
            last_result = Result_QueryError;
            last_result_text = db.lastError().text();
            //QString message = QString(tr("An error occurred while querying the database: %1")).arg(db.lastError().text());
            //QMessageBox::warning(0, tr("Unable to query database"), message);
            archive_db.close();
            QSqlDatabase::removeDatabase(name);
            return false;
        }

        IntervalList list;

        while(archive_query.next())
        {
            QDate in_date = QDate::fromJulianDay(archive_query.value(archive_query.record().indexOf("InDate")).toUInt());
            unsigned t = archive_query.value(archive_query.record().indexOf("InTime")).toUInt();
            QTime in_time((t / 60), t - ((t / 60) * 60));
            QDate out_date = QDate::fromJulianDay(archive_query.value(archive_query.record().indexOf("OutDate")).toUInt());
            t = archive_query.value(archive_query.record().indexOf("OutTime")).toUInt();
            QTime out_time((t / 60), t - ((t / 60) * 60));

            TimeInterval i;
            i.start = QDateTime(in_date, in_time);
            i.end = QDateTime(out_date, out_time);
            i.type = archive_query.value(archive_query.record().indexOf("TimeType")).toInt();

            list.push_back(i);
        }

        QSqlQuery query(db);
        for(int j = 0;j < (int)list.size();j++)
        {
            if(!insert_completed(query, e.id, list[j]))
            {
                last_result = Result_QueryError;
                last_result_text = db.lastError().text();
                //QString message = QString(tr("An error occurred while updating the database: %1")).arg(db.lastError().text());
                //QMessageBox::warning(0, tr("Unable to restore records"), message);
                return false;
            }
        }
    }

    archive_db.close();
    QSqlDatabase::removeDatabase(name);

    QFile::remove(archive_db_file);
    QFile::remove(bin_path);

    last_result = Result_Ok;
    last_result_text.clear();

    return true;
}

bool DatabaseManager::delete_employee(const Employee& e)
{
    if(!db.isOpen())
    {
        if(!open())
            return false;
    }

    if(!archive_employee(e))
        return false;

    QSqlQuery query(db);
    query.prepare("delete from Employees where EmployeeId=:id");
    query.bindValue(0, e.id);
    if(!query.exec())
    {
        last_result = Result_QueryError;
        last_result_text = db.lastError().text();
        //QString message = QString(tr("An error occurred while updating the database: %1")).arg(db.lastError().text());
        //QMessageBox::warning(0, tr("Unable to remove record"), message);
        return false;
    }

    if(!delete_hours_scheduled(e.id))
        return false;
    if(!delete_hours_scheduled(e.id))
        return false;

    last_result = Result_Ok;
    last_result_text.clear();

    return true;
}

bool DatabaseManager::update_employee(const Employee& e)
{
    if(!db.isOpen())
    {
        if(!open())
            return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT * FROM Employees where EmployeeId=:id");
    query.bindValue(":id", e.id);
    if(!query.exec())
    {
        last_result = Result_QueryError;
        last_result_text = db.lastError().text();
        //QString message = QString(tr("An error occurred while querying the database: %1")).arg(db.lastError().text());
        //QMessageBox::warning(0, tr("Unable to retrieve records"), message);
        return false;
    }

    if(!query.next())
        return add_employee(e);

    query.prepare("update Employees set "
                  "First=:first, "
                  "Last=:last, "
                  "Middle=:middle, "
                  "Sex=:sex, "
                  "DOB=:dob, "
                  "Address1=:address1, "
                  "Address2=:address2, "
                  "City=:city, "
                  "State=:state, "
                  "Zip=:zip, "
                  "Phone1=:phone1, "
                  "Phone1Type=:phone1type, "
                  "Phone2=:phone2, "
                  "Phone2Type=:phone2type, "
                  "SSN=:ssn, "

                  "DOE=:doe, "
                  "DOT=:dot, "
                  "Manager=:manager, "
                  "PayRate=:payrate, "
                  "PayInterval=:payinterval, "
                  "Password=:password, "

                  "Avatar=:avatar, "

                  "OnTheClock=:ontheclock, "
                  "ClockedIn=:clockedin, "

                  "VacationTime=:vacationtime, "
                  "SickTime=:sicktime "

                  "where EmployeeId=:id");

    query.bindValue(":first", e.first_name);
    query.bindValue(":last", e.last_name);
    query.bindValue(":middle", e.middle_initial);
    query.bindValue(":sex", static_cast<int>(e.sex));
    query.bindValue(":dob", e.DOB.toString());
    query.bindValue(":address1", e.address1);
    query.bindValue(":address2", e.address2);
    query.bindValue(":city", e.city);
    query.bindValue(":state", e.state);
    query.bindValue(":zip", e.zip);
    query.bindValue(":phone1", e.phone1);
    query.bindValue(":phone1type", static_cast<int>(e.phone1_type));
    query.bindValue(":phone2", e.phone2);
    query.bindValue(":phone2type", static_cast<int>(e.phone2_type));
    query.bindValue(":ssn", e.SSN);

    query.bindValue(":doe", e.DOE.toString());
    query.bindValue(":dot", e.DOT.toString());
    query.bindValue(":manager", e.manager);
    query.bindValue(":payrate", e.pay_rate);
    query.bindValue(":payinterval", static_cast<int>(e.pay_interval));
    query.bindValue(":password", e.password);

    query.bindValue(":avatar", e.avatar_file);

    query.bindValue(":ontheclock", e.on_the_clock ? 1 : 0);
    query.bindValue(":clockedin", e.clocked_in.toString());

    query.bindValue(":vacationtime", (unsigned)e.vacation_time);
    query.bindValue(":sicktime", (unsigned)e.sick_time);

    query.bindValue(":id", e.id);

    if(!query.exec())
    {
        last_result = Result_QueryError;
        last_result_text = db.lastError().text();
        //QSqlError::ErrorType type = db.lastError().type();
        //QString message = QString(tr("An error occurred while updating the database: %1")).arg(db.lastError().text());
        //QMessageBox::warning(0, tr("Unable to update record"), message);
        return false;
    }

    last_result = Result_Ok;
    last_result_text.clear();

    return true;
}

bool DatabaseManager::insert_scheduled(QSqlQuery& query, unsigned id, const TimeInterval& i)
{
    QDate in_date = i.start.date();
    QTime in_time = i.start.time();

    QDate out_date = i.end.date();
    QTime out_time = i.end.time();

    query.prepare("insert into HoursScheduled (EmployeeId, InDate, InTime, OutDate, OutTime, TimeType) "
                    "values (:id, :indate, :intime, :outdate, :outtime, :timetype)");
    query.bindValue(":id", id);
    query.bindValue(":indate", in_date.toJulianDay());
    query.bindValue(":intime", in_time.hour() * 60 + in_time.minute());
    query.bindValue(":outdate", out_date.toJulianDay());
    query.bindValue(":outtime", out_time.hour() * 60 + out_time.minute());
    query.bindValue(":timetype", i.type);

    return query.exec();
}

bool DatabaseManager::add_hours_scheduled(unsigned id, const TimeInterval& i)
{
    if(!db.isOpen())
    {
        if(!open())
            return false;
    }

    QDate in_date = i.start.date();
    QTime in_time = i.start.time();

    QDate out_date = i.end.date();
    QTime out_time = i.end.time();

    QSqlQuery query(db);

    query.prepare("select count(*) from HoursScheduled where EmployeeId=:id and InDate=:indate");
    query.bindValue(":id", id);
    query.bindValue(":indate", in_date.toJulianDay());
    if(!query.exec())
    {
        last_result = Result_QueryError;
        last_result_text = db.lastError().text();
        //QString message = QString(tr("An error occurred while updating the database: %1")).arg(db.lastError().text());
        //QMessageBox::warning(0, tr("Unable to remove records"), message);
        return false;
    }

    query.next();
    if(query.value(0).toUInt())
    {
        query.prepare("delete from HoursScheduled where EmployeeId=:id and InDate=:indate");
        query.bindValue(":id", id);
        query.bindValue(":indate", in_date.toJulianDay());
        if(!query.exec())
        {
            last_result = Result_QueryError;
            last_result_text = db.lastError().text();
            //QString message = QString(tr("An error occurred while updating the database: %1")).arg(db.lastError().text());
            //QMessageBox::warning(0, tr("Unable to remove records"), message);
            return false;
        }
    }

    if(!insert_scheduled(query, id, i))
    {
        //QString message = QString(tr("An error occurred while updating the database: %1")).arg(db.lastError().text());
        //QMessageBox::warning(0, tr("Unable to add records"), message);
        return false;
    }

    last_result = Result_Ok;
    last_result_text.clear();

    return true;
}

bool DatabaseManager::delete_hours_scheduled(unsigned id, const QDate& on)
{
    if(!db.isOpen())
    {
        if(!open())
            return false;
    }

    QSqlQuery query(db);
    if(on.isNull())
        query.prepare("delete from HoursScheduled where EmployeeId=:id");
    else
    {
        query.prepare("delete from HoursScheduled where EmployeeId=:id and InDate=:indate");
        query.bindValue(1, on.toJulianDay());
    }
    query.bindValue(0, id);
    if(!query.exec())
    {
        last_result = Result_QueryError;
        last_result_text = db.lastError().text();
        //QString message = QString(tr("An error occurred while updating the database: %1")).arg(db.lastError().text());
        //QMessageBox::warning(0, tr("Unable to remove records"), message);
        return false;
    }

    last_result = Result_Ok;
    last_result_text.clear();

    return true;
}

bool DatabaseManager::update_hours_scheduled(unsigned id, const IntervalList& i)
{
    if(!db.isOpen())
    {
        if(!open())
            return false;
    }

    if(!delete_hours_scheduled(id))
        return false;

    QSqlQuery query(db);
    for(IntervalListConstIter iter = i.begin();iter != i.end();iter++)
        if(!add_hours_scheduled(id, *iter))
            return false;

    last_result = Result_Ok;
    last_result_text.clear();

    return true;
}

bool DatabaseManager::insert_completed(QSqlQuery& query, unsigned id, const TimeInterval& i)
{
    QDate in_date = i.start.date();
    QTime in_time = i.start.time();

    QDate out_date = i.end.date();
    QTime out_time = i.end.time();

    query.prepare("insert into HoursCompleted (EmployeeId, InDate, InTime, OutDate, OutTime, TimeType) "
                    "values (:id, :indate, :intime, :outdate, :outtime, :timetype)");
    query.bindValue(":id", id);
    query.bindValue(":indate", in_date.toJulianDay());
    query.bindValue(":intime", in_time.hour() * 60 + in_time.minute());
    query.bindValue(":outdate", out_date.toJulianDay());
    query.bindValue(":outtime", out_time.hour() * 60 + out_time.minute());
    query.bindValue(":timetype", i.type);

    return query.exec();
}

bool DatabaseManager::add_hours_completed(unsigned id, const TimeInterval& i)
{
    if(!db.isOpen())
    {
        if(!open())
            return false;
    }

    //QDate in_date = i.start.date();
    //QTime in_time = i.start.time();

    //QDate out_date = i.end.date();
    //QTime out_time = i.end.time();

    QSqlQuery query(db);
    if(!insert_completed(query, id, i))
    {
        last_result = Result_QueryError;
        last_result_text = db.lastError().text();
        //QString message = QString(tr("An error occurred while updating the database: %1")).arg(db.lastError().text());
        //QMessageBox::warning(0, tr("Unable to add records"), message);
        return false;
    }

    last_result = Result_Ok;
    last_result_text.clear();

    return true;
}

bool DatabaseManager::delete_hours_completed(unsigned id, const QDate& on)
{
    if(!db.isOpen())
    {
        if(!open())
            return false;
    }

    QSqlQuery query(db);
    if(on.isNull())
        query.prepare("delete from HoursCompleted where EmployeeId=:id");
    else
    {
        query.prepare("delete from HoursCompleted where EmployeeId=:id and InDate=:indate");
        query.bindValue(1, on.toJulianDay());
    }
    query.bindValue(0, id);
    if(!query.exec())
    {
        last_result = Result_QueryError;
        last_result_text = db.lastError().text();
        //QString message = QString(tr("An error occurred while updating the database: %1")).arg(db.lastError().text());
        //QMessageBox::warning(0, tr("Unable to remove records"), message);
        return false;
    }

    last_result = Result_Ok;
    last_result_text.clear();

    return true;
}

bool DatabaseManager::update_hours_completed(unsigned id, const IntervalList& i)
{
    if(!db.isOpen())
    {
        if(!open())
            return false;
    }

    if(!delete_hours_completed(id))
        return false;

    QSqlQuery query(db);
    for(IntervalListConstIter iter = i.begin();iter != i.end();iter++)
        if(!add_hours_completed(id, *iter))
            return false;

    last_result = Result_Ok;
    last_result_text.clear();

    return true;
}

bool DatabaseManager::update_hours_completed(unsigned id, const TimeInterval& old_entry, const TimeInterval& new_entry)
{
    if(!db.isOpen())
    {
        if(!open())
            return false;
    }

    QSqlQuery query(db);
    query.prepare("delete from HoursCompleted where EmployeeId=:id and InDate=:indate and InTime=:intime");
    query.bindValue(":id", id);
    query.bindValue(":indate", old_entry.start.date().toJulianDay());
    query.bindValue(":intime", old_entry.start.time().hour() * 60 + old_entry.start.time().minute());
    if(!query.exec())
    {
        last_result = Result_QueryError;
        last_result_text = db.lastError().text();
        //QString message = QString(tr("An error occurred while updating the database: %1")).arg(db.lastError().text());
        //QMessageBox::warning(0, tr("Unable to remove records"), message);
        return false;
    }

    return add_hours_completed(id, new_entry);
}
