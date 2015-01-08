#include <stdio.h>
#include <hidapi/hidapi.h>
#include <QtCore>

QTextStream * out;

// перечисление HID устройств
void enumerate_hid()
{
    hid_device_info * hinfo;
    hid_device_info * enumerate;

    enumerate = hid_enumerate(0,0);
    if(enumerate != NULL)
    {
        hinfo = enumerate;

        while(hinfo != NULL)
        {
            * out << QCoreApplication::tr("Устройство: %1:%2 - %3\n").arg(QString::number(hinfo->vendor_id, 16),
                        QString::number(hinfo->product_id,16), hinfo->path);
            * out << QCoreApplication::tr("    производитель: %1\n").arg(QString::fromWCharArray(hinfo->manufacturer_string));
            * out << QCoreApplication::tr("    устройство:    %1\n").arg(QString::fromWCharArray(hinfo->product_string));

            hinfo = hinfo->next;
        }

        hid_free_enumeration(enumerate);
    }
    else
        * out << QCoreApplication::tr("Список устройств пуст\n");
}

// необходимые перечисления
class kl_const
{
  public:
    enum regions      // области клавиатуры
    {
      left   = 1,
      middle = 2,
      right  = 3
    };

    enum colors       // цвета
    {
      off = 0,
      red = 1,
      orange = 2,
      yellow = 3,
      green = 4,
      sky = 5,
      blue = 6,
      purple = 7,
      white = 8
    };

    enum levels     // уровни подсветки
    {
      light = 3,
      low = 2,
      med = 1,
      high = 0
    };

    enum modes      // режимы подсветки
    {
      normal = 1,
      gaming = 2,
      breathe = 3,
      demo = 4,
      wave = 5
    };
};

// установка режима
void set_mode(hid_device * dev, kl_const::modes mode)
{
    unsigned char commit[8];

    // проверка входного параметра
    if (mode != kl_const::normal && mode != kl_const::gaming &&
       mode != kl_const::breathe && mode != kl_const::demo && mode != kl_const::wave)
       mode = kl_const::normal;

    commit[0] = 1;
    commit[1] = 2;
    commit[2] = 65; // commit
    commit[3] = (unsigned char) mode; // set hardware mode
    commit[4] = 0;
    commit[5] = 0;
    commit[6] = 0;
    commit[7] = 236; // EOR

    hid_send_feature_report(dev, commit, 8);
}

// установка цвета
void set_color(hid_device * dev, kl_const::regions region, kl_const::colors color, kl_const::levels level)
{
    unsigned char activate[8];

    // проверим регион
    if(region != kl_const::left && region != kl_const::middle && region != kl_const::right)
        return;

    // проверим цвет
    if(color != kl_const::off && color != kl_const::red && color != kl_const::orange &&
       color != kl_const::yellow && color != kl_const::green && color != kl_const::sky &&
       color != kl_const::blue && color != kl_const::purple && color != kl_const::white)
        return;

    // проверим уровень
    if(level != kl_const::light && level != kl_const::low &&
       level != kl_const::med && level != kl_const::high)
        return;

    activate[0] = 1;
    activate[1] = 2;
    activate[2] = 66; // set
    activate[3] = (unsigned char) region;
    activate[4] = (unsigned char) color;
    activate[5] = (unsigned char) level;
    activate[6] = 0;
    activate[7] = 236; // EOR (end of request)

    hid_send_feature_report(dev, activate, 8);
}

