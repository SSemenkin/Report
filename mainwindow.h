#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDebug>
#include <QSqlError>
#include <QSqlTableModel>
#include <QtSql>
#include <QStringList>
#include <QSqlDriverPlugin>
#include <QIntValidator>
#include <QMessageBox>
#include <QFileDialog>
#include <QVector>
#include <QCompleter>
#include <QAbstractItemModel>
#include <QKeyEvent>
#include <QThread>
#include "xlsxdocument.h"


#include "callstat.h"
#include "analysedata.h"
#include "mylineedit.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

    void onStart();

    void DbConnect();

    QString DateB();

    void GetTime();

    void SetModel(QString);

    void SaveToFile(QString);

    QString getDatesToChart();

private slots:

    void on_showData_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_radioButton_clicked();

    void on_radioButton_5_clicked();

    void on_radioButton_4_clicked();

    void on_oneDay_clicked();

    void on_period_clicked();

    void on_radioButton_2_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pastePB_clicked();

    void onTableCopy(QModelIndex index);

    void on_cellStat_clicked();

    void abonentAnalyse(int);


private:


    Ui::MainWindow *ui;

    QSqlDatabase db;

     QAbstractItemModel *modelFromFile(const QString& fileName);

    QSqlQueryModel *model,*query_model;

    QString request_MN,requestBegin;

    QString lastSelectedAbonent;

    AnalyseData *analyse;

    bool AbonentSelected;

    bool fromCallStat=false;

    QString version = "v.2.6.";

    MyLineEdit *abonentLine;

    MyLineEdit *IMEI;

    QCompleter *completer;

    QThread *thread;

    QString sql = "ADD\r\nALL\r\nALLOCATE\r\nALTER\r\nAND\r\nANY\r\nARE\r\nAS\r\nASC\r\nASSERTION\r\nAT\r\nAUTHORIZATION\r\nAVG\r\nBEGIN\r\nBETWEEN\r\nBIGINT\r\nBIT\r\nBOOLEAN\r\nBOTH\r\nBY\r\nCALL\r\nCASCADE\r\nCASCADED\r\nCASE\r\nCAST\r\nCHAR\r\nCHARACTER\r\nCHECK\r\nCLOSE\r\nCOALESCE\r\nCOLLATE\r\nCOLLATION\r\nCOLUMN\r\nCOMMIT\r\nCONNECT\r\nCONNECTION\r\nCONSTRAINT\r\nCONSTRAINTS\r\nCONTINUE\r\nCONVERT\r\nCORRESPONDING\r\nCREATE\r\nCROSS\r\nCURRENT\r\nCURRENT_DATE\r\nCURRENT_ROLE\r\nCURRENT_TIME\r\nCURRENT_TIMESTAMP\r\nCURRENT_USER\r\nCURSOR\r\nDEALLOCATE\r\nDEC\r\nDECIMAL\r\nDECLARE\r\nDEFAULT\r\nDEFERRABLE\r\nDEFERRED\r\nDELETE\r\nDESC\r\nDESCRIBE\r\nDIAGNOSTICS\r\nDISCONNECT\r\nDISTINCT\r\nDOUBLE\r\nDROP\r\nELSE\r\nEND\r\nEND-EXEC\r\nESCAPE\r\nEXCEPT\r\nEXCEPTION\r\nEXEC\r\nEXECUTE\r\nEXISTS\r\nEXPLAIN\r\nEXTERNAL\r\nFALSE\r\nFETCH\r\nFIRST\r\nFLOAT\r\nFOR\r\nFOREIGN\r\nFOUND\r\nFROM\r\nFULL\r\nFUNCTION\r\nGET\r\nGETCURRENTCONNECTION\r\nGLOBAL\r\nGO\r\nGOTO\r\nGRANT\r\nGROUP\r\nHAVING\r\nHOUR\r\nIDENTITY\r\nIMMEDIATE\r\nIN\r\nINDICATOR\r\nINITIALLY\r\nINNER\r\nINOUT\r\nINPUT\r\nINSENSITIVE\r\nINSERT\r\nINT\r\nINTEGER\r\nINTERSECT\r\nINTO\r\nIS\r\nISOLATION\r\nJOIN\r\nKEY\r\nLAST\r\nLEFT\r\nLIKE\r\nLOWER\r\nLTRIM\r\nMATCH\r\nMAX\r\nMIN\r\nMINUTE\r\nNATIONAL\r\nNATURAL\r\nNCHAR\r\nNVARCHAR\r\nNEXT\r\nNO\r\nNONE\r\nNOT\r\nNULL\r\nNULLIF\r\nNUMERIC\r\nOF\r\nON\r\nONLY\r\nOPEN\r\nOPTION\r\nOR\r\nORDER\r\nOUTER\r\nOUTPUT\r\nOVER\r\nOVERLAPS\r\nPAD\r\nPARTIAL\r\nPREPARE\r\nPRESERVE\r\nPRIMARY\r\nPRIOR\r\nPRIVILEGES\r\nPROCEDURE\r\nPUBLIC\r\nREAD\r\nREAL\r\nREFERENCES\r\nRELATIVE\r\nRESTRICT\r\nREVOKE\r\nRIGHT\r\nROLLBACK\r\nROWS\r\nROW_NUMBER\r\nRTRIM\r\nSCHEMA\r\nSCROLL\r\nSECOND\r\nSELECT\r\nSESSION_USER\r\nSET\r\nSMALLINT\r\nSOME\r\nSPACE\r\nSQL\r\nSQLCODE\r\nSQLERROR\r\nSQLSTATE\r\nSUBSTR\r\nSUBSTRING\r\nSUM\r\nSYSTEM_USER\r\nTABLE\r\nTEMPORARY\r\nTIMEZONE_HOUR\r\nTIMEZONE_MINUTE\r\nTO\r\nTRANSACTION\r\nTRANSLATE\r\nTRANSLATION\r\nTRIM\r\nTRUE\r\nUNION\r\nUNIQUE\r\nUNKNOWN\r\nUPDATE\r\nUPPER\r\nUSER\r\nUSING\r\nVALUES\r\nVARCHAR\r\nVARYING\r\nVIEW\r\nWHENEVER\r\nWHERE\r\nWITH\r\nWORK\r\nWRITE\r\nXML\r\nXMLEXISTS\r\nXMLPARSE\r\nXMLQUERY\r\nXMLSERIALIZE\r\nYEAR\r\ntag\r\ncallingPartyNumber\r\ncalledPartyNumber\r\ndateForStartOfCharge\r\ntimeForStartOfCharge\r\nchargeableDuration\r\ncallingSubscriberIMSI\r\ncallingSubscriberIMEI\r\nfirstCallingLAC\r\nfirstCallingCellID\r\nlastCallingCellID\r\nlastCallingLAC\r\ncdr";

};

#endif // MAINWINDOW_H
