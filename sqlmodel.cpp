#include "sqlmodel.h"
#include "ui_sqlmodel.h"

#include <QSqlQuery>
#include <QDebug>
#include <QMessageBox>
#include<QSqlError>
#include <QFile>
#include <QFileDevice>
#include <QDir>
#include <QXmlStreamWriter>
#include <QTimer>

sqlmodel::sqlmodel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sqlmodel)
{
    this->setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);  //禁用最大化窗口
    //this->setFixedSize(this->width(),this->height());                     // 禁止拖动窗口大小

    current_date_time =QDate::currentDate();
    current_date =current_date_time.toString("yyyy-MM-dd");


    num=1;
    total=0;
    n=0;

    QString path3=QDir::currentPath();
    text1 = path3+"/config/db.ini";
    text2 = path3+"/config/item.xml";


    model = new QStandardItemModel();


    bool sqlflag=sqlConnection();



    if(sqlflag==true)
    {
        ui->setupUi(this);

        ui->dateEdit->setDate(current_date_time);
        ui->dateEdit->setCalendarPopup(true);

        todayStr=current_date_time.toString("yyyy-MM-dd");
        afterStr=current_date_time.addDays(1).toString("yyyy-MM-dd");

        QSqlQuery query(db);

        lastId=0;

        QString sql0="select top 1 [测试信息表].[序号],[测试信息表].[浓度] from [测试信息表]";
        sql0+=" where [测试信息表].[检验日期]<\'"+todayStr+"\'";
        sql0+=" order by [测试信息表].[序号] desc";

        qDebug()<<sql0;
        query.exec(sql0);

       if(query.next())
        {
           lastStr=query.value(0).toString();
           lastConcen=query.value(1).toString();

           lastId=lastStr.toInt();

           qDebug()<<lastStr<<lastId<<lastConcen;
         }

        for(int i=0;i<MAXROW;i++)
        {
          for(int j=0;j<MAXCOL;j++)
               selectData[i][j]="\0";
        }



        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(selectCurrent())); // ***就是你所说的响应函数
        timer->start(10000); // 每隔1s*/
       selectCurrent();
/*
        selectAll(current_date_time);

        selectView();

        saveData(current_date_time);
*/
        db.close();

   }


//    lisConnection();
//    sendData(current_date_time);

}

sqlmodel::~sqlmodel()
{
    delete ui;

    delete dom;
   // dom = NULL ;

    db.close();
    lisdb.close();
}


bool sqlmodel::sqlConnection()
{


    configIniRead = new QSettings(text1, QSettings::IniFormat);
       //将读取到的ini文件保存在QString中，先取值，然后通过toString()函数转换成QString类型
    QString HostName = configIniRead->value("ip").toString();
    QString DatabaseName= configIniRead->value("database").toString();
    QString UserName = configIniRead->value("username").toString();
    QString Password = configIniRead->value("password").toString();

    //qDebug()<<HostName<<DatabaseName<<UserName<<Password;


    if(HostName=="" || UserName=="" || Password==""){
        QMessageBox::warning(this,"警告!","数据库信息错误，请检查!", QMessageBox::Yes);
        this->show();
        return false;
    }else {

       // qDebug()<<HostName<<DatabaseName<<UserName<<Password;

        if(QSqlDatabase::contains("qt_sql_default_connection"))
        {
             db = QSqlDatabase::database("qt_sql_default_connection");
        }
        else
        {
              db = QSqlDatabase::addDatabase("QODBC");
        }

/*
        db.setHostName(HostName);
        db.setDatabaseName(DatabaseName);
        db.setUserName(UserName);
        db.setPassword(Password);*/
        db.setDatabaseName(QString("DRIVER={SQL SERVER};"
                           "SERVER=%1;"
                           "DATABASE=%2;"
                           "UID=%3;"
                           "PWD=%4;").arg(HostName)
                   .arg(DatabaseName)
                   .arg(UserName)
                   .arg(Password));
           db.open();
        if(db.isOpenError()){
            QMessageBox::warning(this,"警告！","数据库打开失败，请检查！", QMessageBox::Yes);
            return false;
        }
        //在Qt数据库连接后，运行"SET NAMES 'UTF8'"语句或"SET NAMES 'GBK'"。
        else{
            db.open();


        }
    }

    db.exec("SET NAMES 'UTF8'");


    return true;
}

