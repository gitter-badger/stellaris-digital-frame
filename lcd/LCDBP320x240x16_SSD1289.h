

//
//*****************************************************************************

#ifndef __LCDBP320X240X16_SSD1289_H__
#define __LCDBP320X240X16_SSD1289_H__

typedef char INT_8;
typedef short int INT_16;
typedef int INT_32;
typedef unsigned char uINT_8;
typedef unsigned short int uINT_16;
typedef unsigned int uINT_32;
//extern struct gRectangle;
typedef struct
{
    //
    //! The minimum X coordinate of the rectangle.
    //
	INT_16 sXMin;

    //
    //! The minimum Y coordinate of the rectangle.
    //
	INT_16 sYMin;

    //
    //! The maximum X coordinate of the rectangle.
    //
	INT_16 sXMax;

    //
    //! The maximum Y coordinate of the rectangle.
    //
	INT_16 sYMax;
}
gRectangle;


//*****************************************************************************
//
// Prototypes for the globals exported by this driver.
//
//*****************************************************************************
extern void LCDBP320x240x16_SSD1289Init(void);
extern void LCDBP320x240x16_SSD1289BacklightOn(unsigned char ucBrightness);
extern void LCDBP320x240x16_SSD1289BacklightOff(void);

extern void WriteDataSec(uINT_16 usData, uINT_16 num);
extern void WriteData(uINT_16 usData);
extern void WriteCommand(uINT_8 usData);
//void WriteData5(unsigned short int usData);
//void init0();
extern void setxy(uINT_16 x,uINT_16 y );

////****
extern uINT_16 LCDBPV2_ColorTranslate(uINT_16 ulValue);
extern void	LCDBPV2_LineDrawV(uINT_16 lX, uINT_16 lY1, uINT_16 lY2, uINT_16 ulValue);
extern void LCDBPV2_LineDrawH(uINT_16 lX1, uINT_16 lX2, uINT_16 lY, uINT_16 ulValue);
extern void LCDBPV2_PixelDraw(uINT_16 lX, uINT_16 lY, uINT_16 ulValue);
extern void LCDBPV2_LineDraw(uINT_16 lX1, uINT_16 lY1, uINT_16 lX2, uINT_16 lY2, uINT_16 ulValue);
extern void LCDBPV2_RectFill(const gRectangle *pRect, uINT_16 ulValue);
extern void LCDBPV2_RectDraw(const gRectangle *pRect, uINT_16 ulValue);
extern void LCDBPV2_CircleFill(uINT_16 lX, uINT_16 lY, uINT_16 lRadius, uINT_16 ulValue);
extern void LCDBPV2_CircleDraw(uINT_16 lX, uINT_16 lY, uINT_16 lRadius, uINT_16 ulValue);
extern void LCDBPV2_StringDraw_(const tContext *pContext, const char *pcString, long lLength, long lX, long lY, unsigned long bOpaque);
extern void LCDBPV2_StringDraw(const tFont *pFont,const uINT_8 *pcString, uINT_16 lX, uINT_16 lY, uINT_16 bOpaque, uINT_16 ulValue);

extern const tDisplay g_sLCDBP320x240x16_SSD1289;


//*****************************************************************************
//
// Defines for the pins that are used to communicate with the SSD1289.
//
//*****************************************************************************

//pinmap:
//Data-High:	D0, D1, D2, D3, A4, A5, A6, A7
//Data-Low:		B0, B1, B2, B3, B4, B5, B6, B7
//Command:		RD: Pullup,		RST: C4,
//				CS: C7,		RS(DC): C6,
//				WR: C5		BL: D6

//
//

#define LCD_DATAL_PERIPH        SYSCTL_PERIPH_GPIOD
#define LCD_DATAL_BASE          GPIO_PORTD_BASE

#define LCD_DATAH_PERIPH        SYSCTL_PERIPH_GPIOE
#define LCD_DATAH_BASE          GPIO_PORTE_BASE

#define LCD_COMMAND_PERIPH      SYSCTL_PERIPH_GPIOJ
#define LCD_COMMAND_BASE		GPIO_PORTJ_BASE

#define LCD_RST_PERIPH          SYSCTL_PERIPH_GPIOF//E5
#define LCD_RST_BASE            GPIO_PORTF_BASE
#define LCD_RST_PIN             GPIO_PIN_0

