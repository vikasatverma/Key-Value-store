#include "header.hpp"

class KVStore {

public:
    int dumpToFile(const std::string &filename) {
        FILE *dumpFilePtr = fopen(filename.c_str(), "w");
        if (!dumpFilePtr) {
            return -errno;
        }
        fprintf(dumpFilePtr, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<KVStore>\n");
        for (int i = 0; i < numSetsInCache; i++) {
            std::string nameStoreFile = "KVStore/" + std::to_string(i);

            FILE *storeFilePtr = fopen(nameStoreFile.c_str(), "r");
            if (!storeFilePtr)
                continue;

            char *buf = nullptr;
            size_t buflen = 0;
            while (getline(&buf, &buflen, storeFilePtr) > 0) {
                char *nl = strchr(buf, '\n');
                if (nl == nullptr)
                    continue;
                *nl = 0;

                char *sep = strchr(buf, '=');
                if (sep == nullptr)
                    continue;
                *sep = 0;
                sep++;

                std::string key = buf;
                std::string value = sep;

                fprintf(dumpFilePtr, " <KVPair>\n");
                fprintf(dumpFilePtr, "%s", ("  <Key>" + key + "</Key>\n").c_str());
                fprintf(dumpFilePtr, "%s", ("  <Value>" + value + "</Value>\n").c_str());
                fprintf(dumpFilePtr, " </KVPair>\n");

            }
            fclose(storeFilePtr);
        }

        fprintf(dumpFilePtr, "</KVStore>\n");

        fclose(dumpFilePtr);


        return 0;
    }

    int RestoreFromFile(const std::string &filename) {

        std::vector<FILE *> fd_vector;

        for (int i = 0; i < numSetsInCache; i++) {
            std::string fname = "KVStore/" + std::to_string(i);
            FILE *fp = fopen(fname.c_str(), "a");
            if (!fp) {
                cout << "IO Error";
                return -errno;
            }
            fd_vector.push_back(fp);
        }

        FILE *sourceFile = fopen(filename.c_str(), "r");
        if (!sourceFile) {
            return -errno;
        }
        char *buf = nullptr;
        size_t buflen = 0;
        getline(&buf, &buflen, sourceFile);
        getline(&buf, &buflen, sourceFile);
        getline(&buf, &buflen, sourceFile);

        while (getline(&buf, &buflen, sourceFile) > 0) {

            std::string key;
            std::string value;

            std::string line = std::string(buf);
            if (buf[3] == 'K' && buf[4] == 'e' && buf[5] == 'y') {
                std::size_t start_of_key = line.find("<Key>");
                std::size_t end_of_key = line.find_last_of("</Key>");
                key = line.substr(start_of_key + 5, line.length() - 14);
            }
            getline(&buf, &buflen, sourceFile);
            line = std::string(buf);

            if (buf[3] == 'V' && buf[4] == 'a' && buf[5] == 'l') {
                std::size_t start_of_Value = line.find("<Value>");
                std::size_t end_of_Value = line.find_last_of("</Value>");
                value = line.substr(start_of_Value + 7, line.length() - 18);
            }

            getline(&buf, &buflen, sourceFile);
            getline(&buf, &buflen, sourceFile);

            FILE *fp = fd_vector[getSetId(key)];
//            cout << key << value << std::endl;
            fprintf(fp, "%s=%s\n", key.c_str(), value.c_str());


        }
        fclose(sourceFile);

        for (int i = 0; i < numSetsInCache; i++) {

            fclose(fd_vector[i]);
        }
        return 0;
    }


};


//int main() {
//
//    KVStore KS;
//    KS.RestoreFromFile("testfile.xml");
////    sleep(10);
//    KS.dumpToFile("testfile.xml");
//
//}