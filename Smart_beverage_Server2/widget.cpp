#include "widget.h"
#include "ui_widget.h"
#include <iostream>
#include <QtGlobal>
#include <QTime>
#include <QStandardItem>
#include <QAbstractItemModel>
#include<Qtime>
#include <QCryptographicHash>
#include<QMessageBox>

using namespace std;

int generateRandomInteger(int min, int max);
QString change_temp(int temp);
QString MD5(QString password);

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("智能饮料机服务器");
    /** 初始化数据库实例 */
    this->db = new Database(this);
    /** 连接数据库 */
    if (!this->db->connect("/Users/m-mac/qtworkspace/build-Smart_beverage_Server2-Desktop_Qt_5_13_0_clang_64bit-Debug/datebase.db"))
    {
        qDebug() << "数据库连接错误";
    }
    else
    {
        qDebug() << "数据库已连接";
        this->db->upQuery();
    }
    ui->line_password->setEchoMode(QLineEdit::Password);
    //设置背景图片
    setAutoFillBackground(true); //有些文档说加这个  ，意义未知
    QPalette palette;
    QPixmap pixmap(":/image/back.jpg");
    palette.setBrush(backgroundRole(),QBrush(pixmap));
    this->setPalette(palette);
    //设置按钮初始状态
    ui->button_close->setEnabled(false);
    ui->button_buyLog->setEnabled(false);
    ui->button_drinks->setEnabled(false);
    ui->button_userLog->setEnabled(false);
    ui->button_equipment->setEnabled(false);
}

Widget::~Widget()
{
    delete ui;
}

/** 点击开始后进入等待连接状态 **/
void Widget::on_button_start_clicked()
{
    ui->button_close->setEnabled(true);
    ui->button_buyLog->setEnabled(true);
    ui->button_drinks->setEnabled(true);
    ui->button_userLog->setEnabled(true);
    ui->button_equipment->setEnabled(true);
    ui->button_start->setEnabled(false);
    /** 开启服务器 */
    this->service = new communication(this);
    this->service->listen();
    /** 建立与客户端连接的信号槽 **/
    connect(service->getTcpServer(),&QTcpServer::newConnection,this,&Widget::newconnectionSLOT);
}

/** 点击停止后断开所有连接 **/
void Widget::on_button_close_clicked()
{
    ui->button_close->setEnabled(false);
    ui->button_buyLog->setEnabled(false);
    ui->button_drinks->setEnabled(false);
    ui->button_userLog->setEnabled(false);
    ui->button_equipment->setEnabled(false);
    ui->button_start->setEnabled(true);
    this->db->query->exec("UPDATE users SET isOnline = 0");
    this->service->write("requireOfbreaklink:");
    for(int i = 0; i < this->service->clients.size();i++){
        this->service->clients[i]->disconnectFromHost();
        this->service->clients[i]->close();
    }
}

/** 点击addAdmin的注册按钮的槽函数 **/
void Widget::on_button_register_clicked()
{
    QString name = ui->line_user->text();
    QString password = MD5(ui->line_password->text());
    bool can_register = true;
    this->db->query->exec(QString("SELECT * FROM users WHERE name = '%1'").arg(name));
    while (this->db->query->next()) {
        can_register = false;
    }
    if(can_register){
//        db->query->exec(QString("INSERT INTO users (name, password, money, type, isOnline) VALUES ('%1', '%2', '%3', %4 , %5)").arg(name).arg(password).arg("0").arg(1).arg(0));
//        this->insertUser(name,password,"0",1,0);
        db->query->exec(QString("INSERT INTO users (name, password, money, type, isOnline) VALUES ('%1', '%2', '%3', %4, %5)").arg(name).arg(password).arg("0").arg(1).arg(0));
        QMessageBox *msb = new QMessageBox;
        msb->setWindowTitle("注册提示");
        msb->setText("注册成功！");
        msb->setIcon(QMessageBox::Information);
        msb->setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        msb->show();
    }
    else {
        QMessageBox *msb = new QMessageBox;
        msb->setWindowTitle("注册提示");
        msb->setText("注册失败，用户名已经存在！");
        msb->setIcon(QMessageBox::Information);
        msb->setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        msb->show();
    }
}