#define LCD_DC_RD_WR_PERIPH     SYSCTL_PERIPH_GPIOJ

#define LCD_DC_BASE             GPIO_PORTJ_BASE			//e3
#define LCD_DC_PIN              GPIO_PIN_1

//#define LCD_RDBK_PERIPH			SYSCTL_PERIPH_GPIOD

#define LCD_RD_BASE             GPIO_PORTJ_BASE		//E2 Pullup
#define LCD_RD_PIN              GPIO_PIN_2

#define LCD_WR_BASE             GPIO_PORTJ_BASE		//e1
#define LCD_WR_PIN              GPIO_PIN_3

#define LCD_CS_PERIPH           SYSCTL_PERIPH_GPIOJ		//d6
#define LCD_CS_BASE             GPIO_PORTJ_BASE
#define LCD_CS_PIN              GPIO_PIN_0

#define LCD_BL_PERIPH           SYSCTL_PERIPH_GPIOD		//d6
#define LCD_BL_BASE             GPIO_PORTD_BASE
#define LCD_BL_PIN              GPIO_PIN_6

#define LCD_BL_TIMER_PERIPH     SYSCTL_PERIPH_TIMER2
#define LCD_BL_TIMER            TIMER_B
#define LCD_BL_TIMER_BASE       TIMER2_BASE


