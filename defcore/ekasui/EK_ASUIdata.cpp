#include "EK_ASUIdata.h"

EK_ASUI::EK_ASUI(const QString& WorkDir, bool LoadAllData)
{
    /*
        QSqlQuery * query;
        bool ErrorFlag = false;

        DB =  QSqlDatabase::addDatabase(DriverType,ConnectionName);
        DB.setDatabaseName("c:/temp/a" / * DBLocalPath* / );

        bool DBOpened = DB.open();
        if (DBOpened == false)
        {
            QSqlError err =  DB.lastError();
    //        sqlErrorToStr(pErrorStr,&err);
            DB.removeDatabase(ConnectionName);
            qDebug() << err;
        }
        else
        {
            query = new QSqlQuery(DB);

            QStringList tbls = DB.tables();
            QString query_text;

            if (tbls.indexOf("info") == - 1) {

                // Общая информация -------------------------------------------------------------------
                // [Таблица: info]
                // [id] - INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE
                // [db_version] - INTEGER
                // [NSIversion] - VARCHAR(50)
                // [carID] - VARCHAR(150)

                query_text.append("CREATE TABLE info (");
                query_text.append("\"id\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, ");
                query_text.append("\"db_version\" INTEGER, ");
                query_text.append("\"NSIversion\" VARCHAR(50), ");
                query_text.append("\"carID\" VARCHAR(150)");
                query_text.append(");");

                query->clear();
                query->prepare(query_text);
                if (!query->exec()) {

                    ErrorFlag = true;
                    return;
                }

                // Дороги -------------------------------------------------------------------
                // [Таблица: RailRoads]
                // [id] - INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE
                // [RRid] - INTEGER: Идентификатор дороги
                // [NAME] - VARCHAR(50): Название дороги

                query_text.clear();
                query_text.append("CREATE TABLE RailRoads (");
                query_text.append("\"id\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, ");
                query_text.append("\"RRid\" INTEGER, ");
                query_text.append("\"NAME\" VARCHAR(50)");
                query_text.append(");");

                query->clear();
                query->prepare(query_text);
                if (!query->exec()) {

                    ErrorFlag = true;
                    return;
                }

                // Предприятия -------------------------------------------------------------------
                // [Таблица: PREDs]
                // [id] - INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE
                // [RRid] - INTEGER: Идентификатор дороги
                // [PREDid] - VARCHAR(50): Идентификатор ПЧ
                // [SNAME] - VARCHAR(50): Короткое название

                query_text.clear();
                query_text.append("CREATE TABLE PREDs (");
                query_text.append("\"id\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, ");
                query_text.append("\"RRid\" INTEGER, ");
                query_text.append("\"PREDid\" VARCHAR(50), ");
                query_text.append("\"SNAME\" VARCHAR(50)");
                query_text.append(");");

                query->clear();
                query->prepare(query_text);
                if (!query->exec()) {

                    ErrorFlag = true;
                    return;
                }

                // Направления -------------------------------------------------------------------
                // [Таблица: DIRs]
                // [id] - INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE
                // [RRid] - INTEGER: Идентификатор дороги
                // [PREDid] - VARCHAR(50): Идентификатор ПЧ
                // [DIRid] - VARCHAR(50): Идентификатор направления
                // [NAME] - VARCHAR(50): Название направления

                query_text.clear();
                query_text.append("CREATE TABLE DIRs (");
                query_text.append("\"id\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, ");
                query_text.append("\"RRid\" INTEGER, ");
                query_text.append("\"PREDid\" VARCHAR(50), ");
                query_text.append("\"DIRid\" VARCHAR(50), ");
                query_text.append("\"NAME\" VARCHAR(50)");
                query_text.append(");");

                query->clear();
                query->prepare(query_text);
                if (!query->exec()) {

                    ErrorFlag = true;
                    return;
                }

                // Главные пути -------------------------------------------------------------------
                // [Таблица: MPATHs]
                // [id] - INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE
                // [RRid] - INTEGER: Идентификатор дороги
                // [PREDid] - VARCHAR(50): Идентификатор ПЧ
                // [DIRid] - VARCHAR(50): Идентификатор направления
                // [PATHnum] - VARCHAR(50): Номер пути
                // [MPATHid] - VARCHAR(50): Идентификатор пути

                query_text.clear();
                query_text.append("CREATE TABLE MPATHs (");
                query_text.append("\"id\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, ");
                query_text.append("\"RRid\" INTEGER, ");
                query_text.append("\"PREDid\" VARCHAR(50), ");
                query_text.append("\"DIRid\" VARCHAR(50), ");
                query_text.append("\"PATHnum\" VARCHAR(50), ");
                query_text.append("\"MPATHid\" VARCHAR(50)");
                query_text.append(");");

                query->clear();
                query->prepare(query_text);
                if (!query->exec()) {

                    ErrorFlag = true;
                    return;
                }

                // Станции -------------------------------------------------------------------
                // [Таблица: STATIONs]
                // [id] - INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE
                // [RRid] - INTEGER: Идентификатор дороги
                // [PREDid] - VARCHAR(50): Идентификатор ПЧ
                // [STATIONid] - VARCHAR(50): Идентификатор станции
                // [NAME] - VARCHAR(50): Название станции
                // [TYPE] - INTEGER: Тип станции

                query_text.clear();
                query_text.append("CREATE TABLE STATIONs (");
                query_text.append("\"id\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, ");
                query_text.append("\"RRid\" INTEGER, ");
                query_text.append("\"PREDid\" VARCHAR(50), ");
                query_text.append("\"STATIONid\" VARCHAR(50), ");
                query_text.append("\"NAME\" VARCHAR(50), ");
                query_text.append("\"TYPE\" INTEGER");
                query_text.append(");");

                query->clear();
                query->prepare(query_text);
                if (!query->exec()) {

                    ErrorFlag = true;
                    return;
                }

                // Станционные пути -------------------------------------------------------------------
                // [Таблица: SPATHs]
                // [id] - INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE
                // [RRid] - INTEGER: Идентификатор дороги
                // [PREDid] - VARCHAR(50): Идентификатор ПЧ
                // [STATIONid] - VARCHAR(50): Идентификатор станции
                // [NAME] - VARCHAR(50): Название пути
                // [SPATHId] - VARCHAR(50): Идентификатор пути
                // [PARKNAME] - VARCHAR(50): Название парка
                // [PARKid] - VARCHAR(50): Идентификатор парка

                query_text.clear();
                query_text.append("CREATE TABLE SPATHs (");
                query_text.append("\"id\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, ");
                query_text.append("\"RRid\" INTEGER, ");
                query_text.append("\"PREDid\" VARCHAR(50), ");
                query_text.append("\"STATIONid\" VARCHAR(50), ");
                query_text.append("\"NAME\" VARCHAR(50), ");
                query_text.append("\"SPATHId\" VARCHAR(50), ");
                query_text.append("\"PARKNAME\" VARCHAR(50), ");
                query_text.append("\"PARKid\" VARCHAR(50)");
                query_text.append(");");

                query->clear();
                query->prepare(query_text);
                if (!query->exec()) {

                    ErrorFlag = true;
                    return;
                }

                // Классификатор дефектов -------------------------------------------------------------------
                // [Таблица: DEF]
                // [id] - INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE
                // [RAid] - INTEGER: Идентификатор дефекта (Радиоавионика)
                // [DEFid] - VARCHAR(50): Идентификатор дефекта ЕКАСУИ
                // [shortName] - VARCHAR(50): Короткое название
                // [fullName] - VARCHAR(512): Полное название

                query_text.clear();
                query_text.append("CREATE TABLE DEF (");
                query_text.append("\"id\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, ");
                query_text.append("\"RAid\" INTEGER, ");
                query_text.append("\"DEFid\" VARCHAR(50), ");
                query_text.append("\"shortName\" VARCHAR(50), ");
                query_text.append("\"fullName\" VARCHAR(512)");
                query_text.append(");");

                query->clear();
                query->prepare(query_text);
                if (!query->exec()) {

                    ErrorFlag = true;
                    return;
                }
            }
        }

    / *
      //  QSqlQuery query;
        query->clear();
        query->prepare("INSERT INTO info (id, db_version, NSIversion, carID) VALUES (?, ?, ?, ?)");
        query->addBindValue(0);
        query->addBindValue(1);
        query->addBindValue("15.06.2017");
        query->addBindValue("0");
        query->exec();
    * /

      / *  query->clear();
        query->prepare("CREATE TABLE names (""id"" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, ""name"" VARCHAR(255));");
        if (!query->exec()) {
         //   QMessageBox::warning(0,"Error", query.lastError().text());
         //   return -1;
        }
    * /
        //addBindValue решает проблему экранирования, поэтому добавлять значения надо именно этой функцией
       //QSqlQuery query.addBindValue(id);
    //    query.addBindValue(name);

        //получаем id вставленной записи
    //    int rer = query.lastInsertId().toInt();

    */
    FWorkDir = WorkDir;
    LoadRailRoads();
    LoadPRED();

    if (LoadAllData) {
        LoadDirections();
        LoadMainPaths();
        LoadStations();
        LoadStationPaths();
    }

    // Загрузка - "Классификатор дефектов"

    DEF_RAid.clear();
    DEF_id.clear();
    DEF_shortName.clear();
    DEF_fullName.clear();

    DEF_RAid.reserve(128);
    DEF_id.reserve(128);

    QFile file1(FWorkDir + "/DEFECTs.txt");

    if (file1.open(QIODevice::ReadOnly)) {
        int line = 0;
        while (!file1.atEnd()) {
            const QString& str = file1.readLine();
            if (line == 0) {
                NSIversion = str;
            }
            else if (line > 1) {
                const QStringList& list = str.split(";");
                DEF_RAid.push_back(list[0].toInt());
                DEF_id.push_back(list[1].toLongLong());
                DEF_shortName << list[2].simplified();
                DEF_fullName << list[3].simplified();
            }
            line++;
        }
        file1.close();
    }
    carID = "A20705822";
}