/** 服务端收到新连接信号后的槽函数 **/
void Widget::newconnectionSLOT(){
    QString temp = this->service->newLink();
    ui->showMessage->append(temp);
    connect(this->service->getTcpSocket(),&QTcpSocket::readyRead,this,&Widget::readDateSLOT);
    connect(this->service->getTcpSocket(),&QTcpSocket::disconnected,this,&Widget::disconnectedSLOT);
}

/** 服务端收到断开连接信号后的槽函数 **/
void Widget::disconnectedSLOT(){
    ui->showMessage->append(this->service->breakLink());
    this->db->query->exec(QString("UPDATE users SET isOnline = 0 WHERE name = '%1'").arg(this->userName));
}

/** 服务端收到客户端请求的槽函数 **/
void Widget::readDateSLOT(){
    this->service->setTcpSocket(qobject_cast<QTcpSocket *>(sender()));
    QString temp = this->service->readAll();
    ui->showMessage->append(temp);
    dealData(temp);
}

/** 服务端收到客户端请求后的路由函数 **/
void Widget::dealData(QString data){
    if(data.split(":")[0] == "requestOflog"){
        this->comfirm_log(data);
    }
    if(data.split(":")[0] == "requestOfregister"){
        this->comfirm_register(data);
    }
    if(data.split(":")[0] == "requestOfdelete"){
        this->comfirm_delete(data);
    }
    if(data.split(":")[0] == "requestOfbuy"){
        this->comfirm_buy(data);
    }
    if(data.split(":")[0] == "requestOfflush"){
        this->service->write("requireOfflush:can_flush"+ this->send_drinks() + "#" + this->send_user(data.split(":")[1]) + "#" + this->comfirm_hot());
        if(data.split(":").size() >=3){
            if(data.split(":")[2] == "changeEquipment"){
                QString equ_id = data.split(":")[3];
                db->query->exec("UPDATE equipments SET isopen = 0");
                db->query->exec(QString("UPDATE equipments SET isopen = 1 WHERE name = '%1'").arg(equ_id));
            }
        }
    }
    if(data.split(":")[0] == "requestOfuserlog"){
        this->insertUserLog(data.split(":")[1],data.split(":")[2],data.split(":")[3],data.split(":")[4]);
        qDebug() << data.split(":")[1] << data.split(":")[2]<<data.split(":")[3]<<data.split(":")[4];
    }
    if(data.split(":")[0] == "requestOflogOff"){
        db->query->exec(QString("UPDATE users SET isOnline = 0 WHERE name = '%1'").arg(data.split(":")[1]));
    }
    if(data.split(":")[0] == "requestOfSendMessage"){
        this->service->write("requireOfSendMessage:can_send");
    }
    if(data.split(":")[0] == "requestOfRecharge"){
        this->db->query->exec(QString("UPDATE users SET money = '%1' WHERE name = '%2'").arg(QString::number(data.split(":")[2].toInt() + data.split(":")[3].toInt())).arg(data.split(":")[1]));
        this->service->write("requireOfRecharge:can_Recharge" + this->send_drinks() + "#" + this->send_user(data.split(":")[1]) + "#" + this->comfirm_hot());
    }
}

/** 处理客户端发来的注册请求 **/
void Widget::comfirm_register(QString data){
    QString name = data.split(":")[1];
    QString password = data.split(":")[2];
    bool can_register = true;
    this->db->query->exec(QString("SELECT * FROM users WHERE name = '%1'").arg(name));
    while(db->query->next()){
        can_register = false;
    }
    if(can_register){
        this->insertUser(name,password,"20",0,0);
        this->service->write("requireOfregister:can_register");
    }
    else {
        this->service->write("requireOfregister:not_register");
    }
}

/** 处理客户端发来的注销请求 **/
void Widget::comfirm_delete(QString data){
    QString name = data.split(":")[1];
    QString password = data.split(":")[2];
    bool can_delete = false;
    this->db->query->exec(QString("SELECT * FROM users WHERE name = '%1' AND password = '%2'").arg(name).arg(password));
    while(db->query->next()){
        can_delete = true;
    }
    if(can_delete){
        this->deletetUser(name,password,"0",0,0);
        this->service->write("requireOfdelete:can_delete");
    }
    else {
        this->service->write("requireOfdelete:not_delete");
    }
}

