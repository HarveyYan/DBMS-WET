#include "wet.h"
#include <stdio.h>
#include "libpq-fe.h"

int main(int argc, char* argv[]) {
	char connect_param[200];
	sprintf(connect_param,
		"host=csl2.cs.technion.ac.il dbname=%s user=%s password=%s",
		USERNAME, USERNAME, PASSWORD);
	PGconn *conn = PQconnectdb(connect_param);
	return 0;
}

void* addUser(const char*    name){

}

void* addUserMin(const char*    name){

}

void* removeUser(const char*    id){

}

void* addPhoto(const char*    user_id,
	const char*    photo_id){

}

void* tagPhoto(const char*    user_id,
	const char*    photo_id,
	const char*    info){


}

void* photosTags(){

}

void* search(const char*    word){

}

void* commonTags(const char*    k){

}

void* mostCommonTags(const char*    k){

}

void* similarPhotos(const char*    k,
	const char*    j){

}

void* autoPhotoOnTagOn(){

}

void* autoPhotoOnTagOFF(){

}