bool sqlmodel::lisConnection()
{
    configIniRead = new QSettings(text1, QSettings::IniFormat);
       //将读取到的ini文件保存在QString中，先取值，然后通过toString()函数转换成QString类型
    QString LisIp = configIniRead->value("lisip").toString();
    QString LisDB=  configIniRead->value("lisdb").toString();
    QString LisUN =configIniRead->value("lisun").toString();
    QString LisPWD= configIniRead->value("lispwd").toString();

    machineCode=configIniRead->value("machineCode").toString();

    //qDebug()<<LisIp<<LisDB<<LisUN<<LisPWD;

    if(LisIp=="" || LisUN=="" || LisPWD==""){
        QMessageBox::warning(this,"警告!","数据库信息错误，请检查!", QMessageBox::Yes);
        this->show();
        return false;
    }else {


        if(QSqlDatabase::contains("qt_sql_default_connection"))
         {
             lisdb = QSqlDatabase::database("qt_sql_default_connection");
          }
         else
         {
              lisdb = QSqlDatabase::addDatabase("QODBC");
         }
      /*  lisdb.setHostName(LisIp);
        lisdb.setDatabaseName(LisDB);
        lisdb.setUserName(LisUN);
        lisdb.setPassword(LisPWD);*/
        lisdb.setDatabaseName(QString("DRIVER={SQL SERVER};"
                           "SERVER=%1;"
                           "DATABASE=%2;"
                           "UID=%3;"
                           "PWD=%4;").arg(LisIp)
                   .arg(LisDB)
                   .arg(LisUN)
                   .arg(LisPWD));
           lisdb.open();


        // QSqlQuery query(lisdb);
        if(!lisdb.open())
        {
            QMessageBox::warning(this,"警告！","lis数据库打开失败，请检查！", QMessageBox::Yes);
            return false;
        }
        //在Qt数据库连接后，运行"SET NAMES 'UTF8'"语句或"SET NAMES 'GBK'"。
        else{
            lisdb.open();


        }
    }
    lisdb.exec("SET NAMES 'UTF8'");



    return true;
}