/** 处理客户端发来的登陆请求 **/
void Widget::comfirm_log(QString data){
    QString name = data.split(":")[1];
    QString password = data.split(":")[2];
    bool can_log = false;
    bool change_log = false;
    QString isOline;
    if(data.split(":").size() == 4){
        change_log = true;
    }
    this->db->query->exec(QString("SELECT * FROM users WHERE name = '%1' AND password = '%2'").arg(name).arg(password));
    while(db->query->next()){
        can_log = true;
        isOline = this->db->query->value(5).toString();
    }
    if(can_log && isOline != "1"){
        this->userName = name;
        this->db->query->exec(QString("UPDATE users SET isOnline = 1 WHERE name = '%1' AND password = '%2'").arg(name).arg(password));
        this->service->write("requireOflog:can_log"+ this->send_drinks() + "#" + this->send_user(name) + "#" + this->comfirm_hot());
        QString test = "requireOflog:can_log"+ this->send_drinks() + "#" + this->send_user(name) + "#" + this->comfirm_hot();
        //当切换用户时将上一个用户的在线状态设为0
        if(change_log){
            this->db->query->exec(QString("UPDATE users SET isOnline = 0 WHERE name = '%1'").arg(data.split(":")[3]));
        }
    }
    else {
        if(isOline == "1"){
            this->service->write("requireOflog:not_log_repeat");
        }
        else {
            this->service->write("requireOflog:not_log");
        }
    }
}

