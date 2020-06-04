#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QDateTime>
#include <QAction>
#include <QMessageBox>
#include <QHostAddress>
#include <QCryptographicHash>
#include <QtMultimedia>
#include <QtMultimediaWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("智能饮料机客户端");
    user = new User();
    /** 初始化通信实例 */
    this->client = new communication(this);
    connect(this->client->getTcpSocket(), &QTcpSocket::connected,this,&MainWindow::connectSLOT);
    connect(this->client->getTcpSocket(), &QTcpSocket::readyRead,this,&MainWindow::readDate);
    /** 界面跳转 */
    connect(ui->button_returnLog, SIGNAL(clicked()),this,SLOT(on_button_returnLog_clicked()));
    connect(ui->button_register, SIGNAL(clicked()),this,SLOT(on_button_register_clicked()));
    /** 注册/注销请求 */
    connect(ui->button_r_2, SIGNAL(clicked()),this,SLOT(register_2_clicked()));
    connect(ui->button_d, SIGNAL(clicked()),this,SLOT(button_d_clicked()));
    /** 购买 */
    connect(ui->button_buy_1, SIGNAL(clicked()),this,SLOT(button_buy_1_clicked()));
    /** menu */
    connect(ui->actionactionBuy,SIGNAL(triggered()),this,SLOT(actionBuy()));
    connect(ui->actionationSendMessage,SIGNAL(triggered()),this,SLOT(actionSendMessage()));
    connect(ui->actionactionRecharge,SIGNAL(triggered()),this,SLOT(actionRecharge()));
    connect(ui->actionactionLink,SIGNAL(triggered()),this,SLOT(actionLink()));
    connect(ui->actionactionDisconnect,SIGNAL(triggered()),this,SLOT(actionDisconnect()));
    //设置背景颜色
    QPalette pal(this->palette());
    pal.setColor(QPalette::Background,Qt::white);
    this->setAutoFillBackground(true);
    this->setPalette(pal);
    //确保图片随着label大小变化
    //饮料图片
    QPixmap image_drink1(":/drinks/drink1.png");
    ui->image_1->setPixmap(image_drink1.scaled(ui->image_1->size(), Qt::KeepAspectRatio));
    QPixmap image_drink2(":/drinks/drink2.jpg");
    ui->image_2->setPixmap(image_drink2.scaled(ui->image_2->size(), Qt::KeepAspectRatio));
    QPixmap image_drink3(":/drinks/drink3.jpg");
    ui->image_3->setPixmap(image_drink3.scaled(ui->image_3->size(), Qt::KeepAspectRatio));
    QPixmap image_drink4(":/drinks/drink4.jpg");
    ui->image_4->setPixmap(image_drink4.scaled(ui->image_4->size(), Qt::KeepAspectRatio));
    QPixmap image_drink5(":/drinks/drink5.jpg");
    ui->image_5->setPixmap(image_drink5.scaled(ui->image_5->size(), Qt::KeepAspectRatio));
    QPixmap image_drink6(":/drinks/drink6.jpg");
    ui->image_6->setPixmap(image_drink6.scaled(ui->image_6->size(), Qt::KeepAspectRatio));
    QPixmap image_drink7(":/drinks/drink7.jpg");
    ui->image_7->setPixmap(image_drink7.scaled(ui->image_7->size(), Qt::KeepAspectRatio));
    QPixmap image_drink8(":/drinks/drink8.jpg");
    ui->image_8->setPixmap(image_drink8.scaled(ui->image_8->size(), Qt::KeepAspectRatio));
    //充值卡图片
    QPixmap image_card(":/drinks/recharge.jpg");
    ui->image_recharge->setPixmap(image_card.scaled(ui->image_recharge->size(), Qt::KeepAspectRatio));
    QPixmap image_code(":/drinks/QRcode.JPG");
    ui->label_QR->setPixmap(image_code.scaled(ui->label_QR->size(), Qt::KeepAspectRatio));
    QPixmap image_code2(":/drinks/QRcode2.JPG");
    ui->label_QR_2->setPixmap(image_code2.scaled(ui->label_QR_2->size(), Qt::KeepAspectRatio));
    //设置密码输入框为密码模式
    ui->line_password->setEchoMode(QLineEdit::Password);
    ui->line_change_password->setEchoMode(QLineEdit::Password);
    ui->line_register_password->setEchoMode(QLineEdit::Password);
    //设置播放广告
    player = new QMediaPlayer();
    playlist = new QMediaPlaylist;
    videoWidget = new QVideoWidget(this);
    videoWidget->setFixedSize(830,600);
    videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);
    player->setPlaylist(playlist);
    player->setVideoOutput(videoWidget);
    playlist->addMedia(QMediaContent(QUrl::fromLocalFile("/Users/m-mac/Desktop/c.mp4")));
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    videoWidget->show();
    player->play();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/** 双击事件*/
