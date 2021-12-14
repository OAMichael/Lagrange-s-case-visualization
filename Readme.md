# Readme

### This repository created by [Pavel Filippenko](https://github.com/pavel-collab) and [Mikhail Ovsiannikov](https://github.com/OAMichael).

### Our tasks this moment.
- [ ] сдеать flock в c++ файле
- [ ] по максимуму синхронизавать исполнение основного файла и .py файла
- [ ] в файле test добавить корректное завершение дочерних процессов
- [ ] в файл test добавить исполнение основного файла с визуализацией 
- [ ] сделать подписи осей к графикам

## How to execute python script

You have two ways to execute python code on your computer. First case:
```
python3 <filename>.py <args>
```
Second case:
```
chmod +x <filename>.py
./<filename>.py <args>
```
ATTENTION. In the secon case the first line in your python script must be
```
#!/usr/bin/env python3
```