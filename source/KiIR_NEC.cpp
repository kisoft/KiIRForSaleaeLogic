#include "KiIR_NEC.h"

const Protocol NEC_ProtocolInfo =
{
	peNEC, "NEC",								// ������������� � �������� ���������
	ptPulseDistance,							// ��� ���������
	{ { 0, 32, 32, 200000 } }, bdLSB,			// ���������� ���, ���-�� ��� ��� ��������� ������, ���������� �������, ������ LSB ��� MSB
	{ 0, bcMARK, { 560, 560 /* 1120 - 560 */ } },	// �������� ���� "0"
	{ 1, bcMARK, { 560, 1690 /* 2250 - 560 */ } },		// �������� ���� "1"
	true, { 1, bcMARK, { 9000, 4500 } },		// ������� ����� ��� Lead, �������� Lead
	true, { 1, bcMARK, { 9000, 2250 } },		// ������� ����� ��� Repeat Lead, �������� Repeat Lead
	true, { rtSpecialLead, ritPeriod, 110000 },	// ������ (�� � �������)
	true, { 0, bcMARK, { 560, 1120 - 560 } }	// ��������� ���
};