void MainWindow::mouseDoubleClickEvent(QMouseEvent* event){
    if(this->user->is_online =="0"){
        this->videoWidget->hide();
        ui->stackedWidget->setCurrentIndex(0);
    }
}

/** 点击事件*/
void MainWindow::mousePressEvent(QMouseEvent *event){
    if(this->user->is_online == "0"){
        if(event->button() == Qt::LeftButton)
        {
    //        qDebug() << "触发左键按钮";
             this->killTimer(time_id);
             time_id = this->startTimer(10000);
        }
    }
}
/** 时间事件*/
void MainWindow::timerEvent( QTimerEvent * event ){
//    qDebug() << "我在计算时间";
//    qDebug() << player->state();
    if(this->user->is_online == "0"){
        if(player->position() > 3000)
            player->setPosition(30);
        if(event->timerId() == time_id){
              this->videoWidget->show();
        }
    }
}

/** 客户端点击menu的show触发的槽函数 */
void MainWindow::actionBuy(){
    if(this->user->is_online == "1"){
        this->client->write("requestOfflush:"+this->user->name);
        ui->stackedWidget->setCurrentIndex(3);
    }

}

/** 客户端点击menu的SendMessage触发的槽函数 */
void MainWindow::actionSendMessage(){
    ui->stackedWidget->setCurrentIndex(5);
}

/** 客户端点击menu的Recharge触发的槽函数 */
void MainWindow::actionRecharge(){
    if(this->user->is_online == "1"){
        ui->stackedWidget->setCurrentIndex(4);
    }
}

/** 客户端点击menu的Link触发的槽函数 */
void MainWindow::actionLink(){
    this->client->link();
    QMessageBox *msb = new QMessageBox;
    msb->setWindowTitle("连接提示");
    msb->setText("连接成功！");
    msb->setIcon(QMessageBox::Information);
    msb->setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    msb->show();
}

/** 客户端点击menu的Disconnect触发的槽函数 */
void MainWindow::actionDisconnect(){
    this->client->breakLink();
    this->user->is_online = "0";    QMessageBox *msb = new QMessageBox;
    msb->setWindowTitle("断开提示");
    msb->setText("断开链接成功！");
    msb->setIcon(QMessageBox::Information);
    msb->setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    msb->show();
}

/** 客户端连接上了服务器触发的槽函数 */
void MainWindow::connectSLOT(){
    ui->textBrowser_read->append("成功和服务器建立好连接");
}

/** 客户端处理服务器发来的命令的槽函数 */
void MainWindow::readDate(){
    QString temp = this->client->readAll();
    ui->textBrowser_read->append(temp);
    this->dealDate(temp);
}

/** 客户端根据服务器命令的路由函数 */
void MainWindow::dealDate(QString data){
    if(data.split(":")[0] == "requireOflog")
        this->decideCanlog(data);
    if(data.split(":")[0] == "requireOfregister")
        this->decideCanRegister(data);
    if(data.split(":")[0] == "requireOfdelete")
        this->decideCanDelete(data);
    if(data.split(":")[0] == "requireOfbuy")
        this->decideCanbuy(data);
    if(data.split(":")[0] == "requireOfflush")
        this->flush_buyWindow(data);
    if(data == "requireOfSendMessage:can_send"){
        ui->textEdit->clear();
    }
    if(data.split(":")[0] == "requireOfRecharge"){
        this->flush_buyWindow(data);
        QMessageBox *msb = new QMessageBox;
        msb->setWindowTitle("充值提示");
        msb->setText("充值成功！");
        msb->setIcon(QMessageBox::Information);
        msb->setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        msb->show();
    }
    if(data.split(":")[0] == "requireOfbreaklink"){
        QMessageBox *msb = new QMessageBox;
        msb->setWindowTitle("断开提示");
        msb->setText("服务器已断开连接！");
        msb->setIcon(QMessageBox::Information);
        msb->setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        msb->show();
    }
    if(data.split(":")[0] == "requireOfAdv"){
        this->op_advertisment(data);
    }

}

/** 客服端的接受到服务器的注册命令时调用的函数 */
void MainWindow::decideCanRegister(QString data){
    if(data.split(":")[1] == "can_register"){
        this->sendUserlog(ui->line_register_user->text(),"1",ui->line_register_user->text()+" has registered!");
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
        msb->setText("用户已存在，请重新注册！");
        msb->setIcon(QMessageBox::Information);
        msb->setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        msb->show();
    }
}

/** 客服端的接受到服务器的注销命令时调用的函数 */
void MainWindow::decideCanDelete(QString data){
    if(data.split(":")[1] == "can_delete"){
        this->sendUserlog(ui->line_register_user->text(),"2",ui->line_register_user->text()+" has been deleted!");
        QMessageBox *msb = new QMessageBox;
        msb->setWindowTitle("注销提示");
        msb->setText("注销成功！");
        msb->setIcon(QMessageBox::Information);
        msb->setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        msb->show();
    }
    else {
        QMessageBox *msb = new QMessageBox;
        msb->setWindowTitle("注销提示");
        msb->setText("用户名已经存在或者密码错误，注销失败！");
        msb->setIcon(QMessageBox::Information);
        msb->setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        msb->show();
    }
}

