#include "KiIR_SPEAKER.h"

const Protocol SPEAKER_ProtocolInfo =
{
	peSPEAKER, "SPEAKER",						// Идентификатор и название протокола
	ptPulseDistanceWidth,						// Тип протокола
	{ { 0, 10, 10, 38000 }, { 1, 10, 10, 120000 } }, bdMSB,	// Количество бит, Кол-во бит для тестового фрейма, Количество фреймов, Начало LSB или MSB
	{ 0, bcMARK, { 440, 1250 } },				// Описание бита "0"
	{ 1, bcMARK, { 1250, 440 } },				// Описание бита "1"
	true, { 1, bcMARK, { 440, 1250 } },			// Наличие битов для Lead, Описание Lead
	false, { 1, bcMARK, { 0, 0 } },				// Наличие битов для Repeat Lead, Описание Repeat Lead
	true, { rtNormal, ritPeriod, 38000 },		// Повтор (всё о повторе)
	true, { 0, bcMARK, { 440, 1250 } }			// Финальный бит
};
