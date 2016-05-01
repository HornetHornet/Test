﻿## Тестовое задание по распознаванию логотипов

#### Как работает

С помощью текстового редактора вы можете открыть и отредактировать **run_detection.bat**, прописав в нем относительные или абсолютные **пути** к

1. .exe файлу, 
2. директории с изображениями (или к единственному изображению) на которых выполняется распознавание
3. директории с изображениями объектов (или к единственному изображению), 

подобно тому, как это уже сделано там, и запустить его. Аналогично приложение можно вызвать из консоли.

Я свел GUI к нулю. В консоли будут появляться сообщения об открытых изображениях, прогрессе, времени и результатах обработки. Также будет создан **.log** файл, частично дублирующий вывод в консоль и **папка с изображениями**, на которых будут отмечены результаты. 

Программа может находить несколько одинаковых объектов на одном изображении. В принципе, может находить несколько разных объектов на одном изображении, но на данном наборе с текущими параметрами этого не происходит. У меня было 47 обнаруженных объектов (без учета распознавания на образцах), без ложных срабатываний, после 12-и минут работы. 

####  Как собрать

Чтобы собрать проект, откройте **.sln** файл в **VS 2013-15**

Если у вас уже есть библиотеки [**Opencv 2.4.12**](http://opencv.org/downloads.html) и [**Boost 1.59**](https://sourceforge.net/projects/boost/files/boost-binaries/1.59.0/) , подключите их.

Укажите пути к библиотекам
> 1. "C:\Libraries\boost_1_59_0; C:\Libraries\opencv 2.4\opencv\build\include"
> 2. "C:\Libraries\boost_1_59_0\lib64-msvc-12.0; C:\Libraries\opencv 2.4\opencv\build\x64\vc12\lib"

соотвественно в
> 1. Project > Property Pages > C\C++ > General > Aditional Dependencies
> 2. Project > Property Pages > Linker > Aditional Library Directories

В случае, если opencv был построен с созданием opencv_world....lib, укажите его вместо библиотек в 
> Project > Property Pages > Linker > Input > Additional Dependencies 

и поместите в папку с .exe файлом соответсвующие **.dll**, иначе эта папка должна содержать :

* opencv_highgui2412d.dll 
* opencv_core2412d.dll 
* opencv_imgproc2412d.dll 
* opencv_features2d2412d.dll 
* opencv_cadll3d2412d.dll 
* opencv_nonfree2412d.dll 
* opencv_flann2412d.dll 
* opencv_ml2412d.dll 
* opencv_objdetect2412d.dll 
* opencv_ocl2412d.dll 

Если используемые вами библиотеки были собраны не под **x64** и не с использованием **Visual Studio 2013 (v120)**, укажите это в настройках проекта:

> 1. Build > Configuration Manager > Active Solution Platform
> 2. Project > Property Pages > Configuration Properties > Platform Toolset 

Однако, я не могу гарантировать работоспособность проекта с версиями библиотек, отличными от тех, что использовал сам ([Opencv 2.4.12](http://opencv.org/downloads.html) и [Boost 1.59 x64 v120](https://sourceforge.net/projects/boost/files/boost-binaries/1.59.0/)), а начиная с Opencv 3.0 работать точно не будет.

Они **prebuild**, просто скачайте и распакуйте, при желании - укажите системные переменные. 