/** 客服端的接受到服务器的登陆命令时调用的函数 */
void MainWindow::decideCanlog(QString data){
    qDebug() << data;
    if(data.split(":")[1] == "can_log"){
        this->flush_buyWindow(data);
        this->sendUserlog(this->user->name,"3",this->user->name+" has been loged!");
        ui->stackedWidget->setCurrentIndex(3);
        this->user->is_online = "1";
    }

    if(data.split(":")[1] == "not_log"){
        QMessageBox *msb = new QMessageBox;
        msb->setWindowTitle("登陆提示");
        msb->setText("用户名不存在或者密码错误，请重新输入！");
        msb->setIcon(QMessageBox::Information);
        msb->setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        msb->show();
    }
    if(data.split(":")[1] == "not_log_repeat"){
        QMessageBox *msb = new QMessageBox;
        msb->setWindowTitle("登陆提示");
        msb->setText("账号已在其他地方登陆！");
        msb->setIcon(QMessageBox::Information);
        msb->setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        msb->show();
    }
}

/** 客服端的接受到服务器的购买命令时调用的函数 */
void MainWindow::decideCanbuy(QString data){
    if(data.split(":")[1] == "can_buy"){
        qDebug() << "我可以购买！";
        qDebug() << data;
        this->flush_buyWindow(data);
        QMessageBox *msb = new QMessageBox;
        msb->setWindowTitle("购买提示");
        msb->setText("购买成功！");
        msb->setIcon(QMessageBox::Information);
        msb->setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        msb->show();
        this->sendUserlog(this->user->name,"5",this->user->name+" has been bought a drink!");
    }
    else if(data.split(":")[1] == "can_add"){
        this->flush_buyWindow(data);
        QMessageBox *msb = new QMessageBox;
        msb->setWindowTitle("添货提示");
        msb->setText("添加货物成功！");
        msb->setIcon(QMessageBox::Information);
        msb->setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        msb->show();
    }
    else if(data.split(":")[1] == "not_buy"){
        QMessageBox *msb = new QMessageBox;
        msb->setWindowTitle("购买提示");
        msb->setText("购买失败，请确认购买数目和金钱数！");
        msb->setIcon(QMessageBox::Information);
        msb->setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        msb->show();
    }
    else if(data.split(":")[1] == "not_add"){
        QMessageBox *msb = new QMessageBox;
        msb->setWindowTitle("添加提示");
        msb->setText("添加失败，请注意添加数目！");
        msb->setIcon(QMessageBox::Information);
        msb->setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        msb->show();
    }
    else if(data.split(":")[1] == "no_open"){
        QMessageBox *msb = new QMessageBox;
        msb->setWindowTitle("设备提示");
        msb->setText("设备未启动");
        msb->setIcon(QMessageBox::Information);
        msb->setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        msb->show();
    }

}

/** 点击SendMessage界面的发送对应的槽函数 */
void MainWindow::on_button_send_message_clicked()
{
    QHostAddress address = QHostAddress::Any;
    QString ip = address.toString();
    qint16 port = this->client->getTcpSocket()->peerPort();
    QString name = QString("%1:%2").arg(ip).arg(port);
    QString data = ui->textEdit->toPlainText();
    qDebug() << data;
    this->client->write(name + ":requestOfSendMessage:" + data);
}

/** 点击注册按钮，客户向服务器发送注册请求的槽函数 */
void MainWindow::register_2_clicked()
{
    QString data = "requestOfregister:" + ui->line_register_user->text() + ":" + this->MD5(ui->line_register_password->text());
    this->client->write(data);
}

/** 点击注销按钮，客户向服务器发送注销请求的槽函数 */
void MainWindow::button_d_clicked()
{
    QString data = "requestOfdelete:" + ui->line_register_user->text() + ":" + this->MD5(ui->line_register_password->text());
    this->client->write(data);
}

/** 在log界面点击登陆时向服务器发送登陆请求的槽函数 */
void MainWindow::on_button_log_clicked()
{
    QString data = "requestOflog:" + ui->line_user->text() + ":" + this->MD5(ui->line_password->text());
    client->write(data);
}

/** 点击切换账户界面的登陆对应的槽函数 */
void MainWindow::on_button_change_log_clicked()
{
    QString data = "requestOflog:" + ui->line_change_user->text() + ":" + this->MD5(ui->line_change_password->text()) + ":" + this->user->name;
    client->write(data);
}

