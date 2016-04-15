"C:\opencv\build\x64\vc14\bin\opencv_traincascade"^
 -data cascade^
 -vec "C:\TheTest\Training\vector.vec"^
 -bg "C:\TheTest\Training\BadExamples\bg.txt"^
 -numPos 300^
 -numNeg 600^
 -numStages 16^
 -precalcValBufSize 1024^
 -precalcIdxBufSize 1024^
 -w 60^
 -h 60^
 -minHitRate 0.999^
 -maxFalseAlarmRate 0.5^
 -weightTrimRate 0.95^
 -maxDepth 1^
 -mode ALL^
 -featureType haar^
 -maxWeakCount 100
pause