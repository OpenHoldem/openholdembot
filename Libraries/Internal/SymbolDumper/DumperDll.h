#ifndef __DUMPER_DLL_H__
#define __DUMPER_DLL_H__

#ifdef WIN32

#ifdef USERDLL_EXPORTS
#define USERDLL_API __declspec(dllexport)
#else
#define USERDLL_API __declspec(dllimport)
#endif
typedef double	(*p_getsym_t)( int c, const char* psym, bool& iserr );

#else // is LINUX

class App;
#define USERDLL_API extern "C"
typedef double	(*p_getsym_t)(App* app_, int c, const char* psym, bool& iserr );
extern App* appForDumpDll;

#endif //WIN32

struct holdem_player {
    char            m_name[16]          ;	//player name if known
    double          m_balance           ;	//player balance 
    double          m_currentbet        ;	//player current bet
    unsigned char   m_cards[2]          ;	//player cards

    unsigned char   m_name_known    : 1 ;	//0=no 1=yes
    unsigned char   m_balance_known : 1 ;	//0=no 1=yes
    unsigned char   m_fillerbits    : 6 ;	//filler bits
    unsigned char   m_fillerbyte        ;	//filler bytes
};

struct holdem_state {
    char            m_title[64]         ;	//table title
    double          m_pot[10]           ;	//total in each pot

    unsigned char   m_cards[5]          ;	//common cards

    unsigned char   m_is_playing    : 1 ;	//0=sitting-out, 1=sitting-in
    unsigned char   m_is_posting    : 1 ;	//0=autopost-off, 1=autopost-on
    unsigned char   m_fillerbits    : 6 ;	//filler bits

    unsigned char   m_fillerbyte        ;	//filler byte
    unsigned char   m_dealer_chair      ;	//0-9

    holdem_player   m_player[10]        ;	//player records
};

typedef double (*process_message_t)(const char* message, const void* param );
USERDLL_API double process_message( const char* message, const void* param );
typedef void	(*p_send_chat_t) (const char* message);

extern	p_getsym_t		p_get_symbol;
extern	p_send_chat_t		p_sendchat;


double getsym( const char* name );
double process_state(holdem_state* pstate);
double process_query(const char* pquery);

#ifdef WIN32
#ifndef OH_2
// Functions exported by OpenHoldem
extern "C" __declspec(dllimport) double __stdcall GetSymbol(const char* name);
extern "C" __declspec(dllimport) void   __stdcall SendChatMessage(char *message);
extern "C" __declspec(dllimport) void*  __stdcall GetPhl1k();
extern "C" __declspec(dllimport) void*  __stdcall GetPrw1326();
extern "C" __declspec(dllimport) void   __stdcall WriteLog(char* fmt, ...);
extern "C" __declspec(dllimport) char*  __stdcall GetHandnumber();
#endif
#endif

#endif
