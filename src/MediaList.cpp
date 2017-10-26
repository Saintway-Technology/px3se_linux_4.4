#include "MediaList.h"
#include "AudioService.h"

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>

MediaList::MediaList() :
    m_currentIndex(-1)
{
    m_list.clear();
    setPlayMode(PlayInOrder);
    init();
}

void MediaList::init()
{
    m_updateSuffixList.push_back("mp3");
    m_updateSuffixList.push_back("wave");
    m_updateSuffixList.push_back("wma");
    m_updateSuffixList.push_back("ogg");
    m_updateSuffixList.push_back("midi");
    m_updateSuffixList.push_back("ra");
    m_updateSuffixList.push_back("mod");
    m_updateSuffixList.push_back("mp1");
    m_updateSuffixList.push_back("mp2");
    m_updateSuffixList.push_back("wav");
    m_updateSuffixList.push_back("flac");
    m_updateSuffixList.push_back("aac");
    m_updateSuffixList.push_back("m4a");
}

void MediaList::setPlayMode(PlayMode playmode)
{
    m_playmode = playmode;
}

void MediaList::onPlayItemChanged(std::string playItem)
{
    bool hasFind = false;

    LIST_STRING::iterator iter = m_list.begin();
    for (int i = 0; i < m_list.size(); i++) {
        if ((*iter) == playItem) {
            m_currentIndex = i;
            hasFind = true;
            break;
        }
        iter++;
    }

    if (!hasFind) {
        m_currentIndex = 0;
        updateList();
    }
}

std::string MediaList::getNextSongPath()
{
    switch (m_playmode) {
    case PlayOneCircle:
        break;
    case PlayInOrder:
        if (m_currentIndex + 1 >= m_list.size())
            m_currentIndex = 0;
        else
            m_currentIndex ++;
        break;
    case PlayRandom: {
        int xxx = rand() % m_list.size();
        m_currentIndex = xxx;
        break;
    }
    default:
        break;
    }

    return getPathAt(m_currentIndex);
}

std::string MediaList::getPathAt(int index)
{
    if (m_list.empty() || index >= m_list.size() || index < 0)
        return std::string("");

    m_currentIndex = index;

    LIST_STRING::iterator iter = m_list.begin();
    for (int i = 0; i < index; i++)
        iter++;

    return (*iter);
}

void MediaList::travelDir(char * path)
{
    DIR *dp;
    struct dirent* file;
    char filePath[512];

    dp = opendir(path);
    if (dp == NULL)
        return;

    while ((file=readdir(dp)) != NULL) {
        if (strncmp(file->d_name, ".", 1) == 0)
            continue;

        memset(filePath, 0, sizeof(filePath));
        sprintf(filePath, "%s/%s", path, file->d_name);

        if (file->d_type == DT_DIR) {
            travelDir(filePath);
        } else {
            std::string filePathStr = std::string(filePath);
            // filter file format
            LIST_STRING::iterator iter;
            for (iter = m_updateSuffixList.begin(); iter != m_updateSuffixList.end(); iter++) {
                std::string suffix = std::string(".") + *iter;
                if (filePathStr.find(suffix) != std::string::npos) {
                    m_list.push_back(filePathStr);
                    break;
                }
            }
        }
    }

    (void)closedir(dp);
}

void MediaList::updateList()
{
    m_list.clear();
    travelDir((char*)SEARCH_PATH);
}
