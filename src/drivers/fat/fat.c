#include "fat.h"

#include "../../api/kernel_functions.h"

#include "../../libs/device.h"
#include "../../libs/driver.h"
#include "../../libs/partition.h"
#include "../../libs/io.h"
#include "../../libs/memory.h"
#include "../../libs/string.h"
#include "../../libs/fat12.h"
#include "../../libs/time.h"

int vfs_read(unsigned char file_name, unsigned char* dst);
int vfs_write(unsigned char file_name, unsigned char* src);
int vfs_readdir(unsigned char* dst);
int vfs_getpart(unsigned int* dst);

void* fat_funcs[] = {

};

int fat_init(struct dev_info* device){
    // определение файловой системы FAT12. если не определена то возврат 0, определена 1
    struct part_disk_info* pdi = (struct part_disk_info*)device->adv_info;

    kput("\n");
    kput("FAT16 init...\nDevice ID: ");
    kprinti(device->id);
    kput("\n");
    
    if (pdi->part_type != 0x0E) return 0;
    
    kput("Number sectors: ");
    kprinti(pdi->num_sectors);
    kput("\n");
    kput("LBA start: ");
    kprinti(pdi->lba_start);
    kput("\n");

    unsigned char first_sector[512];
    _read_sector(pdi->parrent_disk->id, pdi->lba_start, first_sector);
    
    unsigned char fs_type[9];
    memcpy(fs_type, first_sector + 0x36, 8);
    fs_type[8] = 0;

    unsigned char FAT16_TYPE_STR[] = {'F','A','T','1','6',0x20,0x20,0x20};
    if (!is_str_equally(fs_type, FAT16_TYPE_STR)) return 0;

    kput("File System: ");
    kput(fs_type);
    kput("\n");

    unsigned char volume_label[12];
    memcpy(volume_label, first_sector + 0x2B, 11);
    volume_label[11] = 0;

    kput("Volume Label: ");
    kput(volume_label);
    kput("\n");


    // Bytes per sector
    unsigned short bytes_per_sector;
    memcpy(&bytes_per_sector, first_sector + 0x0B, 2);
    kput("Bytes per sector: ");
    kprinti(bytes_per_sector);
    kput("\n");

    // Sectors per cluster
    unsigned char sectors_per_cluster;
    memcpy(&sectors_per_cluster, first_sector + 0x0D, 1);
    kput("Sectors per cluster: ");
    kprinti(sectors_per_cluster);
    kput("\n");

    // Reserved sectors
    unsigned short reserved_sectors;
    memcpy(&reserved_sectors, first_sector + 0x0E, 2);
    kput("Reserved sectors: ");
    kprinti(reserved_sectors);
    kput("\n");

    // Number of fats
    unsigned char number_of_fats;
    memcpy(&number_of_fats, first_sector + 0x10, 1);
    kput("Number of fats: ");
    kprinti(number_of_fats);
    kput("\n");

    // Max root dir entries
    unsigned short max_root_dir_entries;
    memcpy(&max_root_dir_entries, first_sector + 0x11, 2);
    kput("Max root dir entries: ");
    kprinti(max_root_dir_entries);
    kput("\n");

    // Sectors per FAT
    unsigned short sectors_per_fat;
    memcpy(&sectors_per_fat, first_sector + 0x16, 2);
    kput("Sectors per FAT: ");
    kprinti(sectors_per_fat);
    kput("\n");

    // FAT tables
    unsigned int fat_start_lba = pdi->lba_start + reserved_sectors;
    kput("FAT start LBA: ");
    kprinti(fat_start_lba);
    kput("\n");
    unsigned int fat_size_sectors = sectors_per_fat * number_of_fats;
    kput("FAT size sectors: ");
    kprinti(fat_size_sectors);
    kput("\n");

    // Root dir
    unsigned int root_dir_start_lba = fat_start_lba + fat_size_sectors;
    kput("Root Directory Start LBA: ");
    kprinti(root_dir_start_lba);
    kput("\n");
    unsigned int root_dir_size_sectors = ((max_root_dir_entries * 32) + (bytes_per_sector - 1)) / bytes_per_sector;
    kput("Root Directory size sectors: ");
    kprinti(root_dir_size_sectors);
    kput("\n");

    // Data area
    unsigned int data_start_lba = root_dir_start_lba + root_dir_size_sectors;


    kput("\n");

    time_sleep(3000);

    return 1;
}

int fat_probe(struct dev_info* device){
    return 1;
}
