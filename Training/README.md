
�������� LBP �������� 

- generate_bg.bat �������� CreateSamples, ��������� �� �����, ��������� � BadExampleVideos ����� �����������, � �� ������, ������������ ��� ��������� ������� ��� ��������

- run_createsamples_generated.bat �������� opencv_createsamples.exe, ��� ���� "reference" �����������, ��� ����� ���������� ����� ��������� ������� �����������
- run_traincascade_generated.bat �������� opencv_traincascade.exe, ��� ���� "reference" �����������, ��� ����� ���������� ����� ��������� ������� �����������

- params_for_createsamples.txt �������� ��������� ��� ������� opencv_createsamples.exe, � �������� ������������ �������
- params_for_traincascade.txt �������� ��������� ��� ������� opencv_traincascade.exe, � �������� ������������ �������

- generate_scripts_and_folders.bat �������� CreateScriptsAndDirectories, ������������ ������� run_createsamples_generated.bat � run_traincascade_generated.bat, � ��� �� ���������� ��� ������� � ������ opencv_createsamples.exe � opencv_traincascade.exe �� ����� ������������, ���������� � "reference", ���� � ����� ������������ �� ���������� ������

"Train dataset for temp stage can not filled" ��������� ��-�� ������������� ����� ����������� ����������� ���������� ��������, � ������� ����������� (numNeg = 900) 34 000 ����������� ������� �� 5 ������ ��������

� ���������� ��������, ���������� 6 ��������, detectMultiScale() ����������� �� ������� ���� �������, � ���� ����� ����� ������ ������������. 



