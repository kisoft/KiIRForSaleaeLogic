#include "KiIR_ROOMBA.h"

const Protocol ROOMBA_ProtocolInfo =
{
	peROOMBA, "ROOMBA",							// ������������� � �������� ���������
	ptPulseDistanceWidth,						// ��� ���������
	{ { 0, 7, 7, 18000 }, { 1, 7, 7, 18000 }, { 2, 7, 7, 70000 } }, bdMSB,	// ���������� ���, ���-�� ��� ��� ��������� ������, ���������� �������, ������ LSB ��� MSB
	{ 0, bcMARK, { 930, 2790 } },				// �������� ���� "0"
	{ 1, bcMARK, { 2790, 930 } },				// �������� ���� "1"
	true, { 1, bcMARK, { 2790, 930 } },			// ������� ����� ��� Lead, �������� Lead
	false, { 1, bcMARK, { 0, 0 } },				// ������� ����� ��� Repeat Lead, �������� Repeat Lead
	true, { rtNormal, ritPeriod, 18000 },		// ������ (�� � �������)
	false, { 0, bcMARK, { 0, 0 } }				// ��������� ���
};
