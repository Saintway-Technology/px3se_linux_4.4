#include "PlayBackService.h"

int main(int argc ,char *argv[]){
	char *a,*b,*c,*d,*e,*f;
	while(1){
		a = getSongName();
		b = getAlbumName();
		c = getArtistName();
		if(a!=NULL){
			d=a;
		}
		if(b!=NULL){
			e=b;
		}
		if(c!=NULL){
			f = c;
		}
		
		printf("%s--%s--%s\r",d,e,f);
	}
	return 0;
}