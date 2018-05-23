#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <sstream>
#include <cstdint>

std::string to_hex(const char &c) {
    int res_int = c & 0xff;
    std::stringstream s_stream;
    s_stream << std::hex << res_int;
    std::string res(s_stream.str());
//    std::cout << res_int << " : " << res << " : " << c << " - 1\n";
    return res;
}


struct boot {
    std::pair<long, long> text_identifier_OS = std::make_pair(0, 3);
    std::pair<long, long> machine_instruction = std::make_pair(3, 8);
    std::pair<long, long> BytesPerSector = std::make_pair(11, 2);//512
    std::pair<long, long> SectorPerCluster = std::make_pair(13, 1);
    std::pair<long, long> ReservedSector = std::make_pair(14, 2);
    std::pair<long, long> NumberOfFAts = std::make_pair(16, 1);
    std::pair<long, long> RootEntries = std::make_pair(17, 2);
    std::pair<long, long> MediaDescriptor = std::make_pair(21, 1);
    std::pair<long, long> SectorPerFat = std::make_pair(22, 2);
    std::pair<long, long> SectorPerTrack = std::make_pair(24, 2);
    std::pair<long, long> Heads = std::make_pair(26, 2);
    std::pair<long, long> HiddenSectors = std::make_pair(28, 4);
    std::pair<long, long> BigTotalSectors = std::make_pair(32, 4);
    std::pair<long, long> PhysicalDiscNumbers = std::make_pair(36, 1);
    std::pair<long, long> CurrentHead = std::make_pair(37, 1);
    std::pair<long, long> Signature = std::make_pair(38, 1);
    std::pair<long, long> VolumeSerialNumber = std::make_pair(39, 4);
    std::pair<long, long> VolumeLabel = std::make_pair(43, 11);
    std::pair<long, long> SystemID = std::make_pair(54, 8);
    std::pair<long, long> LoaderCodeArea = std::make_pair(62, 448);
    std::pair<long, long> BootSignature = std::make_pair(510, 2);
};

std::vector<uint8_t> read_img(const std::string &filename) {

    std::ifstream file(filename.c_str(), std::ios::binary);
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> img_data;
    img_data.resize(size+1);
    file.read(reinterpret_cast<char*>(img_data.data()), size);
    /*
    uint8_t c;
    while (file >> c) {
        img_data.emplace_back(c);
    }
     */
    return img_data;
}

long long PositionForWrite(long long n) {
    std::string s = "00";
    std::vector<std::string> str; // = read_img("../hd0.img");
    for (long long i = n; i < str.size(); i++) {
        if (str[i].compare(s) == 0) {
            return i;
        }
    }
}

long get_table_data(const std::vector<std::string> &img_data, const std::pair<long, long> &pair) {
    std::string data;

    unsigned int x;

    for (long i = pair.first; i < pair.first + pair.second; i++) {
        data += img_data[i];
    }

    //std::cout << data << "\n";

    std::stringstream ss;
    ss << std::hex << data;
    ss >> x;
    //std::cout << x << "\n";
    long result = static_cast<long>(x);
    return result;
}

uint16_t from_disk_to_mem16(uint16_t x)
{
    uint16_t y = ((x & 0xFF00 >> 8) << 8) | (x >> 8 & 0xFF);
    return y;
}

uint32_t from_disk_to_mem32(uint32_t x)
{
    uint8_t* sa = reinterpret_cast<uint8_t*>(&x);
    uint32_t res = 0;
    uint8_t* sb = reinterpret_cast<uint8_t*>(&res);
    for(int i = 0; i<4; ++i)
        sb[i] = sa[4-i-1];
    return res;
}











int main() {
    boot root;
    auto img_data = read_img("../hd0.img");
    size_t base_sector = 17;
    size_t base_byte = base_sector * 512;
    std::cout << "Boot base offset " << base_byte << std::endl;

    //std::cout << std::hex << from_disk_to_mem32(0x12345678) << std::endl;

    uint16_t bytes_per_sec = (*reinterpret_cast<uint16_t*>(img_data.data() + base_byte + 0x0B));
    uint16_t sec_per_cluster = (*reinterpret_cast<uint8_t*>(img_data.data() + base_byte + 0x0D));
    uint16_t reserved_sectors = (*reinterpret_cast<uint8_t*>(img_data.data() + base_byte + 0x0E));
    std::cout << "bytes_per_sec = " << bytes_per_sec << std::endl;
    std::cout << "sec_per_cluster  = " << sec_per_cluster  << std::endl;
    std::cout << "reserved_sectors = " << reserved_sectors << std::endl;

    std::cout << std::hex << std::uppercase << std::setfill('0');
    for(int i = 0; i<32; ++i)
        std::cout  << std::setw(2) << static_cast<int>(*reinterpret_cast<uint8_t*>(&img_data[i + base_byte])) << ' ';
    std::cout << std::endl;

    //long load_sector_bytes = get_table_data(img_data, root.BytesPerSector) * get_table_data(img_data, root.ReservedSector) +get_table_data(img_data, root.NumberOfFAts) * get_table_data(img_data, root.SectorPerFat) *get_table_data(img_data, root.BytesPerSector);
    //std::cout<<get_table_data(img_data, root.BytesPerSector)<<std::endl;
    //std::pair<long, long> test = std::make_pair(0, 3);
    //std::cout<<img_data[0]<<std::endl;
    //std::cout<<img_data[1]<<std::endl;
    //std::cout<<img_data[2]<<std::endl;
    //std::cout<<get_table_data(img_data,test)<<std::endl;//FA 33 C0


    return 0;
}
//22330816560