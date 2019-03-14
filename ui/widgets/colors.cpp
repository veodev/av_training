#include <qcolor.h>
#include <qmath.h>

//#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
const int HLSMAX = 240;
const int RGBMAX = 255;

float HueToRGB(float n1, float n2, float hue)
{
    float Result;
    if (hue < 0) {
        hue = hue + HLSMAX;
    }
    if (hue > HLSMAX) {
        hue = hue - HLSMAX;
    }

    if (hue < (HLSMAX / 6.0)) {
        Result = (n1 + (((n2 - n1) * hue + (HLSMAX / 12.0)) / (HLSMAX / 6.0)));
    }
    else if (hue < (HLSMAX / 2.0)) {
        Result = n2;
    }
    else if (hue < ((HLSMAX * 2.0) / 3.0)) {
        Result = (n1 + (((n2 - n1) * (((HLSMAX * 2.0) / 3.0) - hue) + (HLSMAX / 12.0)) / (HLSMAX / 6.0)));
    }
    else {
        Result = n1;
    }

    return Result;
}
/*
double HueToRGB(double n1, double n2, double hue )
{
    double Result;
    if (hue < 0) {
        hue= hue + HLSMAX;
    }
    if (hue > HLSMAX) {
        hue= hue - HLSMAX;
    }

    if (hue < (HLSMAX/6.0)) {
        Result= ( n1 + (((n2-n1)*hue+(HLSMAX/12.0))/(HLSMAX/6.0)) );
    }
    else if (hue < (HLSMAX/2.0)) {
        Result= n2;
    }
    else if (hue < ((HLSMAX*2.0)/3.0)) {
        Result= (n1 + (((n2-n1)*(((HLSMAX*2.0)/3.0)-hue)+(HLSMAX/12.0))/(HLSMAX/6.0)));
    }
    else {
        Result= n1;
    }

    return Result;
}
*/
QColor HLStoRGB(int H, int L, int S)
{
    float R, G, B;  //  цвета

    if (S == 0) {
        B = qRound(static_cast<float>(L * RGBMAX) / HLSMAX);
        R = B;
        G = B;
    }
    else {
        float Magic1, Magic2;
        if (L <= qRound(HLSMAX / 2.0f)) {
            Magic2 = static_cast<float>(static_cast<float>(L * (HLSMAX + S)) + (HLSMAX / 2.0f)) / HLSMAX;
        }
        else {
            Magic2 = static_cast<float>(L + S) - static_cast<float>(static_cast<float>(L * S) + (HLSMAX / 2.0f)) / HLSMAX;
        }
        Magic1 = 2.0f * L - Magic2;
        R = static_cast<float>(HueToRGB(Magic1, Magic2, H + (HLSMAX / 3.0f)) * RGBMAX + (HLSMAX / 2.0f)) / HLSMAX;
        G = static_cast<float>(HueToRGB(Magic1, Magic2, H) * RGBMAX + (HLSMAX / 2.0f)) / HLSMAX;
        B = static_cast<float>(HueToRGB(Magic1, Magic2, H - (HLSMAX / 3.0f)) * RGBMAX + (HLSMAX / 2.0f)) / HLSMAX;
    }
    if (R < 0) R = 0;
    if (R > RGBMAX) R = RGBMAX;
    if (G < 0) G = 0;
    if (G > RGBMAX) G = RGBMAX;
    if (B < 0) B = 0;
    if (B > RGBMAX) B = RGBMAX;

    return QColor(qCeil(R), qCeil(G), qCeil(B));
}


