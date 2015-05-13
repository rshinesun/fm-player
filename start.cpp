#include "start.h"
#include "ui_start.h"
#include <QPoint>
#include <QMouseEvent>
#include <QPainter>
#include <QtGui>


Start::Start(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Start)
{
    ui->setupUi(this);
    /*set Ui*/

    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(255,255,255));
    this->setPalette(palette);
    setWindowFlags(Qt::FramelessWindowHint);
    ui->pushButton->setIcon(QIcon(":/image/pause.png"));
    ui->pushButton->setFlat(true);
    ui->label->setFocus();

    ui->pushButton_2->setIcon(QIcon(":/image/Next.png"));
    ui->pushButton_2->setFlat(true);

    /*Add New Player and Seting default Volume*/
    player=new QMediaPlayer();

    /*set Default Volume*/
    ui->horizontalSlider->setValue(100);
    ui->horizontalSlider->setMaximum(100);
    ui->horizontalSlider->setMinimum(0);

    player->setVolume(100);

    /*set Play url*/

    DoingPlay=true;

    NowPlay=0;

    /*set GET*/
    GetEnd=false;
    manager = new QNetworkAccessManager(this);
    manager2 = new QNetworkAccessManager(this);;
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    connect(manager2, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished2(QNetworkReply*)));
    /* set position */

    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChanged(qint64)));

    connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(durationChanged(qint64)));

    connect(ui->horizontalSlider_2,SIGNAL(sliderMoved(int)),SLOT(setPosition(int)));

    ui->horizontalSlider_2->setRange(0, 0);

    /*Start Load*/

    GetSongs();

    /* Test!! Please delete this for build the relase */

    ui->horizontalSlider->setVisible(false);

}

Start::~Start()
{
    delete ui;
    delete player;
    delete manager;
    delete manager2;
}

void Start::mousePressEvent(QMouseEvent *lpEvent)
{
    if (lpEvent->button() == Qt::LeftButton)
    {
        m_WindowPos = this->pos();
        m_MousePos = lpEvent->globalPos();
        this->m_MousePressed = true;
    }
}

void Start::mouseReleaseEvent(QMouseEvent *lpEvent)
{
    if (lpEvent->button() == Qt::LeftButton)
    {
        this->m_MousePressed = false;
    }
}

void Start::mouseMoveEvent(QMouseEvent *lpEvent)
{
    if (m_MousePressed)
    {
        this->move(m_WindowPos + (lpEvent->globalPos() - m_MousePos));
    }
}


void Start::on_horizontalSlider_sliderMoved(int position)
{
    player->setVolume(position);
}


void Start::positionChanged(qint64 position){

    ui->horizontalSlider_2->setValue(position);

    ui->label_7->setText(CovertTime(position));

    UpdateTime(position);
}

void Start::durationChanged(qint64 duration){

    ui->horizontalSlider_2->setRange(0, duration);

    ui->label_8->setText(CovertTime(duration));

    allduration=duration;//直接获取该音频文件的总时长参数，单位为毫秒
}

void Start::setPosition(int position){

    player->setPosition(position);
}

QString Start::CovertTime(qint64 ti){

    qint64 temp=ti/1000;

    qint64 mm=temp/60;

    qint64 ss=temp%60;

    QString time="";

    if(mm<10)time="0"+QString::number(mm);
    else time=QString::number(mm);

    if(ss<10)time+=":0"+QString::number(ss);
    else time+=":"+QString::number(ss);

    return time;

}

void Start::on_pushButton_clicked()
{
    if(DoingPlay==true){
        player->pause();
        ui->pushButton->setIcon(QIcon(":/image/Play.png"));
        DoingPlay=false;
    }else{
        player->play();
        ui->pushButton->setIcon(QIcon(":/image/pause.png"));
        DoingPlay=true;
    }
    ui->label->setFocus();

}

void Start::on_horizontalSlider_2_actionTriggered()
{
    ui->label->setFocus();
}