EK_ASUI::~EK_ASUI()
{
    RailRoads_NDOR.clear();
    RailRoads_NAME.clear();
    PRED_NDOR.clear();
    PRED_LOCATION.clear();
    PRED_SNAME.clear();
    Directions_NDOR.clear();
    Directions_SCOD.clear();
    Directions_NAME.clear();
    Directions_PRED.clear();
    MainPaths_NDOR.clear();
    MainPaths_NAPR.clear();
    MainPaths_NPUT.clear();
    MainPaths_PRED.clear();
    MainPaths_IDPUT.clear();
    Station_NDOR.clear();
    Station_SCOD.clear();
    Station_NAME.clear();
    Station_PODR.clear();
    Station_TYP.clear();
    StationPaths_NDOR.clear();
    StationPaths_NPUT.clear();
    StationPaths_TYP.clear();
    StationPaths_PARK.clear();
    StationPaths_SCOD.clear();
    StationPaths_IDPUT.clear();
    StationPaths_PRED.clear();
    //    PRED_vs_STAN_NDOR.clear();
    //    PRED_vs_STAN_SCOD.clear();
    //    PRED_vs_STAN_LOCATION.clear();
    DEF_RAid.clear();
    DEF_id.clear();
    DEF_shortName.clear();
    DEF_fullName.clear();
    NSIversion.clear();
    carID.clear();
}

