#include "KiIR_FAN.h"

const Protocol FAN_ProtocolInfo =
{
	peFAN, "FAN",								// Идентификатор и название протокола
	ptPulseDistanceWidth,						// Тип протокола
	{ { 0, 11, 11, 13200 } }, bdMSB,			// Количество бит, Кол-во бит для тестового фрейма, Количество фреймов, Начало LSB или MSB
	{ 0, bcMARK, { 380, 1280 } },				// Описание бита "0"
	{ 1, bcMARK, { 1280, 380 } },				// Описание бита "1"
	true, { 1, bcMARK, { 1340, 340 } },			// Наличие битов для Lead, Описание Lead
	false, { 1, bcMARK, { 0, 0 } },				// Наличие битов для Repeat Lead, Описание Repeat Lead
	true, { rtNormal, ritPeriod, 6600 },		// Повтор (всё о повторе)
	true, { 0, bcMARK, { 500, 500 } }			// Финальный бит
};
