#define NONE			-1
#define MBR				2
#define EXT2			3
#define EXT3			4
#define FAT				5
#define HFSPLUS			6
#define EFI				7
#define NTFS			8
#define EXTENDED		9
#define EXFAT			10
#define EXT4			11
#define HFSPLUSJOURNAL	12

#define LASTFS			HFSPLUSJOURNAL

int detect_fs(char *device);
