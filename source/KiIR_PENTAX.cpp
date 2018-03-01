#include "KiIR_PENTAX.h"

const Protocol PENTAX_ProtocolInfo =
{
	pePENTAX, "Pentax (Camera)",				// ������������� � �������� ���������
	ptPulseDistance,							// ��� ���������
	{ { 0, 6, 6, 70000 } }, bdMSB,				// ���������� ���, ���-�� ��� ��� ��������� ������, ���������� �������, ������ LSB ��� MSB
	{ 0, bcMARK, { 1000, 1000 } },				// �������� ���� "0"
	{ 1, bcMARK, { 1000, 3000 } },				// �������� ���� "1"
	true, { 1, bcMARK, { 2200, 27100 } },		// ������� ����� ��� Lead, �������� Lead
	false, { 1, bcMARK, { 0, 0 } },				// ������� ����� ��� Repeat Lead, �������� Repeat Lead
	false, { rtNormal, ritPeriod, 47000 },		// ������ (�� � �������)
	true, { 0, bcMARK, { 1000, 1000 } }			// ��������� ���
};
