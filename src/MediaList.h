#ifndef MEDIALIST_H
#define MEDIALIST_H

#include <list>
#include <string>

#define SEARCH_PATH "/mnt"

enum PlayMode
{
    PlayInOrder = 0,
    PlayRandom = 1,
    PlayOneCircle = 2
};

typedef std::list<std::string> LIST_STRING;

/**
 * Used for manager the music list.
 *
 * Each video item saved with url and you can add、remove
 * or get music item.
 */
class MediaList
{
public:
    MediaList();

    void travelDir(char *path);
    void updateList();
    void setPlayMode(PlayMode);
    std::string getNextSongPath();
    std::string getPathAt(int index);
    void onPlayItemChanged(std::string playItem);

private:
    int m_currentIndex;
    LIST_STRING  m_list;
    PlayMode m_playmode;
    LIST_STRING m_updateSuffixList;

    void init();
};

#endif // MEDIALIST_H
