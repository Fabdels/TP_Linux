#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

int main(int argc, char const *argv[]){
	int i = 0;
	uint32_t * p;
	
	int fd = open("/dev/mem", O_RDWR);
	p = (uint32_t*) mmap(NULL, 4, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0xFF203000);
	
	for(i=0;i<8;i++){
		*p |= (1<<i);
		//*p &= ~(1<<i);
		sleep(1);
	}
	for(i=0;i<8;i++){
		//*p |= (1<<i);
		*p &= ~(1<<i);
		sleep(1);
	}
	
	return 0;
}