void sqlmodel::selectCurrent()
{
 /*   todayStr=current_date_time.toString("yyyy-MM-dd");
    afterStr=current_date_time.addDays(1).toString("yyyy-MM-dd");

    QSqlQuery query(db);

    lastId=0;

    QString sql0="select top 1 [测试信息表].[序号],[测试信息表].[浓度] from [测试信息表]";
    sql0+=" where [测试信息表].[检验日期]<\'"+todayStr+"\'";
    sql0+=" order by [测试信息表].[序号] desc";

    qDebug()<<sql0;
    query.exec(sql0);

    if(query.next())
    {
       lastStr=query.value(0).toString();
        lastConcen=query.value(1).toString();

        lastId=lastStr.toInt();

        qDebug()<<lastStr<<lastId<<lastConcen;
    }

    for(int i=0;i<MAXROW;i++)
    {
        for(int j=0;j<MAXCOL;j++)
             selectData[i][j]="\0";
    }

*/


         QString s="";
         //int n=0;
       //  QSqlQuery query(db);
        //while(1)
       // {
            QString sql="select top 1 [测试信息表].[序号],[测试信息表].[浓度] from [测试信息表] where [测试信息表].[检验日期]>=\'"+todayStr+"\'";
            sql+=" and [测试信息表].[检验日期]<\'"+afterStr+"\' order by [测试信息表].[序号] desc";

            qDebug()<<sql<<"aaa";

            sqlConnection();
            QSqlQuery query(db);
            query.exec(sql);

            if(query.next())
            {
                numberStr=query.value(0).toString();
                lastConcen=query.value(1).toString();

                qDebug()<<numberStr<<lastConcen;
            }
            numberId=numberStr.toInt();

            //numberId=10;
            //lastConcen="5.9";
            if(numberId>lastId)
            {
                if(lastConcen!="")
                {
                    QString sql1="select [样本信息表].[样本条码],[测试信息表].[项目序号],[测试信息表].[浓度] from [样本信息表],[测试信息表] where [测试信息表].[检验日期]>=\'"+todayStr+"\'";
                    sql1+=" and [测试信息表].[检验日期]<\'"+afterStr+"\'";
                    sql1+=" and [样本信息表].[序号]=[测试信息表].[样本序号]";
                    sql1+=" and [测试信息表].[序号]>\'"+lastStr+"\'";
                    qDebug()<<sql1;

                    query.exec(sql1);
                    while(query.next())
                    {
                        QString str0=query.value(0).toString();
                        if(str0.length()==1){
                            str0="00"+str0;
                        }else if(str0.length()==2){
                            str0="0"+str0;
                        }

                        QString str1;
                        str1=query.value(1).toString();
                        str1=getXmlValue(str1);

                        QString str2,str3;
                        str2=query.value(2).toString();
                        int len=str2.length();
                        QChar ch = str2.at(len-1);
                        if(ch>=49&&ch<=57)
                        {
                            str3=str2;
                        }else{
                            str3=str2.left(len-1);
                        }
                        selectData[n][0]=str0;
                        selectData[n][1]=str1;
                        selectData[n][2]=str3;

                     // qDebug()<<selectData[n][0]<<selectData[n][1]<<selectData[n][2];

                      /*  QString pattern("^00[1-9]|0[1-9][0-9]|[1-9][0-9][0-9]$");
                        QRegExp rx(pattern);

                        bool match = rx.exactMatch(str0);

                        if(selectData[n][1]==""||selectData[n][2]==""||match==false)
                        {

                        }else{
                            //lisConnection();
                           // sendOne(current_date_time,str0,str1,str2);
                           // lisdb.close();
                            if(s==str0)
                            {
                                ui->labelnum->setText(QString::number(num));  //显示上传数据个数
                               //this->update();
                             }else
                             {
                                 num++;
                                 s=str0;
                                 ui->labelnum->setText(QString::number(num));  //显示上传数据个数
                              }
                        }*/


                        for(int m=n;m>0;m--){
                           if(QString::compare(selectData[m][0],selectData[m-1][0])<0)
                            {
                               QString t0=selectData[m][0];
                               QString t1=selectData[m][1];
                               QString t2=selectData[m][2];

                               selectData[m][0]=selectData[m-1][0];
                               selectData[m][1]=selectData[m-1][1];
                               selectData[m][2]=selectData[m-1][2];

                               selectData[m-1][0]=t0;
                               selectData[m-1][1]=t1;
                               selectData[m-1][2]=t2;
                             }

                         }//for
              //          qDebug()<<selectData[n][0]<<selectData[n][1]<<selectData[n][2];
                        selectView(n);
                        saveData(current_date_time,n);
                        n++;
                       // sqlConnection();
                    }//while

                 /*   for(int k=0;k<n;k++)
                    {
                        qDebug()<<selectData[k][0]<<selectData[k][1]<<selectData[k][2];
                    }
                   */
               }//if(lastConcen)
            }//if(lastId)
            lastId=numberId;
          //  hh=QTime::currentTime().hour();
           // sleep(10000);
   //     }

            lisConnection();
            sendData(current_date_time);



      // return true;

}

