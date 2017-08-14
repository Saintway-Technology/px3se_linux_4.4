#include "PlayBackService.h"

int main(int argc ,char *argv[]){
	
	long a,b,c;
	
	while(1){
		b=duration();
		a=position();
		if(b > 0){
			c = b;
		}
		if(a > 0 )
			printf("%ld/%ld\n",a,c);
	}

	return 0;
}