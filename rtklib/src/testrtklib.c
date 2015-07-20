#include "rtklib.h"

extern int init_raw(raw_t* raw)
{
	const double lam_glo[NFREQ]={CLIGHT/FREQ1_GLO,CLIGHT/FREQ2_GLO};
    gtime_t time0={0};
    obsd_t data0={{0}};
    eph_t  eph0 ={0,-1,-1};
    alm_t  alm0 ={0,-1};
    geph_t geph0={0,-1};
    seph_t seph0={0};
    sbsmsg_t sbsmsg0={0};
    lexmsg_t lexmsg0={0};
    int i,j,sys;
    
    //trace(3,"init_raw:\n");
    
    raw->time=raw->tobs=time0;
    raw->ephsat=0;
    raw->sbsmsg=sbsmsg0;
    raw->msgtype[0]='\0';
    for (i=0;i<MAXSAT;i++) {
        for (j=0;j<380  ;j++) raw->subfrm[i][j]=0;
        for (j=0;j<NFREQ;j++) raw->lockt[i][j]=0.0;
        for (j=0;j<NFREQ;j++) raw->halfc[i][j]=0;
        raw->icpp[i]=raw->off[i]=raw->prCA[i]=raw->dpCA[i]=0.0;
    }
    for (i=0;i<MAXOBS;i++) raw->freqn[i]=0;
    raw->lexmsg=lexmsg0;
    raw->icpc=0.0;
    raw->nbyte=raw->len=0;
    raw->iod=raw->flag=raw->tbase=raw->outtype=0;
    raw->tod=-1;
    for (i=0;i<MAXRAWLEN;i++) raw->buff[i]=0;
    raw->opt[0]='\0';
    raw->receive_time=0.0;
    raw->plen=raw->pbyte=raw->page=raw->reply=0;
    raw->week=0;
    
    raw->obs.data =NULL;
    raw->obuf.data=NULL;
    raw->nav.eph  =NULL;
    raw->nav.alm  =NULL;
    raw->nav.geph =NULL;
    raw->nav.seph =NULL;
    
    /*if (!(raw->obs.data =(obsd_t *)malloc(sizeof(obsd_t)*MAXOBS))||
        !(raw->obuf.data=(obsd_t *)malloc(sizeof(obsd_t)*MAXOBS))||
        !(raw->nav.eph  =(eph_t  *)malloc(sizeof(eph_t )*MAXSAT))||
        !(raw->nav.alm  =(alm_t  *)malloc(sizeof(alm_t )*MAXSAT))||
        !(raw->nav.geph =(geph_t *)malloc(sizeof(geph_t)*NSATGLO))||
        !(raw->nav.seph =(seph_t *)malloc(sizeof(seph_t)*NSATSBS*2))) {
        free_raw(raw);
        return 0;
    }*/
    raw->obs.n =0;
    raw->obuf.n=0;
    raw->nav.n =MAXSAT;
    raw->nav.na=MAXSAT;
    raw->nav.ng=NSATGLO;
    raw->nav.ns=NSATSBS*2;
    for (i=0;i<MAXOBS   ;i++) raw->obs.data [i]=data0;
    for (i=0;i<MAXOBS   ;i++) raw->obuf.data[i]=data0;
    for (i=0;i<MAXSAT   ;i++) raw->nav.eph  [i]=eph0;
    for (i=0;i<MAXSAT   ;i++) raw->nav.alm  [i]=alm0;
    for (i=0;i<NSATGLO  ;i++) raw->nav.geph [i]=geph0;
    for (i=0;i<NSATSBS*2;i++) raw->nav.seph [i]=seph0;
    /*for (i=0;i<MAXSAT;i++) for (j=0;j<NFREQ;j++) {
        if (!(sys=satsys(i+1,NULL))) continue;
        raw->nav.lam[i][j]=sys==SYS_GLO?lam_glo[j]:lam_carr[j];
    }*/
    raw->sta.name[0]=raw->sta.marker[0]='\0';
    raw->sta.antdes[0]=raw->sta.antsno[0]='\0';
    raw->sta.rectype[0]=raw->sta.recver[0]=raw->sta.recsno[0]='\0';
    raw->sta.antsetup=raw->sta.itrf=raw->sta.deltype=0;
    for (i=0;i<3;i++) {
        raw->sta.pos[i]=raw->sta.del[i]=0.0;
    }
    raw->sta.hgt=0.0;
    return 1;
}

extern int showmsg(char *format, ...)
{
    va_list arg;
    va_start(arg, format); vfprintf(stderr, format, arg); va_end(arg);
    fprintf(stderr, "\r");
    return 0;
}

extern void settspan(gtime_t ts, gtime_t te)
{
}

extern void settime(gtime_t time)
{
}