bool sqlmodel::selectAll(const QDate& date)
{
    todayStr=date.toString("yyyy-MM-dd");
    afterStr=date.addDays(1).toString("yyyy-MM-dd");

    for(int i=0;i<MAXROW;i++)
    {
        for(int j=0;j<MAXCOL;j++)
             selectData[i][j]="\0";
    }

    QString sql="select [样本信息表].[样本条码],[测试信息表].[项目序号],[测试信息表].[浓度] from [样本信息表],[测试信息表] where [测试信息表].[检验日期]>=\'"+todayStr+"\'";
    sql+=" and [测试信息表].[检验日期]<\'"+afterStr+"\'";
    sql+=" and [样本信息表].[序号]=[测试信息表].[样本序号]";

    //qDebug()<<sql;

    QSqlQuery query(db);
    query.exec(sql);

    total=query.numRowsAffected();

    //qDebug()<<total;

    for(int i=0;i<total;i++)
    {
          if(query.next())
            {
                selectData[i][0]=query.value(0).toString();
                QString str0=selectData[i][0];
                if(str0.length()==1){
                    selectData[i][0]="00"+str0;
                }else if(str0.length()==2){
                    selectData[i][0]="0"+str0;
                }



                //selectData[i][1]=query.value(1).toString();

                QString str2;
                str2=query.value(1).toString();
                //selectData[i][1]=itemIdConvert(str2);
                selectData[i][1]=getXmlValue(str2);

                selectData[i][2]=query.value(2).toString();

                QString str=selectData[i][2];
                int len=str.length();
                QChar ch = str.at(len-1);
                if(ch>=49&&ch<=57)
                {
                    selectData[i][MAXCOL-1]=str;
                }else{
                    selectData[i][MAXCOL-1]=str.left(len-1);
                }

            }
            else
            {
                for(int j=0;j<MAXCOL;j++)
                {
                selectData[i][j]="\0";
                }
            }

    }

   for(int k=0;k<total;k++)
    {

        for(int m=k+1;m<total;m++){
            if(QString::compare(selectData[k][0],selectData[m][0])>0)
            {
                QString t0=selectData[k][0];
                QString t1=selectData[k][1];
                QString t2=selectData[k][2];

                selectData[k][0]=selectData[m][0];
                selectData[k][1]=selectData[m][1];
                selectData[k][2]=selectData[m][2];

                selectData[m][0]=t0;
                selectData[m][1]=t1;
                selectData[m][2]=t2;
            }

        }
      // qDebug()<<selectData[k][0]<<selectData[k][1]<<selectData[k][2];
    }

   return true;
}

QString sqlmodel::getXmlValue(const QString &key)
{
   // QFile file(text2);
    QFile *file ;
    file= new QFile(text2);
    QString type = "xml";
    QString result = "";



    if(file->open(QFile::ReadOnly | QFile::Text ))
    {
     dom = new QDomDocument();
     dom->setContent(file);

       QDomNodeList nodelist = dom->elementsByTagName(type);    /*< 读取类型节点集合 */

       for(int i=0; i<nodelist.count(); i++){
         QDomNode node = nodelist.at(i);
         QDomNodeList itemlist = node.childNodes(); /* 获取字节点集合 */
         for(int j=0; j<itemlist.count(); j++){
              QDomNode mynode = itemlist.at(j);
          if(mynode.toElement().attribute("id") == key){ /* 查找所需要的键值 */
             result = mynode.toElement().attribute("value");
             break;
           }
         }
       }
      }
    else
    {
        qDebug()<<"aa";
    }
      // qDebug()<<text2<<result;

       return result;
}

bool sqlmodel::selectOne(const QDate &date, const QString &ybh)
{
    todayStr=date.toString("yyyy-MM-dd");
    afterStr=date.addDays(1).toString("yyyy-MM-dd");

    for(int i=0;i<MAXROW;i++)
    {
        for(int j=0;j<MAXCOL;j++)
             selectData[i][j]="\0";
    }
    int k;
    k=ybh.toInt();
    QString str=QString::number(k);


    QString sql="select [样本信息表].[样本条码],[测试信息表].[项目序号],[测试信息表].[浓度] from [样本信息表],[测试信息表] where [测试信息表].[检验日期]>=\'"+todayStr+"\'";
    sql+=" and [测试信息表].[检验日期]<\'"+afterStr+"\'";
    sql+=" and [样本信息表].[样本条码]=\'"+str+"\'";
    sql+=" and [样本信息表].[序号]=[测试信息表].[样本序号] ";



    QSqlQuery query(db);
    query.exec(sql);

    total=query.numRowsAffected();

    //qDebug()<<total;

    for(int i=0;i<total;i++)
    {
          if(query.next())
            {
              selectData[i][0]=query.value(0).toString();
              QString str0=selectData[i][0];
              if(str0.length()==1){
                  selectData[i][0]="00"+str0;
              }else if(str0.length()==2){
                  selectData[i][0]="0"+str0;
              }


              QString str2;
              str2=query.value(1).toString();
              //selectData[i][1]=itemIdConvert(str2);
              selectData[i][1]=getXmlValue(str2);


              selectData[i][2]=query.value(2).toString();

              QString str=selectData[i][2];
              int len=str.length();
              QChar ch = str.at(len-1);
              if(ch>=49&&ch<=57)
              {
                  selectData[i][MAXCOL-1]=str;
              }else{
                  selectData[i][MAXCOL-1]=str.left(len-1);
              }

          }
          else
          {
              for(int j=0;j<MAXCOL;j++)
              {
                selectData[i][j]="\0";
              }
          }
    }

   return true;
}

