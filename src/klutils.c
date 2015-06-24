
#include "klprivate.h"
#include <ftw.h>

bool ensure_directory(const char *path)
{
	struct stat fileStat;
	if (stat(path, &fileStat) != 0)
	{
		// TODO: do this to ensure intermediate dirs are present
		// TODO: check other error codes
		if (mkdir(path, 0777) != 0)
			return false;
		if (chmod(path, 07777) != 0)
			return false;
	}

	stat(path, &fileStat);
	if ((fileStat.st_mode & S_IFDIR) == 0)
	{
		return false;
	}
	return true;
}

int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    int rv = remove(fpath);
    if (rv) perror(fpath);
    return rv;
}

void rmdirs(const char *path)
{
	nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}