void Start::resolve_lrc(const QString &all_text)
{
    lrc_map.clear();
    if(all_text.isEmpty())
        return;

    // 将歌词按行分解为歌词列表
    QStringList lines = all_text.split("\n");

    //这个是时间标签的格式[00:05.54]
    //正则表达式d{2}表示匹配2个数字
    QRegExp rx("\\[\\d{2}:\\d{2}\\.\\d{2}\\]");
    foreach(QString oneline, lines) {
        QString temp = oneline;
        temp.replace(rx, "");//用空字符串替换正则表达式中所匹配的地方,这样就获得了歌词文本
        // 然后依次获取当前行中的所有时间标签，并分别与歌词文本存入QMap中
        //indexIn()为返回第一个匹配的位置，如果返回为-1，则表示没有匹配成功
        //正常情况下pos后面应该对应的是歌词文件
        int pos = rx.indexIn(oneline, 0);
        while (pos != -1) { //表示匹配成功
            QString cap = rx.cap(0);//返回第0个表达式匹配的内容
            // 将时间标签转换为时间数值，以毫秒为单位
            QRegExp regexp;
            regexp.setPattern("\\d{2}(?=:)");
            regexp.indexIn(cap);
            int minute = regexp.cap(0).toInt();
            regexp.setPattern("\\d{2}(?=\\.)");
            regexp.indexIn(cap);
            int second = regexp.cap(0).toInt();
            regexp.setPattern("\\d{2}(?=\\])");
            regexp.indexIn(cap);
            int millisecond = regexp.cap(0).toInt();
            qint64 totalTime = minute * 60000 + second * 1000 + millisecond * 10;
            // 插入到lrc_map中
            lrc_map.insert(totalTime, temp);
            pos += rx.matchedLength();
            pos = rx.indexIn(oneline, pos);//匹配全部
        }
    }
    // 如果lrc_map为空
    if (lrc_map.isEmpty()) {
        ui->label_5->setText("<html><head/><body><p align=\"center\">最美的风景,和你</p></body></html>");
        return;
    }
}

void Start::UpdateTime(qint64 time)
{

    //这3个参数分别代表了时，分，秒；60000毫秒为1分钟，所以分钟第二个参数是先除6000,第3个参数是直接除1s
    QTime total_time(0, (allduration/60000)%60, (allduration/1000)%60);
    QTime current_time(0, (time/60000)%60, (time/1000)%60);//传进来的time参数代表了当前的时间
    QString str = current_time.toString("mm:ss") + "/" + total_time.toString("mm:ss");
    qint64 previous=0;

    // 获取当期时间对应的歌词
    if(!lrc_map.isEmpty()) {
        // 获取当前时间在歌词中的前后两个时间点
        previous = 0;

        //keys()方法返回lrc_map列表
        foreach (qint64 value, lrc_map.keys()) {
            if (time >= value) {
                previous = value;
            } else {

                break;
            }
        }
    }
        // 获取当前时间所对应的歌词内容
        QString current_lrc = lrc_map.value(previous);
        if(current_lrc==""){
            ui->label_5->setText("<html><head/><body><p align=\"center\">最美的风景,和你</p></body></html>");
        }else{
            ui->label_5->setText("<html><head/><body><p align=\"center\">"+current_lrc+"</p></body></html>");
        }
}

QString Start::FindLyric(QString Lyric){

    int pos1=0,pos2=0;

    for(int i=0;i<Lyric.length();i++){
        if(Lyric[i]=='h'&&Lyric[i+1]=='t'&&Lyric[i+2]=='t'&&Lyric[i+3]=='p'){
            pos1=i;
            break;
        }
    }
    for(int i=pos1;i<Lyric.length();i++){
        if(Lyric[i]=='.'&&Lyric[i+1]=='l'&&Lyric[i+2]=='r'&&Lyric[i+3]=='c'){
            pos2=i+3;
            break;
        }
    }
    Lyric= Lyric.mid(pos1,pos2-pos1+1);
    return Lyric;

}


void Start::Getlyric(QString Song){

    if(Song==""){
        QUrl url=FindLyric(GetDATA);
        GetEnd=false;
        manager->get(QNetworkRequest(url));
    }else{
        GetEnd=true;
        QUrl url("http://geci.me/api/lyric/"+Song);
        manager->get(QNetworkRequest(url));
    }

}

