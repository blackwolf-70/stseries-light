Это программа для управления подсветкой клавиатур Steelseries на 
ноутбуках MSI GS70, GS60 и т.д.

Использование: 
sudo key-light -options

Опции
 -l       список HID устройств (нам требуется 0x1770, 0xff00)
 -off     выключить все
 -red     красная подсветка
 -orange  оранжевая подсветка
 -yellow  желтая подсветка
 -green   зеленая подсветка
 -sky     небесная подсветка
 -blue    голубая подстветка
 -purple  пурпурная подсветка
 -white   белая подсветка
 -p preset название настройки из файла конфигурации
 
 
 Файл конфигурации: /etc/w7/key-light.conf
 
#  Формат строк файла:
#  
#  preset   region   color   intens
#
#  preset - название предварительной настройки
#  region - область клавиатуры: 	left, middle, right
#  color  - цвет: 			off, red, orange, yellow, green, sky, blue, purple, white
#  intens - интенсивность цвета: 	light, low, med, high 
#
# примеры:
#

rgb	left	red	med
rgb	middle	green	med
rgb	right   blue    med

game	left	red	high
game	middle	off	med
game	right   off    med
