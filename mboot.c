#define MCS1_INFO_ADDR_BASE 0xfff80
#define MCS1_INFO_LEN 16 //bytes
#define MCS1_ADDR_BASE 0x80000
int MCS1_LEN;
#define SPI_FLASH_PAGE 256 //byte
#define CMD_LEN 4
void flash_prog_en();

unsigned int crc16_table[256] = {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
        0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
        0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
        0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
        0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
        0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
        0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
        0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
        0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
        0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
        0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
        0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
        0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
        0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
        0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
        0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
        0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
        0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
        0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
        0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
        0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
        0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
        0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
        0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

unsigned short mboot_crc16(unsigned short crc_init, unsigned char *buffer, int len)
{
        unsigned short crc;

        crc = crc_init;
        while(len-- > 0)
            crc = crc16_table[((crc >> 8) ^ (*buffer++)) & 0xFF] ^ (crc << 8);

        return crc;
}


void enable_download(){
	unsigned char buf[1];
	i2c_setslave(0x40);
	buf[0] = 0x2;//enable download
	i2c_write(buf, 1);
	i2c_setslave(0x41);
}

void set_cs(unsigned char dat){
	unsigned char buf[1];
	i2c_setslave(0x40);
	buf[0] = dat;//set cs
	i2c_write(buf, 1);
	i2c_setslave(0x41);
}

void set_mosi_dat(unsigned char *buf, unsigned int len){
	i2c_setslave(0x41);
	i2c_write(buf, len);
}

void flash_earse(){
	unsigned char buf[4];
	unsigned int addr = MCS1_ADDR_BASE;
	int i;
	buf[0] = 0xd8;
	
	for(i = 0; i < 8; i++){
		flash_prog_en();
		set_cs(0);
		buf[1] = ((addr >> 16) & 0xff);
		buf[2] = ((addr >> 8) & 0xff);
		buf[3] = (addr & 0xff);

		set_mosi_dat(buf, 4);
		set_cs(1);
		addr += 0x10000;
		sleep_ms(1000);
	}
}

void flash_prog_page(unsigned char *buf, unsigned int addr, unsigned int len){
	unsigned char buf_cmd[4];
	flash_prog_en();
	set_cs(0);
	buf_cmd[0] = 0x02;
	buf_cmd[1] = ((addr >> 16) & 0xff);
	buf_cmd[2] = ((addr >> 8) & 0xff);
	buf_cmd[3] = (addr & 0xff);
	set_mosi_dat(buf_cmd, 4);
	set_mosi_dat(buf, len);
	set_cs(1);
	sleep_ms(3);
}

void flash_prog_en(){
	unsigned char buf_cmd[1];
	set_cs(0);
	buf_cmd[0] = 0x06;
	set_mosi_dat(buf_cmd, 1);
	set_cs(1);
	sleep_ms(3);
}

void flash_prog_info(unsigned short crc, unsigned int len){
	unsigned char MCS1_INFO[16];
        MCS1_INFO[0] = 0x00;
        MCS1_INFO[1] = 0x01;
        MCS1_INFO[2] = 0x02;
        MCS1_INFO[3] = 0x03;

        /*mcs1 crc*/
        MCS1_INFO[4] = crc >> 8;
        MCS1_INFO[5] = crc;

        /*mcs1 len = {[6],[7],[8],[9]}*/
        MCS1_INFO[6] = 0x00;
        MCS1_INFO[7] = ((len >> 16) & 0xff);
        MCS1_INFO[8] = ((len >> 8) & 0xff);
        MCS1_INFO[9] =  (len & 0xff);

        MCS1_INFO[10] = 0x00;
        MCS1_INFO[11] = 0x00;
        MCS1_INFO[12] = 0x00;
        MCS1_INFO[13] = 0x00;

        MCS1_INFO[14] = 0x00;
        MCS1_INFO[15] = 0x00;
	flash_prog_page(MCS1_INFO, MCS1_INFO_ADDR_BASE, 16);

}

void mboot(){
	unsigned char FLASH_PAGE[SPI_FLASH_PAGE];
	unsigned int addr = MCS1_ADDR_BASE;
	int byte_num, all_byte;
	unsigned short crc_init = 0;
	FILE *mboot_mcs_fp_new;
	int j;
	mboot_mcs_file();
	all_byte = MCS1_LEN;

        i2c_open(I2C_DEV);
        i2c_setslave(I2C_SLAVE_ADDR);

	mboot_mcs_fp_new = fopen("./mm_new.mcs", "rt");

	enable_download();
	flash_prog_en();
	flash_earse();
        while(all_byte){
		if(all_byte >= SPI_FLASH_PAGE){
			byte_num = SPI_FLASH_PAGE;
			all_byte -= SPI_FLASH_PAGE;
		} else {
			byte_num = all_byte;
			all_byte = 0;
		}
		
		for(j = 0; j < byte_num; j++){
                	FLASH_PAGE[j] = fgetc(mboot_mcs_fp_new);
			crc_init = mboot_crc16(crc_init, &FLASH_PAGE[j], 1);
		}
		flash_prog_page(FLASH_PAGE, addr, byte_num);
		addr += byte_num;
        }

	flash_prog_info(crc_init, MCS1_LEN);
        fclose(mboot_mcs_fp_new);
}

char char2byte(char char0, char char1)
{
	if(char0 >= 'A' && char0 <= 'F')
		char0 = char0 - 'A' + 10;
	else
		char0 = char0 - '0';

	if(char1 >= 'A' && char1 <= 'F')
		char1 = char1 - 'A' + 10;
	else
		char1 = char1 - '0';

	return (char0 << 4) | char1;
}

int mboot_mcs_file()
{
	int j, i, i_max, byte_num;
	unsigned char tmp[1000];
	unsigned char data;
	FILE *mboot_mcs_fp;
	FILE *mboot_mcs_fp_new;


	MCS1_LEN = 0;
	mboot_mcs_fp = fopen("./mm.mcs", "rt");
	mboot_mcs_fp_new = fopen("./mm_new.mcs", "wb");
	while(1){
		i = 0;
		while(1){
			tmp[i] = fgetc(mboot_mcs_fp);
			if(tmp[i] == '\n'){
				break;
			}
			i++;
		}
		if(tmp[7] == '0' && tmp[8] == '0'){
			byte_num = char2byte(tmp[1], tmp[2]);
			for(i = 0; i < byte_num*2; i+=2){
				data = char2byte(tmp[9+i], tmp[9+i+1]);
				fputc(data, mboot_mcs_fp_new);
				MCS1_LEN++;
			}
		} else if(tmp[7] == '0' && tmp[8] == '1')
			break;
	}

	fclose(mboot_mcs_fp);
	fclose(mboot_mcs_fp_new);
	return 0;
}
