#ifndef SAMPLE_H
#define SAMPLE_H

///
/// \brief Enum Channel określa tryb poszczególnych widgetów i to jakie kanały aktualnie obserwują/pracują
///
enum class Channel {NONE, CH_1, CH_2, CH_1_AND_CH_2};
///
/// \brief Enum Slope określa zbocze jakie wyzwala oscyloskop
///
enum class Slope {RISING, FALLING};

///
/// \brief Struktura pojędyńczej próbki mieroznego sygnału skłądjąca się z wartości oraz powiąza z tą wartością czasem
/// \warning Wartość value w programie jest używana na przemiennie w zależności od kontekstu jako wartość surowa
/// odczytana z miekorkontorlera i jako wartośc w postaci napiecia w Woltach
/// \warning Wartość time w programie jest używana na przemiennie w zależności od kontekstu jako czas absolutny występieniea
/// wartości od rozopoczęcia pomiaru lub jako czas relatywny id popprzedniego pomiaru
///
struct Sample{
    float value;
    int time;
    Sample(const float value = 0, const int time = 0);
    Sample(const int rawValue, const int maxRawValue, const float maxVoltValue, const int time);
};

///
/// \brief Struktura pojędyńczej próbki mieroznego sygnału w dziedzinie częstotliwości, skłąda się z wartości/siły skłądowej
/// sygnału  danej częstotliwości i częśtotliwosći składowej
///

struct FrequencySample{
    float value;
    float frequency;
    FrequencySample(const float value = 0, const int frequency = 0);
};

#endif // SAMPLE_H
