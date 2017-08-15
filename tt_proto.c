#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "tt_proto.h"
#include "tt_debug.h"

void pro_host2net(PRO_HD *phd)
{
	if(!phd)
		return;
	phd->header = htons(phd->header);
	phd->len = htons(phd->len);
	phd->cmd = htons(phd->cmd);
	phd->seq = htonl(phd->seq);
	phd->reserve = htonl(phd->reserve);
}

void pro_net2host(PRO_HD *phd)
{
	if(!phd)
		return;
	phd->header = ntohs(phd->header);
	phd->len = ntohs(phd->len);
	phd->cmd = ntohs(phd->cmd);
	phd->seq = ntohl(phd->seq);
	phd->reserve = ntohl(phd->reserve);
}

void pro_hd2hexbuf(PRO_HD *phd, uchar *hex)
{
	if(!phd || !hex)
		return;
	
	pro_host2net(phd);
	uchar *cp = (uchar*)phd;
	int i = 0;
	for(i=0; i < sizeof(PRO_HD); i++)
		hex[i] = cp[i];
}

void pro_pro2hexbuf(PRO *pro, uchar *hex, int *len)
{
	if(!pro || !hex)
		return;
	
	int paylen = pro->hd.len;
	
	/* switch to network for sending */
	pro_host2net(&pro->hd);
	
	uchar *cp = (uchar*)&(pro->hd);
	int i = 0, inx = 0;
	for(i=0; i < sizeof(PRO_HD); i++,inx++) {
		hex[inx] = cp[i];
	}
	i = 0;
	for(i=0; i < paylen; i++,inx++) {
		hex[inx] = pro->payload[i];
	}
//	hex[inx++] = (pro->crc & (ushort)0xff00) >> 8;
//	hex[inx++] = (pro->crc & (ushort)0x00ff);

	if(len) *len = inx;
}

void pro_hex2hd(uchar *hex, PRO_HD *phd)
{
	if(!phd || !hex)
		return;
	phd = (PRO_HD*)hex;
	/* switch to local for stored */
	pro_net2host(phd);
}

void pro_hex2pro(uchar *hex, PRO *pro)
{
	if(!pro || !hex)
		return;

	memmove(&pro->hd,hex,sizeof(PRO_HD));
	/* switch to local for stored */
	pro_net2host(&pro->hd);
	
	int i = 0, inx = sizeof(PRO_HD);
	pro->payload = (uchar*)malloc(sizeof(pro->hd.len));
	for(i=0; i < pro->hd.len; i++,inx++) {
		pro->payload[i] = hex[inx];
	}
//	pro->crc = hex[inx++] << 8;
//	pro->crc += hex[inx++];
}

void dump_phd(PRO_HD *phd)
{
	if(!phd) {
		debug(LOG_NOTICE, "Pro Hd is NULL!\n");
		return;
	}
	debug(LOG_DEBUG, "PRO HD Store INFO:\n");
	debug(LOG_DEBUG, "Header:\t%04x\n",phd->header);
	debug(LOG_DEBUG, "Len:\t%04x\n",phd->len);
	debug(LOG_DEBUG, "Ver:\t%02x\n",phd->ver);
	debug(LOG_DEBUG, "Cmd:\t%04x\n",phd->cmd);
	debug(LOG_DEBUG, "Stat:\t%02x\n",phd->stat);
	debug(LOG_DEBUG, "Seq:\t%08x\n",phd->seq);
	debug(LOG_DEBUG, "Reserve:\t%08x\n",phd->reserve);
	
}

void dump_pro(PRO *pro)
{
	if(!pro) {
		debug(LOG_NOTICE, "PRO is NULL!\n");
		return;
	}
	PRO_HD *phd = &pro->hd;
	debug(LOG_DEBUG, "PRO Store INFO:\n");
	debug(LOG_DEBUG, "Header:\t%04x\n",phd->header);
	debug(LOG_DEBUG, "Len:\t%04x\n",phd->len);
	debug(LOG_DEBUG, "Ver:\t%02x\n",phd->ver);
	debug(LOG_DEBUG, "Cmd:\t%04x\n",phd->cmd);
	debug(LOG_DEBUG, "Stat:\t%02x\n",phd->stat);
	debug(LOG_DEBUG, "Seq:\t%08x\n",phd->seq);
	debug(LOG_DEBUG, "Reserve:\t%08x\n",phd->reserve);
	int i = 0;
	debug(LOG_DEBUG, "Payload:\n");
	char strbuf[4] = {0};
	char allbuf[4096] = {0};
	for(i=0; i < phd->len; i++) {
		sprintf(strbuf,"%02x ",pro->payload[i]);
		strcat(allbuf,strbuf);
		if(i%10 == 0 && i > 0)
			strcat(allbuf,"\n");
	}
	debug(LOG_DEBUG, "%s\n",allbuf);
}

