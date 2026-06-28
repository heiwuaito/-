/**
 * Snake  —  Redesigned               🐍
 *  14 languages   浅色 Apple 风格
 *  悬停蓝色边框   点击闪烁淡出
 *
 *  Compile: g++ -o Snake.exe Snake.cpp -static -lgdi32 -O2
 */

#define WIN32_LEAN_AND_MEAN NOMINMAX
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cwchar>
using namespace std;
constexpr int C=14,R=12,SZ=32,PD=24,HDR=52;
constexpr int WW=C*SZ+PD*2,WH=R*SZ+HDR+PD*2+40;
constexpr int TICK=160,TICK_MIN=60,TICK_STEP=8;

constexpr COLORREF cBG=RGB(255,255,255),cCard=RGB(245,245,247),cSep=RGB(229,229,234),
    cTxt=RGB(28,28,30),cCap=RGB(142,142,147),cBlue=RGB(102,204,255),
    cRed=RGB(255,69,58),cOrange=RGB(255,159,10),
    cBtn=RGB(28,28,30),cFlash=RGB(0,200,255),cHover=RGB(0,120,255),cSel=RGB(0,110,245),cRedAccent=RGB(255,102,102),
    cTile1=RGB(240,240,245),cTile2=RGB(232,232,238),
    cSnake=RGB(102,204,255),cSnakeD=RGB(74,159,207);

struct Pt{int x,y;};enum Dir{NONE,UP,DOWN,LEFT,RIGHT};
struct Rect{int x,y,w,h;};

// ═══  Language  ═══════════════════════════════════════════════════
enum{LANG_ZH,LANG_TW,LANG_BO,LANG_MN,LANG_EN,
     LANG_JA,LANG_FR,LANG_MS,LANG_HI,LANG_RU,
     LANG_KO,LANG_ES,LANG_DE,LANG_N};
