/*
 * This app rapidly creates and deletes subpools.
 *
 *
 */

// TODO: use usleep instead of nanosleep for delete interval

#include "legato.h"

#define SubpoolNameBufferSize 50 // 50 characters for a subpool name should be plenty

long SubpoolNum;
char SubpoolNameBuffer[SubpoolNameBufferSize] = {0};
le_mem_PoolRef_t* SubpoolRefArray; // array storing references to the subpools
void** BlockRefArray; // array storing refs to the blocks that each subpool TryAllocs
le_mem_PoolRef_t SuperPoolRef = NULL;


void createSubPools
(
    void
)
{
    // SuperPool contains 10-byte blocks
    SuperPoolRef = le_mem_CreatePool("SuperPool", 10);

    // Create N sub-pools
    long subpoolCnt = 0;
    while (subpoolCnt < SubpoolNum)
    {
        snprintf(SubpoolNameBuffer, SubpoolNameBufferSize, "Subpool%ld", subpoolCnt);

        // need to store the subpool references in an array
        SubpoolRefArray[subpoolCnt] = le_mem_CreateSubPool(SuperPoolRef, SubpoolNameBuffer, 1);

        // Alloc the subpool's free block, just to increase the stat count
        BlockRefArray[subpoolCnt] = le_mem_TryAlloc(SubpoolRefArray[subpoolCnt]);

        subpoolCnt++;
    }

    LE_INFO("========== Created all subpools ===========");
}

// Deletes one subpool
void deleteSubPool
(
    struct timespec* sleepTimeRef,
    int subPoolIndex
)
{
    LE_INFO("==== Deleting subpool %d ====", subPoolIndex);

    // some delay between subpool deletions
    nanosleep(sleepTimeRef, NULL);

    // Reset the stat of the subpool, in order to "taint" this subpool. If this subpool is reported
    // by Inspect, then race condition has occured. However note that some reports could be legit, if
    // they are happening between "ResetStats" and "DeleteSubPool".
    le_mem_ResetStats(SubpoolRefArray[subPoolIndex]);

    // Release the block that the subpool TryAlloc'ed, before deleting the sub-pool
    le_mem_Release(BlockRefArray[subPoolIndex]);

    le_mem_DeleteSubPool(SubpoolRefArray[subPoolIndex]);
}


// deleting subpools from 1 to N, except for the last subpool
void deleteSubPoolsFrom1ToN
(
    long timeIntervalNano // nanosecs
)
{
    LE_INFO("==== Deleting subpools from 1 to N ====");

    // Delete sub-pools from the first to the Nth
    long subpoolCnt = 0;
    struct timespec sleepTime = {0, timeIntervalNano};
    while (subpoolCnt < (SubpoolNum - 1))
    {
        deleteSubPool(&sleepTime, subpoolCnt);
        subpoolCnt++;
    }
}


void deleteSubPoolsAlternately
(
    long timeIntervalNano // nanosecs
)
{
    LE_INFO("==== Deleting subpools alternately ====");

    // Delete sub-pools alternatively from both ends (except of the end of the list)
    long indexMin = 0;
    long indexMax = SubpoolNum - 2;
    int deleteWhichEnd = 0;
    long mid = (SubpoolNum / 2) - 1;
    struct timespec sleepTime = {0, timeIntervalNano};
    long i = 0;

    while ((indexMin < mid) || (indexMax > mid))
    {
        if (deleteWhichEnd == 0) // delete on the lower end
        {
            i = indexMin;
            indexMin++;
            deleteWhichEnd = 1;
        }
        else // delete on the upper end
        {
            i = indexMax;
            indexMax--;
            deleteWhichEnd = 0;
        }

        deleteSubPool(&sleepTime, i);
    }
}


COMPONENT_INIT
{
    char argDeleteStrat[100];
    long argSleepIntervalNano;

    if (le_arg_NumArgs() != 3)
    {
        LE_ERROR("Usage: SubpoolFlux [1toN | Alter | None] [delete interval] [number of pools created]");
        exit(EXIT_FAILURE);
    }

    strcpy(argDeleteStrat, le_arg_GetArg(0));
    argSleepIntervalNano = strtol(le_arg_GetArg(1), NULL, 0);
    SubpoolNum = strtol(le_arg_GetArg(2), NULL, 0);

    // Initializing the global arrays
    SubpoolRefArray = malloc(SubpoolNum * sizeof(le_mem_PoolRef_t));
    BlockRefArray = malloc(SubpoolNum * sizeof(void*));

    // Create/Delete subpools, according to the defined strategy
    if (strcmp(argDeleteStrat, "1toN") == 0)
    {
        createSubPools();
        deleteSubPoolsFrom1ToN(argSleepIntervalNano);
    }
    else if (strcmp(argDeleteStrat, "Alter") == 0)
    {
        createSubPools();
        deleteSubPoolsAlternately(argSleepIntervalNano);
    }
    else if (strcmp(argDeleteStrat, "None") == 0)
    {
        createSubPools();
        LE_INFO("==== No pools deleted ====");
    }
    else
    {
        LE_ERROR("invalid subpool delete strategy option.");
        exit(EXIT_FAILURE);
    }


    LE_INFO("========== FINISHED ===========");
}
