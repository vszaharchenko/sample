#include <QCoreApplication>
#include <QFile>
#include <QDateTime>
#include <QDebug>

#include <iostream>

#define EX_FILENAME "buffer.bin"

#define EX_AIRCRAFTS_IN_ZONE 5

/// Идентификатор ICAO24
#define EX_ICAO24 "EX_ICAO"

/// Идентификатор
#define EX_ID "ID"

/// Путевой угол самолета, градусы
#define EX_COURSE 10.0

/// Воздушная скорость, м/с
#define EX_AIRSPEED 100

/// Барометрическая высота, м
#define EX_AERIAL_ALT 500

/// Вертикальная скорость, м/с
#define EX_VERTICAL_SPEED 30

/// Произвольное описание самолета или его состояния
#define EX_DESCRIPTION "new bla bla bla"

#include <proto/sample.pb.h>


/**
 * @brief printMessage функция выводит в консоль сообщение в HR формате
 * @param message
 */
void printMessage(const ADSB::Sample& message)
{
    std::cout <<"Device name:" << message.source().devicename() << "\n" <<
                "Source name:" << message.source().sourcename() << "\n" <<
                "AIRCRAFTS IN ZONE:" << message.aircraft_size() << "\n\n" << std::endl;

    for (int i = 0; i < message.aircraft_size(); i++)
    {
        ADSB::Aircraft aircraft = message.aircraft(i);
        std::cout << "Aircraft:"<< (i + 1) << "\n" <<
                     "Time:" << QDateTime::fromMSecsSinceEpoch(aircraft.timestamp()).toString().toStdString() << "\n" <<
                     "ID:" << aircraft.id() << "\n" <<
                     "ICAO:" << aircraft.icao24() << "\n" <<
                     "GPS Course:" << aircraft.gpscourse() << "\n" <<
                     "Aerial Speed:" << aircraft.aerialspeed() << "\n" <<
                     "Aerial Alt:" << aircraft.aerialalt() << "\n" <<
                     "Vertical Speed:" << aircraft.verticalspeed() << "\n" <<
                     "Description:" << aircraft.description() << "\n" <<
                     "Altitude:" << aircraft.position().altitude() << "\n" <<
                     "Longitude:" << aircraft.position().longitude() << "\n" <<
                     "Latitude:" << aircraft.position().latitude() << "\n\n" <<std::endl;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    /// создаем описание источника
    ADSB::Source* source = new ADSB::Source();
    source->set_devicename("Sample");
    source->set_sourcename("ADBS type 1");

    /// создаем сообщение
    ADSB::ADSB_Message adsbMessage;
    /// Типизируем вкладываемый тип сообщения
    adsbMessage.set_sourcetype(ADSB::ADSB_Message_SourceType_Sample);
    /// Получаем указатель на сруктуру данных АЗНВ, получаемых с
    ADSB::Sample* messageSample = adsbMessage.MutableExtension(ADSB::Sample::data);
    messageSample->set_allocated_source(source);

    double deltaValue = 10.1;

    /// в цикле заполняем поля структуры данных по АЗНВ, в примере их 5
    for (uint i = 0; i < EX_AIRCRAFTS_IN_ZONE; i++)
    {
        /// это просто чтобы задать разные значения параметров для каждого самолета
        deltaValue = deltaValue + i;

        ADSB::Aircraft* aircraft = messageSample->add_aircraft();
        aircraft->set_timestamp(QDateTime::currentMSecsSinceEpoch());
        aircraft->set_aerialalt(EX_AERIAL_ALT * deltaValue);
        aircraft->set_aerialspeed(EX_AIRSPEED * deltaValue);
        aircraft->set_gpscourse(EX_COURSE * deltaValue);
        aircraft->set_description(EX_DESCRIPTION);
        aircraft->set_icao24(EX_ICAO24);
        aircraft->set_id(EX_ID);
        aircraft->set_verticalspeed(EX_VERTICAL_SPEED * deltaValue);

        ADSB::GeoCoordinates* coordinates = new ADSB::GeoCoordinates();
        coordinates->set_altitude(EX_AERIAL_ALT * deltaValue);
        coordinates->set_latitude(12.36 + deltaValue/10);
        coordinates->set_longitude(36.12 + deltaValue/10);
        aircraft->set_allocated_position(coordinates);
    }

    /// Выодим на экран, то что записано в пакет
    printMessage(*messageSample);


    /// Пишем данные в файл (как пример записи данных в бинарном виде, типа в сеть...)
    QFile writeFile;
    writeFile.setFileName(EX_FILENAME);

    if (writeFile.open(QIODevice::WriteOnly))
    {
        u_int64_t bytesWritten = writeFile.write(adsbMessage.SerializeAsString().data(), adsbMessage.ByteSize());
        if(((int)bytesWritten) == adsbMessage.ByteSize())
            qDebug() << "Данные записаны в буфер: "<< bytesWritten << "байт";
        else
            qDebug() << "Чего-то пошло не так, записано: "<< bytesWritten << "байт,"
                     << "а хотелось " <<  adsbMessage.ByteSize();
        writeFile.close();
    }
    else
        qDebug() << "Ошибка при открытии файла для записи";


    /// Читаем данные из файла (типа как из сети)
    QFile readFile;
    readFile.setFileName(EX_FILENAME);

    if (readFile.open(QIODevice::ReadOnly))
    {
        QByteArray array = readFile.readAll();
        ADSB::ADSB_Message message;
        if(message.ParseFromArray(array.data(), array.size()))
        {
            qDebug() << "Читаем сообщение";
            /// Выводим на экран, то что прочитано
            printMessage(message.GetExtension(ADSB::Sample::data));
        }
        else
            qDebug() << "Чего-то пошло не так, не удалось распарсить сообщение";

        readFile.close();
    }
    else
        qDebug() << "Ошибка при открытии файла для чтения";

    return a.exec();
}
