#include "KiIR_SPEAKER.h"

const Protocol SPEAKER_ProtocolInfo =
{
	peSPEAKER, "SPEAKER",						// ������������� � �������� ���������
	ptPulseDistanceWidth,						// ��� ���������
	{ { 0, 10, 10, 38000 }, { 1, 10, 10, 120000 } }, bdMSB,	// ���������� ���, ���-�� ��� ��� ��������� ������, ���������� �������, ������ LSB ��� MSB
	{ 0, bcMARK, { 440, 1250 } },				// �������� ���� "0"
	{ 1, bcMARK, { 1250, 440 } },				// �������� ���� "1"
	true, { 1, bcMARK, { 440, 1250 } },			// ������� ����� ��� Lead, �������� Lead
	false, { 1, bcMARK, { 0, 0 } },				// ������� ����� ��� Repeat Lead, �������� Repeat Lead
	true, { rtNormal, ritPeriod, 38000 },		// ������ (�� � �������)
	true, { 0, bcMARK, { 440, 1250 } }			// ��������� ���
};
