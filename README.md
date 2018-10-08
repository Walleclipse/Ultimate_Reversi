# Ultimate_Reversi
带图形界面的黑白棋程序，有保存、悔棋、联机对战、与电脑对战等功能。AI由alpha-beta剪枝+迭代加深+置换表构成  
  
程序名称：翻转棋阵（黑白棋）  
编译环境：Visual Studio 2017 + EasyX  
说明：根据“千千”的“墨攻棋阵”做了一些修改,这些修改包括：强化了AI（欢迎挑战 困难难度）、加入了悔棋、保存、背景音乐等功能。感谢可爱的千千
(https://github.com/im0qianqian/Reversi)  

运行 Ultimate Reversi.exe 快速开始游戏。  
music文件夹里面的是游戏中的背景音乐，picture文件夹里面的是游戏中的背景图片。  
用VS打开 Ultimate Reversi.sln 即可看到项目的解决方案，核心代码位在 Ultimate Reversi/source.cpp

## 游戏介绍  
游戏模式包括 单人模式（与电脑对战，难度：简单、中等、困难），双人模式，联机对战，并且可以载入上一次的存档。  
<img src="https://github.com/Walleclipse/Ultimate_Reversi/raw/master/demo/main.png"  width="700" >  
<img src="https://github.com/Walleclipse/Ultimate_Reversi/raw/master/demo/play.png"  width="700" >  
<img src="https://github.com/Walleclipse/Ultimate_Reversi/raw/master/demo/about.png" width="700" >   
单人模式提供简单、中等、困难AI，并且玩家可以选择执黑棋或者白棋。  
其中简单AI与中等AI是 naive 极大极小搜索。  
困难AI的搜索部分用到了 alpha-beta剪枝+迭代加深+置换表(http://www.xqbase.com/computer/search_hashing.htm)  
估值函数综合考虑了对战双方行动力，棋盘位置的权重，棋子数量，边缘子和内部子等因素。
