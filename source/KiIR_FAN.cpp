#include "KiIR_FAN.h"

const Protocol FAN_ProtocolInfo =
{
	peFAN, "FAN",								// ������������� � �������� ���������
	ptPulseDistanceWidth,						// ��� ���������
	{ { 0, 11, 11, 13200 } }, bdMSB,			// ���������� ���, ���-�� ��� ��� ��������� ������, ���������� �������, ������ LSB ��� MSB
	{ 0, bcMARK, { 380, 1280 } },				// �������� ���� "0"
	{ 1, bcMARK, { 1280, 380 } },				// �������� ���� "1"
	true, { 1, bcMARK, { 1340, 340 } },			// ������� ����� ��� Lead, �������� Lead
	false, { 1, bcMARK, { 0, 0 } },				// ������� ����� ��� Repeat Lead, �������� Repeat Lead
	true, { rtNormal, ritPeriod, 6600 },		// ������ (�� � �������)
	true, { 0, bcMARK, { 500, 500 } }			// ��������� ���
};