void test_hex2pro()
{
	uchar prodata[] = {0xa5,0xa5,0x00,0x04,0x01,0x00,0xf0,0x01,0x00,0x00,0x00,0x01,0x11,0x11,0x11,0x11,0x30,0x31,0x32,0x33,0x0a,0x0d};

	PRO *prohex = NULL;
	prohex = (PRO*)malloc(sizeof(PRO));
	
	/* will malloc for pro->payload witch length is pro->hd.len */
	pro_hex2pro(prodata,prohex);
	
	printf("==============hex buf switch PRO struct\n");
	dump_pro(prohex);

	uchar outhex[256];
	int outlen = 0;
	pro_pro2hexbuf(prohex,outhex,&outlen);
	printf("\n%d bak outhex:\n",outlen);
	int i = 0;
	for(i=0; i < outlen; i++)
		printf("%02x ",outhex[i]);
	printf("\n");
	
	if(prohex && prohex->payload) {
		free(prohex->payload);
		free(prohex);
	}
}

void test_pro2hex()
{
	printf("=================PRO struct switch to hexbuf\n");
	PRO *pro = NULL;
	pro = (PRO*)malloc(sizeof(PRO));
	pro->hd.header = 0xa5a5;
	pro->hd.len = 0x04;
	pro->hd.ver = 0x01;
	pro->hd.cmd = 0xf0;
	pro->hd.stat = 0x01;
	pro->hd.seq = 0x01;
	pro->hd.reserve = 0x11111111;
	int i = 0;
	int a = 0x30;
	pro->payload = (uchar*)malloc(pro->hd.len);
	for(i=0; i < pro->hd.len; i++)
		pro->payload[i] = a++;
//	pro->crc = 0x0a0d;
	
	dump_pro(pro);
	
	uchar outhex[256];
	int outlen = 0;
	pro_pro2hexbuf(pro,outhex,&outlen);
	printf("\n%d outhex:\n",outlen);
	for(i=0; i < outlen; i++)
		printf("%02x ",outhex[i]);
	printf("\n");

	printf("outhex back to PRO:\n");
	PRO *bakpro = (PRO*)malloc(sizeof(PRO));
	pro_hex2pro(outhex,bakpro);
	dump_pro(bakpro);
	if(bakpro && bakpro->payload) {
		free(bakpro->payload);
		free(bakpro);
	}
	
	if(pro && pro->payload) {
		free(pro->payload);
		free(pro);
	}
}

#if 0
void out_bgorlit()
{
	union {
		int interg;
		char a[4];
	}*p,u;
	p = &u;
	p->interg = 0x12345678;
	printf("%p: %p %p %p %p\n",u.a,&u.a[0],&u.a[1],&u.a[2],&u.a[3]);
	printf("%02x %02x %02x %02x\n",u.a[0],u.a[1],u.a[2],u.a[3]);
	if(u.a[0] == 0x12 && u.a[3] == 0x78)
		printf("BIG !!\n");
	else if(u.a[0] == 0x78 && u.a[3] == 0x12)
		printf("LIT !!\n");
}

int main(int argc, char **argv)
{
	out_bgorlit();
	
	PRO pro;
	pro.hd.header = 0xa5a5;
	pro.hd.len = 0x04;
	pro.hd.ver = 0x01;
	pro.hd.cmd = 0xf0;
	pro.hd.stat = 0x01;
	pro.hd.seq = 0x01;
	pro.hd.reserve = 0x11111111;

	//pro.crc = 0x0a0d;
	
	dump_phd(&pro.hd);
	test_hex2pro();
	test_pro2hex();
	
	uchar *cp = (char*)&pro;
	int i = 0;
	for(i;i<22;i++)
		printf("%02x ",cp[i]);
	printf("\n");
	
	printf("pro to net-pro\n");
	pro.hd.header = htons(pro.hd.header);
	pro.hd.len = htons(pro.hd.len);
	pro.hd.cmd = htons(pro.hd.cmd);
	pro.hd.seq = htonl(pro.hd.seq);
	pro.hd.reserve = htonl(pro.hd.reserve);
//	pro.payload = htonl(pro.payload);
//	pro.crc = htons(pro.crc);
	
	for(i=0;i<22;i++)
		printf("%02x ",cp[i]);
	printf("\n");
	
	
//	PRO dt2pro;
//	memset(&dt2pro,0,sizeof(dt2pro));
//	dt2pro = *((PRO*)&prodata);
//	printf("dt2pro:%x,%x,%x,%x\n",dt2pro.hd.header,dt2pro.hd.cmd,dt2pro.hd.seq,dt2pro.payload);
	
//	printf("dt2pro net2host:%04x,%04x,%08x,%08x\n",ntohs(dt2pro.hd.header),ntohs(dt2pro.hd.cmd),
//			ntohl(dt2pro.hd.seq),ntohl(dt2pro.payload));
	
	//printf("PRO size:%ld,data:%ld\n",sizeof(PRO),sizeof(prodata)/sizeof(prodata[0]));
	int a = 0x12345678;
	char *p = (char*)&a;
	printf("host:%0x    %0x   %0x   %0x\n",  p[0], p[1], p[2], p[3]);  
	printf("&ff=%x\n",a&0xff);
	
	int b = htonl(a); 
                  
    p = (char *)(&b);  
  
    printf("htonl:%0x    %0x   %0x   %0x\n",  p[0], p[1], p[2], p[3]); 
				
	return 0;
}
#endif
