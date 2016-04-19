
Обучение LBP скакадов 

- generate_bg.bat вызывает CreateSamples, нарезующу из видео, найденные в BadExampleVideos набор изображений, и их списки, используемые как негитвные примеры при обучении

- run_createsamples_generated.bat вызывает opencv_createsamples.exe, для всех "reference" изображений, его можно остановить после обработки первого изображения
- run_traincascade_generated.bat вызывает opencv_traincascade.exe, для всех "reference" изображений, его можно остановить после обработки первого изображения

- params_for_createsamples.txt содержит параметры для запуска opencv_createsamples.exe, с которыми генерируются скрипты
- params_for_traincascade.txt содержит параметры для запуска opencv_traincascade.exe, с которыми генерируются скрипты

- generate_scripts_and_folders.bat вызывает CreateScriptsAndDirectories, генерирующую скрипты run_createsamples_generated.bat и run_traincascade_generated.bat, а так же директории для запуска и работы opencv_createsamples.exe и opencv_traincascade.exe со всеми иображениями, найденными в "reference", либо с одним изображением по указанному адресу

"Train dataset for temp stage can not filled" возникает из-за невозможности найти достаточное колличество негативных примеров, с текщими параметрами (numNeg = 900) 34 000 изображений хватает на 5 этапов обучения

С полученным каскадом, занимающим 6 килобайт, detectMultiScale() выполняется за десятые доли секунды, и дает очень много ложных срабатываний. 



