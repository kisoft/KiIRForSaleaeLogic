#include "KiIR_NIKON.h"

const Protocol NIKON_ProtocolInfo =
{
	peNIKON, "Nikon (Camera)",					// Идентификатор и название протокола
	ptPulseDistance,							// Тип протокола
	{ { 0, 2, 2, 70000 } }, bdMSB,				// Количество бит, Кол-во бит для тестового фрейма, Количество фреймов, Начало LSB или MSB
	{ 0, bcMARK, { 500, 1500 } },				// Описание бита "0"
	{ 1, bcMARK, { 500, 3500 } },				// Описание бита "1"
	true, { 1, bcMARK, { 2200, 27100 } },		// Наличие битов для Lead, Описание Lead
	false, { 1, bcMARK, { 0, 0 } },				// Наличие битов для Repeat Lead, Описание Repeat Lead
	false, { rtNormal, ritPeriod, 47000 },		// Повтор (всё о повторе)
	true, { 0, bcMARK, { 500, 1500 } }			// Финальный бит
};
