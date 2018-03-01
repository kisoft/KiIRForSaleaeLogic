#include "KiIR_SIRC.h"

const Protocol SIRC_ProtocolInfo =
{
	peSIRC, "SIRC",								// Идентификатор и название протокола
	ptPulseWidth,								// Тип протокола
	{ { 0, static_cast<U8>(FLOAT_BITS_COUNT), 12, 150000 } }, bdLSB, // Количество бит, Количество бит для тестового фрейма, Количество фреймов, Начало LSB или MSB
	{ 0, bcMARK, { 600, 600 } },				// Описание бита "0"
	{ 1, bcMARK, { 1200, 600 } },				// Описание бита "1"
	true, { 1, bcMARK, { 2400, 600 } },			// Наличие битов для Lead, Описание Lead
	false, { 0, bcMARK, { 0, 0 } },				// Наличие битов для Repeat Lead, Описание Repeat Lead
	true, { rtNormal, ritPeriod, 45000 },		// Повтор (всё о повторе)
	false, { 0, bcMARK, { 0, 0 } }				// Финальный бит
};
