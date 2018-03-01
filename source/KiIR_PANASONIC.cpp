#include "KiIR_PANASONIC.h"

const Protocol PANASONIC_ProtocolInfo =
{
	pePANASONIC, "Panasonic",					// ������������� � �������� ���������
	ptPulseDistance,							// ��� ���������
	{ { 0, 56, 56, 100000 } }, bdLSB,			// ���������� ���, ���-�� ��� ��� ��������� ������, ���������� �������, ������ LSB ��� MSB
	{ 0, bcMARK, { 565, 316 } },				// �������� ���� "0"
	{ 1, bcMARK, { 565, 1140 } },				// �������� ���� "1"
	true, { 1, bcMARK, { 3600, 1600 } },		// ������� ����� ��� Lead, �������� Lead
	false, { 1, bcMARK, { 0, 0 } },				// ������� ����� ��� Repeat Lead, �������� Repeat Lead
	true, { rtNormal, ritPeriod, 47000 },		// ������ (�� � �������)
	true, { 0, bcMARK, { 565, 565 } }			// ��������� ���
};
