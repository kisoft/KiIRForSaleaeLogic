#include "KiIR_LEGO.h"

const Protocol LEGO_ProtocolInfo =
{
	peLEGO, "LEGO",								// ������������� � �������� ���������
	ptPulseDistance,							// ��� ���������
	{ { 0, 16, 16, 70000 } }, bdMSB,			// ���������� ���, ���-�� ��� ��� ��������� ������, ���������� �������, ������ LSB ��� MSB
	{ 0, bcMARK, { 158, 263 } },				// �������� ���� "0"
	{ 1, bcMARK, { 158, 553 } },				// �������� ���� "1"
	true, { 1, bcMARK, { 158, 1026 } },			// ������� ����� ��� Lead, �������� Lead
	false, { 1, bcMARK, { 0, 0 } },				// ������� ����� ��� Repeat Lead, �������� Repeat Lead
	false, { rtNormal, ritPeriod, 35000 },		// ������ (�� � �������)
	true, { 0, bcMARK, { 158, 263 } }			// ��������� ���
};