//some color definitions
#define ClrAliceBlue            0x00F0F8FF
#define ClrAntiqueWhite         0x00FAEBD7
#define ClrAqua                 0x0000FFFF
#define ClrAquamarine           0x007FFFD4
#define ClrAzure                0x00F0FFFF
#define ClrBeige                0x00F5F5DC
#define ClrBisque               0x00FFE4C4
#define ClrBlack                0x00000000
#define ClrBlanchedAlmond       0x00FFEBCD
#define ClrBlue                 0x000000FF
#define ClrBlueViolet           0x008A2BE2
#define ClrBrown                0x00A52A2A
#define ClrBurlyWood            0x00DEB887
#define ClrCadetBlue            0x005F9EA0
#define ClrChartreuse           0x007FFF00
#define ClrChocolate            0x00D2691E
#define ClrCoral                0x00FF7F50
#define ClrCornflowerBlue       0x006495ED
#define ClrCornsilk             0x00FFF8DC
#define ClrCrimson              0x00DC143C
#define ClrCyan                 0x0000FFFF
#define ClrDarkBlue             0x0000008B
#define ClrDarkCyan             0x00008B8B
#define ClrDarkGoldenrod        0x00B8860B
#define ClrDarkGray             0x00A9A9A9
#define ClrDarkGreen            0x00006400
#define ClrDarkKhaki            0x00BDB76B
#define ClrDarkMagenta          0x008B008B
#define ClrDarkOliveGreen       0x00556B2F
#define ClrDarkOrange           0x00FF8C00
#define ClrDarkOrchid           0x009932CC
#define ClrDarkRed              0x008B0000
#define ClrDarkSalmon           0x00E9967A
#define ClrDarkSeaGreen         0x008FBC8F
#define ClrDarkSlateBlue        0x00483D8B
#define ClrDarkSlateGray        0x002F4F4F
#define ClrDarkTurquoise        0x0000CED1
#define ClrDarkViolet           0x009400D3
#define ClrDeepPink             0x00FF1493
#define ClrDeepSkyBlue          0x0000BFFF
#define ClrDimGray              0x00696969
#define ClrDodgerBlue           0x001E90FF
#define ClrFireBrick            0x00B22222
#define ClrFloralWhite          0x00FFFAF0
#define ClrForestGreen          0x00228B22
#define ClrFuchsia              0x00FF00FF
#define ClrGainsboro            0x00DCDCDC
#define ClrGhostWhite           0x00F8F8FF
#define ClrGold                 0x00FFD700
#define ClrGoldenrod            0x00DAA520
#define ClrGray                 0x00808080
#define ClrGreen                0x00008000
#define ClrGreenYellow          0x00ADFF2F
#define ClrHoneydew             0x00F0FFF0
#define ClrHotPink              0x00FF69B4
#define ClrIndianRed            0x00CD5C5C
#define ClrIndigo               0x004B0082
#define ClrIvory                0x00FFFFF0
#define ClrKhaki                0x00F0E68C
#define ClrLavender             0x00E6E6FA
#define ClrLavenderBlush        0x00FFF0F5
#define ClrLawnGreen            0x007CFC00
#define ClrLemonChiffon         0x00FFFACD
#define ClrLightBlue            0x00ADD8E6
#define ClrLightCoral           0x00F08080
#define ClrLightCyan            0x00E0FFFF
#define ClrLightGoldenrodYellow 0x00FAFAD2
#define ClrLightGreen           0x0090EE90
#define ClrLightGrey            0x00D3D3D3
#define ClrLightPink            0x00FFB6C1
#define ClrLightSalmon          0x00FFA07A
#define ClrLightSeaGreen        0x0020B2AA
#define ClrLightSkyBlue         0x0087CEFA
#define ClrLightSlateGray       0x00778899
#define ClrLightSteelBlue       0x00B0C4DE
#define ClrLightYellow          0x00FFFFE0
#define ClrLime                 0x0000FF00
#define ClrLimeGreen            0x0032CD32
#define ClrLinen                0x00FAF0E6
#define ClrMagenta              0x00FF00FF
#define ClrMaroon               0x00800000
#define ClrMediumAquamarine     0x0066CDAA
#define ClrMediumBlue           0x000000CD
#define ClrMediumOrchid         0x00BA55D3
#define ClrMediumPurple         0x009370DB
#define ClrMediumSeaGreen       0x003CB371
#define ClrMediumSlateBlue      0x007B68EE
#define ClrMediumSpringGreen    0x0000FA9A
#define ClrMediumTurquoise      0x0048D1CC
#define ClrMediumVioletRed      0x00C71585
#define ClrMidnightBlue         0x00191970
#define ClrMintCream            0x00F5FFFA
#define ClrMistyRose            0x00FFE4E1
#define ClrMoccasin             0x00FFE4B5
#define ClrNavajoWhite          0x00FFDEAD
#define ClrNavy                 0x00000080
#define ClrOldLace              0x00FDF5E6
#define ClrOlive                0x00808000
#define ClrOliveDrab            0x006B8E23
#define ClrOrange               0x00FFA500
#define ClrOrangeRed            0x00FF4500
#define ClrOrchid               0x00DA70D6
#define ClrPaleGoldenrod        0x00EEE8AA
#define ClrPaleGreen            0x0098FB98
#define ClrPaleTurquoise        0x00AFEEEE
#define ClrPaleVioletRed        0x00DB7093
#define ClrPapayaWhip           0x00FFEFD5
#define ClrPeachPuff            0x00FFDAB9
#define ClrPeru                 0x00CD853F
#define ClrPink                 0x00FFC0CB
#define ClrPlum                 0x00DDA0DD
#define ClrPowderBlue           0x00B0E0E6
#define ClrPurple               0x00800080
#define ClrRed                  0x00FF0000
#define ClrRosyBrown            0x00BC8F8F
#define ClrRoyalBlue            0x004169E1
#define ClrSaddleBrown          0x008B4513
#define ClrSalmon               0x00FA8072
#define ClrSandyBrown           0x00F4A460
#define ClrSeaGreen             0x002E8B57
#define ClrSeashell             0x00FFF5EE
#define ClrSienna               0x00A0522D
#define ClrSilver               0x00C0C0C0
#define ClrSkyBlue              0x0087CEEB
#define ClrSlateBlue            0x006A5ACD
#define ClrSlateGray            0x00708090
#define ClrSnow                 0x00FFFAFA
#define ClrSpringGreen          0x0000FF7F
#define ClrSteelBlue            0x004682B4
#define ClrTan                  0x00D2B48C
#define ClrTeal                 0x00008080
#define ClrThistle              0x00D8BFD8
#define ClrTomato               0x00FF6347
#define ClrTurquoise            0x0040E0D0
#define ClrViolet               0x00EE82EE
#define ClrWheat                0x00F5DEB3
#define ClrWhite                0x00FFFFFF
#define ClrWhiteSmoke           0x00F5F5F5
#define ClrYellow               0x00FFFF00
#define ClrYellowGreen          0x009ACD32

#endif

