/////////////////////////////////////////////////////////////////////
// www.gamecodi.com						이주행 master@gamecodi.com
//
//
/////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------

패킷만들기 관련 함수.

---------------------------------------------------------------*/
#include "CommonInclude.h"
#include "AyaStreamSQ.h"

#include "Network.h"
#include "PacketDefine.h"
#include "MakePacket.h"


////////////////////////////////////////////////////////////////////
// dfPACKET_CS_MOVE_START
//
// Parameters: (CAyaPacket *)PacketClass. (BYTE)Direction. (short)X. (short)Y.
// Return: 없음.
////////////////////////////////////////////////////////////////////
void	mpMoveStart(CAyaPacket *clpPacket, BYTE byDirection, short shX, short shY)
{
	st_NETWORK_PACKET_HEADER	stPacketHeader;

	stPacketHeader.byCode = dfNETWORK_PACKET_CODE;
	stPacketHeader.bySize = 5;
	stPacketHeader.byType = dfPACKET_CS_MOVE_START;

	clpPacket->PutData((char *)&stPacketHeader, dfNETWORK_PACKET_HEADER_SIZE);

	*clpPacket << byDirection;
	*clpPacket << shX;
	*clpPacket << shY;

	*clpPacket << (BYTE)dfNETWORK_PACKET_END;
}


////////////////////////////////////////////////////////////////////
// dfPACKET_CS_MOVE_STOP
//
// Parameters: (CAyaPacket *)PacketClass. (BYTE)Direction. (short)X. (short)Y.
// Return: 없음.
////////////////////////////////////////////////////////////////////
void	mpMoveStop(CAyaPacket *clpPacket, BYTE byDirection, short shX, short shY)
{
	st_NETWORK_PACKET_HEADER	stPacketHeader;

	stPacketHeader.byCode = dfNETWORK_PACKET_CODE;
	stPacketHeader.bySize = 5;
	stPacketHeader.byType = dfPACKET_CS_MOVE_STOP;

	clpPacket->PutData((char *)&stPacketHeader, dfNETWORK_PACKET_HEADER_SIZE);

	*clpPacket << byDirection;
	*clpPacket << shX;
	*clpPacket << shY;

	*clpPacket << (BYTE)dfNETWORK_PACKET_END;

}


////////////////////////////////////////////////////////////////////
// dfPACKET_CS_ATTACK1
//
// Parameters: (CAyaPacket *)PacketClass. (BYTE)Direction. (short)X. (short)Y.
// Return: 없음.
////////////////////////////////////////////////////////////////////
void	mpAttack1(CAyaPacket *clpPacket, BYTE byDir, short shX, short shY)
{
	st_NETWORK_PACKET_HEADER	stPacketHeader;

	stPacketHeader.byCode = dfNETWORK_PACKET_CODE;
	stPacketHeader.bySize = 5;
	stPacketHeader.byType = dfPACKET_CS_ATTACK1;

	clpPacket->PutData((char *)&stPacketHeader, dfNETWORK_PACKET_HEADER_SIZE);

	*clpPacket << byDir;
	*clpPacket << shX;
	*clpPacket << shY;

	*clpPacket << (BYTE)dfNETWORK_PACKET_END;
}


////////////////////////////////////////////////////////////////////
// dfPACKET_CS_ATTACK2
//
// Parameters: (CAyaPacket *)PacketClass. (BYTE)Direction. (short)X. (short)Y.
// Return: 없음.
////////////////////////////////////////////////////////////////////
void	mpAttack2(CAyaPacket *clpPacket, BYTE byDir, short shX, short shY)
{
	st_NETWORK_PACKET_HEADER	stPacketHeader;

	stPacketHeader.byCode = dfNETWORK_PACKET_CODE;
	stPacketHeader.bySize = 5;
	stPacketHeader.byType = dfPACKET_CS_ATTACK2;

	clpPacket->PutData((char *)&stPacketHeader, dfNETWORK_PACKET_HEADER_SIZE);

	*clpPacket << byDir;
	*clpPacket << shX;
	*clpPacket << shY;

	*clpPacket << (BYTE)dfNETWORK_PACKET_END;

}


////////////////////////////////////////////////////////////////////
// dfPACKET_CS_ATTACK3
//
// Parameters: (CAyaPacket *)PacketClass. (BYTE)Direction. (short)X. (short)Y.
// Return: 없음.
////////////////////////////////////////////////////////////////////
void	mpAttack3(CAyaPacket *clpPacket, BYTE byDir, short shX, short shY)
{
	st_NETWORK_PACKET_HEADER	stPacketHeader;

	stPacketHeader.byCode = dfNETWORK_PACKET_CODE;
	stPacketHeader.bySize = 5;
	stPacketHeader.byType = dfPACKET_CS_ATTACK3;

	clpPacket->PutData((char *)&stPacketHeader, dfNETWORK_PACKET_HEADER_SIZE);

	*clpPacket << byDir;
	*clpPacket << shX;
	*clpPacket << shY;

	*clpPacket << (BYTE)dfNETWORK_PACKET_END;
}
