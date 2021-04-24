#include<stdio.h>
#include<string.h>
char BNAME[10] = "TANXUAN";
char BPWD[11] = "U201915033";

extern int __stdcall FIND_FUN();
extern int __stdcall SOLD_FUN();
extern int __stdcall SUPPLE_FUN();
extern int __stdcall CLU_FUN();
extern int __stdcall SORT_FUN();
extern int __stdcall CLO_FUN();
extern int __stdcall ADD_FUN();
int main() {
	int op = 1,FLAG = 0;
	char IN_NAME[10] = { 0 };
	char IN_PWD[11] = { 0 };
	while (!FLAG) {
		int i = 0;
		printf("Please enter your username:\n");
		scanf("%s",IN_NAME);
		for (; i < 10; i++) {
			if (BNAME[i] != IN_NAME[i]) {
				printf("wrong username or password\n");
				break;
			}
		}
		if (i == 10) {
			FLAG = 1;
		}
	}
	FLAG = 0;
	while (!FLAG) {
		int i = 0;
		printf("Please enter your password:\n");
		scanf("%s", IN_PWD);
		for (; i < 11; i++) {
			if (BPWD[i] != IN_PWD[i]) {
				printf("wrong username or password\n");
				break;
			}
		}
		if (i == 11) {
			FLAG = 1;
		}
	}
	while (op!=8) {
		printf("Please enter number1-5 to choose function:\n");
		printf("1:Find products\n");
		printf("2:Shipment\n");
		printf("3:replenishment\n");
		printf("4:Calculate the profit margin of goods\n");
		printf("5.Sort\n");
		printf("6:Clock\n");
		printf("7:Add\n");
		printf("8:Exit\n");
		scanf("%d", &op);
		switch (op) {
		case 1:
			FIND_FUN();
			break;
		case 2:
			SOLD_FUN();
			break;
		case 3:
			SUPPLE_FUN();
			break;
		case 4:
			CLU_FUN();
			break;
		case 5:
			SORT_FUN();
			break;
		case 6:
			CLO_FUN();
			break;
		case 7:
			ADD_FUN();
			break;
		case 8:
			break;
		default:
			printf("please enter 1-8");
			break;
		}
	}
	return 0;
}