struct Str{
    const wchar_t *title,*sub,*start,*scores,*exit;
    const wchar_t *score,*level,*best,*leaderboard,*gameOver;
    const wchar_t *paused,*cont,*restart,*back;
    const wchar_t *noScores,*rank,*date,*time,*lev;
    const wchar_t *selLang,*closeTip,*clickBack;
    const wchar_t *move,*pause,*rst,*sc,*sel,*esc;
};
static const Str S[LANG_N]={
 {L"贪吃蛇",L"经典",L"开始",L"排行",L"退出",L"分数",L"等级",L"最高",L"排行榜",L"游戏结束",
  L"已暂停",L"空格继续",L"R 重开",L"ESC 返回",L"暂无记录",L"排名",L"日期",L"时间",L"等级",
  L"选择语言",L"×",L"点击返回",L"移动 WASD/方向键",L"暂停 空格",L"重开 R",L"排行榜 C",L"选择 Enter",L"退出 Esc"},
 {L"貪吃蛇",L"經典",L"開始",L"排行",L"退出",L"分數",L"等級",L"最高",L"排行榜",L"遊戲結束",
  L"已暫停",L"空格繼續",L"R 重開",L"ESC 返回",L"暫無記錄",L"排名",L"日期",L"時間",L"等級",
  L"選擇語言",L"×",L"點擊返回",L"移動 WASD/方向鍵",L"暫停 空格",L"重開 R",L"排行榜 C",L"選擇 Enter",L"退出 Esc"},
 {L"སྦྲུལ་གྱི་རྩེད་མོ།",L"སྔ་མ།",L"འགོ་ཚུགས།",L"མཐོ་ཤོར།",L"ཕྱིར་འབྱོན།",
  L"མཐོ་ཤོར།",L"རིམ་པ།",L"མཆོག",L"མཐོ་ཤོར།",L"རྩེད་མོ་རྫོགས།",
  L"རེ་སྐྱོང་།",L"Space མུ་མཐུད།",L"R གསར།",L"ESC ཕྱིར།",L"མཐོ་ཤོར་མེད།",L"རིམ་པ།",L"ཚེས།",L"དུས།",L"རིམ།",
  L"སྐད་ཡིག་འདེམས།",L"×",L"ཕྱིར་ནོན།",L"མདའ་/WASD བསྐྱོད།",L"Space རེ་སྐྱོང་།",L"R གསར།",L"C མཐོ།",L"འདེམས།",L"ESC ཕྱིར།"},
 {L"Могой тоглоом",L"Сонгодог",L"Эхлэх",L"Оноо",L"Гарах",
  L"Оноо",L"Түвшин",L"Шилдэг",L"Онооны самбар",L"Тоглоом дууслаа",
  L"Түр зогссон",L"Space үргэлжлүүлэх",L"R шинээр",L"ESC буцах",L"Оноо байхгүй",L"Чансаа",L"Огноо",L"Цаг",L"Түвшин",
  L"Хэл сонгох",L"×",L"Буцах",L"Сум/WASD",L"Space түр зогсох",L"R шинээр",L"C Оноо",L"Сонгох",L"ESC гарах"},
 {L"SNAKE",L"Classic",L"START",L"SCORES",L"EXIT",
  L"SCORE",L"LEVEL",L"BEST",L"LEADERBOARD",L"GAME OVER",
  L"PAUSED",L"Press Space",L"R restart",L"ESC back",L"No scores yet",L"Rank",L"Date",L"Time",L"Level",
  L"Select Language",L"×",L"Click back",L"Arrow/WASD move",L"Space pause",L"R restart",L"C Scores",L"Enter select",L"ESC back"},
 {L"ヘビゲーム",L"クラシック",L"スタート",L"ランキング",L"終了",
  L"スコア",L"レベル",L"最高",L"ランキング",L"ゲームオーバー",
  L"一時停止",L"Space再開",L"R再開",L"ESC戻る",L"記録なし",L"順位",L"日付",L"時間",L"レベル",
  L"言語選択",L"×",L"戻る",L"矢印/WASD",L"Space停止",L"R再開",L"Cランク",L"選択",L"ESC戻る"},
 {L"SERPENT",L"Classique",L"DÉMARRER",L"SCORES",L"QUITTER",
  L"SCORE",L"NIVEAU",L"MEILLEUR",L"CLASSEMENT",L"FIN",
  L"PAUSE",L"Espace continuer",L"R rejouer",L"ESC retour",L"Aucun score",L"Rang",L"Date",L"Heure",L"Niveau",
  L"Choisir langue",L"×",L"Retour",L"Flèches/WASD",L"Espace pause",L"R rejouer",L"C Scores",L"Choisir",L"ESC retour"},
 {L"ULAR",L"Klasik",L"MULA",L"SKOR",L"KELUAR",
  L"SKOR",L"TAHAP",L"TERBAIK",L"PAPAN SKOR",L"TAMAT",
  L"JEDA",L"Space terus",L"R ulang",L"ESC balik",L"Tiada skor",L"Kedudukan",L"Tarikh",L"Masa",L"Tahap",
  L"Pilih Bahasa",L"×",L"Balik",L"Anak panah/WASD",L"Ruang jeda",L"R ulang",L"C Skor",L"Pilih",L"ESC balik"},
 {L"साँप का खेल",L"क्लासिक",L"शुरू",L"स्कोर",L"बाहर",
  L"स्कोर",L"स्तर",L"श्रेष्ठ",L"बोर्ड",L"खेल समाप्त",
  L"विराम",L"Space जारी",L"R पुनः",L"ESC वापस",L"कोई स्कोर नहीं",L"क्रम",L"दिनांक",L"समय",L"स्तर",
  L"भाषा चुनें",L"×",L"वापस",L"तीर/WASD",L"Space विराम",L"R पुनः",L"C स्कोर",L"चुनें",L"ESC वापस"},
 {L"ЗМЕЙКА",L"Классика",L"СТАРТ",L"РЕКОРДЫ",L"ВЫХОД",
  L"ОЧКИ",L"УРОВЕНЬ",L"ЛУЧШИЙ",L"ТАБЛИЦА",L"ИГРА ОКОНЧЕНА",
  L"ПАУЗА",L"Space пробел",L"R заново",L"ESC назад",L"Нет рекордов",L"Место",L"Дата",L"Время",L"Уровень",
  L"Язык",L"×",L"Назад",L"Стрелки/WASD",L"Space пауза",L"R заново",L"C Рекорды",L"Выбрать",L"ESC назад"},
 {L"뱀 게임",L"클래식",L"시작",L"순위",L"종료",
  L"점수",L"레벨",L"최고",L"순위표",L"게임 오버",
  L"일시정지",L"Space 계속",L"R 다시",L"ESC 뒤로",L"기록 없음",L"순위",L"날짜",L"시간",L"레벨",
  L"언어 선택",L"×",L"돌아가기",L"방향키/WASD",L"Space 정지",L"R 재시작",L"C 순위",L"선택",L"ESC 뒤로"},
 {L"SERPIENTE",L"Clásico",L"INICIAR",L"PUNTOS",L"SALIR",
  L"PUNTOS",L"NIVEL",L"MÁXIMO",L"CLASIFICACIÓN",L"FIN",
  L"PAUSA",L"Espacio seguir",L"R reiniciar",L"ESC volver",L"Sin puntos",L"Rango",L"Fecha",L"Hora",L"Nivel",
  L"Idioma",L"×",L"Volver",L"Flechas/WASD",L"Espacio pausa",L"R reiniciar",L"C Puntos",L"Seleccionar",L"ESC volver"},
 {L"SCHLANGE",L"Klassisch",L"START",L"RANGLISTE",L"BEENDEN",
  L"PUNKTE",L"LEVEL",L"BESTE",L"RANGLISTE",L"SPIEL AUS",
  L"PAUSE",L"Leertaste",L"R neustart",L"ESC zurück",L"Keine Punkte",L"Rang",L"Datum",L"Zeit",L"Level",
  L"Sprache",L"×",L"Zurück",L"Pfeile/WASD",L"Pause",L"R Neustart",L"C Rangliste",L"Wählen",L"ESC zurück"},
};
static int g_lang=0;static const Str&T(){return S[g_lang];}
static const wchar_t*g_langNames[]={L"简体中文",L"繁體中文",L"བོད་སྐད",L"Монгол",L"English",
    L"日本語",L"Français",L"Bahasa Melayu",L"हिन्दी",L"Русский",L"한국어",L"Español",L"Deutsch"};

// ═══  Scores  ════════════════════════════════════════════════════
struct Entry{int s;wchar_t d[16],t[16],l[16];};
static void saveScore(int s,int lv){
    if(s<=0)return;CreateDirectoryW(L"D:\\05_AIproject\\SnakeNew\\scores",0);
    SYSTEMTIME tm;GetLocalTime(&tm);
    wchar_t ln[128];int n=swprintf(ln,128,L"%d|%04d-%02d-%02d|%02d:%02d|Lv.%d\n",s,tm.wYear,tm.wMonth,tm.wDay,tm.wHour,tm.wMinute,lv);
    if(n<=0)return;HANDLE f=CreateFileW(L"D:\\05_AIproject\\SnakeNew\\scores\\rank.txt",FILE_APPEND_DATA,0,0,OPEN_ALWAYS,0,0);
    if(f==INVALID_HANDLE_VALUE)return;SetFilePointer(f,0,0,FILE_END);DWORD w;WriteFile(f,ln,n*2,&w,0);CloseHandle(f);}