void sqlmodel::selectView(int sampleNum)
{
   // QStandardItemModel  *model = new QStandardItemModel();

    model->setColumnCount(3);
    model->setHeaderData(0,Qt::Horizontal,"样本编号");
    model->setHeaderData(1,Qt::Horizontal,"项目序号");
    model->setHeaderData(2,Qt::Horizontal,"浓度");


    ui->tableView->setModel(model);
    //表头信息显示居左
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    //设置列宽不可变
    ui->tableView->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Fixed);
    ui->tableView->setColumnWidth(0,90);
    ui->tableView->setColumnWidth(1,90);
    ui->tableView->setColumnWidth(2,90);


    for(int i = 0; i < sampleNum; i++)
     {
         model->setItem(i,0,new QStandardItem(selectData[i][0]));
         QString str=selectData[i][0];
            //设置字符颜色
         //model->item(i,0)->setForeground(QBrush(QColor(255, 0, 0)));
            //设置字符位置
         model->item(i,0)->setTextAlignment(Qt::AlignCenter);
         model->setItem(i,1,new QStandardItem(selectData[i][1]));
         model->item(i,1)->setTextAlignment(Qt::AlignCenter);
         model->setItem(i,2,new QStandardItem(selectData[i][2]));
         model->item(i,2)->setTextAlignment(Qt::AlignCenter);

         if(selectData[i][1]==""||selectData[i][2]=="")
         {
             // 行背景色
             model->item(i,0)->setBackground(QBrush(QColor(0,255,255)));
             model->item(i,1)->setBackground(QBrush(QColor(0,255,255)));
             model->item(i,2)->setBackground(QBrush(QColor(0,255,255)));
         }
         else
         {
             // 行背景色
             model->item(i)->setBackground(QBrush(QColor(255, 255, 255)));
             //ui->tableView->setStyleSheet("QTableView::Item{background-color:#000000}");
         }
         /*

         if(str.length()>3)//||(str.at(0)<"0"&&str.at(0)>"9")||(str.at(1)<"0"&&str.at(1)>"9")||(str.at(2)<"0"&&str.at(2)>"9"))
         {
           //  行背景色
           model->item(i,0)->setBackground(QBrush(QColor(0,255,255)));
           model->item(i,1)->setBackground(QBrush(QColor(0,255,255)));
           model->item(i,2)->setBackground(QBrush(QColor(0,255,255)));
         }*/
         QString pattern("^00[1-9]|0[1-9][0-9]|[1-9][0-9][0-9]$");
         QRegExp rx(pattern);

         bool match = rx.exactMatch(str);
         //qDebug() << match;
         if(match==false)
         {
             // 行背景色
             model->item(i,0)->setBackground(QBrush(QColor(0,255,255)));
             model->item(i,1)->setBackground(QBrush(QColor(0,255,255)));
             model->item(i,2)->setBackground(QBrush(QColor(0,255,255)));
         }
         else
         {
             // 行背景色
             model->item(i)->setBackground(QBrush(QColor(255, 255, 255)));
         }


        // model->setData()
     }

}

