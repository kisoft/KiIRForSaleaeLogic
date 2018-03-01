#include "KiIR_KATHREIN.h"

const Protocol KATHREIN_ProtocolInfo =
{
	peKATHREIN, "KATHREIN",						// Идентификатор и название протокола
	ptPulseDistance,							// Тип протокола
	{ { 0, 11, 11, 70000 } }, bdMSB,			// Количество бит, Кол-во бит для тестового фрейма, Количество фреймов, Начало LSB или MSB
	{ 0, bcMARK, { 210, 1400 } },				// Описание бита "0"
	{ 1, bcMARK, { 210, 3000 } },				// Описание бита "1"
	true, { 1, bcMARK, { 210, 6218 } },			// Наличие битов для Lead, Описание Lead
	false, { 1, bcMARK, { 0, 0 } },				// Наличие битов для Repeat Lead, Описание Repeat Lead
	true, { rtNormal, ritPeriod, 35000 },		// Повтор (всё о повторе)
	true, { 0, bcMARK, { 210, 1400 } }			// Финальный бит
};