/*
bool EK_ASUI::UpdateDB()
{

    if (true) {

        QSqlQuery * query;
        QFile * file;

        query = new QSqlQuery(DB);

        // Список дорог

        file = new QFile(FWorkDir + "/DOR.txt");
        if (file->open(QIODevice::ReadOnly)) {
            QString str="";
            int line = 0;
            while(!file->atEnd()) {
                str = file->readLine();
                if (line != 0) {
                    QStringList list = str.split(";");

                    query->prepare("SELECT * FROM RailRoads WHERE RRid = :id");
                    query->bindValue(":id", list[0].toInt());
                    query->exec();
                    if (!query->next()) {

                        query->prepare("INSERT INTO RailRoads (RRid, NAME) VALUES (:RRid, :NAME)");
                        query->bindValue(":RRid", list[0].toInt());
                        query->bindValue(":NAME", list[1].simplified());
                        query->exec();
                    }
                }
                line++;
            }
            file->close();
        }
        delete file;

        // Список ПЧ

        file = new QFile(FWorkDir + "/PRED.txt");
        if (file->open(QIODevice::ReadOnly)) {
            QString str="";
            int line = 0;
            while(!file->atEnd()) {
                str = file->readLine();
                if (line != 0) {
                    QStringList list = str.split(";");
                    //PRED_NDOR.push_back(list[0].toInt());
                    //PRED_LOCATION << list[1].simplified();
                    //PRED_SNAME << list[2].simplified();

                    query->prepare("SELECT * FROM PREDs WHERE PREDid = :id");
                    query->bindValue(":id", list[1].simplified());
                    query->exec();
                    if (!query->next()) {

                        query->prepare("INSERT INTO PREDs (RRid, PREDid, SNAME) VALUES (:RRid, :PREDid, :SNAME)");
                        query->bindValue(":RRid", list[0].toInt());
                        query->bindValue(":PREDid", list[1].simplified());
                        query->bindValue(":SNAME", list[2].simplified());
                        query->exec();
                    }
                }
                line++;
            }
            file->close();
        }
        delete file;

        // Направления
/ *
        int NDOR;
        QString PREDLOC;

        int idx = RailRoads_NAME.indexOf(RRname);
        if (idx == - 1) NDOR = - 1;
                   else NDOR = RailRoads_NDOR[idx];

        idx = PRED_SNAME.indexOf(PREDname);
        if (idx == - 1) PREDLOC = "";
                   else PREDLOC = PRED_LOCATION[idx];
* /
        file = new QFile(FWorkDir + "/DIRECTIONS.txt");
        if (file->open(QIODevice::ReadOnly)) {
            QString str="";
            int line = 0;
            while(!file->atEnd()) {
                str = file->readLine();
                if (line != 0) {
                    QStringList list = str.split(";");

                  / *  if ((list.size() == 4) &&
                        ((NDOR == -1) || (NDOR == list[0].toInt())) &&
                        ((PREDLOC == "") || (PREDLOC == list[3].simplified()))) {

                        Directions_NDOR.push_back(list[0].toInt());
                        Directions_SCOD << list[1].simplified();
                        Directions_NAME << list[2].simplified();
                        Directions_PRED << list[3].simplified();
                    } * /

                    query->prepare("SELECT * FROM DIRs WHERE DIRid = :id");
                    query->bindValue(":id", list[1].simplified());
                    query->exec();
                    if (!query->next()) {

                        query->prepare("INSERT INTO DIRs (RRid, PREDid, DIRid, NAME) VALUES (:RRid, :PREDid, :DIRid, :NAME)");
                        query->bindValue(":RRid", list[0].toInt());
                        query->bindValue(":PREDid", list[3].simplified());
                        query->bindValue(":DIRid", list[1].simplified());
                        query->bindValue(":NAME", list[2].simplified());
                        query->exec();
                    }
                }
                line++;
            }
            file->close();
        }
        delete file;

        // Главные пути
/ *
        int NDOR;
        QString PREDLOC;

        int idx = RailRoads_NAME.indexOf(RRname);
        if (idx == - 1) NDOR = - 1;
                   else NDOR = RailRoads_NDOR[idx];

        idx = PRED_SNAME.indexOf(PREDname);
        if (idx == - 1) PREDLOC = "";
                   else PREDLOC = PRED_LOCATION[idx];
* /
        file = new QFile(FWorkDir + "/MAINPATHs.txt");
        if (file->open(QIODevice::ReadOnly)) {
            QString str="";
            int line = 0;
            while(!file->atEnd()) {
                str = file->readLine();
                if (line != 0) {
                    QStringList list = str.split(";");
/ *
                    if ((list.size() == 5) &&
                        ((NDOR == -1) || (NDOR == list[0].toInt())) &&
                        ((PREDLOC == "") || (PREDLOC == list[4].simplified()))) {

                        MainPaths_NDOR.push_back(list[0].toInt());
                        MainPaths_NAPR << list[1].simplified();
                        MainPaths_NPUT << list[2].simplified();
                        MainPaths_IDPUT << list[3].simplified();
                        MainPaths_PRED << list[4].simplified();
                    }* /
                    query->prepare("SELECT * FROM MPATHs WHERE MPATHid = :id");
                    query->bindValue(":id", list[3].simplified());
                    query->exec();
                    if (!query->next()) {

                        query->prepare("INSERT INTO MPATHs (RRid, PREDid, DIRid, PATHnum, MPATHid) VALUES (:RRid, :PREDid, :DIRid, :PATHnum, :MPATHid)");
                        query->bindValue(":RRid", list[0].toInt());
                        query->bindValue(":PREDid", list[4].simplified());
                        query->bindValue(":DIRid", list[1].simplified());
                        query->bindValue(":PATHnum", list[2].simplified());
                        query->bindValue(":MPATHid", list[3].simplified());
                        query->exec();
                    }

                }
                line++;
            }
            file->close();
        }
        delete file;


        // Станции

/ *        int NDOR;
        QString PREDLOC;

        int idx = RailRoads_NAME.indexOf(RRname);
        if (idx == - 1) NDOR = - 1;
                   else NDOR = RailRoads_NDOR[idx];

        idx = PRED_SNAME.indexOf(PREDname);
        if (idx == - 1) PREDLOC = "";
                   else PREDLOC = PRED_LOCATION[idx];
* /
        file = new QFile(FWorkDir + "/STATIONS.txt");
        if (file->open(QIODevice::ReadOnly)) {
            QString str="";
            int line = 0;
            while(!file->atEnd()) {
                str = file->readLine();
                if (line != 0) {
                    QStringList list = str.split(";");

               / *   if ((list.size() == 4) &&
                        ((NDOR == -1) || (NDOR == list[0].toInt())) &&
                        ((PREDLOC == "") || (PREDLOC == list[3].simplified()))) {

                        Station_NDOR.push_back(list[0].toInt());
                        Station_SCOD << list[1].simplified();
                        Station_NAME << list[2].simplified();
                        Station_PODR << list[3].simplified();
                    } * /
                    query->prepare("SELECT * FROM STATIONs WHERE STATIONid = :id");
                    query->bindValue(":id", list[1].simplified());
                    query->exec();
                    if (!query->next()) {

                        query->prepare("INSERT INTO STATIONs (RRid, PREDid, STATIONid, NAME, TYPE) VALUES (:RRid, :PREDid, :STATIONid, :NAME, :TYPE)");
                        query->bindValue(":RRid", list[0].toInt());
                        query->bindValue(":PREDid", list[3].simplified());
                        query->bindValue(":STATIONid", list[1].simplified());
                        query->bindValue(":NAME", list[2].simplified());
                        query->bindValue(":TYPE", 0);
                        query->exec();
                    }
                }
                line++;
            }
            file->close();
        }
        delete file;


        // Станционные пути
/ *
        int NDOR;
        QString PREDLOC;

        int idx = RailRoads_NAME.indexOf(RRname);
        if (idx == - 1) NDOR = - 1;
                   else NDOR = RailRoads_NDOR[idx];

        idx = PRED_SNAME.indexOf(PREDname);
        if (idx == - 1) PREDLOC = "";
                   else PREDLOC = PRED_LOCATION[idx];
* /
        file = new QFile(FWorkDir + "/STATIONPATHs.txt");
        if (file->open(QIODevice::ReadOnly)) {
            QString str="";
            int line = 0;
            while(!file->atEnd()) {
                str = file->readLine();
                if (line != 0) {
                    QStringList list = str.split(";");
/ *
                    if ((list.size() == 6) &&
                        ((NDOR == -1) || (NDOR == list[0].toInt())) &&
                        ((PREDLOC == "") || (PREDLOC == list[5].simplified()))) {

                        StationPaths_NDOR.push_back(list[0].toInt());
                        StationPaths_NPUT << list[1].simplified();
                        StationPaths_PARK << list[2].simplified();
                        StationPaths_SCOD << list[3].simplified();
                        StationPaths_IDPUT << list[4].simplified();
                        StationPaths_PRED << list[5].simplified();
                    } * /
                    query->prepare("SELECT * FROM SPATHs WHERE STATIONid = :id");
                    query->bindValue(":id", list[4].simplified());
                    query->exec();
                    if (!query->next()) {

                        query->prepare("INSERT INTO SPATHs (RRid, PREDid, STATIONid, NAME, SPATHId, PARKNAME, PARKid) VALUES (:RRid, :PREDid, :STATIONid, :NAME, :SPATHId, :PARKNAME, :PARKid)");
                        query->bindValue(":RRid", list[0].toInt());
                        query->bindValue(":PREDid", list[5].simplified());
                        query->bindValue(":STATIONid", list[3].simplified());
                        query->bindValue(":NAME", list[1].simplified());
                        query->bindValue(":SPATHId", list[4].simplified());
                        query->bindValue(":PARKNAME", list[2].simplified());
                        query->bindValue(":PARKid", "");
                        query->exec();
                    }
                }
                line++;
            }
            file->close();
        }
        delete file;
    }


    // [Таблица: RailRoads]
    // [id] - INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE
    // [RRid] - INTEGER: Идентификатор дороги
    // [NAME] - VARCHAR(50): Название дороги

}
*/
bool EK_ASUI::LoadRailRoads()
{
    RailRoads_NAME.clear();
    RailRoads_NDOR.clear();

    QFile file(FWorkDir + "/DOR.txt");
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    int line = 0;
    while (!file.atEnd()) {
        const QString& str = file.readLine();
        if (line != 0) {
            const QStringList& list = str.split(";");
            RailRoads_NAME << list[1].simplified();
            RailRoads_NDOR.push_back(list[0].toInt());
        }
        line++;
    }
    file.close();
    return true;
}

bool EK_ASUI::LoadPRED()
{
    QFile file(FWorkDir + "/PRED.txt");
    if (file.open(QIODevice::ReadOnly)) {
        int line = 0;
        while (!file.atEnd()) {
            const QString& str = file.readLine();
            if (line != 0) {
                const QStringList& list = str.split(";");
                PRED_NDOR.push_back(list[0].toInt());
                PRED_LOCATION << list[1].simplified();
                PRED_SNAME << list[2].simplified();
            }
            line++;
        }
        file.close();
        return true;
    }
    return false;
}

bool EK_ASUI::LoadData(const QString& RRname, const QString& PREDname)
{
    int RRidx = load_RR.indexOf(RRname);
    int PREDidx = -1;
    if (RRidx != -1) {
        PREDidx = load_PRED.indexOf(PREDname);
    }

    if ((RRidx == -1) || (PREDidx == -1)) {
        LoadDirections(RRname, PREDname);
        LoadMainPaths(RRname, PREDname);
        LoadStations(RRname, PREDname);
        LoadStationPaths(RRname, PREDname);

        load_RR.push_back(RRname);
        load_PRED.push_back(PREDname);
    }
    return true;
}

void EK_ASUI::SetCarID(const QString& carID_)
{
    carID = carID_;
}

bool EK_ASUI::LoadDirections(const QString& RRname, const QString& PREDname)
{
    int NDOR;
    QString PREDLOC;

    int idx = RailRoads_NAME.indexOf(RRname);
    if (idx == -1) {
        NDOR = -1;
    }
    else {
        NDOR = RailRoads_NDOR[idx];
    }

    idx = PRED_SNAME.indexOf(PREDname);
    if (idx == -1) {
        PREDLOC = "";
    }
    else {
        PREDLOC = PRED_LOCATION[idx];
    }

    QFile file(FWorkDir + "/DIRECTIONS.txt");
    if (file.open(QIODevice::ReadOnly)) {
        Directions_NDOR.reserve(128);
        QString str = "";
        int line = 0;
        while (!file.atEnd()) {
            str = file.readLine();
            if (line != 0) {
                const QStringList& list = str.split(";");

                if ((list.size() == 4) && ((NDOR == -1) || (NDOR == list[0].toInt())) && ((PREDLOC == "") || (PREDLOC == list[3].simplified()))) {
                    Directions_NDOR.push_back(list[0].toInt());
                    Directions_SCOD << list[1].simplified();
                    Directions_NAME << list[2].simplified();
                    Directions_PRED << list[3].simplified();
                }
            }
            line++;
        }
        file.close();
        return true;
    }
    return false;
}

