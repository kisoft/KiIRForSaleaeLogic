#include "KiIR_SHARP.h"

const Protocol SHARP_ProtocolInfo =
{
	peSHARP, "SHARP",							// ������������� � �������� ���������
	ptPulseDistance,							// ��� ���������
	{ { 0, 13, 13, 65000 }, { 1, 13, 13, 150000 } }, bdLSB,	// ���������� ���, ���-�� ��� ��� ��������� ������, ���������� �������, ������ LSB ��� MSB
	{ 0, bcMARK, { 320, 2000 - 320 } },			// �������� ���� "0"
	{ 1, bcMARK, { 320, 1000 - 320 } },			// �������� ���� "1"
	false, { 0, bcMARK, { 0, 0 } },				// ������� ����� ��� Lead, �������� Lead
	false, { 0, bcMARK, { 0, 0 } },				// ������� ����� ��� Repeat Lead, �������� Repeat Lead
	true, { rtNormal, ritGap, 40000 },			// ������ (�� � �������)
	true, { 0, bcMARK, { 320, 1000 - 320 } }	// ��������� ���
};
