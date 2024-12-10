#include "FileManager.h"



FileManager::~FileManager()
{
    std::cout << "File Manager Stopping..." << std::endl;
#ifdef USE_EMULATOR
// Running on the emulator
#else
    // Running on the ESP32
    root.close();
    SD.end();
#endif
    std::cout << "🛑 File Manager Stopped..." << std::endl;
}

bool FileManager::MountFileSystem()
{
    std::cout << "File Manager Mounting SD Card..." << std::endl;
#ifdef USE_EMULATOR
// Running on the emulator
#else
    // Running on the ESP32
    std::cout << "  Running on the ESP32... Initializing SD via SPI on pins:" << std::endl;
    std::cout << "    MISO: " << SPI_MISO_PIN << std::endl;
    std::cout << "    MOSI: " << SPI_MOSI_PIN << std::endl;
    std::cout << "    SCK: " << SPI_SCK_PIN << std::endl;
    std::cout << "    SS: " << VSPI_SS << std::endl;

    if (!SD.begin(VSPI_SS))
    {
        std::cout << "❌ Card Mount Failed" << std::endl;
        return (bool) false;
    }

    mounted = true;

    root = SD.open("/");

    #if DEBUG_MODE
    printCardInformation();
    std::cout << "  SD Card Files:" << std::endl;
    printDirectory(root, 1);
    #endif

    std::cout << "✅ File Manager Mounted SD Card." << std::endl;
#endif
    return (bool) true;
}

void FileManager::printCardInformation()
{

    std::cout << " Card Type: ";
    switch (SD.cardType())
    {
    case CARD_NONE:
        std::cout << "None" << std::endl;
        break;
    case CARD_MMC:
        std::cout << "MMC" << std::endl;
        break;
    case CARD_SD:
        std::cout << "SDSC" << std::endl;
        break;
    case CARD_SDHC:
        std::cout << "SDHC" << std::endl;
        break;
    case CARD_UNKNOWN:
        std::cout << "Unknown" << std::endl;
        break;
    default:
        std::cout << "Error" << std::endl;
        break;
    }

    if (SD.cardType() == CARD_NONE)
    {
        std::cout << "No SD card attached" << std::endl;
        return;
    }

    std::cout << " Card Size: " << SD.cardSize() / (1024 * 1024) << " MB" << std::endl;
}

void FileManager::printDirectory(File dir, int numTabs, bool showHiddenItems)
{
    while (true)
    {
        File entry = dir.openNextFile();
        if (!entry)
        {
            // no more files
            break;
        }

        if (!showHiddenItems && entry.name()[0] == '.')
        {
            // skip hidden files
            continue;
        }

        for (uint8_t i = 0; i < numTabs; i++)
        {
            std::cout << '\t';
        }
        std::cout << entry.name();
        if (entry.isDirectory())
        {
            std::cout << "/" << std::endl;
            printDirectory(entry, numTabs + 1);
        }
        else
        {
            // files have sizes, directories do not
            std::cout << "\t\t";
            std::cout << entry.size() << " bytes" << std::endl;
        }
        entry.close();
    }
}

File FileManager::getJsonFile(const std::string &filename)
{
    File file = SD.open(filename.c_str());
    if (!file)
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        throw std::runtime_error("Failed to open file");
    }
    return file;
}