/** 点击切换界面对应的退出对应的槽函数 */
void MainWindow::on_button_change_logOff_clicked()
{
    QString data = "requestOflogOff:"+ this->user->name;
    client->write(data);
    QMessageBox *msb = new QMessageBox;
    msb->setWindowTitle("退出登陆提示");
    msb->setText("退出登录成功！");
    msb->setIcon(QMessageBox::Information);
    msb->setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    msb->show();
    ui->stackedWidget->setCurrentIndex(0);
    this->user->is_online = "0";
}

/** 点击返回对应的槽函数 */
void MainWindow::on_button_return_clicked()
{
    if(this->user->is_online == "1"){
        ui->stackedWidget->setCurrentIndex(3);
    }
    else {
        ui->stackedWidget->setCurrentIndex(0);
    }
}

/** 点击充值界面的充值对应的槽函数 */
void MainWindow::on_button_recharge_clicked()
{
    this->ui->stackedWidget->setCurrentIndex(6);
}

/** 向服务器发送userlog */
void MainWindow::sendUserlog(QString name,QString type,QString message){
    this->client->write("requestOfuserlog:" + name + ":" + type + ":" + QDateTime::currentDateTime().toString("yyyy.MM.dd hh.mm") + ":" +message);
}

/** 点击设备下拉框并且改变设备的时候的槽函数 */
void MainWindow::on_comboBox_equipment_currentIndexChanged(int index)
{

    this->client->write(QString("requestOfflush:"+this->user->name + ":changeEquipment:%1").arg(index));
    qDebug() << index;
}

/** 改变饮料的种类的时候的槽函数 */
void MainWindow::on_comboBox_temperature_currentIndexChanged(int index)
{
    this->client->write("requestOfflush:"+this->user->name);
}

/** 点击购买1对应的槽函数 */
void MainWindow::button_buy_1_clicked()
{
    //购买的饮料信息：name、equ_id、温度、价格、购买数量、饮料数。
    //用户的信息:name、money
    //购买情况信息：add/buy
    QString equ = QString::number(this->change_equ(ui->comboBox_equipment->currentText()));
    QString nameOfdrink = "0";
    QString num_buy = QString::number(ui->residue_1->value());
    QString temp = QString::number(this->change_temp(ui->comboBox_temperature->currentText()));
    QString price = ui->label_1->text().split(":")[1];
    QString numOfuser = this->user->name;
    QString money = this->user->money;
    QString num_drink = ui->number_1->text().split(":")[1];
    if(ui->button_buy_1->text() == "购买"){
         QMessageBox message(QMessageBox::Warning,"购买提醒","确认购买?",QMessageBox::Yes|QMessageBox::No,NULL);
         if (message.exec()==QMessageBox::Yes)
                 this->client->write("requestOfbuy:1:" + nameOfdrink + ":" + equ + ":" + temp + ":" + price + ":" + num_buy + ":" + numOfuser + ":" + money+ ":" + num_drink + ":" + "buy");
    }
    if(ui->button_buy_1->text() == "补货"){
        QMessageBox message(QMessageBox::Warning,"添加","确认添加?",QMessageBox::Yes|QMessageBox::No,NULL);
        if (message.exec()==QMessageBox::Yes)
         this->client->write("requestOfbuy:1:" + nameOfdrink + ":" + equ + ":" + temp + ":" + price + ":" + num_buy + ":" + numOfuser + ":" + money+ ":" + num_drink + ":" + "add");
    }
}

/** 点击购买2对应的槽函数 */
void MainWindow::on_button_buy_2_clicked()
{
    QString equ = QString::number(this->change_equ(ui->comboBox_equipment->currentText()));
    QString nameOfdrink = "1";
    QString num_buy = QString::number(ui->residue_2->value());
    QString temp = QString::number(this->change_temp(ui->comboBox_temperature->currentText()));
    QString price = ui->label_2->text().split(":")[1];
    QString numOfuser = this->user->name;
    QString money = this->user->money;
    QString num_drink = ui->number_2->text().split(":")[1];
    if(ui->button_buy_2->text() == "购买"){
         QMessageBox message(QMessageBox::Warning,"购买提醒","确认购买?",QMessageBox::Yes|QMessageBox::No,NULL);
         if (message.exec()==QMessageBox::Yes)
                 this->client->write("requestOfbuy:2:" + nameOfdrink + ":" + equ + ":" + temp + ":" + price + ":" + num_buy + ":" + numOfuser + ":" + money+ ":" + num_drink + ":" + "buy");
    }
    if(ui->button_buy_2->text() == "补货"){
        QMessageBox message(QMessageBox::Warning,"添加","确认添加?",QMessageBox::Yes|QMessageBox::No,NULL);
        if (message.exec()==QMessageBox::Yes)
         this->client->write("requestOfbuy:2:" + nameOfdrink + ":" + equ + ":" + temp + ":" + price + ":" + num_buy + ":" + numOfuser + ":" + money+ ":" + num_drink + ":" + "add");
    }
}

