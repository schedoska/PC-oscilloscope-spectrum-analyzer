#ifndef CURSORDATA_H
#define CURSORDATA_H

#include "sample.h"

///
/// \brief CursorData - strukutra posidająca informacje z aktualnie mierzonej chwili sygnału
/// Struktura informuje o źródle danych pomiarowych (Ch1 lub Ch2), o wartości w woltach i chwili czasu pomiaru
///
struct CursorData
{
    Channel observedChannel;
    float value;
    float time;
};

#endif // CURSORDATA_H
