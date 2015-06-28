
#include "klprivate.h"
#include <ftw.h>

bool kl_ensure_dir(const char *path)
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

void kl_rmdirs(const char *path)
{
	nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}

long long kl_current_timestamp() {
	struct timeval te; 
	gettimeofday(&te, NULL); // get current time
	return te.tv_sec*1000LL + te.tv_usec/1000; // caculate milliseconds
}