void Start::replyFinished(QNetworkReply *reply)
{
   QByteArray bytes = reply->readAll();
    /* it`s GET date*/
   GetDATA = QString::fromUtf8(bytes);

   if(GetEnd==true){
       Getlyric("");
   }else{
       resolve_lrc(GetDATA);
   }

   reply->deleteLater();   //最后要释放reply对象*/
}

void Start::replyFinished2(QNetworkReply *reply){

    QByteArray bytes = reply->readAll();
    /* it`s GET date*/
    GetDATA2 = QString::fromUtf8(bytes);
    FindSongsList(GetDATA2);
    reply->deleteLater();
}


void Start::FindSongsList(QString Songs){

    int pos1,pos2=0;

    for(int i=0;i<5;i++){

        //Search Pic
        pos1=pos2;
        for(;pos1<Songs.length();pos1++){
             if(Songs[pos1]=='u'&&Songs[pos1+1]=='r'&&Songs[pos1+2]=='e'&&Songs[pos1+3]=='"'){
                 pos1+=6;
                 break;
              }
          }
        for(pos2=pos1;pos2<Songs.length();pos2++){
             if(Songs[pos2]=='.'&&Songs[pos2+1]=='j'&&Songs[pos2+2]=='p'&&Songs[pos2+3]=='g'){
                pos2+=4;
                break;
             }
        }
        playlist[i].pic= Songs.mid(pos1,pos2-pos1);
        playlist[i].pic.replace(QString("\\"),QString(""));
        pos1=pos2;
        //Search Artist

        for(;pos1<Songs.length();pos1++){
             if(Songs[pos1]=='t'&&Songs[pos1+1]=='i'&&Songs[pos1+2]=='s'&&Songs[pos1+3]=='t'){
                 pos1+=7;
                 break;
              }
          }
        for(pos2=pos1;pos2<Songs.length();pos2++){
             if(Songs[pos2]=='"'&&Songs[pos2+1]==','&&Songs[pos2+2]=='"'&&Songs[pos2+3]=='u'){
                break;
             }
        }
        playlist[i].artist= Songs.mid(pos1,pos2-pos1);
        pos1=pos2;
        //Serach url
        for(;pos1<Songs.length();pos1++){
           if(Songs[pos1]=='h'&&Songs[pos1+1]=='t'&&Songs[pos1+2]=='t'&&Songs[pos1+3]=='p'){
               break;
            }
        }
        for(pos2=pos1;pos2<Songs.length();pos2++){
           if(Songs[pos2]=='.'&&Songs[pos2+1]=='m'&&Songs[pos2+2]=='p'&&Songs[pos2+3]=='3'){
              pos2+=4;
              break;
            }
        }
        playlist[i].url= Songs.mid(pos1,pos2-pos1);
        playlist[i].url.replace(QString("\\"),QString(""));
        pos1=pos2;
        //Serach title
        for(;pos1<Songs.length();pos1++){
           if(Songs[pos1]=='i'&&Songs[pos1+1]=='t'&&Songs[pos1+2]=='l'&&Songs[pos1+3]=='e'){
               pos1+=7;
               break;
            }
        }
        for(pos2=pos1;pos2<Songs.length();pos2++){
           if(Songs[pos2]=='"'&&Songs[pos2+1]==','&&Songs[pos2+2]=='"'){
              pos2-=1;
              break;
            }
        }
        playlist[i].title= Songs.mid(pos1,pos2-pos1+1);
    }

   _Play();
}

void Start::_Play(){

    if(NowPlay==4){
        GetSongs();
        NowPlay=0;
        player->stop();
    }

    player->setMedia(QUrl(playlist[NowPlay].url));

    player->play();

    QString title=playlist[NowPlay].title;

    Getlyric(title);

    ui->label_10->setText("<html><head/><body><p align=\"center\">"+playlist[NowPlay].title+"</p></body></html>");

    ui->label_11->setText("<html><head/><body><p align=\"center\">"+playlist[NowPlay].artist+"</p></body></html>");

    ui->label_5->setText("<html><head/><body><p align=\"center\">最美的风景,和你</p></body></html>");

    NowPlay++;
}

void Start::on_pushButton_2_clicked()
{

    _Play();

    ui->label->setFocus();
}


void Start::GetSongs(){
    QUrl url("http://douban.fm/j/mine/playlist?channel=1");
    manager2->get(QNetworkRequest(url));
}