/** 点击购买3对应的槽函数 */
void MainWindow::on_button_buy_3_clicked()
{
    QString equ = QString::number(this->change_equ(ui->comboBox_equipment->currentText()));
    QString nameOfdrink = "2";
    QString num_buy = QString::number(ui->residue_3->value());
    QString temp = QString::number(this->change_temp(ui->comboBox_temperature->currentText()));
    QString price = ui->label_3->text().split(":")[1];
    QString numOfuser = this->user->name;
    QString money = this->user->money;
    QString num_drink = ui->number_3->text().split(":")[1];
    if(ui->button_buy_3->text() == "购买"){
         QMessageBox message(QMessageBox::Warning,"购买提醒","确认购买?",QMessageBox::Yes|QMessageBox::No,NULL);
         if (message.exec()==QMessageBox::Yes)
                 this->client->write("requestOfbuy:3:" + nameOfdrink + ":" + equ + ":" + temp + ":" + price + ":" + num_buy + ":" + numOfuser + ":" + money+ ":" + num_drink + ":" + "buy");
    }
    if(ui->button_buy_3->text() == "补货"){
        QMessageBox message(QMessageBox::Warning,"添加","确认添加?",QMessageBox::Yes|QMessageBox::No,NULL);
        if (message.exec()==QMessageBox::Yes)
         this->client->write("requestOfbuy:3:" + nameOfdrink + ":" + equ + ":" + temp + ":" + price + ":" + num_buy + ":" + numOfuser + ":" + money+ ":" + num_drink + ":" + "add");
    }
}

/** 点击购买4对应的槽函数 */
void MainWindow::on_button_buy_4_clicked()
{
    QString equ = QString::number(this->change_equ(ui->comboBox_equipment->currentText()));
    QString nameOfdrink = "3";
    QString num_buy = QString::number(ui->residue_4->value());
    QString temp = QString::number(this->change_temp(ui->comboBox_temperature->currentText()));
    QString price = ui->label_4->text().split(":")[1];
    QString numOfuser = this->user->name;
    QString money = this->user->money;
    QString num_drink = ui->number_4->text().split(":")[1];
    if(ui->button_buy_4->text() == "购买"){
         QMessageBox message(QMessageBox::Warning,"购买提醒","确认购买?",QMessageBox::Yes|QMessageBox::No,NULL);
         if (message.exec()==QMessageBox::Yes)
                 this->client->write("requestOfbuy:4:" + nameOfdrink + ":" + equ + ":" + temp + ":" + price + ":" + num_buy + ":" + numOfuser + ":" + money+ ":" + num_drink + ":" + "buy");
    }
    if(ui->button_buy_4->text() == "补货"){
        QMessageBox message(QMessageBox::Warning,"添加","确认添加?",QMessageBox::Yes|QMessageBox::No,NULL);
        if (message.exec()==QMessageBox::Yes)
         this->client->write("requestOfbuy:4:" + nameOfdrink + ":" + equ + ":" + temp + ":" + price + ":" + num_buy + ":" + numOfuser + ":" + money+ ":" + num_drink + ":" + "add");
    }
}

/** 点击购买5对应的槽函数 */
void MainWindow::on_button_buy_5_clicked(){
    QString equ = QString::number(this->change_equ(ui->comboBox_equipment->currentText()));
    QString nameOfdrink = "4";
    QString num_buy = QString::number(ui->residue_5->value());
    QString temp = QString::number(this->change_temp(ui->comboBox_temperature->currentText()));
    QString price = ui->label_5->text().split(":")[1];
    QString numOfuser = this->user->name;
    QString money = this->user->money;
    QString num_drink = ui->number_5->text().split(":")[1];
    if(ui->button_buy_5->text() == "购买"){
         QMessageBox message(QMessageBox::Warning,"购买提醒","确认购买?",QMessageBox::Yes|QMessageBox::No,NULL);
         if (message.exec()==QMessageBox::Yes)
                 this->client->write("requestOfbuy:5:" + nameOfdrink + ":" + equ + ":" + temp + ":" + price + ":" + num_buy + ":" + numOfuser + ":" + money+ ":" + num_drink + ":" + "buy");
    }
    if(ui->button_buy_5->text() == "补货"){
        QMessageBox message(QMessageBox::Warning,"添加","确认添加?",QMessageBox::Yes|QMessageBox::No,NULL);
        if (message.exec()==QMessageBox::Yes)
         this->client->write("requestOfbuy:5:" + nameOfdrink + ":" + equ + ":" + temp + ":" + price + ":" + num_buy + ":" + numOfuser + ":" + money+ ":" + num_drink + ":" + "add");
    }
}

