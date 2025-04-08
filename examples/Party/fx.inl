#define V2 ImVec2
#define ARFM d->AddRectFilledMultiColor
#define ACF d->AddCircleFilled
#define CH s.SetCurrentChannel
void FX(ImDrawList* d, V2 a, V2 b, V2 sz, ImVec4, float t)
{
    ARFM(a, b, 0xFF1E1E1E, 0xFF1E281E, 0xFF1E1E5A, 0xFF321E78);
    ARFM(a, V2(b.x, a.y + sz.y*0.4f), 0x1EFFDCFF, 0x14FFFFDC, 0x001E1E5A, 0x00321E78);
    auto s = d->_Splitter;
    s.Split(d, 2);
    for (int n = 0; n < 256; n++)
    {
        V2 c(a.x + n / 256.f * sz.x, b.y + 20 - cos(t - 0.1f * n / 2) * 10 + cos(t) * 5);
        float r(40 + sin(t + n / 2) * 40);
        CH(d, 0);
        ACF(c, r + 1, 0x80FFFFFF);
        CH(d, 1);
        ACF(c, r, IM_COL32(255, n / 2, n / 8, 255));
    }
    s.Merge(d);
    d->AddRect(a, b, IM_COL32(128, 128, 128, 100));
}

#define V2 ImVec2
void FX2(ImDrawList* d, V2 a, V2 b, V2 sz, ImVec4, float t)
{
    float sx = 1.f / 16.f;
    float sy = 1.f / 9.f;
    for (float ty = 0.0f; ty < 1.0f; ty += sy)
        for (float tx = 0.0f; tx < 1.0f; tx += sx)
        {
            V2 c((tx + 0.5f * sx), (ty + 0.5f * sy));
            float k = 0.45f;
            d->AddRectFilled(
                V2(a.x + (c.x - k * sx) * sz.x, a.y + (c.y - k * sy) * sz.y),
                V2(a.x + (c.x + k * sx) * sz.x, a.y + (c.y + k * sy) * sz.y),
                IM_COL32(ty * 200, tx * 255, 100, 255));
        }
}

//v1.1
#define V2 ImVec2
void FX3(ImDrawList*d,V2 a,V2,V2 sz,ImVec4,float t)
{
  static struct{int y,h,c; float t,s;} m[40]={0};
  static int S=0x1234;
  static float t0=t;
  float ZI=t*.07f,Z=ZI+1.f;
  for(int x=0;x<40;x++)
  {
    auto& M=m[x];
    int i=x>=15&&x<25;
    if(M.s==0.f||M.y>16)
    {
      M.h = (t<7.f||i)*((int)(2+t*.5f) + S%(int)(6+(t*0.3f)));
      M.y = (M.s==0.f)*-(S%15)-M.h;
      M.c += S;
      M.s = (5+(S%14))*(.01f-t*.001f);
      if(t>5.f&&i)
      {
        M.c = (340003872375972UL>>(x*5-75))&31;
        M.h = i?(x!=19):0;
      }
    }
    if((M.t-=t-t0)<0.f)
    {
      if(t<6.f||!i||M.y!=6)
        M.y++;
      M.t += M.s;
    }
    char c=64|M.c%42;
    for(int j=0; j<M.h; j++,c=64|(c^M.c+M.h^j)%42)
      for(int f=(j+1==M.h)?13:4+(M.c&1);f--;)
        d->AddText(0, 13*(i?Z:-Z), V2(a.x-(sz.x*.5f*ZI)+x*8*Z+sin(j+t*f), a.y-(sz.y*.5f*ZI)+(M.y+j)*13*Z+cos(x*f-t)), 0x3c68bb5b, &c, &c+1);
    S|=((S&1)^((S&8)>>2))<<16;
    S>>=1;
  }
  t0 = t;
}


#define V2 ImVec2
void FX4(ImDrawList* d, V2 a, V2 b, V2 s, ImVec4 m, float t)
{
	int N = 25;
	float sp = s.y / N, y, st = sin(t) * 0.5 + 0.5,
	      r[3] = { 1500, 1087 + 200 * st, 1650 },
	      ctr[3][2] = { { 150, 140 }, { s.x * m.x, s.y * m.y },
		      { 40 + 200 * st, 73 + 20*sin(st * 5) } };
	for (int i = 0; i < N; i++) {
		y = a.y + sp*(i+.5);
		for (int x = a.x; x <= b.x; x += 2) {
			float D = 0, o = 255;
			for (int k = 0; k < 3; k++)
				D += r[k]/(pow(x-a.x-ctr[k][0], 2) + pow(y-a.y-ctr[k][1], 2));
			if (D < 1) continue;
			if (D>2.5) D = 2.5;
			if (D < 1.15) o /= 2;
			d->AddLine(V2(x, y), V2(x+2, y), IM_COL32(239, 129, 19, o), D + sin(2.3 * t + 0.3 * i));
		}
	}
}


#define V2 ImVec2
#define S sinf
#define C cosf
#define I int
#define F float
#define CL(x,l,h) (((x)>(h))?(h):(((x)<(l))?(l):(x)))
#define PI 3.1415926535
#define CO(c,b) (int(c*255)<<b)
void FX5(ImDrawList* d, V2 a, V2 b, V2 s, ImVec4 m, float t)
{
    t*=3;
    F ix=s.x/320;
    F iy=s.y/180;
    F sz=s.x / 15;
    for (F x=a.x;x<b.x;x+=ix)
        for (F y=a.y;y<b.y;y+=iy){
            F v=0;
            v+=S((x/sz+t));
            v+=S((y/sz+t)/2.0f);
            v+=S((x/sz+y/sz+t)/2.0f);
            F cx=x/sz/10+0.3*S(t/3.0);
            F cy=y/sz/10+0.3f*C(t/2.0);
            v+=S(sqrt(100*(cx*cx+cy*cy+1))+t);
            v=CL(v/2, 0, 1);
            F r=S(v*PI)*.5f+.5f;
            F g=S(v*PI+PI/3)*.5f+.5f;
            F b=S(v*PI+PI)*.5f+.5f;
            d->AddQuadFilled({x,y},{x+ix,y},{x+ix,y+iy},{x,y+iy},0xff000000|CO(b,16)|CO(g,8)|CO(r,0));
        }
}

#define v2 ImVec2
#define wh(a) ImColor(1.f,1.f,1.f,a)
#define px(v,c) d->AddRectFilled(a+v*2,a+v*2+v2(2,2),c);
#define fr(v,x) for(int v=0;v<x;++v)
#define frc(x) ((x)-int(x))
#define hash(x) (srand(x),ImLerp(rand()/32767.f,rand()/32767.f,0.5f))
#define noise(x) ImLerp(hash(x),hash(x+1),frc(x))
void FX6(ImDrawList* d, v2 a, v2 b, v2 sz, v2 mm, float t)
{
	fr(x,160)fr(y,90){
		auto rx=x-80;
		auto ry=y-45;
		auto an=ImAtan2(rx,ry);an=an<0?M_PI*2+an:an;
		auto len=(rx*rx+ry*ry+0.1f)+t*4;
		auto n0 = noise(an);
		auto n1 = noise(len);
		auto al= n0+n1;
		px(v2(x,y),wh(al));
	}
}