bool EK_ASUI::LoadMainPaths(const QString& RRname, const QString& PREDname)
{
    int NDOR;
    QString PREDLOC;

    int idx = RailRoads_NAME.indexOf(RRname);
    if (idx == -1) {
        NDOR = -1;
    }
    else {
        NDOR = RailRoads_NDOR[idx];
    }

    idx = PRED_SNAME.indexOf(PREDname);
    if (idx == -1) {
        PREDLOC = "";
    }
    else {
        PREDLOC = PRED_LOCATION[idx];
    }

    QFile file(FWorkDir + "/MAINPATHs.txt");
    if (file.open(QIODevice::ReadOnly)) {
        MainPaths_NDOR.reserve(128);
        int line = 0;
        while (!file.atEnd()) {
            const QString& str = file.readLine();
            if (line != 0) {
                const QStringList& list = str.split(";");

                if ((list.size() == 5) && ((NDOR == -1) || (NDOR == list[0].toInt())) && ((PREDLOC == "") || (PREDLOC == list[4].simplified()))) {
                    MainPaths_NDOR.push_back(list[0].toInt());
                    MainPaths_NAPR << list[1].simplified();
                    MainPaths_NPUT << list[2].simplified();
                    MainPaths_IDPUT << list[3].simplified();
                    MainPaths_PRED << list[4].simplified();
                }
            }
            line++;
        }
        file.close();
        return true;
    }
    return false;
}
// Станции

bool EK_ASUI::LoadStations(const QString& RRname, const QString& PREDname)
{
    int NDOR;
    QString PREDLOC;

    int idx = RailRoads_NAME.indexOf(RRname);
    if (idx == -1) {
        NDOR = -1;
    }
    else {
        NDOR = RailRoads_NDOR[idx];
    }

    idx = PRED_SNAME.indexOf(PREDname);
    if (idx == -1) {
        PREDLOC = "";
    }
    else {
        PREDLOC = PRED_LOCATION[idx];
    }

    QFile file(FWorkDir + "/STATIONS.txt");
    if (file.open(QIODevice::ReadOnly)) {
        Station_NDOR.reserve(128);
        int line = 0;
        while (!file.atEnd()) {
            const QString& str = file.readLine();
            if (line != 0) {
                const QStringList& list = str.split(";");

                if ((list.size() == 4) && ((NDOR == -1) || (NDOR == list[0].toInt())) && ((PREDLOC == "") || (PREDLOC == list[3].simplified()))) {
                    Station_NDOR.push_back(list[0].toInt());
                    Station_SCOD << list[1].simplified();
                    Station_NAME << list[2].simplified();
                    Station_PODR << list[3].simplified();
                }
            }
            line++;
        }
        file.close();
        return true;
    }
    return false;
}

// Станционные пути

bool EK_ASUI::LoadStationPaths(const QString& RRname, const QString& PREDname)
{
    int NDOR;
    QString PREDLOC;

    int idx = RailRoads_NAME.indexOf(RRname);
    if (idx == -1) {
        NDOR = -1;
    }
    else {
        NDOR = RailRoads_NDOR[idx];
    }

    idx = PRED_SNAME.indexOf(PREDname);
    if (idx == -1) {
        PREDLOC = "";
    }
    else {
        PREDLOC = PRED_LOCATION[idx];
    }

    QFile file(FWorkDir + "/STATIONPATHs.txt");
    if (file.open(QIODevice::ReadOnly)) {
        StationPaths_NDOR.reserve(128);
        int line = 0;
        while (!file.atEnd()) {
            const QString& str = file.readLine();
            if (line != 0) {
                const QStringList& list = str.split(";");

                if ((list.size() == 6) && ((NDOR == -1) || (NDOR == list[0].toInt())) && ((PREDLOC == "") || (PREDLOC == list[5].simplified()))) {
                    StationPaths_NDOR.push_back(list[0].toInt());
                    StationPaths_NPUT << list[1].simplified();
                    StationPaths_PARK << list[2].simplified();
                    StationPaths_SCOD << list[3].simplified();
                    StationPaths_IDPUT << list[4].simplified();
                    StationPaths_PRED << list[5].simplified();
                }
            }
            line++;
        }
        file.close();
        return true;
    }
    return false;
}

