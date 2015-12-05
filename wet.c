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

//TODO: Should we check if the table is empty?
void* addUser(const char*    name){
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

//TODO: Find a more efficient algorithm
void* addUserMin(const char*    name){
	int min, i, n;
	char cmd[200];
	PGresult *res;
	
	res = PQexec(conn, "SELECT MIN(ID) FROM users");
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "Error executing commands: %s\n",
			PQresultErrorMessage(res));
		PQclear(res);
		return;
	}

	if (PQgetisnull(res, 0, 0))/*if table is empty*/{
		sprintf(cmd, "INSERT INTO users values(0, \'%s\')",name);
		res = PQexec(conn, cmd);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
			PQclear(res);
		}
		else{
			printf(USER_HEADER);
			printf(USER_RESULT, 0, name);
		}
	}
	else{
		min = atoi(PQgetvalue(res, 0, 0));
		PQclear(res);

		for (i = min + 1;; i++){
			sprintf(cmd, "INSERT INTO users values(%d, \'%s\')", i, name);
			res = PQexec(conn, cmd);
			if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
				PQclear(res);
			}
			else{
				printf(USER_HEADER);
				PQclear(res);
				res = PQexec(conn, "SELECT * FROM users ORDER BY id");
				n = PQntuples(res);
				for (i = 0; i < n; i++)
					printf(USER_RESULT, PQgetvalue(res, i, 0), PQgetvalue(res, i, 1));
				break;
			}
		}
	}
	
}

//Could be improved
void* removeUser(const char*    id){
	char cmd[200];
	PGresult *res;

	sprintf(cmd, "SELECT * FROM users WHERE id = %s", id);
	res = PQexec(conn, cmd);

	if (PQntuples(res) == 0) {
		printf(ILL_PARAMS);
	}
	else{
		sprintf(cmd, "DELETE FROM users WHERE id = %s", id);
		res = PQexec(conn, cmd);
		PQclear(res);

		sprintf(cmd, "DELETE FROM photos WHERE user_id = %s", id);
		res = PQexec(conn, cmd);
		PQclear(res);

		sprintf(cmd, "DELETE FROM tags WHERE user_id = %s", id);
		res = PQexec(conn, cmd);
		PQclear(res);
	}
}

void* addPhoto(const char*    user_id,
	const char*    photo_id){
	char cmd[200];
	PGresult *res;

	sprintf(cmd, "SELECT * FROM users WHERE id = %s", user_id);
	res = PQexec(conn,cmd);
	if (PQntuples(res) == 0) {
		printf(ILL_PARAMS);
		return;
	}
	PQclear(res);

	sprintf(cmd, "SELECT * FROM photos WHERE id = %s AND user_id = %s", photo_id, user_id);
	res = PQexec(conn, cmd);
	if (PQntuples(res) > 0) {
		printf(EXISTING_RECORD);
		return;
	}
	PQclear(res);

	sprintf(cmd, "INSERT INTO photos VALUES(%s,%s)", photo_id, user_id);
	res = PQexec(conn,cmd);

}

void* tagPhoto(const char*    user_id,
	const char*    photo_id,
	const char*    info){
	char cmd[200];
	PGresult *res;

	sprintf(cmd, "SELECT * FROM users WHERE id = %s", user_id);
	res = PQexec(conn, cmd);
	if (PQntuples(res) == 0) {
		printf(ILL_PARAMS);
		return;
	}
	PQclear(res);

	sprintf(cmd, "SELECT * FROM tags WHERE photo_id = %s AND user_id = %s", photo_id, user_id);
	res = PQexec(conn, cmd);
	if (PQntuples(res) > 0) {
		printf(EXISTING_RECORD);
		return;
	}
	PQclear(res);

	sprintf(cmd, "INSERT INTO photos VALUES(%s,%s,%s)", photo_id, user_id, info);
	res = PQexec(conn, cmd);

}

void* photosTags(){
	char cmd[200];
	PGresult *res;
	int i;

	sprintf(cmd, "SELECT p.id, p.user_id, COUNT(info) AS tag_count FROM photos p LEFT OUTER JOIN tags t ON ( p.user_id = t.user_id AND p.id = t.photo_id ) GROUP BY p.id, p.user_id ORDER BY tag_count DESC, p.user_id ASC, p.id ASC");
	res = PQexec(conn, cmd);
	
	if (PQntuples(res) == 0)
		printf(EMPTY);
	else{
		printf(PHOTOS_HEADER);
		for (i = 0; i < PQntuples(res); i++)
			printf(PHOTOS_RESULT, PQgetvalue(res, i, 1), PQgetvalue(res, i, 0), PQgetvalue(res, i, 2));
	}
}

void* search(const char*    word){
	char cmd[200];
	PGresult *res;
	int i;

	sprintf(cmd, "SELECT p.id, p.user_id, COUNT(info) AS tag_count FROM photos p LEFT OUTER JOIN tags t ON ( p.user_id = t.user_id AND p.id = t.photo_id ) WHERE t.info = \'%s\' GROUP BY p.id, p.user_id ORDER BY tag_count DESC, p.user_id ASC, p.id DESC", word);
	res = PQexec(conn, cmd);

	if (PQntuples(res) == 0)
		printf(EMPTY);
	else{
		printf(PHOTOS_HEADER);
		for (i = 0; i < PQntuples(res); i++)
			printf(PHOTOS_RESULT, PQgetvalue(res, i, 1), PQgetvalue(res, i, 0), PQgetvalue(res, i, 2));
	}
}

//NOTE: Here counts photos instead of records
void* commonTags(const char*    k){
	char cmd[200];
	PGresult *res;
	int i;

	sprintf(cmd, "SELECT info, count(DISTINCT photo_id) AS photo_count FROM tags GROUP BY info HAVING count(DISTINCT photo_id) >= %s ORDER BY photo_count DESC, info ASC", k);
	res = PQexec(conn, cmd);

	if (PQntuples(res) == 0)
		printf(EMPTY);
	else{
		printf(COMMON_HEADER);
		for (i = 0; i < PQntuples(res); i++)
			printf(COMMON_LINE, PQgetvalue(res, i, 0), PQgetvalue(res, i, 1));
	}
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