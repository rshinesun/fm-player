# fm-player
2015/5/30 更新文档

本程序基于GPL V3协议开源，程序在任何地方使用请附带GPLV3 协议副本
本文档基于 CC-BY-SA 3.0协议授权
/* BY：Steven    2015/5/30 */


项目进度：
finish为已实现功能
develop为正在开发中
closed为已经放弃项目功能


项目笔记

项目需求分析：
1、实现在线网络播放（finined)
2、提供收藏功能(closed)
3、提供歌词显示功能(finished)
4、与gnome面板融合(closed)
5、多平台编译(finished)



项目BUG：

进度条拖动调节进度功能未实现(已解决，信号槽连接错误)


项目遇到的问题解决方案
1、跨平台采用QT5解决
2、由于QT5中已经取消了phonon库，所以播放功能改由QMediaPlayer实现
3、由于使用了QMediaplayer，Linux 下需要使用 0.1 版本的Gstrmedia包
4、Qslider的调节功能,需要调用QMediaPlayer::setVolume，使用前必须初始化QMediaPlayer的最大区间QMediaPlayer::SetMaximum=100 和最小区间QMediaPlayer::SetMinmum=0
5、播放进度的同步使用了QMediaPlayer::ChangedPosition(qint64) 使用需要做信号槽连接connect(player,,),并使用自定义的信号槽调用QMediaplayer::setVisture
6、窗口无边框以后必须连接mouseMove事件来保证窗口的移动
===========================================================================


2015-05-13

这个算是我的第一个开源程序，使用了网上的一些素材，实现了以下功能

1、调用豆瓣FM随机获取歌曲并播放 2、使用了歌词迷的API帮助实现获取歌词，但是并不是太完美，尤其对于一些英文歌曲来说 3、由于是第一次开源，没有写太多的注释和文档还请大家谅解

程序基于GPL V3协议开源，可以按协议自由使用。

另外才刚开始用ARCHLINUX还不太会打包，所以需要直接编译源码，如果有童鞋可以帮忙打包的话请联系我。

欢迎大家在此基础上修改，fork
