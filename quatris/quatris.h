#include "nwol_array.h"
#include "coord.h"
#include "timer.h"
#include "array_view_bit.h"

#ifndef __CUATRIS_H__892364987234__
#define __CUATRIS_H__892364987234__

namespace qtris
{
	enum CUATRIS_COMMAND : uint8_t					
	{	CUATRIS_COMMAND_IDLE						= 0x0
	,	CUATRIS_COMMAND_MOVE_LEFT					= 0x1
	,	CUATRIS_COMMAND_MOVE_RIGHT					= 0x2
	,	CUATRIS_COMMAND_MOVE_DOWN					= 0x3
	,	CUATRIS_COMMAND_MOVE_DROP					= 0x4
	,	CUATRIS_COMMAND_ROTATE_CLOCKWISE			= 0x10
	,	CUATRIS_COMMAND_ROTATE_COUNTER_CLOCKWISE	= 0x20
	};	// enum

	template <uint32_t _Width, uint32_t _Height> 
	struct SQuatrisBoard16 : public ::nwol::grid_view_bit<uint32_t>
	{
		static constexpr	const uint32_t			Height							= _Height;
		static constexpr	const uint32_t			Width							= _Width;
							uint16_t				Cells[_Height]					= {};

		inline constexpr							SQuatrisBoard16					()																		: grid_view_bit(Cells, Width, Height)		{}

							int32_t					EvaluateLines					(::nwol::array_pod<int16_t>& lines)								const	{
			static	const uint32_t						lineMask	= 0x3FF;
					uint16_t		i;
			for(uint8_t y=0; y<Height; ++y) {
				if((Cells[y] & lineMask) == lineMask)
					lines.push_back(y);
			}

			return lines.size();
		}
	};	// struct


	template <uint32_t _Width, uint32_t _Height> 
	struct SQuatrisBoard32 : public ::nwol::grid_view_bit<uint32_t>
	{
		static constexpr	const uint32_t			Height							= _Height;
		static constexpr	const uint32_t			Width							= _Width;
							uint32_t				Cells[_Height]					= {};

		inline constexpr							SQuatrisBoard32					()																		: grid_view_bit(Cells, Width, Height)		{}
		inline				operator				SQuatrisBoard16<Width, Height>	()																const	{
			static	const uint32_t						lineMask	= 0x7;
			SQuatrisBoard16<Width, Height>				newBoard	= {};
			for(uint8_t y=0; y < Height; ++y) {
				int											count		= 0;
				for(uint8_t x=0; x < Width; ++x) {
					const uint32_t								shiftedMask	= lineMask << x;
					if((Cells[y] & shiftedMask))
						newBoard.Cells[y] |= 1 << x;
				}
			}

			return newBoard;
		}
	};	// struct

	template <uint32_t _Width, uint32_t _Height>
	struct Quatris
	{
		SQuatrisBoard32<_Width, _Height>			Board							= {};
		uint32_t									Score							= 0;
		::nwol::STimer								Timer							= {};
		::nwol::SAccumulator<double>				DropTime						= {0, 0.75};
		::nwol::SAccumulator<double>				MoveTime						= {0, 0.15};
		::nwol::SCoord2<int32_t>					CoordPiece						= {0, 0};
		int32_t										PieceOrientation				= 0;
		int32_t										CurrentPiece					= -1;
		const uint16_t								Pieces			[2][7]			=	{	{0x1111, 0x63, 0x36, 0x17, 0x47, 0x33, 0x27}
																						,	{0x1111, 0x63, 0x36, 0x17, 0x47, 0x33, 0x27}
																						};	

		int32_t										MovePiece						(CUATRIS_COMMAND command)												{
			switch(command)	{
			case CUATRIS_COMMAND_MOVE_RIGHT					: break;
			case CUATRIS_COMMAND_MOVE_LEFT					: break;
			case CUATRIS_COMMAND_MOVE_DOWN					: break;
			case CUATRIS_COMMAND_MOVE_DROP					: break;
			case CUATRIS_COMMAND_ROTATE_CLOCKWISE			: break;
			case CUATRIS_COMMAND_ROTATE_COUNTER_CLOCKWISE	: break;
			}
			return 0;
		}
		int32_t										Tick							(CUATRIS_COMMAND command, ::nwol::array_pod<int16_t>& lines)			{
			Timer.Frame();
			bool					stop	= false;
			if(DropTime.Accumulate(Timer.LastTimeSeconds))	{
				
			}

			do
			{
				SQuatrisBoard16 quickBoard = Board;
				quickBoard.EvaluateLines(lines);
			} while(lines);

			if(!command)
				return 0;

			if(MoveTime.Accumulate(Timer.LastTimeSeconds))	{
				switch(command)	{
				case CUATRIS_COMMAND_MOVE_RIGHT					: break;
				case CUATRIS_COMMAND_MOVE_LEFT					: break;
				case CUATRIS_COMMAND_MOVE_DOWN					: break;
				case CUATRIS_COMMAND_MOVE_DROP					: break;
				case CUATRIS_COMMAND_ROTATE_CLOCKWISE			: break;
				case CUATRIS_COMMAND_ROTATE_COUNTER_CLOCKWISE	: break;
				}
			}

			return 0;
		}
	};	// struct
} // namespace

#endif	// __CUATRIS_H__892364987234__
		//bool			Cells	[7][16]	=	{	{	0, 1, 0, 0			// 0x2222
		//										,	0, 1, 0, 0
		//										,	0, 1, 0, 0
		//										,	0, 1, 0, 0
		//										},
		//										{	1, 1				// 0x660
		//										,	1, 1
		//										},
		//										{	1, 1, 0
		//										,	0, 1, 1
		//										,	0, 0, 0
		//										},
		//										{	0, 1, 1
		//										,	1, 1, 0
		//										,	0, 0, 0
		//										},
		//										{	1, 1, 1
		//										,	1, 0, 0
		//										,	0, 0, 0
		//										},
		//										{	1, 1, 1
		//										,	0, 0, 1
		//										,	0, 0, 0
		//										},
		//										{	0, 0, 0				// 0x270
		//										,	1, 1, 1
		//										,	0, 1, 0
		//										},
		//										{	0, 1, 0, 0			// 0x2222
		//										,	0, 1, 0, 0
		//										,	0, 1, 0, 0
		//										,	0, 1, 0, 0
		//										},
		//										{	0, 0, 0, 0			// 0x630
		//										,	1, 1, 0, 0
		//										,	0, 1, 1, 0
		//										,	0, 0, 0, 0
		//										},
		//										{	0, 0, 0, 0			// 0x360
		//										,	0, 0, 1, 1
		//										,	0, 1, 1, 0
		//										,	0, 0, 0, 0
		//										},
		//										{	0, 0, 0, 0			// 0x2E0
		//										,	0, 1, 1, 1
		//										,	0, 1, 0, 0
		//										,	0, 0, 0, 0
		//										},
		//										{	0, 0, 0, 0			// 0x470
		//										,	1, 1, 1, 0
		//										,	0, 0, 1, 0
		//										,	0, 0, 0, 0
		//										},
		//										{	0, 0, 0, 0			// 0x660
		//										,	0, 1, 1, 0
		//										,	0, 1, 1, 0
		//										,	0, 0, 0, 0
		//										},
		//										{	0, 0, 0, 0			// 0x270
		//										,	1, 1, 1, 0
		//										,	0, 1, 0, 0
		//										,	0, 0, 0, 0
		//										},
		//									};
