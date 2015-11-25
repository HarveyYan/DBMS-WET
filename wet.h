#ifndef __WET_H_
#define __WET_H_

#include <stdbool.h>

/*==========================================================*/
/*  -----  DBMS course, 236363, Winter - 2015 - 2016  ----- */
/*==========================================================*/
/* This is a sample header file for the "wet" assignment.   */
/* This file should not be submitted, it'll be replaced     */
/* during the automatic tests. Therefore, DO NOT change     */
/* anything in this file except your USERNAME and PASSWORD. */

/* Update these two macros to your account's details */
/* USERNAME = t2 user, PASSWORD = your id number     */
#define USERNAME "yanzichao"
#define PASSWORD "921150256"

/* DO NOT change the names of the macros -      */
/* your submitted program will fail compilation */
#define ILL_PARAMS              "Illegal parameters\n"
#define EXISTING_RECORD         "Already exists\n"
#define EMPTY                   "Empty\n"

/* Function names */
#define COMMAND_ADD_USER                "addUser"
#define COMMAND_ADD_USER_MIN            "addUserMin"
#define COMMAND_REMOVE_USER             "removeUser"
#define COMMAND_ADD_PHOTO               "addPhoto"
#define COMMAND_TAG_PHOTO               "tagPhoto"
#define COMMAND_PHOTOS_TAGS             "photosTags"
#define COMMAND_SEARCH                  "search"
#define COMMAND_COMMON_TAGS             "commonTags"
#define COMMAND_MOST_COMMON_TAGS        "mostCommonTags "
#define COMMAND_SIMILAR_PHOTOS          "similarPhotos"
#define COMMAND_ADD_PHOTOS_ON_TAG_ON    "autoPhotoOnTagOn"
#define COMMAND_ADD_PHOTOS_ON_TAG_OFF   "autoPhotoOnTagOFF"
#define COMMAND_EXIT                    "exit"

#define ADD_USER        "User ID: %s\n"

#define USER_HEADER     ".--------------.--------------.\n" \
                        "|   User ID    |  User Name   |\n" \
                        ".--------------.--------------.\n"

#define USER_RESULT     "| %-12s | %-12s |\n"

#define PHOTOS_HEADER   ".--------------.--------------.--------------.\n" \
                        "|   User ID    |   Photo ID   |    #Tags     |\n" \
                        ".--------------.--------------.--------------.\n"

#define PHOTOS_RESULT   "| %-12s | %-12s | %-12s |\n"

#define COMMON_HEADER   ".------------------------.--------------.\n" \
                        "|          Tag           |   #Photos    |\n" \
                        ".------------------------.--------------.\n"

#define COMMON_LINE     "| %-25s | %-12s |\n"

#define SIMILAR_HEADER  ".--------------.--------------.--------------.\n" \
                        "|   User ID    |  User Name   |   Photo ID   |\n" \
                        ".--------------.--------------.--------------.\n"

#define SIMILAR_RESULT  "| %-12s | %-12s | %-12s |\n"

// This function is implemented in the parser.c file.
void parseInput();

// The functions you have to implement:
// All of the functions gets strings because we don't want you to process this input
// in the C language, but forward it as is to the SQL.
void* addUser           (const char*    name);
void* addUserMin        (const char*    name);
void* removeUser        (const char*    id);
void* addPhoto          (const char*    user_id,
                         const char*    photo_id);
void* tagPhoto          (const char*    user_id,
                         const char*    photo_id,
                         const char*    info);
void* photosTags        ();
void* search            (const char*    word);
void* commonTags        (const char*    k);
void* mostCommonTags    (const char*    k);
void* similarPhotos     (const char*    k,
                         const char*    j);
void* autoPhotoOnTagOn  ();
void* autoPhotoOnTagOFF ();


#endif  /* __WET_H_ */