int main(int argc, char *argv[])
{
    int res;
    hid_device * hiddev;
    bool arg_error = false;
    QString AllowedParms = "-l,-off,-red,-orange,-sky,-blue,-yellow,-green,-purple,-white,";
    QString Arg1;

    // инициализация
    QCoreApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextStream _out(stdout, QIODevice::WriteOnly);
    _out.setCodec(QTextCodec::codecForLocale());
    out = & _out;

    //матрицы сопоставления параметров
    QMap <QString, kl_const::regions> MapRegions;
    QMap <QString, kl_const::colors>  MapColors;
    QMap <QString, kl_const::levels>  MapLevels;

    MapRegions.insert("left",   kl_const::left);
    MapRegions.insert("middle", kl_const::middle);
    MapRegions.insert("right",  kl_const::right);

    MapColors.insert("off",    kl_const::off);
    MapColors.insert("red",    kl_const::red);
    MapColors.insert("orange", kl_const::orange);
    MapColors.insert("sky",    kl_const::sky);
    MapColors.insert("blue",   kl_const::blue);
    MapColors.insert("yellow", kl_const::yellow);
    MapColors.insert("green",  kl_const::green);
    MapColors.insert("purple", kl_const::purple);
    MapColors.insert("white",  kl_const::white);

    MapLevels.insert("light",  kl_const::light);
    MapLevels.insert("low",    kl_const::low);
    MapLevels.insert("med",    kl_const::med);
    MapLevels.insert("high",   kl_const::high);

    // проверка параметров
    if(argc < 2 || argc > 3)
        arg_error = true;

    if(argc == 2)
    {
      Arg1 = argv[1];

      if(Arg1 == "-p")
        arg_error = true;

      if(!(AllowedParms.contains(Arg1 + ",")))
        arg_error = true;
    }

    if(argc == 3)
    {
        Arg1 = argv[1];
        if(Arg1 != "-p")
          arg_error = true;
    }

    if(arg_error)
    {
      *out << a.tr("Использование: \n");
      *out << a.tr(" -l       список HID устройств (нам требуется 0x1770, 0xff00)\n");
      *out << a.tr(" -off     выключить все\n");
      *out << a.tr(" -red     красная подсветка\n");
      *out << a.tr(" -orange  оранжевая подсветка\n");
      *out << a.tr(" -yellow  желтая подсветка\n");
      *out << a.tr(" -green   зеленая подсветка\n");
      *out << a.tr(" -sky     небесная подсветка\n");
      *out << a.tr(" -blue    голубая подстветка\n");
      *out << a.tr(" -purple  пурпурная подсветка\n");
      *out << a.tr(" -white   белая подсветка\n");
      *out << a.tr(" -p preset название настройки из файла конфигурации\n\n");
      return -1;
    }

    res = hid_init();
    if(res != 0)
    {
        *out << a.tr("Ошибка инициализации HIDAPI: %1").arg(res);
        return -1;
    }

    if(Arg1 == "-l")
      enumerate_hid();
    else
    {
        // получаем нужное нам устройство // 0x1770, 0xff00 делаем две попытки
        hiddev = hid_open(0x1770, 0xff00, 0);
        if(hiddev == NULL)
        {
            hid_exit();
            res = hid_init();
            if(res != 0)
            {
                *out << a.tr("Ошибка инициализации HIDAPI: %1").arg(res);
                return -1;
            }

            hiddev = hid_open(0x1770, 0xff00, 0);
            if(hiddev == NULL)
            {
                *out << a.tr("Ошибка открытия устройства!\n");
                hid_exit();
                return -1;
            }
        }

        if(argc == 3 && Arg1 == "-p")
        {
          // читаем данные из файла конфигурации
          QFile f("/etc/w7/key-light.conf");
          if(f.open(QIODevice::ReadOnly))
          {
            QString Line, preset(argv[2]);
            QStringList fields;
            kl_const::regions region = kl_const::left;
            kl_const::colors color   = kl_const::white;
            kl_const::levels level   = kl_const::high;
            bool preset_found = false;
            QTextStream in(&f);
            int LineCount = 0;

            in.setCodec("UTF-8");
            while (!in.atEnd())
            {
               LineCount++;
               Line = in.readLine().trimmed();
               if(Line.isEmpty() || Line.left(1) == "#")
                   continue;

               fields = Line.split(QRegExp("[ \t]{1,100}"));
               if(fields.size() != 4)
                 continue;

               if(fields.at(0) == preset)
               {
                 preset_found = true;

                 // регион
                 if(MapRegions.contains(fields.at(1)))
                    region = MapRegions.value(fields.at(1));
                 else
                 {
                   *out << a.tr("Ошибочное значение региона в строке: %1").arg(LineCount);
                   arg_error = true;
                 }

                 // цвет
                 if(MapColors.contains(fields.at(2)))
                    color = MapColors.value(fields.at(2));
                 else
                 {
                   *out << a.tr("Ошибочное значение региона в цвета: %1").arg(LineCount);
                   arg_error = true;
                 }

                 // интенсивность
                 if(MapLevels.contains(fields.at(3)))
                    level = MapLevels.value(fields.at(3));
                 else
                 {
                   *out << a.tr("Ошибочное значение региона в строке: %1").arg(LineCount);
                   arg_error = true;
                 }

                 if(!arg_error)
                   set_color(hiddev, region, color, level);
               }
            }

           set_mode(hiddev,  kl_const::normal);
           f.close();

           if(!preset_found)
           {
               *out << a.tr("Указанная настройка не найдена\n");
               arg_error = true;
           }
          }
          else
          {
              *out << a.tr("Ошибка открытия файла конфигурации /etc/w7/key-light.conf\n");
              arg_error = true;
          }
        }
        else
        {
           // цвет прямо указан в параметре
           kl_const::colors color;

           Arg1.remove(0,1);

           if(MapColors.contains(Arg1))
             color = MapColors.value(Arg1);
           else
             color = kl_const::white;

           set_color(hiddev, kl_const::left,   color, kl_const::high);
           set_color(hiddev, kl_const::middle, color, kl_const::high);
           set_color(hiddev, kl_const::right,  color, kl_const::high);
           set_mode(hiddev,  kl_const::normal);
        }

        hid_close(hiddev);
    }

    hid_exit();

    if(arg_error)
      return -1;
    else
      return 0;
}