/** 点击购买6对应的槽函数 */
void MainWindow::on_button_buy_6_clicked()
{
    QString equ = QString::number(this->change_equ(ui->comboBox_equipment->currentText()));
    QString nameOfdrink = "5";
    QString num_buy = QString::number(ui->residue_6->value());
    QString temp = QString::number(this->change_temp(ui->comboBox_temperature->currentText()));
    QString price = ui->label_6->text().split(":")[1];
    QString numOfuser = this->user->name;
    QString money = this->user->money;
    QString num_drink = ui->number_6->text().split(":")[1];
    if(ui->button_buy_6->text() == "购买"){
         QMessageBox message(QMessageBox::Warning,"购买提醒","确认购买?",QMessageBox::Yes|QMessageBox::No,NULL);
         if (message.exec()==QMessageBox::Yes)
                 this->client->write("requestOfbuy:6:" + nameOfdrink + ":" + equ + ":" + temp + ":" + price + ":" + num_buy + ":" + numOfuser + ":" + money+ ":" + num_drink + ":" + "buy");
    }
    if(ui->button_buy_6->text() == "补货"){
        QMessageBox message(QMessageBox::Warning,"添加","确认添加?",QMessageBox::Yes|QMessageBox::No,NULL);
        if (message.exec()==QMessageBox::Yes)
         this->client->write("requestOfbuy:6:" + nameOfdrink + ":" + equ + ":" + temp + ":" + price + ":" + num_buy + ":" + numOfuser + ":" + money+ ":" + num_drink + ":" + "add");
    }
}

/** 点击购买7对应的槽函数 */
void MainWindow::on_button_buy_7_clicked()
{
    QString equ = QString::number(this->change_equ(ui->comboBox_equipment->currentText()));
    QString nameOfdrink = "6";
    QString num_buy = QString::number(ui->residue_7->value());
    QString temp = QString::number(this->change_temp(ui->comboBox_temperature->currentText()));
    QString price = ui->label_7->text().split(":")[1];
    QString numOfuser = this->user->name;
    QString money = this->user->money;
    QString num_drink = ui->number_7->text().split(":")[1];
    if(ui->button_buy_7->text() == "购买"){
         QMessageBox message(QMessageBox::Warning,"购买提醒","确认购买?",QMessageBox::Yes|QMessageBox::No,NULL);
         if (message.exec()==QMessageBox::Yes)
                 this->client->write("requestOfbuy:7:" + nameOfdrink + ":" + equ + ":" + temp + ":" + price + ":" + num_buy + ":" + numOfuser + ":" + money+ ":" + num_drink + ":" + "buy");
    }
    if(ui->button_buy_7->text() == "补货"){
        QMessageBox message(QMessageBox::Warning,"添加","确认添加?",QMessageBox::Yes|QMessageBox::No,NULL);
        if (message.exec()==QMessageBox::Yes)
         this->client->write("requestOfbuy:7:" + nameOfdrink + ":" + equ + ":" + temp + ":" + price + ":" + num_buy + ":" + numOfuser + ":" + money+ ":" + num_drink + ":" + "add");
    }
}

/** 点击购买8对应的槽函数 */
void MainWindow::on_button_buy_8_clicked()
{
    QString equ = QString::number(this->change_equ(ui->comboBox_equipment->currentText()));
    QString nameOfdrink = "7";
    QString num_buy = QString::number(ui->residue_8->value());
    QString temp = QString::number(this->change_temp(ui->comboBox_temperature->currentText()));
    QString price = ui->label_8->text().split(":")[1];
    QString numOfuser = this->user->name;
    QString money = this->user->money;
    QString num_drink = ui->number_8->text().split(":")[1];
    if(ui->button_buy_8->text() == "购买"){
         QMessageBox message(QMessageBox::Warning,"购买提醒","确认购买?",QMessageBox::Yes|QMessageBox::No,NULL);
         if (message.exec()==QMessageBox::Yes)
                 this->client->write("requestOfbuy:8:" + nameOfdrink + ":" + equ + ":" + temp + ":" + price + ":" + num_buy + ":" + numOfuser + ":" + money+ ":" + num_drink + ":" + "buy");
    }
    if(ui->button_buy_8->text() == "补货"){
        QMessageBox message(QMessageBox::Warning,"添加","确认添加?",QMessageBox::Yes|QMessageBox::No,NULL);
        if (message.exec()==QMessageBox::Yes)
         this->client->write("requestOfbuy:8:" + nameOfdrink + ":" + equ + ":" + temp + ":" + price + ":" + num_buy + ":" + numOfuser + ":" + money+ ":" + num_drink + ":" + "add");
    }
}

