/*
 * display.c
 * 表示系ラッパー関数を定義する
 */

#include <string.h>
#include <curses.h>
#include <iconv.h>
#include <stdlib.h>

#include "rogue.h"
#include "display.h"
#include "machdep.h"
#include "message.h"

/*
 * Ncurses Colors
 */
enum rogue_colors{
  WHITE = 1,
  RED = 2,
  GREEN = 3,
  YELLOW = 4,
  BLUE = 5,
  MAGENTA = 6,
  CYAN = 7,
  WHITE_REVERSE = 8,
  RED_REVERSE = 9,
  GREEN_REVERSE = 10,
  YELLOW_REVERSE = 11,
  BLUE_REVERSE = 12,
  MAGENTA_REVERSE = 13,
  CYAN_REVERSE = 14,
  TRANSPARENT = -1,
};
//
#define RWHITE	 8
#define RRED	 9
#define RGREEN	 10
#define RYELLOW	 11
#define RBLUE	 12
#define RMAGENTA 13
#define RCYAN	 14
//
static int ch_attr[256];
char *color_str = "cbmyg";
extern boolean use_color;

/*
 * init_color_attr
 * カラー属性配列の初期化
 */
void init_color_attr(void){
  char *chx, chy, *chz;
  int i, j, k;
  char color_type[] = "wrgybmcWRGYBMC";
  int colormap_list[5];
#if defined( COLOR )
  static boolean first_init = 1;
  /* 最初の一回のみ実行する命令 */
  if(first_init){
	  start_color();
    /* 背景色の定義 */
    assume_default_colors(COLOR_WHITE, TRANSPARENT);
    /* 表示色の定義 */
    init_pair(WHITE, COLOR_WHITE, TRANSPARENT);
    init_pair(RED, COLOR_RED, TRANSPARENT);
    init_pair(GREEN, COLOR_GREEN, -TRANSPARENT);
    init_pair(YELLOW, COLOR_YELLOW, TRANSPARENT);
    init_pair(BLUE, COLOR_BLUE, TRANSPARENT);
    init_pair(MAGENTA, COLOR_MAGENTA, TRANSPARENT);
    init_pair(CYAN, COLOR_CYAN, TRANSPARENT);
    init_pair(WHITE_REVERSE, TRANSPARENT, COLOR_WHITE);
    init_pair(RED_REVERSE, TRANSPARENT, COLOR_RED);
    init_pair(GREEN_REVERSE, TRANSPARENT, COLOR_GREEN);
    init_pair(YELLOW_REVERSE, TRANSPARENT, COLOR_YELLOW);
    init_pair(BLUE_REVERSE, TRANSPARENT, COLOR_BLUE);
    init_pair(MAGENTA_REVERSE, TRANSPARENT, COLOR_MAGENTA);
    init_pair(CYAN_REVERSE, TRANSPARENT, COLOR_CYAN);
  }
#endif /* COLOR */
  /* 表示設定解析 */
  for(i = 0; i < 5 && color_str[i]; i++){
    j = r_index(color_type, color_str[i], 0);
    if (j >= 0) {
      switch (color_type[j]) {
        default:
          colormap_list[i] = 0;
          break;
        case 'w':
          colormap_list[i] = WHITE;
          break;
        case 'r':
          colormap_list[i] = RED;
          break;
        case 'g':
          colormap_list[i] = GREEN;
          break;
        case 'y':
          colormap_list[i] = YELLOW;
          break;
        case 'b':
          colormap_list[i] = BLUE;
          break;
        case 'm':
          colormap_list[i] = MAGENTA;
          break;
        case 'c':
          colormap_list[i] = CYAN;
          break;
        case 'W':
          colormap_list[i] = WHITE_REVERSE;
          break;
        case 'R':
          colormap_list[i] = RED_REVERSE;
          break;
        case 'G':
          colormap_list[i] = GREEN_REVERSE;
          break;
        case 'Y':
          colormap_list[i] = YELLOW_REVERSE;
          break;
        case 'B':
          colormap_list[i] = BLUE_REVERSE;
          break;
        case 'M':
          colormap_list[i] = MAGENTA_REVERSE;
          break;
        case 'C':
          colormap_list[i] = CYAN_REVERSE;
          break;
      }
    }
  }
  /* 文字のカラーマップの作成 */
  for(chx = "-|#+"; *chx; chx++){
    get_colorpair_number((signed) *chx, colormap_list[0]);
  }
  get_colorpair_number((signed) '.', colormap_list[1]);
  for (chy = 'A'; chy <= 'Z'; chy++) {
    get_colorpair_number((signed) chy, colormap_list[2]);
  }
  for (chz = "%!?/=)]^*:,"; *chz; chz++) {
    get_colorpair_number((signed) *chz, colormap_list[3]);
  }
  get_colorpair_number(rogue.fchar, colormap_list[4]);
  if (!use_color) {
    for(k=0; k<128; k++) {
      get_colorpair_number(k,0);
    }
  }
}
/*
 * put_color_pair
 * 指定の文字のカラーペア番号を出力する
 */
int put_colorpair_number(char ch){
  return ch_attr[(signed) ch];
}
/*
 * get_color_pair
 * 指定の文字列のカラーペア番号を入力する
 */
void get_colorpair_number(char ch, int num){
  ch_attr[(signed) ch] = num;
}

/*
 * addch_rogue
 * addch のラッパー関数
 * 文字毎のカラー表示も一括して請け負う
 */
