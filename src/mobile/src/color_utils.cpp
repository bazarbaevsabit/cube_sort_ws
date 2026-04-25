#include "mobile/color_utils.hpp"
/**
     * Определение названия цвета по компонентам HSV.
     * @param h – оттенок (0..180 в OpenCV)
     * @param s – насыщенность (0..255)
     * @param v – яркость (0..255) – не используется, оставлен для совместимости
     * @return строка с названием цвета или "NO_COLOR"
     *
     * Пороги по оттенку подобраны эмпирически для семи цветов.
     * Насыщенность ниже 80 означает, что цвет слишком блёклый (белый/серый) –
     * такие пиксели игнорируются.
     */
std::string getColor(float h, float s, float v)
{
    (void)v; // параметр v не используется, убираем предупреждение

    if (s < 80) return "NO_COLOR";

    if ((h >= 0 && h < 10) || (h >= 170 && h <= 180)) return "RED";
    if (h >= 10 && h < 25)  return "ORANGE";
    if (h >= 25 && h < 35)  return "YELLOW";
    if (h >= 35 && h < 85)  return "GREEN";
    if (h >= 85 && h < 100) return "CYAN";
    if (h >= 100 && h < 130) return "BLUE";
    if (h >= 130 && h < 160) return "PURPLE";

    return "NO_COLOR";
}