/** 处理客户端发来的购买/添加请求 **/
//index = 10 为购买添加标识符
void Widget::comfirm_buy(QString data){
    //data的形式为requestOfbuy:index:nameOfdrink:equ:temp:price:num_buy:numOfuser:money:num_drinks:status
    //获得的这些数据都是之前在服务器里面get的，所以就是服务器的数据，就不需要再次从服务器获取了
    QString nameOfdrink = data.split(":")[2];
    QString equ = data.split(":")[3];
    QString temp = data.split(":")[4];
    QString price = data.split(":")[5];
    QString num_buy = data.split(":")[6];
    QString numOfuser = data.split(":")[7];
    QString money = data.split(":")[8];
    QString num_drinks = data.split(":")[9];
    QString status = data.split(":")[10];
    QString remain_drink = QString::number(num_drinks.toInt() - num_buy.toInt());
    QString after_drinks = QString::number(num_drinks.toInt() + num_buy.toInt());
//    QString str = "nameOfdrink" + nameOfdrink + "numOfuser" + numOfuser + "price" + price + "num_buy" + num_buy + "money" + money + "num_drinks" + num_drinks;
//    qDebug() << str;
    bool can_buy = false;
    bool is_open = false;
    //检查设备的开启状态
    this->db->query->exec(QString("SELECT * FROM equipments WHERE name = '%1' AND isOpen = '1'").arg(equ));
    while (this->db->query->next()) {
        is_open = true;
    }
    //服务器端每次收到添加或者购买请求的时候，就会监控所有的饮料数量
    if(is_open){
        if(status == "buy"){
            if(num_buy.toInt() <= num_drinks.toInt()  && num_drinks.toInt() != 0 && num_buy.toInt() != 0){
                if(num_buy.toInt() * price.toInt() <= money.toInt()){
                    //库存够，钱够，可以买
                    can_buy = true;
                    qDebug() << "可以买！！！";
                    //进行服务端数据库user的更新
                    this->db->query->exec(QString("UPDATE users SET money = '%1' WHERE name = '%2'").arg(QString::number(money.toInt() - num_buy.toInt() * price.toInt())).arg(numOfuser));
                    //进行服务端数据库drinks的更新
                    this->db->query->exec(QString("UPDATE drinks SET num%1 = '%2' WHERE name = '%3' AND equipment_id = '%4'").arg(temp).arg(remain_drink).arg(nameOfdrink).arg(equ));
                    //进行服务端数据库buylog的更新
                    this->db->query->exec(QString("INSERT INTO buyLog (userName, drinkrName, equipment, tempture, type,creat_time,message,numOfdrink)"
                                                  "VALUES ('%1', '%2', '%3', '%4', '%5','%6','%7','%8')").arg(numOfuser).arg(nameOfdrink).arg(equ).arg(temp).arg("1").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh.mm")).arg(numOfuser+":buy the drinks"+nameOfdrink).arg(num_buy));
                    //让客户端收到可以购买的指令并且刷新购买购买界面
                    this->service->write("requireOfbuy:can_buy"+ this->send_drinks() + "#" + this->send_user(numOfuser) + "#" + this->comfirm_hot());
                }
            }
            if(!can_buy){
                //让客户端收到不可以购买的指令
                this->service->write("requireOfbuy:not_buy"+ this->send_drinks() + "#" + this->send_user(numOfuser) + "#" + this->comfirm_hot());
                qDebug() << "不可以买！！！";
            }
        }
        else if(status == "add" && num_buy != "0"){
            //进行服务端数据库drinks的更新
            this->db->query->exec(QString("UPDATE drinks SET num%1 = '%2' WHERE name = '%3' AND equipment_id = '%4'").arg(temp).arg(after_drinks).arg(nameOfdrink).arg(equ));
            //进行服务端数据库buyLog的更新
            this->db->query->exec(QString("INSERT INTO buyLog (userName, drinkrName, equipment, tempture, type,creat_time,message,numOfdrink)"
                                          "VALUES ('%1', '%2', '%3', '%4', '%5','%6','%7','%8')").arg(numOfuser).arg(nameOfdrink).arg(equ).arg(temp).arg("2").arg(QDateTime::currentDateTime().toString("yyyy.MM.dd hh.mm")).arg(numOfuser+":buy the drinks"+nameOfdrink).arg(num_buy));
            //让客户端收到可以添加的指令并且刷新购买页面
            this->service->write("requireOfbuy:can_add"+ this->send_drinks() + "#" + this->send_user(numOfuser) + "#" + this->comfirm_hot());
        }
        else if(status == "add"){
            this->service->write("requireOfbuy:not_add");
        }
        //监控饮料的数目
        this->comfirm_drinks();
    }
    else{
        this->service->write("requireOfbuy:no_open"+ this->send_drinks() + "#" + this->send_user(numOfuser) + "#" + this->comfirm_hot());
    }
}

/** 处理客户端发来的q确认热门饮料的请求 **/
QString Widget::comfirm_hot(){
    int counts[8] = {0};
    this->db->query->exec("SELECT * FROM buyLog");
    while (this->db->query->next()) {
        if(this->db->query->value(5) == "1")
            switch (this->db->query->value(2).toInt()) {
            case 0:counts[0]+=this->db->query->value(8).toInt();break;
            case 1:counts[1]+=this->db->query->value(8).toInt();break;
            case 2:counts[2]+=this->db->query->value(8).toInt();break;
            case 3:counts[3]+=this->db->query->value(8).toInt();break;
            case 4:counts[4]+=this->db->query->value(8).toInt();break;
            case 5:counts[5]+=this->db->query->value(8).toInt();break;
            case 6:counts[6]+=this->db->query->value(8).toInt();break;
            case 7:counts[7]+=this->db->query->value(8).toInt();break;
            }
    }
    int maxOfindex = 0,max = counts[0];
    for(int i = 1 ; i < 8;i++){
        if(counts[i] > max){
            maxOfindex = i;
            max = counts[i];
        }
    }
    return QString::number(maxOfindex);
}

/** 计算一天饮料机的饮料销量 **/
QString Widget::count_sale(){
    int count = 0;
    QString time = QDateTime::currentDateTime().toString("yyyy.MM.dd");
    this->db->query->exec("SELECT * FROM buyLog");
    while (this->db->query->next()) {
        if(time == this->db->query->value(6).toString().mid(0,10))
            if(this->db->query->value(5) == "1"){
                count += this->db->query->value(8).toInt();
            }
    }
    return QString::number(count);
}

/** 服务端从数据库获得饮料信息并且打包成字串发送给客户端 **/
QString Widget::send_drinks(){
    QString data;
    this->db->query->exec("SELECT * FROM drinks");
    while(this->db->query->next()){
        data += ":" + this->db->query->value(1).toString();
        data += ":" + this->db->query->value(2).toString();
        data += ":" + this->db->query->value(3).toString();
        data += ":" + this->db->query->value(4).toString();
        data += ":" + this->db->query->value(5).toString();
        data += ":" + this->db->query->value(6).toString();
    }
    return data;
}

/** 客户端将用户信息打包成字串发送给客户端 **/
QString Widget::send_user(QString name){
    QString data;
    this->db->query->exec(QString("SELECT * FROM users WHERE name = '%1'").arg(name));
    while(this->db->query->next()){
        data += this->db->query->value(1).toString();
        data += ":" + this->db->query->value(3).toString();
        data += ":" + this->db->query->value(4).toString();
        data += ":" + this->db->query->value(5).toString();
    }
    return data;
}

/** 向数据库插入一个用户日志 **/
void Widget::insertUserLog(QString name, QString type,QString creat_time,QString message){
    db->query->exec(QString("INSERT INTO userLog (name, type, creat_time, message) "
                            "VALUES ('%1', '%2', '%3', '%4')").arg(name).arg(type).arg(creat_time).arg(message));
}

/** 向数据库插入一个用户 **/
void Widget::insertUser(QString name, QString password,QString money,int type, int isOnline){
    QString type_str = QString::number(type);
    QString isOnline_str = QString::number(isOnline);
    db->query->exec(QString("INSERT INTO users (name, password, money, type, isOnline) "
                            "VALUES ('%1', '%2', '%3', '%4', '%5')").arg(name).arg(password).arg(money).arg(type_str).arg(isOnline_str));
}

/** 向数据库删除一个用户 **/
void Widget::deletetUser(QString name, QString password,QString money,int type, int isOnline){
    db->query->exec(QString("DELETE FROM users WHERE "
                            "name = '%1' AND password = '%2'").arg(name).arg(password));
}

/** 刷新userLog */
void Widget::on_button_flush_userLog_clicked()
{
    QStandardItemModel* model = new QStandardItemModel(this);
    /** 设置列字段名 */
    model->setColumnCount(5);
    model->setHeaderData(0,Qt::Horizontal, "id");
    model->setHeaderData(1,Qt::Horizontal, "用户id");
    model->setHeaderData(2,Qt::Horizontal, "活动类型");
    model->setHeaderData(3,Qt::Horizontal, "创建时间");
    model->setHeaderData(4,Qt::Horizontal, "message");
    QStandardItem* qStandardItem;
    this->db->query->exec("SELECT * FROM userLog");
    int row = 0;
    while(this->db->query->next()){
        qStandardItem= new QStandardItem(this->db->query->value(0).toString());
        qStandardItem->setEditable(false);
        model->setItem(row, 0, qStandardItem);
        qStandardItem= new QStandardItem(this->db->query->value(1).toString());
        qStandardItem->setEditable(false);
        model->setItem(row, 1, qStandardItem);
        qStandardItem= new QStandardItem(this->db->query->value(2).toString());
        qStandardItem->setEditable(false);
        model->setItem(row, 2, qStandardItem);
        qStandardItem= new QStandardItem(this->db->query->value(3).toString());
        qStandardItem->setEditable(false);
        model->setItem(row, 3, qStandardItem);
        qStandardItem= new QStandardItem(this->db->query->value(4).toString());
        qStandardItem->setEditable(false);
        model->setItem(row, 4, qStandardItem);
        row++;
    }
    this->ui->tableView_userLog->setModel(model);
}

/** 刷新buyLog */
void Widget::on_button_flush_buyLog_clicked()
{
    QStandardItemModel* model = new QStandardItemModel(this);
    /** 设置列字段名 */
    model->setColumnCount(8);
    model->setHeaderData(0,Qt::Horizontal, "id");
    model->setHeaderData(1,Qt::Horizontal, "购买者id");
    model->setHeaderData(2,Qt::Horizontal, "饮料id");
    model->setHeaderData(3,Qt::Horizontal, "购买所在设备");
    model->setHeaderData(4,Qt::Horizontal, "饮料温度");
    model->setHeaderData(5,Qt::Horizontal, "购买/添加");
    model->setHeaderData(6,Qt::Horizontal, "创建时间");
    model->setHeaderData(7,Qt::Horizontal, "message");
    QStandardItem* qStandardItem;
    this->db->query->exec("SELECT * FROM buyLog");
    int row = 0;
    while(this->db->query->next()){
        qStandardItem= new QStandardItem(this->db->query->value(0).toString());
        qStandardItem->setEditable(false);
        model->setItem(row, 0, qStandardItem);
        qStandardItem= new QStandardItem(this->db->query->value(1).toString());
        qStandardItem->setEditable(false);
        model->setItem(row, 1, qStandardItem);
        qStandardItem= new QStandardItem(this->db->query->value(2).toString());
        qStandardItem->setEditable(false);
        model->setItem(row, 2, qStandardItem);
        qStandardItem= new QStandardItem(this->db->query->value(3).toString());
        qStandardItem->setEditable(false);
        model->setItem(row, 3, qStandardItem);
        qStandardItem= new QStandardItem(this->db->query->value(4).toString());
        qStandardItem->setEditable(false);
        model->setItem(row, 4, qStandardItem);
        qStandardItem= new QStandardItem(this->db->query->value(5).toString());
        qStandardItem->setEditable(false);
        model->setItem(row, 5, qStandardItem);
        qStandardItem= new QStandardItem(this->db->query->value(6).toString());
        qStandardItem->setEditable(false);
        model->setItem(row, 6, qStandardItem);
        qStandardItem= new QStandardItem(this->db->query->value(7).toString());
        qStandardItem->setEditable(false);
        model->setItem(row, 7, qStandardItem);
        row++;
    }
    this->ui->tableView_buyLog->setModel(model);
    this->ui->label_buyLog_sale->setText("Today'sale:" + this->count_sale());
}

/** 刷新equipment */
void Widget::on_button_flush_equipment_clicked()
{
    QStandardItemModel* model = new QStandardItemModel(this);
    /** 设置列字段名 */
    model->setColumnCount(3);
    model->setHeaderData(0,Qt::Horizontal, "id");
    model->setHeaderData(1,Qt::Horizontal, "设备id");
    model->setHeaderData(2,Qt::Horizontal, "是否开启");
    QStandardItem* qStandardItem;
    this->db->query->exec("SELECT * FROM equipments");
    int row = 0;
    while(this->db->query->next()){
        qStandardItem= new QStandardItem(this->db->query->value(0).toString());
        qStandardItem->setEditable(false);
        model->setItem(row, 0, qStandardItem);
        qStandardItem= new QStandardItem(this->db->query->value(1).toString());
        qStandardItem->setEditable(false);
        model->setItem(row, 1, qStandardItem);
        qStandardItem= new QStandardItem(this->db->query->value(2).toString());
        model->setItem(row, 2, qStandardItem);
        row++;
    }
    this->ui->tableView_equipment->setModel(model);
}

/** 刷新drinks */
void Widget::on_button_flush_drink_clicked()
{
    QStandardItemModel* model = new QStandardItemModel(this);
    /** 设置列字段名 */
    model->setColumnCount(7);
    model->setHeaderData(0,Qt::Horizontal, "id");
    model->setHeaderData(1,Qt::Horizontal, "饮料id");
    model->setHeaderData(2,Qt::Horizontal, "价格");
    model->setHeaderData(3,Qt::Horizontal, "设备id");
    model->setHeaderData(4,Qt::Horizontal, "冷饮数量");
    model->setHeaderData(5,Qt::Horizontal, "热饮数量");
    model->setHeaderData(6,Qt::Horizontal, "常温数量");
    QStandardItem* qStandardItem;
    this->db->query->exec("SELECT * FROM drinks");
    int row = 0;
    while(this->db->query->next()){
        qStandardItem= new QStandardItem(this->db->query->value(0).toString());
        qStandardItem->setEditable(false);
        model->setItem(row, 0, qStandardItem);
        qStandardItem= new QStandardItem(this->db->query->value(1).toString());
        qStandardItem->setEditable(false);
        model->setItem(row, 1, qStandardItem);
        qStandardItem= new QStandardItem(this->db->query->value(2).toString());
        model->setItem(row, 2, qStandardItem);
        qStandardItem= new QStandardItem(this->db->query->value(3).toString());
        qStandardItem->setEditable(false);
        model->setItem(row, 3, qStandardItem);
        qStandardItem= new QStandardItem(this->db->query->value(4).toString());
        model->setItem(row, 4, qStandardItem);
        qStandardItem= new QStandardItem(this->db->query->value(5).toString());
        model->setItem(row, 5, qStandardItem);
        qStandardItem= new QStandardItem(this->db->query->value(6).toString());
        model->setItem(row, 6, qStandardItem);
        row++;
    }
    this->ui->tableView_drink->setModel(model);
}

/** 获得修改后设备表格信息来更新数据库 */
void Widget::on_button_update_equipment_clicked(){

    QAbstractItemModel* model = this->ui->tableView_equipment->model();
    for(int i = 0; i < 3;i++){
        this->db->query->exec(QString("UPDATE equipments SET isOpen = '%1' WHERE id = %2").arg(model->data(model->index(i,2)).toString()).arg(QString::number(i+1)));
        qDebug() << model->data(model->index(i,2)).toString();
    }
    //命令客户端更新数据
    this->service->write("requireOfflush:can_flush"+ this->send_drinks() + "#" + this->send_user(this->userName) + "#" + this->comfirm_hot());
}

/** 获得修改后的饮料表格信息来更新数据库 */
void Widget::on_button_update_drink_clicked()
{

    QAbstractItemModel* model = this->ui->tableView_drink->model();
    for(int i = 0; i < 24;i++){
        this->db->query->exec(QString("UPDATE drinks SET price = '%1' WHERE id = %2").arg(model->data(model->index(i,2)).toString()).arg(QString::number(i+1)));
        qDebug() << model->data(model->index(i,2)).toString();
    }
    for(int i = 0; i < 24;i++){
        this->db->query->exec(QString("UPDATE drinks SET num1 = '%1' WHERE id = %2").arg(model->data(model->index(i,4)).toString()).arg(QString::number(i+1)));
        qDebug() << model->data(model->index(i,4)).toString();
    }
    for(int i = 0; i < 24;i++){
        this->db->query->exec(QString("UPDATE drinks SET num2 = '%1' WHERE id = %2").arg(model->data(model->index(i,5)).toString()).arg(QString::number(i+1)));
        qDebug() << model->data(model->index(i,2)).toString();
    }
    for(int i = 0; i < 24;i++){
        this->db->query->exec(QString("UPDATE drinks SET num3 = '%1' WHERE id = %2").arg(model->data(model->index(i,6)).toString()).arg(QString::number(i+1)));
        qDebug() << model->data(model->index(i,2)).toString();
    }
    this->service->write("requireOfflush:can_flush"+ this->send_drinks() + "#" + this->send_user(userName) + "#" + this->comfirm_hot());

}

/** 服务端从数据库检索饮料的数量情况并发送相应的warning到control */
void Widget::comfirm_drinks(){
    this->db->query->exec("SELECT * FROM drinks");
    while (this->db->query->next()) {
        if(this->db->query->value(4) < 2){
            ui->showMessage->append(QString("Warning:The drink-%1 of %2 in %3 is less than two bottles,please add quickly!").arg(this->db->query->value(1).toString()).arg(change_temp(1)).arg(this->db->query->value(3).toString()));
        }
        if(this->db->query->value(5) < 2){
            ui->showMessage->append(QString("Warning:The drink-%1 of %2 in %3 is less than two bottles,please add quickly!").arg(this->db->query->value(1).toString()).arg(change_temp(2)).arg(this->db->query->value(3).toString()));
        }
        if(this->db->query->value(6) < 2){
            ui->showMessage->append(QString("Warning:The drink-%1 of %2 in %3 is less than two bottles,please add quickly!").arg(this->db->query->value(1).toString()).arg(change_temp(3)).arg(this->db->query->value(3).toString()));
        }
    }
}

/** 点击转换到userlog界面 */
void Widget::on_button_userLog_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

/** 点击转换到buyLog界面 */
void Widget::on_button_buyLog_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

/** 点击转换到equipment界面 */
void Widget::on_button_equipment_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

/** 点击转换到drink界面 */
void Widget::on_button_drinks_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

/** 点击转换到control界面 */
void Widget::on_button_returnControl_ulog_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

/** 点击转换到control界面 */
void Widget::on_button_returnControl_ulog_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

/** 点击转换到control界面 */
void Widget::on_button_returnControl_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

/** 点击转换到设备界面 */
void Widget::on_button_control_equipment_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

/** 点击切换到control界面 */
void Widget::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

/** 点击addAdmin进入添加管理员界面 **/
void Widget::on_button_addAdmin_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}

