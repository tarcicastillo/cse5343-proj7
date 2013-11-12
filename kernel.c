/*
 *Project 7
 *Tarci Castillo
 *
 *System calls 
 *Printing to the screen - interrupt 0x10
 *Reading from the keyboard - interrupt 0x16
 *Reading a sector from disk - iterrupt 0x13
 *Creating your own interrupt (interrupt handler 0x21)
 *Make printString, readString, and readSector interrupt calls
 *0x10 = video
 *0x13 = disk
 *0x16 = keyboard
 *0x21 = own
 *0xE = print character
 *0xD = enter
 *0x8 = backspace
 */

void printString(char *line);
void readString(char *line);
int mod(int a, int b);
void readSector(char *buffer, int sector);
void handleInterrupt21(int ax, int bx, int cx, int dx);

int main() {
	/*
	 *all steps are called here
    */
    char line[80];
    char sectorBuf[512];

    /*Step 1: Printing to the Screen */
    printString("HELLO WORLD!\n\r\0");

    /*Step 2: Reading from Keyboard */
    printString("\nUSER INPUT: \0");
    readString(line);
    printString("\n\r\0");
    printString(line);
    printString("\n\r\0");

    /*Step 3: Reading a Sector from Disk */
    printString("\nSECTOR BUFFER\n\r\0");
    readSector(sectorBuf, 30);
    printString(sectorBuf);
    printString("\n\r\0");

    /*Step 4: Creating my Own Interrupt */
    makeInterrupt21();
    /*interrupt(0x21, 0, 0, 0, 0);/*commented out to do #5*/

    /*Step 5: Make printString, readString, &
      readSector interrupt calls */
    interrupt(0x21, 1, line, 0, 0);
    interrupt(0x21, 0, line, 0, 0); 
    interrupt(0x21, 2, line, 0, 0);
    interrupt(0x21, 0, line, 0, 0); 

	while(1) {
		/*loops infinitly*/
	};
}
/*Step 1: Printing to the Screen - Interrupt 0x10 */
void printString(char *line) {
    int index = 0;
    char c = line[index];

    while(c != 0) {
        interrupt(0x10, 0xE * 256 + c, 0, 0, 0);
        c = line[++index];/*there is a difference between index++ & ++index*/
    }
}

/*Step 2: Reading from the Keyboard - Interrupt 0
 *0x10 = video
 *0x13 = disk
 *0x16 = keyboard
 *0x21 = own
 *0xE = print character
 *0xD = enter
 *0x8 = backspace*/
void readString(char *line) {
    int index = 0;
    char c = interrupt(0x16, 0, 0, 0, 0);

    while((c != 0xD) && (index < 80)) {
        interrupt(0x10, 0xE * 256 + c, 0, 0, 0);

        /*0x8 = backspace*/
        if(c != 0x8) {
            line[index] = c;
            index++;
        }else if(index > 0) {
            index--;
        }

        c = interrupt(0x16, 0, 0, 0, 0);
    }

    line[index] = 0;
}

/*Step 3: Reading a Sector from Disk - Interrupt 0x13 */
void readSector(char *buffer, int sector) {
    int relativeSec = mod(sector, 18) +1;
    int head = mod(sector/18, 2);
    int track = sector/36;

    interrupt(0x13, 2 *256 +1, buffer, track *256 + relativeSec, head *256);
}

int mod(int a, int b) {
    while(a >= b) {
        a = a - b;
    }
    return a;
}

/*Step 4: Create my own Interrupt */
/*get garbage values but confused as to why its that exactly (its contents on bx)*/
void handleInterrupt21(int ax, int bx, int cx, int dx) {
    if(ax == 0) {
        printString(bx);
        printString("\n\r\0");

    }else if(ax == 1) {
        readString(bx);
        printString("\n\r\0");

    }else if(ax == 2) {
        readSector(bx, cx);
        
    }else {
        printString("INVALID INTERRUPT 21 COMMAND...\n\r\0");
    }
}