#define RGB(r, g, b) ((ushort)(((uchar)(b) | ((ushort)((uchar)(g)) << 5)) | ((ushort)((uchar)(r)) << 11)))
const int RBMAX = 31;
const int GMAX = 63;
/*
ushort HLStoRGB16(int H, int L, int S)
{
    float R, G, B; //  цвета

    if (S == 0) {
        B = qRound(static_cast<float>(L*RBMAX)/HLSMAX);
        R = B;
        G = B;
    }
    else {
        float Magic1, Magic2;
        if (L <= qRound(HLSMAX/2.0f)) {
            Magic2 = static_cast<float>( static_cast<float>(L*(HLSMAX + S)) + (HLSMAX/2.0f))/HLSMAX;
        }
        else {
            Magic2 = static_cast<float>(L + S) - static_cast<float>( static_cast<float>(L*S) + (HLSMAX/2.0f))/HLSMAX;
        }
        Magic1= 2.0f * L - Magic2;
        R = static_cast<float>(HueToRGB(Magic1,Magic2,H+(HLSMAX/3.0f))*RBMAX + (HLSMAX/2.0f))/HLSMAX;
        G = static_cast<float>(HueToRGB(Magic1,Magic2,H)*GMAX + (HLSMAX/2.0f)) / HLSMAX ;
        B = static_cast<float>(HueToRGB(Magic1,Magic2,H-(HLSMAX/3.0f))*RBMAX + (HLSMAX/2.0f))/HLSMAX;
    }
    if (R<0) R=0;
    if (R>RBMAX) R= RBMAX;
    if (G<0) G=0;
    if (G>GMAX) G= GMAX;
    if (B<0) B=0;
    if (B>RBMAX) B= RBMAX;

//    return QColor(qCeil(R),qCeil(G),qCeil(B));
    return ( RGB(R,G,B) );
}
*/
#define HLS_L 116
#define HLS_S 218
// const float Magic2=static_cast<float>( static_cast<float>(HLS_L*(HLSMAX + HLS_S)) + (HLSMAX/2.0f))/HLSMAX;
ushort HLStoRGB16(int H)
{
    float R, G, B;  //  цвета

    float Magic1, Magic2;

    if (HLS_L <= qRound(HLSMAX / 2.0f)) {
        Magic2 = static_cast<float>(static_cast<float>(HLS_L * (HLSMAX + HLS_S)) + (HLSMAX / 2.0f)) / HLSMAX;
    }
    else {
        Magic2 = static_cast<float>(HLS_L + HLS_S) - static_cast<float>(static_cast<float>(HLS_L * HLS_S) + (HLSMAX / 2.0f)) / HLSMAX;
    }
    Magic1 = 2.0f * HLS_L - Magic2;
    R = static_cast<float>(HueToRGB(Magic1, Magic2, H + (HLSMAX / 3.0f)) * RBMAX + (HLSMAX / 2.0f)) / HLSMAX;
    G = static_cast<float>(HueToRGB(Magic1, Magic2, H) * GMAX + (HLSMAX / 2.0f)) / HLSMAX;
    B = static_cast<float>(HueToRGB(Magic1, Magic2, H - (HLSMAX / 3.0f)) * RBMAX + (HLSMAX / 2.0f)) / HLSMAX;
    if (R < 0) R = 0;
    if (R > RBMAX) R = RBMAX;
    if (G < 0) G = 0;
    if (G > GMAX) G = GMAX;
    if (B < 0) B = 0;
    if (B > RBMAX) B = RBMAX;

    return (RGB(R, G, B));
}

QColor getColorFromRGB565(unsigned short color)
{
    unsigned char r = (color >> 11) & 0b0000000000011111;
    unsigned char g = (color >> 5) & 0b0000000000111111;
    unsigned char b = color & 0b0000000000011111;
    float redF = static_cast<float>(r) / static_cast<float>(0b0000000000011111);
    float greenF = static_cast<float>(g) / static_cast<float>(0b0000000000111111);
    float blueF = static_cast<float>(b) / static_cast<float>(0b0000000000011111);
    return QColor::fromRgbF(redF, greenF, blueF);
}

unsigned short getRGB565FromQColor(const QColor& color)
{
    int r, g, b;
    color.getRgb(&r, &g, &b);
    unsigned short red = (31 * r) / 255;
    unsigned short green = (63 * g) / 255;
    unsigned short blue = (31 * b) / 255;
    unsigned short result = (red << 11) | (green << 5) | blue;
    return result;
}

unsigned short getGradientColor(float gradPosition, unsigned short begin, unsigned short end)
{
    unsigned char rBegin = (begin >> 11) & 0b0000000000011111;
    unsigned char gBegin = (begin >> 5) & 0b0000000000111111;
    unsigned char bBegin = begin & 0b0000000000011111;

    unsigned char rEnd = (end >> 11) & 0b0000000000011111;
    unsigned char gEnd = (end >> 5) & 0b0000000000111111;
    unsigned char bEnd = end & 0b0000000000011111;

    unsigned short red = static_cast<unsigned short>(gradPosition * rBegin + (1 - gradPosition) * rEnd);
    unsigned short green = static_cast<unsigned short>(gradPosition * gBegin + (1 - gradPosition) * gEnd);
    unsigned short blue = static_cast<unsigned short>(gradPosition * bBegin + (1 - gradPosition) * bEnd);

    unsigned short result = (red << 11) | (green << 5) | blue;

    return result;
}