/*
bool EK_ASUI::CreateIncidentXML(const QString& fileName,         // Полное имя создоваемого файла
                                const QString& fwVersion,        // Версия ПО БУИ [+]
                                const QString& decoderName,      // Оператор/расшифровщик [+]
                                const QString& RRName,           // Название Ж/Д  [+]
                                const QString& PRED,             // Название ПЧ  [+]
                                const QString& DIRECTION,        // Название направления [+ ГП]
                                const QString& NPUT,             // Номер пути [+]
                                bool isStan,                     // Инциндент на стации [+]
                                const QString& StanName,         // Название станции [+ СП]
                                int DefLength,                   // Длина дефекта [+]
                                int DefDepth,                    // Глубина дефекта [+]
                                int DefWidth,                    // Ширина дефекта [+]
                                int speedLimit,                  // Ограничение скорости [+]
                                int posKM,                       // Километр [+]
                                int posPK,                       // Пикет [+]
                                int posM,                        // Метр [+]
                                const QString& DefectShortname,  // Код дефекта [+][+]
                                const QString& comment,          // Коментарий [+]
                                const QString& posHint,          // Привязка [+]
                                int Rail,                        // Нить пути: 0-Левая; 1-Правая [+]
                                void* Image_Ptr,                 // Изображение дефекта - указатель на изображение в формате PNG
                                unsigned int Image_Size,         // Изображение дефекта - размер изображения
                                const QString& Image_Name,
                                double GPS_latitude,   // GPS коодината - Широта
                                double GPS_longitude)  // GPS коодината - Долгота
{
    int RRidx = RailRoads_NAME.indexOf(RRName);
    if (RRidx == -1) {
        return false;
    }

    int DIRidx = 0;
    // TODO: unused variable : int STANidx = 0;

    if (!isStan) {
        DIRidx = Directions_NAME.indexOf(DIRECTION);
        if (DIRidx == -1) {
            return false;
        }
    }
    else {
        if (Station_NAME.indexOf(StanName) == -1) {
            return false;
        }
    }

    int DEFidx = DEF_shortName.indexOf(DefectShortname);
    if (DEFidx == -1) {
        return false;
    }

    QDateTime dt = QDateTime::currentDateTime();

    int recNo = 0;
    QDomDocument* doc;
    QDomElement docbody;
    if (QFile::exists(fileName)) {
        QFile file(fileName);
        file.open(QFile::ReadOnly | QIODevice::Text);

        doc = new QDomDocument();
        doc->setContent(file.readAll());

        docbody = doc->documentElement();
        if (docbody.tagName() != "OutFile") {
            return false;
        }

        recNo = 1;
        QDomNode node = docbody.firstChild();
        while (!node.isNull()) {
            if (node.toElement().tagName() == "incident") {
                recNo++;
            }
            node = node.nextSibling();
        }
    }
    else {
        doc = new QDomDocument(fileName);

        //  QDomNode node = doc->createProcessingInstruction("xml",  "version=\"1.0\" encoding=\"UTF-8\"");
        //  doc->insertBefore(node, doc->firstChild());

        //  QString xmlstr = "<?xml version=\"1.0\"?>";
        QString xmlstr = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
        doc->setContent(xmlstr, true);

        docbody = doc->createElement("OutFile");


        docbody.setAttribute("xmlns", "http://ns-it.ru/XSD/VRel");
        // ID средства НК
        docbody.setAttribute("version", fwVersion);                             // ВЕРСИЯ ПО, КОТОРЫМ ПРОИЗВОДИЛАСЬ РАСШИФРОВКА
        docbody.setAttribute("nsiver", NSIversion.simplified());                // Версия нормативно справочная информации
        docbody.setAttribute("timestamp", dt.toString("dd.MM.yyyy hh:mm:ss"));  // дата формирования файла
        doc->appendChild(docbody);

        QDomElement docheader = doc->createElement("header");

        docheader.setAttribute("carmafucafturer", "ОАО Радиоавионика");            //
        docheader.setAttribute("siteID", RailRoads_NDOR.at(RRidx));                // - КОД ДОРОГИ (ЕСЛИ НУЖНО МОЖЕМ ВЫСЛАТЬ КЛАССИФИКАТОР)
        docheader.setAttribute("runDate", dt.toString("dd.MM.yyyy"));              // ДАТА ПРОЕЗДА
        docheader.setAttribute("decodeDate", dt.toString("dd.MM.yyyy hh:mm:ss"));  // ДАТА РАСШИФРОВКИ
        docheader.setAttribute("decoder", decoderName);                            // ФИО РАСШИФРОВЩИКА
        docheader.setAttribute("carID", carID);

        // docheader.setAttribute("carID", carID); // ВНУТРЕННИЙ ИДЕНТИФИКАТОР СРЕДСТВА ДИАНОСТИКИ
        // docheader.setAttribute("carClass", "Дефектоскопная тележка"); // КЛАССИФКАЦИЯ СРЕДСТВА ДИАГНОСТИКИ
        // docheader.setAttribute("carMod", "Авикон-31");  // СЕРИЯ СРЕДСТВА ДИАГНОСТИКИ
        // docheader.setAttribute("softID", "А31CDU_0.3.0.299"); // КОД СОФТА
        // docheader.setAttribute("softText", "А31CDU"); // НАЗВАНИЕ СОФТА

        docbody.appendChild(docheader);
        recNo = 1;
    }

    QDomElement docincident = doc->createElement("incident");

    docincident.setAttribute("recNo", QString::number(recNo));                      // НОМЕР ВНУТРИ ДАННОГО ФАЙЛА
    docincident.setAttribute("recID", dt.toString("yyyyMMddhhmmsszzz"));            // УНИКАЛЬНЫЙ НОМЕР ЗАПИСИ ОБ ИНЦИДЕНТЕ
    docincident.setAttribute("posKM", QString::number(posKM));                      // КООРДИНАТА КМ ИНЦИДЕНТА
    docincident.setAttribute("posM", QString::number(posM + posPK * 100));          // КООРДИНАТА М ИНЦИДЕНТА
    docincident.setAttribute("posPK", QString::number(posPK));                      // КООРДИНАТА ПИКЕТА ИНЦИДЕНТА
    docincident.setAttribute("latitude", QString::number(GPS_latitude, 'g', 8));    // GPS - Широта
    docincident.setAttribute("longitude", QString::number(GPS_longitude, 'g', 8));  // GPS - Долгота

    docincident.setAttribute("siteID", RailRoads_NDOR.at(RRidx));  // КОД ДОРОГИ
    if (!isStan)                                                   // НЕ НА СТАНЦИИ
    {
        docincident.setAttribute("isStan", "false");
        docincident.setAttribute("stanID", "");                                                                 // КОД СТАНЦИИ
        docincident.setAttribute("parkID", "");                                                                 // КОД ПАРКА
        docincident.setAttribute("routeID", Directions_SCOD.at(DIRidx));                                        // ID ПУТИ (КОД НАПРАВЛЕНИЯ)
        docincident.setAttribute("pathID", GetMAINPATH_ID_by_RR_PRED_DIR_PATH(RRName, PRED, DIRECTION, NPUT));  // КОД ПУТИ
    }
    else  // НА СТАНЦИИ
    {
        docincident.setAttribute("isStan", "true");
        docincident.setAttribute("stanID", GetSTATIONcode_by_Name(StanName));                                            // КОД СТАНЦИИ
        docincident.setAttribute("parkID", GetSTATIONSPATH_PARK_by_RR_PRED_STATION_PATH(RRName, PRED, StanName, NPUT));  // КОД ПАРКА
        docincident.setAttribute("routeID", "");
        docincident.setAttribute("pathID", GetSTATIONSPATH_ID_by_RR_PRED_STATION_PATH(RRName, PRED, StanName, NPUT));  // КОД ПУТИ
    }

    QString predLoc;
    int predIndex = PRED_SNAME.indexOf(PRED);
    if (predIndex == -1) {
        predLoc = "";
    }
    else {
        predLoc = PRED_LOCATION.at(predIndex);
    }

    docincident.setAttribute("distanceID", predLoc);                     // КОД ПРЕДПРИЯТИЯ ЗОНЫ ОТВЕТСТВЕННОСТИ
    docincident.setAttribute("thread", QString::number(Rail));           // НИТЬ (ЛЕВАЯ, ПРАВАЯ, ОБЕ)
    docincident.setAttribute("defectID", DEF_id.at(DEFidx));             // КОД ИНЦИДЕНТА
    docincident.setAttribute("defectText", DEF_fullName.at(DEFidx));     // ОПИСАНИЕ ИНЦИДЕНТА
    docincident.setAttribute("sizeLength", QString::number(DefLength));  // ДЛИНА ОСТУПЛЕНИЯ
    docincident.setAttribute("sizeWidth", QString::number(DefWidth));    // ШИРИНА ОТСТУПЛЕНИЯ
    docincident.setAttribute("sizeDepth", QString::number(DefDepth));    // ГЛУБИНА ОТСТУПЛЕНИЯ
    if (speedLimit != -1) {
        docincident.setAttribute("speedLimitID", QString::number(speedLimit));  // ОГРАНИЧЕНИЕ СКОРОСТИ
    }
    else {
        docincident.setAttribute("speedLimitID", "");  // ОГРАНИЧЕНИЕ СКОРОСТИ
    }
    docincident.setAttribute("comment", posHint);  // СВОБОДНЫЙ КОММЕНТАРИЙ
    docincident.setAttribute("pathText", NPUT);    // НОМЕР ПУТИ / описание пути
    // docincident.setAttribute("posHint", posHint); // текстовая привязка


    QDomElement binsection = doc->createElement("bin");

    binsection.setAttribute("key", "img" + dt.toString("yyyyMMddhhmmsszzz"));
    // QFileInfo fileInfo(fileName);
    // binsection.setAttribute("filename", fileInfo.baseName() + ".png");
    binsection.setAttribute("filename", Image_Name);
    binsection.setAttribute("extention", "PNG");

    QByteArray Image((char*) Image_Ptr, Image_Size);
    QDomElement base64 = doc->createElement("base64");


    QDomText child_node = doc->createTextNode("/ *this will be ignored* /");
    child_node.setNodeValue(QString(Image.toBase64()));
    doc->appendChild(base64).appendChild(child_node);

    docincident.appendChild(binsection);
    binsection.appendChild(base64);
    docbody.appendChild(docincident);

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        doc->save(out, 0);
        file.close();
    }

    return true;
}
*/