/** 点击购买界面的进入管理员模式的槽函数 */
void MainWindow::on_button_admin_clicked()
{
    if(this->user->type != "1"){
        QMessageBox *msb = new QMessageBox;
        msb->setWindowTitle("管理员模式提示");
        msb->setText("您不是管理员，无法进入管理员模式！");
        msb->setIcon(QMessageBox::Information);
        msb->setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        msb->show();
    }
    if(this->user->type == "1" && ui->button_admin->text() == "进入管理员模式"){
        ui->button_admin->setText("进入用户模式");
        ui->button_buy_1->setText("补货");
        ui->button_buy_2->setText("补货");
        ui->button_buy_3->setText("补货");
        ui->button_buy_4->setText("补货");
        ui->button_buy_5->setText("补货");
        ui->button_buy_6->setText("补货");
        ui->button_buy_7->setText("补货");
        ui->button_buy_8->setText("补货");
    }
    else if(ui->button_admin->text() == "进入用户模式"){
        ui->button_admin->setText("进入管理员模式");
        ui->button_buy_1->setText("购买");
        ui->button_buy_2->setText("购买");
        ui->button_buy_3->setText("购买");
        ui->button_buy_4->setText("购买");
        ui->button_buy_5->setText("购买");
        ui->button_buy_6->setText("购买");
        ui->button_buy_7->setText("购买");
        ui->button_buy_8->setText("购买");
    }

}

/** 从服务器获取饮料信息、用户信息、hot信息来刷新页面 */
void MainWindow::flush_buyWindow(QString data){
    qDebug() << "我在刷新";
    QString data1,data2,data3;
    data1 = data.split("#")[0];
    data2 = data.split("#")[1];
    data3 = data.split("#")[2];
    //更新用户和饮料的数据库
    this->load_drinks(data1);
    this->load_user(data2);
    //用户的信息和饮料的信息展现到界面
    int  temp = this->change_temp(this->ui->comboBox_temperature->currentText());
    int equ = this->change_equ(this->ui->comboBox_equipment->currentText());
    for(int i = 0; i < drinks.size();i++){
        if(drinks[i]->equipment_id == QString::number(equ) ){
            this->setOneDrink(i,temp);
        }
    }
    this->setOneUser();
    this->setHot(data3);
}

/** 从服务器的数据库获取饮料信息来更新自己的drinks */
void MainWindow::load_drinks(QString data){
    QStringList datas = data.split(":");
    int count = 0,j = 2;
    while(j < datas.length()){
        drink *dr = new drink;
        drinks.append(dr);
        drinks[count]->name = datas[j++];
        drinks[count]->price = datas[j++];
        drinks[count]->equipment_id = datas[j++];
        drinks[count]->num1 = datas[j++];
        drinks[count]->num2 = datas[j++];
        drinks[count++]->num3 = datas[j++];
    }
}

/** 从服务器的数据库获取用户信息来更新自己的user */
void MainWindow::load_user(QString data){
    QStringList datas = data.split(":");
    this->user->name = datas[0];
    this->user->money = datas[1];
    this->user->type = datas[2];
    this->user->is_online = datas[3];
}

/** 设置每一个饮料的界面信息 */
void MainWindow::setOneDrink(int index,int temp){
    switch (drinks[index]->name.toInt()) {
    case 0: this->ui->number_1->setText(this->get_drink_num(index,temp));this->ui->label_1->setText("价格:" + drinks[index]->price);break;
    case 1: this->ui->number_2->setText(this->get_drink_num(index,temp));this->ui->label_2->setText("价格:" + drinks[index]->price);break;
    case 2: this->ui->number_3->setText(this->get_drink_num(index,temp));this->ui->label_3->setText("价格:" + drinks[index]->price);break;
    case 3: this->ui->number_4->setText(this->get_drink_num(index,temp));this->ui->label_4->setText("价格:" + drinks[index]->price);break;
    case 4: this->ui->number_5->setText(this->get_drink_num(index,temp));this->ui->label_5->setText("价格:" + drinks[index]->price);break;
    case 5: this->ui->number_6->setText(this->get_drink_num(index,temp));this->ui->label_6->setText("价格:" + drinks[index]->price);break;
    case 6: this->ui->number_7->setText(this->get_drink_num(index,temp));this->ui->label_7->setText("价格:" + drinks[index]->price);break;
    case 7: this->ui->number_8->setText(this->get_drink_num(index,temp));this->ui->label_8->setText("价格:" + drinks[index]->price);break;
    }
}

/** 设置购买界面的用户属性 */
void MainWindow::setOneUser(){
    ui->user_name->setText("用户名:" + this->user->name);
    ui->user_money->setText("金钱:" + this->user->money);
}

/** 获得某一种饮料的数量 */
QString MainWindow::get_drink_num(int index,int temp){
    switch (temp) {
    case 1: return "剩余数量:" + drinks[index]->num1;
    case 2: return "剩余数量:" + drinks[index]->num2;
    case 3: return "剩余数量:" + drinks[index]->num3;
    }
}

/** 将box_temp文本转化为数字 */
int MainWindow::change_temp(QString temp){
    if(temp == "冷饮")
        return 1;
    if(temp == "热饮")
        return 2;
    if(temp == "常温")
        return 3;

}