static vector<Entry>loadScores(){
    vector<Entry>e;HANDLE f=CreateFileW(L"D:\\05_AIproject\\SnakeNew\\scores\\rank.txt",GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
    if(f==INVALID_HANDLE_VALUE)return e;DWORD sz=GetFileSize(f,0);
    if(sz>=2){vector<wchar_t>b(sz/2+2);DWORD r;ReadFile(f,b.data(),sz,&r,0);b[r/2]=0;
        wchar_t*p=b.data();while(*p){wchar_t*nl=wcschr(p,L'\n');if(nl)*nl=0;
            Entry et={0,{},{},{}};swscanf(p,L"%d|%15[^|]|%15[^|]|%15[^\n]",&et.s,et.d,et.t,et.l);
            if(et.s>0)e.push_back(et);if(!nl)break;p=nl+1;}
    }CloseHandle(f);sort(e.begin(),e.end(),[](auto&a,auto&b){return a.s>b.s;});return e;}
static void saveLang(){CreateDirectoryW(L"D:\\05_AIproject\\SnakeNew\\scores",0);
    HANDLE f=CreateFileW(L"D:\\05_AIproject\\SnakeNew\\scores\\lang.txt",GENERIC_WRITE,0,0,CREATE_ALWAYS,0,0);
    if(f==INVALID_HANDLE_VALUE)return;wchar_t c=L'0'+g_lang;DWORD n;WriteFile(f,&c,2,&n,0);CloseHandle(f);}
static void loadLang(){HANDLE f=CreateFileW(L"D:\\05_AIproject\\SnakeNew\\scores\\lang.txt",GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
    if(f==INVALID_HANDLE_VALUE)return;wchar_t c=0;DWORD r;ReadFile(f,&c,2,&r,0);CloseHandle(f);
    int v=c-L'0';if(v>=0&&v<LANG_N)g_lang=v;}

// ═══  Snake  ═════════════════════════════════════════════════════
class Snake{
    deque<Pt>body_,prev_;Pt food_;Dir dir_=RIGHT,ndir_=RIGHT;
    int sc_=0,hi_=0,lv_=1,ms_=TICK,pulse_=0,pop_=0,sv_=0;bool dead_=0,pau_=0;double ac_=0,ip_=0;
public:
    void reset(){
        body_.clear();prev_.clear();body_={{C/2,R/2},{C/2-1,R/2},{C/2-2,R/2}};prev_=body_;
        dir_=ndir_=RIGHT;sc_=0;lv_=1;ms_=TICK;dead_=pau_=0;ac_=ip_=0;pulse_=pop_=sv_=0;spawn();
    }
    void spawn(){
        vector<Pt>c;c.reserve(C*R);
        for(int x=0;x<C;++x)for(int y=0;y<R;++y){bool h=0;for(auto&p:body_)if(p.x==x&&p.y==y){h=1;break;}if(!h)c.push_back({x,y});}
        if(c.empty()){dead_=1;return;}food_=c[rand()%c.size()];}
    void tick(double dt){
        if(dead_||pau_)return;ac_+=dt;
        while(ac_>=ms_){ac_-=ms_;prev_=body_;step();if(dead_)break;}
        ip_=ms_>0?ac_/ms_:0;if(ip_<0)ip_=0;if(ip_>1)ip_=1;}
    void step(){
        if(dead_)return;dir_=ndir_;Pt h=body_.front(),n=h;
        switch(dir_){case UP:n.y--;break;case DOWN:n.y++;break;case LEFT:n.x--;break;case RIGHT:n.x++;break;default:break;}
        if(n.x<0||n.x>=C||n.y<0||n.y>=R){dead_=1;return;}
        bool ate=(n.x==food_.x&&n.y==food_.y);body_.push_front(n);
        if(!ate)body_.pop_back();else{sc_+=10;pop_=12;int l=sc_/40+1;if(l>lv_){lv_=l;ms_=max(TICK_MIN,TICK-(lv_-1)*TICK_STEP);}spawn();}
        for(size_t i=1;i<body_.size();++i)if(n.x==body_[i].x&&n.y==body_[i].y){dead_=1;return;}}
    void key(int vk){if(dead_||pau_)return;
        switch(vk){case VK_UP:case'W':if(dir_!=DOWN)ndir_=UP;break;
        case VK_DOWN:case'S':if(dir_!=UP)ndir_=DOWN;break;
        case VK_LEFT:case'A':if(dir_!=RIGHT)ndir_=LEFT;break;
        case VK_RIGHT:case'D':if(dir_!=LEFT)ndir_=RIGHT;break;}}
    void pause(){if(!dead_)pau_=!pau_;}
    double sx(size_t i)const{if(i>=body_.size())return 0;double a=i<prev_.size()?prev_[i].x:body_[i].x,b=body_[i].x;return a+(b-a)*ip_;}
    double sy(size_t i)const{if(i>=body_.size())return 0;double a=i<prev_.size()?prev_[i].y:body_[i].y,b=body_[i].y;return a+(b-a)*ip_;}
    double fx()const{return food_.x+0.5;}double fy()const{return food_.y+0.5;}
    int sc()const{return sc_;}int lv()const{return lv_;}int hi()const{return hi_;}int&hiR(){return hi_;}
    bool dead()const{return dead_;}bool pau()const{return pau_;}Dir dir()const{return dir_;}size_t len()const{return body_.size();}
    int pulse()const{return pulse_;}int&pulseR(){return pulse_;}int pop()const{return pop_;}int&popR(){return pop_;}
    int sv()const{return sv_;}int&svR(){return sv_;}int ms()const{return ms_;}};

// ═══  Renderer  ═════════════════════════════════════════════════
class Render{
    HDC dc_,mem_;HBITMAP bmp_;int w_,h_;BLENDFUNCTION bf_{AC_SRC_OVER,0,255,0};
public:
    Render(HDC dc,int w,int h):dc_(dc),w_(w),h_(h){mem_=CreateCompatibleDC(dc);bmp_=CreateCompatibleBitmap(dc,w,h);SelectObject(mem_,bmp_);}
    ~Render(){DeleteObject(bmp_);DeleteDC(mem_);}
    HDC operator()(){return mem_;}void flush(){BitBlt(dc_,0,0,w_,h_,mem_,0,0,SRCCOPY);}
    void clear(){RECT r={0,0,w_,h_};HBRUSH b=CreateSolidBrush(cBG);FillRect(mem_,&r,b);DeleteObject(b);}
    void fill(int x,int y,int w,int h,COLORREF c){RECT r={x,y,x+w,y+h};HBRUSH b=CreateSolidBrush(c);FillRect(mem_,&r,b);DeleteObject(b);}
    void box(int x,int y,int w,int h,COLORREF c,int t=1){
        HPEN p=CreatePen(PS_SOLID,t,c),op=(HPEN)SelectObject(mem_,p);
        HBRUSH ob=(HBRUSH)SelectObject(mem_,GetStockObject(NULL_BRUSH));
        Rectangle(mem_,x,y,x+w,y+h);SelectObject(mem_,ob);SelectObject(mem_,op);DeleteObject(p);}
    void rnd(int x,int y,int w,int h,COLORREF c,int rr=8){
        HBRUSH b=CreateSolidBrush(c);HPEN op=(HPEN)SelectObject(mem_,GetStockObject(NULL_PEN));
        RoundRect(mem_,x,y,x+w,y+h,rr*2,rr*2);SelectObject(mem_,op);DeleteObject(b);}
    void cir(int cx,int cy,int r,COLORREF c){
        HBRUSH b=CreateSolidBrush(c);HPEN op=(HPEN)SelectObject(mem_,GetStockObject(NULL_PEN));
        Ellipse(mem_,cx-r,cy-r,cx+r,cy+r);SelectObject(mem_,op);DeleteObject(b);}
    void txt(int x,int y,const wchar_t*s,COLORREF c,int sz=15,bool b=false){
        SetTextColor(mem_,c);SetBkMode(mem_,TRANSPARENT);
        HFONT hf=CreateFontW(sz,0,0,0,b?FW_SEMIBOLD:FW_NORMAL,0,0,0,DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,L"Segoe UI");
        HFONT of=(HFONT)SelectObject(mem_,hf);TextOutW(mem_,x,y,s,(int)wcslen(s));SelectObject(mem_,of);DeleteObject(hf);}
    void tc(int x,int y,const wchar_t*s,COLORREF c,int sz=15,bool b=false){
        SIZE sz_{};GetTextExtentPoint32W(mem_,s,(int)wcslen(s),&sz_);txt(x-sz_.cx/2,y-sz_.cy/2,s,c,sz,b);}
    void fade(int alpha){
        bf_.SourceConstantAlpha=alpha;HDC tmp=CreateCompatibleDC(mem_);
        HBITMAP b=CreateCompatibleBitmap(mem_,w_,h_);SelectObject(tmp,b);
        RECT r={0,0,w_,h_};HBRUSH br=CreateSolidBrush(RGB(255,255,255));FillRect(tmp,&r,br);DeleteObject(br);
        AlphaBlend(mem_,0,0,w_,h_,tmp,0,0,w_,h_,bf_);DeleteObject(b);DeleteDC(tmp);}
};

// ═══  State  ═════════════════════════════════════════════════════
struct Btn{int x,y,w,h;};
enum{A_IDLE,A_FLASH,A_FADE};
struct AppState{
    int screen=0,sel=-1,hover=-1,langFirst=1;
    int anim=A_IDLE,animBtn=-1,animTarget=0;
    double animT=0,langEnterT=0;
    Btn btns[3]={{0,0,140,42}};
    RECT globe={0,0,0,0},langClose={0,0,0,0},langItems[LANG_N]={};
    int langHover=-1;
    RECT goRestart={0,0,0,0},goBack={0,0,0,0};int goHover=-1;
};
static AppState g_st;static Snake g_sn;static LARGE_INTEGER g_fq{};
static double nowMs(){LARGE_INTEGER n;QueryPerformanceCounter(&n);return n.QuadPart*1000.0/g_fq.QuadPart;}
static void startAnim(int btn,int target){g_st.anim=A_FLASH;g_st.animBtn=btn;g_st.animTarget=target;g_st.animT=nowMs();}
static void startFade(int target){g_st.anim=A_FADE;g_st.animBtn=-1;g_st.animTarget=target;g_st.animT=nowMs();}
static void drawGlobe(Render&r){
    int cx=WW-26,cy=22,rad=12;
    r.box(cx-rad,cy-rad,rad*2,rad*2,cCap,2);
    HPEN p=CreatePen(PS_SOLID,1,cCap),op=(HPEN)SelectObject(r(),p);
    MoveToEx(r(),cx,cy-rad,0);LineTo(r(),cx,cy+rad);MoveToEx(r(),cx-rad,cy,0);LineTo(r(),cx+rad,cy);
    SelectObject(r(),op);DeleteObject(p);SetRect(&g_st.globe,cx-rad,cy-rad,cx+rad,cy+rad);}

// ═══  Screens  ══════════════════════════════════════════════════
static void drawTitle(Render&r){
    r.clear();r.tc(WW/2,50,T().title,cTxt,30,true);
    r.tc(WW/2,78,T().sub,cCap,14);r.fill(WW/2-60,92,120,2,cBlue);
    const wchar_t*lb[]={T().start,T().scores,T().exit};
    int bw=140,bh=42,gap=16,tw=bw*3+gap*2,sx=(WW-tw)/2,by=172;
    for(int i=0;i<3;++i){
        int x=sx+i*(bw+gap);g_st.btns[i]={x,by,bw,bh};
        bool hovered=(i==g_st.hover);
        bool selected=(i==g_st.sel&&g_st.hover<0);
        bool flash=(g_st.anim==A_FLASH&&g_st.animBtn==i);
        if(flash){
            double t=(nowMs()-g_st.animT)/180.0;int cyc=int(t*6)%2;
            COLORREF fc=cyc?cRed:cFlash;
            r.rnd(x-3,by-3,bw+6,bh+6,fc,10);
            r.rnd(x,by,bw,bh,fc,8);r.fill(x+8,by+bh-3,bw-16,2,cBG);
            r.tc(x+bw/2,by+bh/2,lb[i],cBG,14,true);
            continue;
        }
        if(hovered&&g_st.anim==A_IDLE){
            r.rnd(x-3,by-3,bw+6,bh+6,cHover,9);
            r.rnd(x,by,bw,bh,cBtn,9);r.fill(x+8,by+bh-3,bw-16,2,cHover);
            r.tc(x+bw/2,by+bh/2,lb[i],cHover,14,true);
        }else if(selected&&g_st.anim==A_IDLE){
            r.rnd(x,by,bw,bh,cBtn,8);
            r.fill(x+8,by+bh-3,bw-16,2,cRedAccent);
            r.tc(x+bw/2,by+bh/2,lb[i],cBlue,14,true);
        }else{
            r.rnd(x,by,bw,bh,cBtn,8);r.fill(x+8,by+bh-3,bw-16,2,cBlue);
            r.tc(x+bw/2,by+bh/2,lb[i],cBlue,14,true);
        }
    }
    drawGlobe(r);
    if(g_st.anim==A_FADE){double t=(nowMs()-g_st.animT)/300.0;if(t>1)t=1;r.fade(int(t*200));}
}

static void drawBoard(Render&r){
    r.clear();wchar_t buf[128];
    r.tc(WW/2,40,T().leaderboard,cTxt,24,true);r.fill(WW/2-90,58,180,2,cBlue);
    swprintf(buf,128,L"%-5s  %-7s  %-10s  %-6s  %s",T().rank,T().score,T().date,T().time,T().lev);
    r.txt(WW/2-160,75,buf,cCap,13);
    auto e=loadScores();int y=98;
    if(e.empty())r.tc(WW/2,y,T().noScores,cCap,14);
    else for(int i=0;i<min((int)e.size(),10);++i){
        bool cur=(e[i].s==g_sn.sc()&&g_sn.sc()>0);
        swprintf(buf,128,L" %-4d  %-6d   %s   %s   %s",i+1,e[i].s,e[i].d,e[i].t,e[i].l);
        r.txt(WW/2-160,y,buf,cur?cOrange:cTxt,14,cur);y+=22;}
    r.tc(WW/2,y+20,T().clickBack,cCap,13);drawGlobe(r);
    if(g_st.anim==A_FADE){double t=(nowMs()-g_st.animT)/300.0;if(t>1)t=1;r.fade(int(t*200));}
}

static void drawGame(Render&r){
    r.clear();wchar_t buf[64];int gx=PD,gy=HDR,gd=SZ;
    r.txt(gx,14,T().score,cCap,13);
    swprintf(buf,64,L"%d",g_sn.sc());r.txt(gx+48,12,buf,cTxt,16,true);
    r.txt(gx+100,14,T().level,cCap,13);
    swprintf(buf,64,L"%d",g_sn.lv());r.txt(gx+140,12,buf,cTxt,16,true);
    r.txt(gx+185,14,T().best,cCap,13);
    swprintf(buf,64,L"%d",max(g_sn.hi(),g_sn.sc()));r.txt(gx+222,12,buf,cTxt,16,true);
    if(g_sn.sc()>g_sn.hi())g_sn.hiR()=g_sn.sc();r.fill(gx,48,C*gd,1,cSep);
    for(int y=0;y<R;++y)for(int x=0;x<C;++x)r.fill(gx+x*gd,gy+y*gd,gd,gd,((x+y)%2==0)?cTile1:cTile2);
    r.box(gx-1,gy-1,C*gd+2,R*gd+2,cSep,1);
    double ft=g_sn.pulse()/30.0,bob=sin(ft*6.283)*2;
    double fx=g_sn.fx()*gd+bob*0.3,fy=g_sn.fy()*gd+bob;
    bool gl=g_sn.pulse()<5||(g_sn.pulse()>15&&g_sn.pulse()<20);
    r.cir(int(gx+fx),int(gy+fy),gd/2-4,cRedAccent);
    size_t n=g_sn.len();
    for(size_t i=0;i<n;++i){
        double sx=gx+g_sn.sx(i)*gd,sy=gy+g_sn.sy(i)*gd;
        if(i==0){
            r.rnd(int(sx)+1,int(sy)+1,gd-2,gd-2,cSnakeD,8);
            r.cir(int(sx)+gd/2-6,int(sy)+gd/2-6,3,cBG);
            r.cir(int(sx)+gd/2+4,int(sy)+gd/2-6,3,cBG);
            switch(g_sn.dir()){case RIGHT:r.fill(int(sx)+gd-2,int(sy)+gd/2-2,8,4,cBlue);break;
            case LEFT:r.fill(int(sx)-6,int(sy)+gd/2-2,8,4,cBlue);break;
            case UP:r.fill(int(sx)+gd/2-2,int(sy)-6,4,8,cBlue);break;
            case DOWN:r.fill(int(sx)+gd/2-2,int(sy)+gd-2,4,8,cBlue);break;}
        }else{float t_=(float)i/n;int sr=102-int(t_*40),sg=204-int(t_*85),sb=255-int(t_*60);
            r.rnd(int(sx)+2,int(sy)+2,gd-4,gd-4,RGB(sr,sg,sb),5);}
    }
    if(g_sn.pop()>0){int px=gx+int(g_sn.fx()*gd),py=gy+int(g_sn.fy()*gd-gd/2);r.fill(px-14,py-4,28,16,cRedAccent);r.tc(px,py+4,L"+10",cBG,12,true);}
    if(g_sn.pau()){int px=gx+C*gd/2-70,py=gy+R*gd/2-22;r.fill(px,py,140,44,cBG);r.box(px,py,140,44,cSep,1);
        r.tc(px+70,py+10,T().paused,cTxt,16,true);r.tc(px+70,py+28,T().cont,cCap,11);}
    if(g_sn.dead()){
        int pw=240,ph=116,px=gx+C*gd/2-pw/2,py=gy+R*gd/2-ph/2;
        r.rnd(px,py,pw,ph,cCard,10);r.box(px,py,pw,ph,cRed,2);
        r.tc(px+pw/2,py+18,T().gameOver,cRed,18,true);
        swprintf(buf,64,L"%s %d    %s %d",T().score,g_sn.sc(),T().level,g_sn.lv());
        r.tc(px+pw/2,py+42,buf,cTxt,13);
        int bw=88,bh=34,gap=16,bx1=px+(pw-(bw*2+gap))/2,bx2=bx1+bw+gap,by=py+ph-bh-14;
        SetRect(&g_st.goRestart,bx1,by,bx1+bw,by+bh);
        SetRect(&g_st.goBack,bx2,by,bx2+bw,by+bh);
        bool flash0=(g_st.anim==A_FLASH&&g_st.animBtn==0);
        bool flash1=(g_st.anim==A_FLASH&&g_st.animBtn==1);
        bool h0=(g_st.goHover==0)&&!flash0&&!flash1;
        bool h1=(g_st.goHover==1)&&!flash0&&!flash1;
        // Restart button
        if(flash0){double t=(nowMs()-g_st.animT)/180.0;int cyc=int(t*6)%2;COLORREF fc=cyc?cRed:cFlash;
            r.rnd(bx1-3,by-3,bw+6,bh+6,fc,10);r.rnd(bx1,by,bw,bh,fc,8);r.fill(bx1+6,by+bh-3,bw-12,2,cBG);}
        else{if(h0)r.rnd(bx1-3,by-3,bw+6,bh+6,cHover,9);r.rnd(bx1,by,bw,bh,cBtn,9);r.fill(bx1+6,by+bh-3,bw-12,2,h0?cHover:cBlue);}
        {const wchar_t*s=T().restart;if(!wcsncmp(s,L"R ",2))s+=2;else if(!wcsncmp(s,L"R",1))s+=1;
            r.tc(bx1+bw/2,by+bh/2,s,flash0?cBG:cBlue,13,true);}
        // Back button
        if(flash1){double t=(nowMs()-g_st.animT)/180.0;int cyc=int(t*6)%2;COLORREF fc=cyc?cRed:cFlash;
            r.rnd(bx2-3,by-3,bw+6,bh+6,fc,10);r.rnd(bx2,by,bw,bh,fc,8);r.fill(bx2+6,by+bh-3,bw-12,2,cBG);}
        else{if(h1)r.rnd(bx2-3,by-3,bw+6,bh+6,cHover,9);r.rnd(bx2,by,bw,bh,cBtn,9);r.fill(bx2+6,by+bh-3,bw-12,2,h1?cHover:cBlue);}
        {const wchar_t*s=T().back;if(!wcsncmp(s,L"ESC ",4))s+=4;else if(!wcsncmp(s,L"ESC",3))s+=3;
            r.tc(bx2+bw/2,by+bh/2,s,flash1?cBG:cBlue,13,true);}
    }
    if(g_st.anim==A_FADE){double t=(nowMs()-g_st.animT)/300.0;if(t>1)t=1;r.fade(int(t*200));}
}

static void drawLang(Render&r){
    r.clear();r.fill(0,0,WW,WH,cBG);int pw=320,ph=58+LANG_N*34+18,px=(WW-pw)/2,py=(WH-ph)/2;
    r.rnd(px,py,pw,ph,cCard,12);r.box(px,py,pw,ph,cSep,1);
    r.tc(px+pw/2,py+18,T().selLang,cTxt,16,true);r.fill(px+24,py+38,pw-48,1,cSep);
    // Close button
    bool closeFlash=(g_st.anim==A_FLASH&&g_st.animBtn==-2);
    if(closeFlash){double t=(nowMs()-g_st.animT)/180.0;int cyc=int(t*6)%2;COLORREF fc=cyc?cRed:cFlash;
        r.rnd(px+pw-44,py+10,32,32,fc,6);r.tc(px+pw-28,py+26,T().closeTip,cBG,16);}
    else{r.rnd(px+pw-44,py+10,32,32,cBtn,6);r.fill(px+pw-40,py+38,24,2,cBlue);
        r.tc(px+pw-28,py+26,T().closeTip,cBlue,16);}
    SetRect(&g_st.langClose,px+pw-44,py+10,px+pw-12,py+42);
    for(int i=0;i<LANG_N;++i){
        int ly=py+50+i*34;bool sel=(i==g_lang),hov=(i==g_st.langHover);
        bool itemFlash=(g_st.anim==A_FLASH&&g_st.animBtn==i);
        if(itemFlash){double t=(nowMs()-g_st.animT)/180.0;int cyc=int(t*6)%2;COLORREF fc=cyc?cRed:cFlash;
            r.rnd(px+14,ly,pw-28,28,fc,6);r.tc(px+28,ly+6,g_langNames[i],cBG,14,true);}
        else if(sel){
            r.rnd(px+14,ly,pw-28,28,cBtn,6);
            r.fill(px+20,ly+22,pw-36,2,cRedAccent);
            r.txt(px+28,ly+6,g_langNames[i],cBlue,14,true);
        }else if(hov){
            r.rnd(px+12,ly-2,pw-24,32,cHover,8);
            r.rnd(px+14,ly,pw-28,28,cBtn,6);
            r.fill(px+20,ly+22,pw-36,2,cHover);
            r.txt(px+28,ly+6,g_langNames[i],cHover,14,true);
        }else{
            r.rnd(px+14,ly,pw-28,28,cBtn,6);
            r.fill(px+20,ly+22,pw-36,2,cBlue);
            r.txt(px+28,ly+6,g_langNames[i],cBlue,14,false);
        }
        SetRect(&g_st.langItems[i],px+14,ly,px+pw-14,ly+28);}
    double lt=(nowMs()-g_st.langEnterT)/300.0;
    if(lt<1.0)r.fade(int((1.0-lt)*255));
    if(g_st.anim==A_FADE){double t=(nowMs()-g_st.animT)/300.0;if(t>1)t=1;r.fade(int(t*200));}
}

// ═══  Window Proc  ══════════════════════════════════════════════
LRESULT CALLBACK WndProc(HWND h,UINT m,WPARAM w,LPARAM l){
    switch(m){
    case WM_CREATE:srand((unsigned)time(0));loadLang();g_sn.reset();
        QueryPerformanceFrequency(&g_fq);
        g_st.langEnterT=nowMs();g_st.screen=g_st.langFirst?3:0;g_st.langFirst=0;
        SetTimer(h,1,8,0);return 0;
    case WM_TIMER:{
        if(g_st.anim!=A_IDLE){
            double el=nowMs()-g_st.animT;
            if(g_st.anim==A_FLASH&&el>=180){
                if(g_st.animTarget==-1){g_sn.reset();g_st.anim=A_IDLE;g_st.goHover=-1;}
                else{g_st.anim=A_FADE;g_st.animT=nowMs();}}

            if(g_st.anim==A_FADE&&el>=300){
                g_st.anim=A_IDLE;g_st.screen=g_st.animTarget;
                g_st.sel=g_st.hover=g_st.langHover=-1;
                if(g_st.animTarget==1)g_sn.reset();}
        }
        if(g_st.screen==1&&g_st.anim==A_IDLE){
            g_sn.tick(8);
            if(g_sn.pop()>0)g_sn.popR()--;g_sn.pulseR()=(g_sn.pulse()+1)%30;
            if(g_sn.dead()&&g_sn.sv()!=g_sn.sc()){saveScore(g_sn.sc(),g_sn.lv());g_sn.svR()=g_sn.sc();}}
        InvalidateRect(h,0,FALSE);break;}
    case WM_PAINT:{PAINTSTRUCT ps;HDC dc=BeginPaint(h,&ps);
        RECT cr;GetClientRect(h,&cr);Render r(dc,cr.right-cr.left,cr.bottom-cr.top);
        switch(g_st.screen){case 0:drawTitle(r);break;case 1:drawGame(r);break;case 2:drawBoard(r);break;case 3:drawLang(r);break;}
        r.flush();EndPaint(h,&ps);return 0;}
    case WM_LBUTTONDOWN:{int mx=LOWORD(l),my=HIWORD(l);
        if(g_st.anim!=A_IDLE)break;
        if(g_st.screen==3){
            if(mx>=g_st.langClose.left&&mx<=g_st.langClose.right&&my>=g_st.langClose.top&&my<=g_st.langClose.bottom)
                {g_st.anim=A_FLASH;g_st.animBtn=-2;g_st.animTarget=0;g_st.animT=nowMs();saveLang();InvalidateRect(h,0,FALSE);break;}
            for(int i=0;i<LANG_N;++i){RECT&it=g_st.langItems[i];
                if(mx>=it.left&&mx<=it.right&&my>=it.top&&my<=it.bottom)
                    {g_lang=i;g_st.anim=A_FLASH;g_st.animBtn=i;g_st.animTarget=0;g_st.animT=nowMs();saveLang();InvalidateRect(h,0,FALSE);break;}
            }break;}
        if((g_st.screen==0||g_st.screen==2)&&mx>=g_st.globe.left&&mx<=g_st.globe.right&&my>=g_st.globe.top&&my<=g_st.globe.bottom)
            {g_st.langEnterT=nowMs();g_st.screen=3;InvalidateRect(h,0,FALSE);break;}
        if(g_st.screen==0){
            for(int i=0;i<3;++i){auto&b=g_st.btns[i];
                if(mx>=b.x&&mx<=b.x+b.w&&my>=b.y&&my<=b.y+b.h){
                    if(i==0)startAnim(i,1);else if(i==2)startAnim(i,-1);else startAnim(i,2);
                    InvalidateRect(h,0,FALSE);break;}}
        }else if(g_st.screen==1&&g_sn.dead()){
            if(mx>=g_st.goRestart.left&&mx<=g_st.goRestart.right&&my>=g_st.goRestart.top&&my<=g_st.goRestart.bottom)
                {g_st.anim=A_FLASH;g_st.animBtn=0;g_st.animTarget=-1;g_st.animT=nowMs();InvalidateRect(h,0,FALSE);break;}
            if(mx>=g_st.goBack.left&&mx<=g_st.goBack.right&&my>=g_st.goBack.top&&my<=g_st.goBack.bottom)
                {g_st.anim=A_FLASH;g_st.animBtn=1;g_st.animTarget=0;g_st.animT=nowMs();InvalidateRect(h,0,FALSE);break;}
        }else if(g_st.screen==2){g_st.screen=0;g_st.sel=g_st.hover=-1;InvalidateRect(h,0,FALSE);}
        break;}
    case WM_MOUSEMOVE:{
        if(g_st.anim!=A_IDLE)break;
        if(g_st.screen==0){int mx=LOWORD(l),my=HIWORD(l),nh=-1;
            for(int i=0;i<3;++i)if(mx>=g_st.btns[i].x&&mx<=g_st.btns[i].x+g_st.btns[i].w&&my>=g_st.btns[i].y&&my<=g_st.btns[i].y+g_st.btns[i].h)nh=i;
            if(nh!=g_st.hover){g_st.hover=nh;if(nh>=0)g_st.sel=-1;InvalidateRect(h,0,FALSE);}}
        else if(g_st.screen==1&&g_sn.dead()){int mx=LOWORD(l),my=HIWORD(l),nh=-1;
            if(mx>=g_st.goRestart.left&&mx<=g_st.goRestart.right&&my>=g_st.goRestart.top&&my<=g_st.goRestart.bottom)nh=0;
            else if(mx>=g_st.goBack.left&&mx<=g_st.goBack.right&&my>=g_st.goBack.top&&my<=g_st.goBack.bottom)nh=1;
            if(nh!=g_st.goHover){g_st.goHover=nh;InvalidateRect(h,0,FALSE);}}
        else if(g_st.screen==3){int mx=LOWORD(l),my=HIWORD(l),nh=-1;
            for(int i=0;i<LANG_N;++i){RECT&it=g_st.langItems[i];
                if(mx>=it.left&&mx<=it.right&&my>=it.top&&my<=it.bottom){nh=i;break;}}
            if(nh!=g_st.langHover){g_st.langHover=nh;InvalidateRect(h,0,FALSE);}}
        break;}
    case WM_KEYDOWN:{int vk=(int)w;
        if(g_st.anim!=A_IDLE)break;
        if(g_st.screen==3){
            if(vk==VK_ESCAPE){g_st.anim=A_FLASH;g_st.animBtn=-2;g_st.animTarget=0;g_st.animT=nowMs();saveLang();InvalidateRect(h,0,FALSE);break;}
            if(vk==VK_UP||vk==VK_DOWN){g_lang=(g_lang+(vk==VK_UP?LANG_N-1:1))%LANG_N;InvalidateRect(h,0,FALSE);break;}
            if(vk==VK_RETURN||vk==VK_SPACE){g_st.anim=A_FLASH;g_st.animBtn=g_lang;g_st.animTarget=0;g_st.animT=nowMs();saveLang();InvalidateRect(h,0,FALSE);break;}break;}
        if(g_st.screen==0){
            if(vk==VK_RETURN){if(g_st.sel<0)g_st.sel=0;
                if(g_st.sel==0)startAnim(0,1);else if(g_st.sel==2)startAnim(2,-1);else startAnim(1,2);}
            else if(vk==VK_ESCAPE)PostQuitMessage(0);
            else if(vk==VK_LEFT||vk==VK_RIGHT){g_st.sel=g_st.sel<0?0:(g_st.sel+((vk==VK_LEFT)?2:1))%3;}
            else if(w=='C'||w=='c')startAnim(1,2);else if(w=='S'||w=='s')g_st.sel=0;else if(w=='E'||w=='e')g_st.sel=2;
            else if(w==VK_SPACE)startAnim(0,1);
            InvalidateRect(h,0,FALSE);}
        else if(g_st.screen==1){
            if(g_sn.dead()){if(vk=='R'||vk=='r')g_sn.reset();else if(vk==VK_ESCAPE){g_st.screen=0;g_st.sel=g_st.hover=-1;}}
            else{if(vk==VK_SPACE||w=='P'||w=='p')g_sn.pause();else if(vk=='R'||vk=='r')g_sn.reset();else if(vk==VK_ESCAPE){g_st.screen=0;g_st.sel=g_st.hover=-1;}else g_sn.key(vk);}
            InvalidateRect(h,0,FALSE);}
        else if(g_st.screen==2){if(vk==VK_ESCAPE||vk==VK_RETURN||vk==VK_SPACE){g_st.screen=0;g_st.sel=g_st.hover=-1;InvalidateRect(h,0,FALSE);}}
        break;}
    case WM_DESTROY:PostQuitMessage(0);return 0;}
    return DefWindowProcW(h,m,w,l);}

// ═══  Entry  ═══════════════════════════════════════════════════
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR,int){
    WNDCLASSW wc={};wc.lpfnWndProc=WndProc;wc.hInstance=hInst;
    wc.hCursor=LoadCursor(0,IDC_ARROW);wc.lpszClassName=L"SnakeNew";
    RegisterClassW(&wc);
    int sw=GetSystemMetrics(SM_CXSCREEN),sh=GetSystemMetrics(SM_CYSCREEN);
    HWND hw=CreateWindowExW(0,L"SnakeNew",L"Snake",
        WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
        (sw-WW)/2,(sh-WH)/2,WW+16,WH+38,0,0,hInst,0);
    if(!hw)return 1;ShowWindow(hw,SW_SHOW);UpdateWindow(hw);
    MSG msg;while(GetMessage(&msg,0,0,0)){TranslateMessage(&msg);DispatchMessage(&msg);}
    return 0;}