bool EK_ASUI::CreateIncidentXML(const QString& fileName,         // Полное имя создоваемого файла
                                const QString& fwVersion,        // Версия ПО БУИ [+]
                                const QString& decoderName,      // Оператор/расшифровщик [+]
                                const QString& RRName,           // Название Ж/Д  [+]
                                const QString& PRED,             // Название ПЧ  [+]
                                const QString& DIRECTION,        // Название направления [+ ГП]
                                const QString& NPUT,             // Номер пути [+]
                                bool isStan,                     // Инциндент на стации [+]
                                const QString& StanName,         // Название станции [+ СП]
                                int DefLength,                   // Длина дефекта [+]
                                int DefDepth,                    // Глубина дефекта [+]
                                int DefWidth,                    // Ширина дефекта [+]
                                int speedLimit,                  // Ограничение скорости [+]
                                int posKM,                       // Километр [+]
                                int posPK,                       // Пикет [+]
                                int posM,                        // Метр [+]
                                const QString& DefectShortname,  // Код дефекта [+][+]
                                const QString& comment,          // Коментарий [+]
                                const QString& posHint,          // Привязка [+]
                                int Rail,                        // Нить пути: 0-Левая; 1-Правая [+]
                                void* Image_Ptr,                 // Изображение дефекта - указатель на изображение в формате PNG
                                int Image_Size,         // Изображение дефекта - размер изображения
                                const QString& Image_Name,
                                double GPS_latitude,   // GPS коодината - Широта
                                double GPS_longitude)  // GPS коодината - Долгота
{
    int RRidx = RailRoads_NAME.indexOf(RRName);
    if (RRidx == -1) {
        return false;
    }

    int DIRidx = 0;
    // TODO: unused variable : int STANidx = 0;

    if (!isStan) {
        DIRidx = Directions_NAME.indexOf(DIRECTION);
        if (DIRidx == -1) {
            return false;
        }
    }
    else {
        if (Station_NAME.indexOf(StanName) == -1) {
            return false;
        }
    }

    int DEFidx = DEF_shortName.indexOf(DefectShortname);
    if (DEFidx == -1) {
        return false;
    }

    QDateTime dt = QDateTime::currentDateTime();

    int recNo = 0;
    QDomDocument* doc;
    QDomElement docbody;
    if (QFile::exists(fileName)) {
        QFile file(fileName);
        file.open(QFile::ReadOnly | QIODevice::Text);

        doc = new QDomDocument();
        doc->setContent(file.readAll());

        docbody = doc->documentElement();
        if (docbody.tagName() != "OutFile") {
            return false;
        }

        recNo = 1;
        QDomNode node = docbody.firstChild();
        while (!node.isNull()) {
            if (node.toElement().tagName() == "incident") {
                recNo++;
            }
            node = node.nextSibling();
        }
    }
    else {
        doc = new QDomDocument(fileName);

        QString xmlstr = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
        doc->setContent(xmlstr, true);

        docbody = doc->createElement("Defect");
        docbody.setAttribute("NSIver", "25.06.2018");                // Версия нормативно справочная информации
        docbody.setAttribute("Manufacture", "RDVNKA");            //
        docbody.setAttribute("recID", dt.toString("yyyyMMddhhmmsszzz"));            // УНИКАЛЬНЫЙ НОМЕР ЗАПИСИ ОБ ИНЦИДЕНТЕ
        docbody.setAttribute("SiteID", RailRoads_NDOR.at(RRidx));                // - КОД ДОРОГИ (ЕСЛИ НУЖНО МОЖЕМ ВЫСЛАТЬ КЛАССИФИКАТОР)
		carID = "A19938139";
        docbody.setAttribute("carID", carID);
        docbody.setAttribute("soft", "А15CDU_4.6.0"); // КОД СОФТА
        docbody.setAttribute("runTime", dt.toString("dd.MM.yyyy hh:mm:ss"));  // дата формирования файла
        docbody.setAttribute("decodeTime", dt.toString("dd.MM.yyyy hh:mm:ss"));  // ДАТА РАСШИФРОВКИ
        docbody.setAttribute("decoder", decoderName);                            // ФИО РАСШИФРОВЩИКА

        QString predLoc;
        int predIndex = PRED_SNAME.indexOf(PRED);
        if (predIndex == -1) {
            predLoc = "";
        }
        else {
            predLoc = PRED_LOCATION.at(predIndex);
        }

        docbody.setAttribute("PredID", predLoc);                            // код ПЧ
		
		if (!isStan)                                                   // НЕ НА СТАНЦИИ
		{
			docbody.setAttribute("pathType", "1");
			docbody.setAttribute("pathID", GetMAINPATH_ID_by_RR_PRED_DIR_PATH(RRName, PRED, DIRECTION, NPUT));  // КОД ПУТИ
		}
		else  // НА СТАНЦИИ
		{
			docbody.setAttribute("pathType", "0");
            docbody.setAttribute("pathID", GetSTATIONSPATH_ID_by_RR_PRED_STATION_PATH(RRName, PRED, StanName, NPUT));  // КОД ПУТИ
		}		
		docbody.setAttribute("pathText", NPUT);  // ПУТЬ
		docbody.setAttribute("thread", QString::number(Rail));           // НИТЬ (ЛЕВАЯ, ПРАВАЯ, ОБЕ)
		docbody.setAttribute("posKM", QString::number(posKM));                      // КООРДИНАТА КМ ИНЦИДЕНТА
		docbody.setAttribute("posM", QString::number(posM + posPK * 100));          // КООРДИНАТА М ИНЦИДЕНТА
		
		docbody.setAttribute("NotificationNum", "45157");
//		docbody.setAttribute("defectID", DEF_id.at(DEFidx));             // КОД ИНЦИДЕНТА
		docbody.setAttribute("DefPic", DEF_shortName.at(DEFidx));     // ОПИСАНИЕ ИНЦИДЕНТА
		docbody.setAttribute("DefType", "0");     // ОПИСАНИЕ ИНЦИДЕНТА
		docbody.setAttribute("Fabric", "122");
		docbody.setAttribute("Month", "1");
		docbody.setAttribute("Year", "2018");
        docbody.setAttribute("TypeRail", "102");
		docbody.setAttribute("SmeltingNum", "5454877");
		docbody.setAttribute("sizeDepth", QString::number(DefDepth));    // ГЛУБИНА ОТСТУПЛЕНИЯ
		docbody.setAttribute("sizeLength", QString::number(DefLength));  // ДЛИНА ОСТУПЛЕНИЯ
		docbody.setAttribute("sizeWidth", QString::number(DefWidth));    // ШИРИНА ОТСТУПЛЕНИЯ
		if (speedLimit != -1) {
			docbody.setAttribute("speedLimitID", QString::number(speedLimit));  // ОГРАНИЧЕНИЕ СКОРОСТИ
		}
		else {
			docbody.setAttribute("speedLimitID", "");  // ОГРАНИЧЕНИЕ СКОРОСТИ
		}
		docbody.setAttribute("comment", posHint);  // СВОБОДНЫЙ КОММЕНТАРИЙ
		docbody.setAttribute("lon", QString::number(GPS_longitude, 'g', 8));  // GPS - Долгота
		docbody.setAttribute("lat", QString::number(GPS_latitude, 'g', 8));    // GPS - Широта

        QByteArray Image((char*)Image_Ptr, Image_Size);
		docbody.setAttribute("Pic", QString(Image.toBase64())); 
        doc->appendChild(docbody);


		QFile file(fileName);
		if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			QTextStream out(&file);
			doc->save(out, 0);
			file.close();
		}
		return true;
   }
}

bool EK_ASUI::CreateMainPathsIncidentXML(QString fileName,         // Полное имя создоваемого файла
                                         QString fwVersion,        // Версия ПО БУИ [+]
                                         QString decoderName,      // Оператор/расшифровщик [+]
                                         QString RRName,           // Название Ж/Д  [+]
                                         QString PRED,             // Название ПЧ  [+]
                                         QString DIRECTION,        // Название направления [+ ГП]
                                         QString NPUT,             // Номер пути [+]
                                         int DefLength,            // Длина дефекта [+]
                                         int DefDepth,             // Глубина дефекта [+]
                                         int DefWidth,             // Ширина дефекта [+]
                                         int speedLimit,           // Ограничение скорости [+]
                                         int posKM,                // Километр [+]
                                         int posPK,                // Пикет [+]
                                         int posM,                 // Метр [+]
                                         QString DefectShortname,  // Код дефекта [+][+]
                                         QString comment,          // Коментарий [+]
                                         QString posHint,          // Привязка [+]
                                         int Rail,                 // Нить пути: 0-Левая; 1-Правая [+]
                                         void* Image_Ptr,          // Изображение дефекта - указатель на изображение в формате PNG
                                         int Image_Size,  // Изображение дефекта - размер изображения
                                         QString Image_Name,       // Имя картинки
                                         double GPS_latitude,      // GPS коодината - Широта
                                         double GPS_longitude)     // GPS коодината - Долгота
{
    return CreateIncidentXML(fileName, fwVersion, decoderName, RRName, PRED, DIRECTION, NPUT, false, "", DefLength, DefDepth, DefWidth, speedLimit, posKM, posPK, posM, DefectShortname, comment, posHint, Rail, Image_Ptr, Image_Size, Image_Name, GPS_latitude, GPS_longitude);
}