/** 产生随机数的函数 **/
// min:随机数的最小值，max:随机数的最大值
int generateRandomInteger(int min, int max)
{
    Q_ASSERT(min < max);
    // 加入随机种子。种子是当前时间距离0点0分0秒的秒数。
    // 每次启动程序，只添加一次种子，以做到数字真正随机。
    static bool seedStatus;
    if (!seedStatus)
    {
        qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
        seedStatus = true;
    }
    int nRandom = qrand() % (max - min);
    nRandom = min + nRandom;

    return nRandom;
}

/** 将box_temp文本转化为数字 */
QString change_temp(int temp){
    if(temp == 1)
        return "冷饮";
    if(temp == 2)
        return "热饮";
    if(temp == 3)
        return "常温";
}

/** MD5加密 */
QString MD5(QString password){
    QByteArray bb;
    QString passwordMD5;
    bb = QCryptographicHash::hash ( password.toLatin1(), QCryptographicHash::Md5 );
    passwordMD5.append(bb.toHex());
    return passwordMD5;
}


//    /** 建立一个饮料表 */
//    this->db->query->exec("CREATE TABLE drinks("
//                          "id integer primary key autoincrement, "
//                          "name VARCHAR(40) not null, "
//                          "price VARCHAR(40) not null, "
//                          "equipment_id VARCHAR(40) not null, "
//                          "num1 VARCHAR(40) not null, "
//                          "num2 VARCHAR(40) not null, "
//                          "num3 VARCHAR(40) not null)");
//    this->db->query->prepare("INSERT INTO drinks(name,price,equipment_id,num1,num2,num3)"
//                             "VALUES(:name,:price,:equipment_id,:num1,:num2,:num3)");
//    for(int i = 0; i < 8;i++){
//        for(int j = 0 ; j < 3; j++){
//            int random1 = generateRandomInteger(0,3);
//            int random2 = generateRandomInteger(0,3);
//            int random3 = generateRandomInteger(0,3);
//            int random4 = generateRandomInteger(0,3);
//            this->db->query->bindValue(":name",QString::number(i));
//            this->db->query->bindValue(":price",QString::number(random1));
//            this->db->query->bindValue(":equipment_id",QString::number(j));
//            this->db->query->bindValue(":num1",QString::number(random2));
//            this->db->query->bindValue(":num2",QString::number(random3));
//            this->db->query->bindValue(":num3",QString::number(random4));
//            this->db->query->exec();
//        }
//    }

