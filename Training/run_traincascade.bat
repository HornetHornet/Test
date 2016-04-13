"C:\opencv\build\x64\vc14\bin\opencv_traincascade"^
 -data cascade^
 -vec vector.vec^
 -bg BadExamples\bg.txt^
 -numPos 300^
 -numNeg 700^
 -numStages 128^
 -precalcValBufSize 2048^
 -precalcIdxBufSize 2048^
 -w 60^
 -h 60^
 -minHitRate 0.999^
 -maxFalseAlarmRate 0.5^
 -weightTrimRate 0.95^
 -maxDepth 1^
 -mode BASIC^
 -featureType LBP^
 -maxWeakCount 100
pause