bool EK_ASUI::CreateStationIncidentXML(QString fileName,         // Полное имя создоваемого файла
                                       QString fwVersion,        // Версия ПО БУИ [+]
                                       QString decoderName,      // Оператор/расшифровщик [+]
                                       QString RRName,           // Название Ж/Д  [+]
                                       QString PRED,             // Название ПЧ  [+]
                                       QString StanName,         // Название станции [+ СП]
                                       QString NPUT,             // Номер пути [+]
                                       int DefLength,            // Длина дефекта [+]
                                       int DefDepth,             // Глубина дефекта [+]
                                       int DefWidth,             // Ширина дефекта [+]
                                       int speedLimit,           // Ограничение скорости [+]
                                       int posKM,                // Километр [+]
                                       int posPK,                // Пикет [+]
                                       int posM,                 // Метр [+]
                                       QString DefectShortname,  // Код дефекта [+][+]
                                       QString comment,          // Коментарий [+]
                                       QString posHint,          // Привязка [+]
                                       int Rail,                 // Нить пути: 0-Левая; 1-Правая [+]
                                       void* Image_Ptr,          // Изображение дефекта - указатель на изображение в формате PNG
                                       int Image_Size,  // Изображение дефекта - размер изображения
                                       QString Image_Name,       // Имя картинки
                                       double GPS_latitude,      // GPS коодината - Широта
                                       double GPS_longitude)     // GPS коодината - Долгота
{
    return CreateIncidentXML(fileName, fwVersion, decoderName, RRName, PRED, "", NPUT, true, StanName, DefLength, DefDepth, DefWidth, speedLimit, posKM, posPK, posM, DefectShortname, comment, posHint, Rail, Image_Ptr, Image_Size, Image_Name, GPS_latitude, GPS_longitude);
}

int EK_ASUI::getIncidentIndex()
{
    return IncidentIndex++;
}

void EK_ASUI::resetIncidentIndex()
{
    IncidentIndex = 1;
}

// --------------------------------------------------------------------------

// Получение списка Дорог
QStringList EK_ASUI::GetRRs()
{
    return RailRoads_NAME;
}

// --------------------------------------------------------------------------

// Получение списка ПЧ на Дороге
QStringList EK_ASUI::GetPREDs_by_RR(const QString& RRname)
{
    QStringList list;

    int idx = RailRoads_NAME.indexOf(RRname);
    if (idx == -1) return list;
    int NDOR = RailRoads_NDOR[idx];
    size_t size = PRED_NDOR.size();
    for (unsigned int i = 0; i < size; ++i)
        if (PRED_NDOR[i] == NDOR) list.append(PRED_SNAME[i]);

    return list;
}

// --------------------------------------------------------------------------

// Получение Имен направлений по Дороге и ПЧ
QStringList EK_ASUI::GetDIRNames_by_RR_PRED(const QString& RRname, const QString& PREDname)
{
    QStringList list;

    int idx = RailRoads_NAME.indexOf(RRname);
    if (idx == -1) return list;
    int NDOR = RailRoads_NDOR[idx];

    idx = PRED_SNAME.indexOf(PREDname);
    if (idx == -1) return list;
    QString PREDLOC = PRED_LOCATION[idx];

    size_t size = Directions_NDOR.size();
    for (unsigned int i = 0; i < size; ++i)
        if ((Directions_NDOR[i] == NDOR) && (Directions_PRED[i] == PREDLOC)) list.append(Directions_NAME[i]);

    return list;
}

// Получение Кодов направлений по Дороге и ПЧ
QStringList EK_ASUI::GetDIRCodes_by_RR_PRED(const QString& RRname, const QString& PREDname)  // Получение Направлений по Дороге и ПЧ
{
    QStringList list;

    int idx = RailRoads_NAME.indexOf(RRname);
    if (idx == -1) return list;
    int NDOR = RailRoads_NDOR[idx];

    idx = PRED_SNAME.indexOf(PREDname);
    if (idx == -1) return list;
    QString PREDLOC = PRED_LOCATION[idx];

    size_t size = Directions_NDOR.size();
    for (unsigned int i = 0; i < size; ++i)
        if ((Directions_NDOR[i] == NDOR) && (Directions_PRED[i] == PREDLOC)) list.append(Directions_SCOD[i]);

    return list;
}

// Получение Кода Направления по Дороге, ПЧ и Названию направления
QString EK_ASUI::GetDIRCode_by_RRs_PRED_DIR(const QString& RRname, const QString& PREDname, const QString& DIRname)
{
    QStringList DIRNames = GetDIRNames_by_RR_PRED(RRname, PREDname);
    QStringList DIRCodes = GetDIRCodes_by_RR_PRED(RRname, PREDname);

    int idx = DIRNames.indexOf(DIRname);
    if (idx != -1)
        return DIRCodes[idx];
    else
        return "";
}

// --------------------------------------------------------------------------

// Получение Идентификаторов главных путей по Дороге, ПЧ и Направлению
QStringList EK_ASUI::GetMAINPATH_IDs_by_RR_PRED_DIR(const QString& RRname, const QString& PREDname, const QString& DIRname)
{
    QStringList list;

    int idx = RailRoads_NAME.indexOf(RRname);
    if (idx == -1) return list;
    int NDOR = RailRoads_NDOR[idx];

    idx = PRED_SNAME.indexOf(PREDname);
    if (idx == -1) return list;
    QString PREDLOC = PRED_LOCATION[idx];

    idx = Directions_NAME.indexOf(DIRname);
    if (idx == -1) return list;
    QString SCOD = Directions_SCOD[idx];

    size_t size = MainPaths_IDPUT.size();
    for (unsigned int i = 0; i < size; ++i)
        if ((MainPaths_NDOR[i] == NDOR) && (MainPaths_PRED[i] == PREDLOC) && (MainPaths_NAPR[i] == SCOD)) list.append(MainPaths_IDPUT[i]);

    return list;
}

// Получение Номеров главных путей по Дороге, ПЧ и Направлению
QStringList EK_ASUI::GetMAINPATH_NPUTs_by_RR_PRED_DIR(const QString& RRname, const QString& PREDname, const QString& DIRname)
{
    QStringList list;

    int idx = RailRoads_NAME.indexOf(RRname);
    if (idx == -1) return list;
    int NDOR = RailRoads_NDOR[idx];

    idx = PRED_SNAME.indexOf(PREDname);
    if (idx == -1) return list;
    QString PREDLOC = PRED_LOCATION[idx];

    idx = Directions_NAME.indexOf(DIRname);
    if (idx == -1) return list;
    QString SCOD = Directions_SCOD[idx];

    size_t size = MainPaths_IDPUT.size();
    for (unsigned int i = 0; i < size; ++i)
        if ((MainPaths_NDOR[i] == NDOR) && (MainPaths_PRED[i] == PREDLOC) && (MainPaths_NAPR[i] == SCOD)) list.append(MainPaths_NPUT[i]);

    return list;
}

// Получение Идентификатора главного пути по Дороге, ПЧ, Направлению и Номеру пути
QString EK_ASUI::GetMAINPATH_ID_by_RR_PRED_DIR_PATH(const QString& RRname, const QString& PREDname, const QString& DIRname, const QString& PATH)
{
    int idx = RailRoads_NAME.indexOf(RRname);
    if (idx == -1) return "";
    int NDOR = RailRoads_NDOR[idx];

    idx = PRED_SNAME.indexOf(PREDname);
    if (idx == -1) return "";
    QString PREDLOC = PRED_LOCATION[idx];

    idx = Directions_NAME.indexOf(DIRname);
    if (idx == -1) return "";
    QString SCOD = Directions_SCOD[idx];

    size_t size = MainPaths_IDPUT.size();
    for (unsigned int i = 0; i < size; ++i)
        if ((MainPaths_NDOR[i] == NDOR) && (MainPaths_PRED[i] == PREDLOC) && (MainPaths_NAPR[i] == SCOD) && (MainPaths_NPUT[i] == PATH)) return MainPaths_IDPUT[i];

    return "";
}

// --------------------------------------------------------------------------

