#include "wet.h"
#include <stdio.h>
#include "libpq-fe.h"

PGconn *conn;

int main(int argc, char* argv[]) {
	char connect_param[200];
	sprintf(connect_param,
		"host=csl2.cs.technion.ac.il dbname=%s user=%s password=%s",
		USERNAME, USERNAME, PASSWORD);
	conn = PQconnectdb(connect_param);
	
	if (!conn || PQstatus(conn) == CONNECTION_BAD) {
		fprintf(stderr, "Connection to server failed: %s\n", PQerrorMessage(conn));
		PQfinish(conn);
		return 1;
	}


	parseInput();
	return 0;
}

void* addUser(const char*    name){
	int i;
	char cmd[200];
	PGresult *res;
	sprintf(cmd, "INSERT INTO users SELECT MAX(id) + 1,\'%s\' FROM users", name);
	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "Error executing commands: %s\n",
			PQresultErrorMessage(res));
		PQclear(res);
		return;
	}

	PQclear(res);
	for (i = 0; i < 200;  i++)
		cmd[i] = '\0';
	sprintf(cmd, "SELECT id FROM users WHERE users.name =\'%s\'", name );
	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "Error executing commands: %s\n",
			PQresultErrorMessage(res));
		PQclear(res);
		return;
	}
	printf(ADD_USER, PQgetvalue(res,0,0));
}

void* addUserMin(const char*    name){
	int min, i, j, n;
	char cmd[200];
	PGresult *res;
	
	res = PQexec(conn, "SELECT MIN(ID) FROM users");
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "Error executing commands: %s\n",
			PQresultErrorMessage(res));
		PQclear(res);
		return;
	}
	min = atoi(PQgetvalue(res,0,0));
	PQclear(res);

	for (i = min + 1;; i++){
		sprintf(cmd, "INSERT INTO users values(%d, \'%s\')", i, name);
		res = PQexec(conn, cmd);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
			PQclear(res);
			for (j = 0; j < 200; j++)
				cmd[j] = '\0';
		}
		else{
			printf(USER_HEADER);
			PQclear(res);
			res = PQexec(conn,"SELECT * FROM users");
			n = PQntuples(res);
			for (i = 0; i < n; i++)
				printf(USER_RESULT, PQgetvalue(res, i, 0), PQgetvalue(res, i, 1));
			break;
		}
	}
	
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