bool sqlmodel::saveData(const QDate& date,const int& sampleNum)
{
    QString path3=QDir::currentPath();
    QString fileName= path3+"/data/";
    QDir dir(fileName);
    if(!dir.exists())
        dir.mkdir(fileName);

   // fileName+=date.toString("yyyyMMdd")+".txt";
    fileName+=date.toString("yyyyMMdd")+".xml";
    QFile file(fileName);


    if(file.open(QFile::WriteOnly | QFile::Text ))
     {
       // QTextStream out(&file);

        QXmlStreamWriter writer(&file);
        writer.setAutoFormatting(true);
        writer.writeStartDocument();

        //ui->progressBar->setRange(0,total);

        //QString s=selectData[0][0];

        writer.writeStartElement("xml");

        for(int i=0;i<sampleNum;i++)
         {
            QString str=selectData[i][0];

            QString pattern("^00[1-9]|0[1-9][0-9]|[1-9][0-9][0-9]$");
            QRegExp rx(pattern);

            bool match = rx.exactMatch(str);

            if(selectData[i][1]==""||selectData[i][2]==""||match==false)
            {

            }else{
                writer.writeStartElement("sample");
                writer.writeTextElement("sampleId",selectData[i][0]);
                writer.writeTextElement("itemCode",selectData[i][1]);
                writer.writeTextElement("concentration",selectData[i][2]);
                writer.writeEndElement();

            }

            /* if(s==selectData[i][0]){
                 ui->labelnum->setText(QString::number(num));  //显示上传数据个数
                   //this->update();
             }else
             {
                 num++;
                 s=selectData[i][0];
                 ui->labelnum->setText(QString::number(num));  //显示上传数据个数
             }*/
             //ui->progressBar->setValue(i+1);
         }

        writer.writeEndElement();
        writer.writeEndDocument();
     }

     file.flush();
     file.close();


     return true;

}
void sqlmodel::sendOne(const QDate &date,const QString& sampleId, const QString& itemCode,const QString& concentration)
{
    todayStr=date.toString("yyyy-MM-dd");
    afterStr=date.addDays(1).toString("yyyy-MM-dd");

    QString itemid;

    QString sql1="select itemid,itemcode from T_EXAMITEM  where machinecode=\'"+machineCode+"\'";
    sql1+=" and itemcode=\'"+itemCode+"\'";

    //qDebug()<<sql1;
    QSqlQuery query(lisdb);
    query.exec(sql1);

    while(query.next())
    {
        itemid=query.value(0).toString();
       // qDebug()<<query.value(0).toString();
    }
    QString sql2="update T_SAMPLEITEM set checkvalue="+concentration;
    sql2+=" where itemid="+itemid+" and samplecode="+sampleId;
    sql2+=" and RECEIVEDATE=\'"+todayStr+"\'";

    qDebug()<<sql2;
    query.exec(sql2);   /***********执行上传数据**************/
}