int addch_rogue(const chtype ch){
#if defined( COLOR )
  attrset(COLOR_PAIR(put_colorpair_number(ch)));
#endif /* COLOR */
  return addch(ch);
}
/*
 * mvaddch_rogue
 * mvaddch のラッパー関数
 * 文字毎のカラー表示も一括して請け負う
 */
int mvaddch_rogue(int y, int x, const chtype ch){
#if defined( COLOR )
  attrset(COLOR_PAIR(put_colorpair_number(ch)));
#endif /* COLOR */
  return mvaddch(y, x, ch);
}
/*
 * addstr_rogue
 * addstr のラッパー関数
 * 文字列のカラー表示も一括して請け負う
 */
int addstr_rogue(const char *str){
#if defined( COLOR )
  attrset(COLOR_PAIR(0));
#endif /* COLOR */
  return addstr(str);
}
/**
 *
 */
/**
#define pr_err(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
//int sjis2utf8(str,outbuf,sizeof(outbuf)); 
int sjis2utf8(const char *src, char *dst, size_t len)
{
    iconv_t conv; // conversion descriptor
    char buf[1024];
    char *src_buf = buf;
    char *dst_buf = dst;
    size_t src_len = strlen(src);
    size_t dst_len = len - 1;
    int ret = 0;
    strncpy(buf, src, sizeof(buf));
    if ((conv = iconv_open("UTF-8", "CP932")) == (iconv_t) - 1) {
        //pr_err("error: %s: %s\n", __FUNCTION__, "iconv open");
        return pr_err("error: %s: %s\n", __FUNCTION__, "iconv open");
        //return -errno;
    }
    if (iconv(conv, &src_buf, &src_len, &dst_buf, &dst_len) == (size_t) - 1) {
        //pr_err("error: %s: %s\n", __FUNCTION__, "iconv");
        return pr_err("error: %s: %s\n", __FUNCTION__, "iconv");
        //ret = -errno;
    }
    *dst_buf = '\0';
    if (iconv_close(conv) == -1) {
        //pr_err("error: %s: %s\n", __FUNCTION__, "iconv_close");
        return pr_err("error: %s: %s\n", __FUNCTION__, "iconv_close");
        //ret = -errno;
    }
    return ret;
}
*/
static iconv_t conv=NULL;
//int convert(char const *src,char const *dest,char const *text,char *buf,size_t bufsize){
int convert(char const *src,char const *dest,char *text,char *buf,size_t bufsize){
  iconv_t cd;
  size_t srclen, destlen;
  size_t ret;
  // dest:UTF-8//TRANSLIT
  // src :UTF-8
  cd=iconv_open(dest, src);
  if (cd==(iconv_t)-1){ perror("iconv open"); return 0; }
  srclen = strlen(text);
  destlen = bufsize-1;
  memset(buf,'\0',bufsize);
  ret = iconv(cd,&text,&srclen,&buf,&destlen);
  /**
  if (ret==-1) { 
    printf("%s\n",text);
    printf( "ERROR in S2U (%s)\n",text);
    perror("iconv"); 
    return 0; 
  }
  */
  iconv_close(cd);
  return 1;
}
/*
 * mvaddstr_rogue
 * mvaddstr のラッパー関数
 * 文字列のカラー表示も一括して請け負う
 */
  int
//mvaddstr_rogue(int y, int x, const char *str)
mvaddstr_rogue(int y, int x, char *str)
{
#if defined( COLOR )
  attr_t attr_stats;
  short color_stats;

  attr_get(&attr_stats, &color_stats, NULL);
  if (attr_stats & A_REVERSE) {
    attrset(COLOR_PAIR(CYAN));
    attron(A_REVERSE);
  } else if (strcmp(str, mesg[187]) == 0) {
    attrset(COLOR_PAIR(YELLOW));
  } else if (strcmp(str, mesg[188]) == 0) {
    attrset(COLOR_PAIR(GREEN));
  } else {
    attrset(COLOR_PAIR(0));
  }
#endif /* COLOR */

  char outbuf[1024];
  //sjis2utf8(str,outbuf,sizeof(outbuf)); 
  int ret=convert("UTF-8","UTF-8//TRANSLIT",str,outbuf,sizeof(outbuf));
  //int ret=convert("UTF-8","ASCII//TRANSLIT",str,outbuf,sizeof(outbuf));

  /**
    size_t inleft=utf8strlen(str);
    size_t outleft=1024-1;
    char *inptr=str;
    char *outbuf[outleft];
    char *outptr=outbuf;

    if(!conv) {
    conv=iconv_open("UTF-8//TRANSLIT", "UTF-8");
    }else {
    iconv(conv, NULL,NULL,NULL,NULL);
    }
  //int n=iconv(conv,&inptr,&inleft,&outptr,&outleft);
  int n=iconv(conv,&inptr,&inleft,&outptr,&outleft);

  int res=mvaddstr(y,x,outbuf);
  free(outbuf);
  return res;
  */
  int res=mvaddstr(y,x,outbuf);
  return res;
}

/*
 * mvinch_rogue
 * mvinch のラッパー関数
 * 属性を除去して文字情報だけを抽出する
 */
  chtype
mvinch_rogue(int y, int x)
{
#if defined( COLOR )
  return mvinch(y, x) & A_CHARTEXT;
#else /* not COLOR */
  return mvinch(y, x);
#endif /* not COLOR */
}
//あ