//        /** 建立一个设备表 */
//    this->db->query->exec("CREATE TABLE equipments("
//                             "id integer primary key autoincrement, "
//                             "name VARCHAR(40) not null, "
//                             "isOpen VARCHAR(40) not null)");
//        this->db->query->prepare("INSERT INTO equipments(name,isOpen)"
//                                 "VALUES(:name,:isOpen)");
//    for(int i = 0 ; i < 3; i++){
//        this->db->query->bindValue(":name",QString::number(i));
//        if(i == 0){
//            this->db->query->bindValue(":isOpen",QString::number(1));
//        }
//        else {
//            this->db->query->bindValue(":isOpen",QString::number(0));
//        }
//        this->db->query->exec();
//    }
//    /** 建立一个用户日志表 */
//    this->db->query->exec("CREATE TABLE userLog("
//                             "id integer primary key autoincrement, "
//                             "name VARCHAR(40) not null, "
//                             "type VARCHAR(40) not null, "
//                             "creat_time VARCHAR(40) not null, "
//                             "message VARCHAR(40) not null) ");
//    /** 建立一个购买日志表 */
//    this->db->query->exec("CREATE TABLE buyLog("
//                             "id integer primary key autoincrement, "
//                             "userName VARCHAR(40) not null, "
//                             "drinkrName VARCHAR(40) not null, "
//                             "equipment VARCHAR(40) not null, "
//                             "tempture VARCHAR(40) not null, "
//                             "type VARCHAR(40) not null, "
//                             "creat_time VARCHAR(40) not null, "
//                             "message VARCHAR(40) not null) ");