bool sqlmodel::sendData(const QDate &date)
{
    todayStr=date.toString("yyyy-MM-dd");
    afterStr=date.addDays(1).toString("yyyy-MM-dd");

    QString itemid;
    QString sampleId;
    QString itemCode;
    QString concentration;

    QString path3=QDir::currentPath();
    QFile *file ;
    QString fileName= path3+"/data/";
    fileName+=date.toString("yyyyMMdd")+".xml";
    file= new QFile(fileName);
    QString type = "xml";
    QString result = "";



    if(file->open(QFile::ReadOnly | QFile::Text ))
    {
     dom = new QDomDocument();
     dom->setContent(file);


     QDomNodeList nodelist = dom->elementsByTagName(type);

     ui->progressBar->setRange(0,nodelist.count());

       for(int i=0; i<nodelist.count(); i++){
         QDomNode node = nodelist.at(i);
         QDomNodeList itemlist = node.childNodes();

         QString s=itemlist.at(0).childNodes().at(0).firstChild().nodeValue();

         for(int j=0; j<itemlist.count(); j++){
              QDomNode mynode = itemlist.at(j);
              QDomNodeList samplelist =mynode.childNodes();
              sampleId= samplelist.at(0).firstChild().nodeValue();
              itemCode= samplelist.at(1).firstChild().nodeValue();
              concentration= samplelist.at(2).firstChild().nodeValue();

              //qDebug()<<sampleId<<itemCode<<concentration;
              QString sql1="select itemid,itemcode from T_EXAMITEM  where machinecode=\'"+machineCode+"\'";
              sql1+=" and itemcode=\'"+itemCode+"\'";

              //qDebug()<<sql1;
              QSqlQuery query(lisdb);
              query.exec(sql1);

              while(query.next())
              {
                  itemid=query.value(0).toString();
                 // qDebug()<<query.value(0).toString();
              }
/*
              QString sql3="select count(*) from T_SAMPLEITEM";
              sql3+=" where itemid="+itemid+" and samplecode="+sampleId;
              sql3+=" and RECEIVEDATE=\'"+todayStr+"\'";

              int vcount;
              query.exec(sql3);

              while(query.next())
              {
                  vcount=query.value(0).toInt();
                 // qDebug()<<query.value(0).toString();
              }
*/

              QString sql2="update T_SAMPLEITEM set checkvalue="+concentration;
              sql2+=" where itemid="+itemid+" and samplecode="+sampleId;
              sql2+=" and RECEIVEDATE=\'"+todayStr+"\'";

              qDebug()<<sql2;
           //  query.exec(sql2);   /***********执行上传数据**************/

              if(s==sampleId){
                    ui->labelnum->setText(QString::number(num));  //显示上传数据个数
                  //this->update();
                }else
                {
                   num++;
                   s=sampleId;
                   ui->labelnum->setText(QString::number(num));  //显示上传数据个数
                }
               ui->progressBar->setValue(j+1);

           }
         }
       }
    num=1;
/*

    for(int i=0;i<total;i++)
    {
    QString sql1="select itemid,itemcode from T_EXAMITEM  where machinecode=\'"+machineCode+"\'";
    sql1+=" and itemcode=\'"+selectData[i][1]+"\'";

   // qDebug()<<sql1;

    QSqlQuery query(lisdb);
    query.exec(sql1);

    while(query.next())
    {
        itemid=query.value(0).toString();
        //qDebug()<<query.value(0).toString()<<query.value(1).toString();
    }

    QString sql2="update T_SAMPLEITEM set checkvalue="+selectData[i][2];
    sql2+=" where itemid="+itemid+" and samplecode="+selectData[i][0];
    sql2+=" and RECEIVEDATE=\'"+todayStr+"\'";

    //qDebug()<<sql2;
    query.exec(sql2);

    }

*/
    return true;

}

void sqlmodel::on_btnSelect_clicked()
{
    selectdate=ui->dateEdit->date();

    model->clear();

    if(selectdate==current_date_time){
       // QMessageBox::warning(this, "warning", "请不要选择当天日期", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        selectCurrent();
        return;
    }

    if(selectCheckBox==true)
    {
        yangbenhao=ui->ldtyangben->text();

        QString pattern("^00[1-9]|0[1-9][0-9]|[1-9][0-9][0-9]$");
        QRegExp rx(pattern);

        bool match = rx.exactMatch(yangbenhao);
        //qDebug() << match;
        if(match==false)
        {
            QMessageBox::warning(this, "warning", "请输入三位数字样本号", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            return;
        }


        /*
        if(yangbenhao=="")
        {
            QMessageBox::warning(NULL, "warning", "请输入样本号", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            return;
        }
        else if(yangbenhao.length()!=3){
            QMessageBox::warning(NULL, "warning", "请输入三位样本号", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            return;
        }*/

        //qDebug()<<yangbenhao;
        if(sqlConnection())
        {
         selectOne(selectdate,yangbenhao);
        //qDebug()<<total;
         selectView(total);
         saveData(selectdate,total);
        }
        db.close();

    }
    else
    {

        if(sqlConnection())
        {
        selectAll(selectdate);
        selectView(total);
        saveData(selectdate,total);
        }
        db.close();
    }

}


void sqlmodel::on_ckByb_clicked()
{
    selectCheckBox=ui->ckByb->isChecked();
    if(selectCheckBox==true)
    {
      ui->ldtyangben->setEnabled(true);
    }else{
        ui->ldtyangben->setEnabled(false);
    }
}


void sqlmodel::on_btnSend_clicked()
{
    if(lisConnection())
    sendData(selectdate);
}
