#include "KiIR_TELEFUNKEN.h"

const Protocol TELEFUNKEN_ProtocolInfo =
{
	peTELEFUNKEN, "TELEFUNKEN",					// ������������� � �������� ���������
	ptPulseDistance,							// ��� ���������
	{ { 0, 15, 15, 70000 } }, bdMSB,			// ���������� ���, ���-�� ��� ��� ��������� ������, ���������� �������, ������ LSB ��� MSB
	{ 0, bcMARK, { 600, 600 } },				// �������� ���� "0"
	{ 1, bcMARK, { 600, 1500 } },				// �������� ���� "1"
	true, { 1, bcMARK, { 600, 1500 } },			// ������� ����� ��� Lead, �������� Lead
	false, { 1, bcMARK, { 0, 0 } },				// ������� ����� ��� Repeat Lead, �������� Repeat Lead
	false, { rtNormal, ritPeriod, 35000 },		// ������ (�� � �������)
	true, { 0, bcMARK, { 550, 2000 } }			// ��������� ���
};
