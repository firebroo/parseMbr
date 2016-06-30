#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MBR_FLAG_1               0x55
#define MBR_FLAG_2               0xaa
#define MBR_SECTOR_SIZE          512
#define MBR_PARTITION_START_PTR  0x1BE
#define MBR_PARTITION_ENTRY_SIZE 0x10
#define MBR_PARTITION_ENTRY_NUM  0x4

/*referer https://en.wikipedia.org/wiki/Master_boot_record*/

enum mbr_status {
    ACTIVE = 0x80,
    INACTIVE = 0x00
};

enum partition_type {
    Empty = 0x00,               
    FAT12,                
    XENIX_root,           
    XENIX_usr,            
    FAT16_32M,           
    Extended,            
    FAT16,                
    HPFS_NTFS_exFAT,      
    AIX,                  
    AIX_bootable,        
    OS_2_Boot_Manag,      
    W95_FAT32,            
    W95_FAT32_LBA,      
    W95_FAT16_LBA,      
    W95_Ext_LBA,      
    OPUS,                 
    Hidden_FAT12,         
    Compaq_diagnost,      
    Hidden_FAT16_1,      
    Hidden_FAT16_2,         
    Hidden_HPFS_NTF,      
    AST_SmartSleep,       
    Hidden_W95_FAT3_1,      
    Hidden_W95_FAT3_2,      
    Hidden_W95_FAT1, 
    NEC_DOS = 0x24,        
    Hidden_NTFS_Win,
    Plan,          
    PartitionMagic, 
    Venix_80286,    
    PPC_PReP_Boot,  
    SFS,            
    QNX4_x,         
    QNX4_x_2nd_part,
    QNX4_x_3rd_part,
    OnTrack_DM,     
    OnTrack_DM6_Aux_1,
    CP_M,           
    OnTrack_DM6_Aux_2,
    OnTrackDM6,     
    EZ_Drive,       
    Golden_Bow,     
    Priam_Edisk,    
    SpeedStor,      
    GNU_HURD_or_Sys,
    Novell_Netware_1, 
    Novell_Netware_2, 
    DiskSecure_Mult,
    PC_IX,          
    Old_Minix,
    Minix_old_Lin = 0x81,
    Linux_swap_So,
    Linux,          
    OS_2_hidden_C, 
    Linux_extended, 
    NTFS_volume_set_1,
    NTFS_volume_set_2,
    Linux_plaintext,
    Linux_LVM,      
    Amoeba,         
    Amoeba_BBT,     
    BSD_OS,         
    IBM_Thinkpad_hi,
    FreeBSD,        
    OpenBSD,        
    NeXTSTEP,      
    Darwin_UFS,     
    NetBSD,         
    Darwin_boot,    
    HFS_HFS,     
    BSDI_fs,        
    BSDI_swap,      
    Boot_Wizard_hid,
    Solaris_boot,   
    Solaris = 0xbf,        
    DRDOS_sec_FAT_1,
    DRDOS_sec_FAT_2,
    DRDOS_sec_FAT_3,
    Syrinx,        
    Non_FS_data,   
    CP_M_TOS,
    Dell_Utility,   
    BootIt,         
    DOS_access,     
    DOS_R_O,        
    SpeedStor_1,      
    BeOS_fs,        
    GPT,            
    EFI_FAT_12_16,
    Linux_PA_RISC_b,
    SpeedStor_2,      
    SpeedStor_3,      
    DOS_secondary,  
    VMware_VMFS,    
    VMware_VMKCORE, 
    Linux_raid_auto,
    LANstep,        
    BBT
};

typedef struct partition_entry {
    unsigned char         status_and_physical_driver;
    unsigned char         chs_address_first_absolute_sector[3]; 
    unsigned char         partition_type;
    unsigned char         chs_address_last_absolute_sector[3];
    unsigned int          label_first_absolute_sector;
    unsigned int          number_sector;
} partition_entry_t;

void
die (char *ret)
{
    fprintf(stderr, "%s\n", ret);
    exit(-1);
}

bool
is_mbr_partition (unsigned char *mbr)
{
    return (mbr[510] == MBR_FLAG_1 && mbr[511] == MBR_FLAG_2) ? true : false;
}

void
parse_chs (unsigned char *chs)
{
    /* cylinder, head, sector. chs */
    unsigned char       head;
    unsigned char       sector;
    unsigned short      cylinder;

    head = chs[0];
    /*sector in bits 5-0,,bits 7-6 are high bits of cylinder*/
    sector = chs[1] & 0x3f;
    /*bits 7-0 of cylinder*/
    cylinder = chs[2] | ((chs[1] & 0xc0) << 2);
    printf("head: %d\tsector: %d\tcylinder: %d\n",head, sector, cylinder);
}

void
parse_mbr_partition (unsigned char *mbr)
{
    int                 i; 
    enum partition_type partition_type;
    enum mbr_status     status;
    partition_entry_t  *partition_entry;     

    for (i = 0; i < MBR_PARTITION_ENTRY_NUM; i++) {
        partition_entry = (partition_entry_t*)(mbr + MBR_PARTITION_START_PTR + i * MBR_PARTITION_ENTRY_SIZE); 

        status = partition_entry->status_and_physical_driver;
        switch (status) {

        case ACTIVE:
            printf("partion %i is active\t", i + 1);
            break;
        case INACTIVE:
            printf("partion %i is inactive\t", i + 1);
            break;
        default:
            die("unknown error.\n");
        }

        partition_type = partition_entry->partition_type;
        printf("partion %i's type: hexvalue=%02x(", i + 1, partition_type);
        switch (partition_type) {
       
        case HPFS_NTFS_exFAT:
            printf ("windows NTFS type");
            break;
        case Extended:
            printf ("extended type");
            break;
        case Linux_swap_So:
            printf ("linux swap");
            break;
        case Linux_LVM:
            printf ("linux lvm type");
            break;
        case Linux:
            printf ("Linux type");
            break;
        default:
            printf ("unknown type");
        }
        printf (")\n");

        printf ("first absolute sector: ");
        parse_chs (partition_entry->chs_address_first_absolute_sector);
        printf ("last absolute sector: ");
        parse_chs (partition_entry->chs_address_last_absolute_sector);
        printf ("absolute sector Strart: %d\tEnd: %d", 
                partition_entry->label_first_absolute_sector,
                partition_entry->label_first_absolute_sector + 
                partition_entry->number_sector - 1);
        printf ("\tSector Number: %d\tSize: %fGB\n", 
                partition_entry->number_sector,
                (partition_entry->number_sector) / 1024.0 / 1024.0 / 1024.0 * MBR_SECTOR_SIZE);
        printf ("\n");
    }
}

char *
check_argv(int argc, char *argv[])
{
    int    opt;

    if (argc > 3) {
        goto end;
    }
    while ( (opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
            case 'f':
                return optarg;
            default:
                goto end;
        }
    }
end:
    printf("Usage: ./parseMbr <-f mbr.bin>\n");
    exit(-1);

}


int
main (int argc, char *argv[])
{
    int            fd;
    int            ret;
    unsigned char  buf[MBR_SECTOR_SIZE];
    char          *mbr_bin;

    mbr_bin = check_argv (argc, argv);
    fd = open (mbr_bin, O_RDONLY);
    if (fd < 0)
        die ("open error");
    if ( (ret = read (fd, buf, MBR_SECTOR_SIZE) == 0) ) {
        printf("file end\n");
        exit(0);
    }
    if (ret < 0)
        die ("read error.\n");

    if (!is_mbr_partition (buf))
        die ("is not a mbr.\n");
    parse_mbr_partition (buf);
}