/** 点击服务端广告控制界面的开启 */
void Widget::on_button_adv_start_clicked()
{
    this->service->write("requireOfAdv:start");
}

/** 点击服务端广告控制界面的停止 */
void Widget::on_button_adv_stop_clicked()
{
     this->service->write("requireOfAdv:stop");
}

/** 点击服务端广告控制界面的前一个 */
void Widget::on_button_adv_pre_clicked()
{
    this->service->write("requireOfAdv:pre");
}

/** 点击服务端广告控制界面的后一个 */
void Widget::on_button_adv_next_clicked()
{
    this->service->write("requireOfAdv:next");
}

/** 点击服务端广告控制界面的返回 */
void Widget::on_pushButton_2_clicked()
{
    this->ui->stackedWidget->setCurrentIndex(0);
}

/** 点击服务端的控制界面的广告 */
void Widget::on_button_advertisement_clicked()
{
    this->ui->stackedWidget->setCurrentIndex(6);
}

void Widget::on_button_adv_insert_clicked()
{
    QString data = "requireOfAdv:insert:/Users/m-mac/Desktop/"+ui->lineEdit_adv->text();
    this->service->write(data);
}

void Widget::on_button_adv_delete_clicked()
{
    QString data = ui->lineEdit_adv->text();
    this->service->write("requireOfAdv:delete:"+data);
}
