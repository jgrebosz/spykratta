#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

//static char *locklinkname = "spylock";
string locklinkname = "spylock";
static char lockfilename[256];
//*********************************************************************
/* remove lockfile
 * lockfilename must already be filled by create_lock
 */
//*********************************************************************
static void remove_lock()
{
    // ignore errors
    unlink(lockfilename);
    unlink(locklinkname.c_str());
}
//*********************************************************************
/* create a lockfile
 * (also fills lockfilename)
 * return true if locking was successful, false otherwise
 */
//*********************************************************************
static bool create_lock()
{
    // see man creat() for the following procedure

    // get the current processes PID (will go into the lockfile name)
    pid_t pid = getpid();

    // get the hostname (will go into the lockfile name)
    char hostname[64];
    gethostname(hostname, sizeof(hostname));

    // build the filename for the lockfile
    snprintf(lockfilename, sizeof(lockfilename),
             "%s-%s-%d", locklinkname.c_str(), hostname, pid);

    // create the lockfile
    int lockfd = open(lockfilename, O_RDONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if(lockfd == -1)
    {
        // lockfile creation failed
        return false;
    }

    int linkok = link(lockfilename, locklinkname.c_str());
    if(linkok == 0)
    {
        // locking successful
        return true;
    }

    struct stat tempstat;
    int statok = stat(lockfilename, &tempstat);

    // lock is successful if linkcount equals 2
    return (statok == 0 && tempstat.st_nlink == 2);
}
//*********************************************************************
// handle SIGTERM, SIGINT, SIGHUP
static void termination_handler(int signum)
{
    //remove_lock(); is done by exit (via atexit call in setup_lock)
    exit(0);
}
//*********************************************************************
// try to create a lockfile and terminate program, if that fails
// otherwise prepare for lockfile deletion at exit
//*********************************************************************
static void setup_lock()
{
    bool locked = create_lock();
    if(!locked)
    {
        fprintf(stderr, "Could not create lock.\n"
                "Please make sure that no other spy is running in this\n"
                "directory, remove all \"%s\"-files and restart spy.\n",
                locklinkname.c_str());
        exit(-1);
    }

    printf("Setting exit and signal handlers...");
    // remove lock when program terminates normally
    atexit(remove_lock);

    // handle some signals, so that locks will be removed after
    // normal kill, Ctrl-C
    signal(SIGINT, termination_handler);
    signal(SIGHUP, termination_handler);
    signal(SIGTERM, termination_handler);
    printf("done.\n");
}
//*********************************************************************