/** 将box_equ文本转化为数字 */
int MainWindow::change_equ(QString equ){
    if(equ == "0号机")
        return 0;
    if(equ == "1号机")
        return 1;
    if(equ == "2号机")
        return 2;
}

/** 设置hot图标 */
void MainWindow::setHot(QString data){
    int index = data.toInt();
    ui->hot_1->setVisible(false);
    ui->hot_2->setVisible(false);
    ui->hot_3->setVisible(false);
    ui->hot_4->setVisible(false);
    ui->hot_5->setVisible(false);
    ui->hot_6->setVisible(false);
    ui->hot_7->setVisible(false);
    ui->hot_8->setVisible(false);
    switch (index) {
    case 0:
    {
        ui->hot_1->setVisible(true);
        QPixmap image_hot1(":/drinks/hot.jpg");ui->hot_1->setPixmap(image_hot1.scaled(ui->hot_1->size(), Qt::KeepAspectRatio));
        break;
    }
    case 1:
    {
        ui->hot_2->setVisible(true);
        QPixmap image_hot2(":/drinks/hot.jpg");ui->hot_2->setPixmap(image_hot2.scaled(ui->hot_2->size(), Qt::KeepAspectRatio));
        break;
    }
    case 2:
    {
        ui->hot_3->setVisible(true);
        QPixmap image_hot3(":/drinks/hot.jpg");ui->hot_3->setPixmap(image_hot3.scaled(ui->hot_3->size(), Qt::KeepAspectRatio));
        break;
    }
    case 3:
    {
        ui->hot_4->setVisible(true);
        QPixmap image_hot4(":/drinks/hot.jpg");ui->hot_4->setPixmap(image_hot4.scaled(ui->hot_4->size(), Qt::KeepAspectRatio));
        break;
    }
    case 4:
    {
        ui->hot_5->setVisible(true);
        QPixmap image_hot5(":/drinks/hot.jpg");ui->hot_5->setPixmap(image_hot5.scaled(ui->hot_5->size(), Qt::KeepAspectRatio));
        break;
    }
    case 5:
    {
        ui->hot_6->setVisible(true);
        QPixmap image_hot6(":/drinks/hot.jpg");ui->hot_6->setPixmap(image_hot6.scaled(ui->hot_6->size(), Qt::KeepAspectRatio));
        break;
    }
    case 6:
    {
        ui->hot_7->setVisible(true);
        QPixmap image_hot7(":/drinks/hot.jpg");ui->hot_7->setPixmap(image_hot7.scaled(ui->hot_7->size(), Qt::KeepAspectRatio));
        break;
    }
    case 7:
    {
        ui->hot_8->setVisible(true);
        QPixmap image_hot8(":/drinks/hot.jpg");ui->hot_8->setPixmap(image_hot8.scaled(ui->hot_8->size(), Qt::KeepAspectRatio));
        break;
    }
    }
}


/** 跳转到注册界面的槽函数 */
void MainWindow::on_button_register_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

/** 跳转回登陆界面的槽函数 */
void MainWindow::on_button_returnLog_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

/** 点击切换界面对应的返回对应的槽函数 */
void MainWindow::on_button_change_returnBuy_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

/** 点击购买界面的求换账户对应的槽函数 */
void MainWindow::on_change_user_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

/** MD5加密 */
QString MainWindow::MD5(QString password){
    QByteArray bb;
    QString passwordMD5;
    bb = QCryptographicHash::hash ( password.toLatin1(), QCryptographicHash::Md5 );
    passwordMD5.append(bb.toHex());
    return passwordMD5;
}

/** 点击二维码界面的确认 */
void MainWindow::on_Button_QR_clicked()
{
    QString money_recharge = ui->line_recharge->text();
    QString money_remain = ui->user_money->text().split(":")[1];
    this->client->write("requestOfRecharge:" + this->user->name + ":" + money_remain + ":" + money_recharge);
    QMessageBox *msb = new QMessageBox;
    msb->setWindowTitle("充值提示");
    msb->setText("充值成功！");
    msb->setIcon(QMessageBox::Information);
    msb->setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    msb->show();
    ui->stackedWidget->setCurrentIndex(3);
}

/** 客户端收到服务端的广告命令时调用的处理函数 */
void MainWindow::op_advertisment(QString data){
    if(data.split(":")[1] == "start"){
        this->player->play();
    }
    if(data.split(":")[1] == "stop"){
        this->player->stop();
    }
    if(data.split(":")[1] == "pre"){
        playlist->previous();
    }
    if(data.split(":")[1] == "next"){
       playlist->next();
    }
    if(data.split(":")[1] == "insert"){
        playlist->addMedia(QMediaContent(QUrl::fromLocalFile(data.split(":")[2])));
    }
    if(data.split(":")[1] == "delete"){
        playlist->removeMedia(data.split(":")[2].toInt());
    }
}