// Получение Станций по Дороге и ПЧ
QStringList EK_ASUI::GetSTATIONSs_by_RR_PRED(const QString& RRname, const QString& PREDname)
{
    QStringList list;

    int idx = RailRoads_NAME.indexOf(RRname);
    if (idx == -1) return list;
    int NDOR = RailRoads_NDOR[idx];

    idx = PRED_SNAME.indexOf(PREDname);
    if (idx == -1) return list;
    QString PREDLOC = PRED_LOCATION[idx];

    for (int i = 0; i < Station_SCOD.size(); i++)

        if ((Station_NDOR[i] == NDOR) && (Station_PODR[i] == PREDLOC)) {
            list.append(Station_NAME[i]);
        }
    return list;
}

// Получение ID станции по Названию станции
QString EK_ASUI::GetSTATIONcode_by_Name(const QString& name)
{
    int idx = Station_NAME.indexOf(name);
    if (idx != -1) {
        return Station_SCOD[idx];
    }
    else {
        return "";
    }
}

// Получение идентификаторов станционного пути по Дороге, ПЧ и направлению
QStringList EK_ASUI::GetSTATIONSPATH_IDs_by_RR_PRED_STATION(const QString& RRname, const QString& PREDname, const QString& STATIONname)
{
    QStringList list;

    int idx = RailRoads_NAME.indexOf(RRname);
    if (idx == -1) {
        return list;
    }
    int NDOR = RailRoads_NDOR[idx];

    idx = PRED_SNAME.indexOf(PREDname);
    if (idx == -1) {
        return list;
    }
    QString PREDLOC = PRED_LOCATION[idx];

    idx = Station_NAME.indexOf(STATIONname);
    if (idx == -1) {
        return list;
    }
    QString STATIONCOD = Station_SCOD[idx];

    for (unsigned int i = 0; i < StationPaths_NDOR.size(); i++) {
        if ((StationPaths_NDOR[i] == NDOR) && (StationPaths_PRED[i] == PREDLOC) && (StationPaths_SCOD[i] == STATIONCOD)) {
            list.append(StationPaths_IDPUT[i]);
        }
    }

    return list;
}

// Получение номеров путей станционного пути по Дороге, ПЧ и направлению
QStringList EK_ASUI::GetSTATIONSPATH_NPUTs_by_RR_PRED_STATION(const QString& RRname, const QString& PREDname, const QString& STATIONname)
{
    QStringList list;

    int idx = RailRoads_NAME.indexOf(RRname);
    if (idx == -1) {
        return list;
    }
    int NDOR = RailRoads_NDOR[RailRoads_NAME.indexOf(RRname)];

    idx = PRED_SNAME.indexOf(PREDname);
    if (idx == -1) {
        return list;
    }
    QString PREDLOC = PRED_LOCATION[idx];

    idx = Station_NAME.indexOf(STATIONname);
    if (idx == -1) {
        return list;
    }
    QString STATIONCOD = Station_SCOD[idx];

    size_t size = StationPaths_NDOR.size();
    for (size_t i = 0; i < size; i++) {
        if ((StationPaths_NDOR[i] == NDOR) && (StationPaths_PRED[i] == PREDLOC) && (StationPaths_SCOD[i] == STATIONCOD)) {
            list.append(StationPaths_NPUT[i]);
        }
    }

    return list;
}

// Получение парков станционного пути по Дороге, ПЧ и направлению
QStringList EK_ASUI::GetSTATIONSPATH_PARKs_by_RR_PRED_STATION(const QString& RRname, const QString& PREDname, const QString& STATIONname)
{
    QStringList list;

    int idx = RailRoads_NAME.indexOf(RRname);
    if (idx == -1) {
        return list;
    }
    int NDOR = RailRoads_NDOR[RailRoads_NAME.indexOf(RRname)];

    idx = PRED_SNAME.indexOf(PREDname);
    if (idx == -1) {
        return list;
    }
    QString PREDLOC = PRED_LOCATION[idx];

    idx = Station_NAME.indexOf(STATIONname);
    if (idx == -1) {
        return list;
    }
    QString STATIONCOD = Station_SCOD[idx];

    for (unsigned int i = 0; i < StationPaths_NDOR.size(); i++) {
        if ((StationPaths_NDOR[i] == NDOR) && (StationPaths_PRED[i] == PREDLOC) && (StationPaths_SCOD[i] == STATIONCOD)) {
            list.append(StationPaths_PARK[i]);
        }
    }

    return list;
}

// Получение идентификатора станционного пути по Дороге, ПЧ, направлению и номеру пути
QString EK_ASUI::GetSTATIONSPATH_ID_by_RR_PRED_STATION_PATH(const QString& RRname, const QString& PREDname, const QString& STATIONname, const QString& PATH)
{
    int idx = RailRoads_NAME.indexOf(RRname);
    if (idx == -1) {
        return "";
    }
    int NDOR = RailRoads_NDOR[RailRoads_NAME.indexOf(RRname)];

    idx = PRED_SNAME.indexOf(PREDname);
    if (idx == -1) {
        return "";
    }
    QString PREDLOC = PRED_LOCATION[idx];

    idx = Station_NAME.indexOf(STATIONname);
    if (idx == -1) {
        return "";
    }
    QString STATIONCOD = Station_SCOD[idx];

    for (unsigned int i = 0; i < StationPaths_NDOR.size(); i++) {
        if ((StationPaths_NDOR[i] == NDOR) && (StationPaths_PRED[i] == PREDLOC) && (StationPaths_SCOD[i] == STATIONCOD) && (StationPaths_NPUT[i] == PATH)) {
            return StationPaths_IDPUT[i];
        }
    }
    return "";
}

// Получение идентификатора парка станционного пути по Дороге, ПЧ, направлению и номеру пути
QString EK_ASUI::GetSTATIONSPATH_PARK_by_RR_PRED_STATION_PATH(const QString& RRname, const QString& PREDname, const QString& STATIONname, const QString& PATH)
{
    int idx = RailRoads_NAME.indexOf(RRname);
    if (idx == -1) {
        return "";
    }
    int NDOR = RailRoads_NDOR[RailRoads_NAME.indexOf(RRname)];

    idx = PRED_SNAME.indexOf(PREDname);
    if (idx == -1) {
        return "";
    }
    QString PREDLOC = PRED_LOCATION[idx];

    idx = Station_NAME.indexOf(STATIONname);
    if (idx == -1) {
        return "";
    }
    QString STATIONCOD = Station_SCOD[idx];

    for (unsigned int i = 0; i < StationPaths_NDOR.size(); i++) {
        if ((StationPaths_NDOR[i] == NDOR) && (StationPaths_PRED[i] == PREDLOC) && (StationPaths_SCOD[i] == STATIONCOD) && (StationPaths_NPUT[i] == PATH)) {
            return StationPaths_PARK[i];
        }
    }
    return "";
}

// --------------------------------------------------------------------------

QStringList EK_ASUI::GetDEFECTshortNames()  // Получение коротких названий дефектов
{
    return DEF_shortName;
    /*
        QStringList list;

        for (unsigned int idx = 0; idx < DEF_shortName.size(); idx++)
            list.append("ra_id: " + QString::number(DEF_RAid[idx]) +
                        " EKASUI_id: " + QString::number(DEF_id[idx]) +
                        " " + DEF_shortName[idx] +
                        " " + DEF_fullName[idx]);
        return list;
    */
}

QStringList EK_ASUI::GetDEFECTdataBYshortName(const QString& name)  // Получение инфомации о дефекте по короткму названию
{
    QStringList list;
    int idx = DEF_shortName.indexOf(name);
    if (idx != -1) {
        list.append(QString::number(DEF_RAid[idx]));
        list.append(QString::number(DEF_id[idx]));
        list.append(DEF_fullName[idx]);
    }
    return list;
}

/*
QStringList EK_ASUI::GetDEFECTdataBYshortName(QString name) // Получение инфомации о станционном пути дефекте по короткму названию
{
    QStringList list;
    int idx = DEF_shortName.indexOf(name);
    if (idx != -1) {

        list.append(QString::number(DEF_RAid[idx]));
        list.append(QString::number(DEF_id[idx]));
        list.append(DEF_fullName[idx]);
    }
    return list;
}
*/
